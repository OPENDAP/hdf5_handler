/*-------------------------------------------------------------------------
 * Copyright (C) 1999	National Center for Supercomputing Applications.
 *			All rights reserved.
 *
 *-------------------------------------------------------------------------
 */

// This file contains functions which uses depth-first search to walk through a 
// hdf5 file and build the in-memeory DDS. 
//

#include "h5dds.h"

#include "InternalErr.h"

//for DODS grid only 
extern "C" {char *get_dimname(hid_t,int);}
extern "C" {char *correct_name(char*);}
extern "C" {int get_dimnum(hid_t);}
extern "C" {hid_t get_diminfo(hid_t dataset,int index,int*nelmptr,size_t* dsizeptr,hid_t* dimtypeptr);}


// for general DODS information
extern "C" { hid_t genfile(char * filename);}
extern "C" { int H5Gn_members(hid_t loc_id, char * group_name);}
extern "C" { herr_t H5Gget_obj_info_idx(hid_t loc_id, char *group_name,int idx, char **objname,int *type);} 
extern "C" { hid_t get_dataset(hid_t pid,char*dname,DS_t*dt_inst_ptr,char *error);}
extern "C" {int get_data( hid_t dset, void *buf,char *error);}

static char Msgt[255];



/*-------------------------------------------------------------------------
 * Function:	depth_first
 *
 * Purpose:	this function will walk through hdf5 group and using depth-
 *              first approach to obtain data information(data type and
                data pattern) of all hdf5 dataset and put it into dds table.
 *         	hard link is treated as a dataset at hdf5. 
 *            

 * Errors:      will return error message to the interface.
 * Return:	false, failed. otherwise,success.
  *
 * In :	        group id
                gname: group name(absolute name from root group).
                dds: reference of DDS object.
                error: error message to dods inteface.
		fname: file name.
 * Out:         in the process of depth first search. DDS table will be filled.
 *	     
 *
 *-------------------------------------------------------------------------
 */		
bool
depth_first(hid_t pid, char *gname, DDS &dds, const char *fname)
{
  /* Iterate through the file to see members of the root group */

  int nelems = H5Gn_members(pid, gname);

  if (nelems < 0 ) {
    string msg = "h5_dds handler: counting hdf5 group elements error for ";
    msg += gname;
    throw InternalErr(msg, __FILE__, __LINE__);
  }
       
  for (int i = 0; i < nelems; i++) {
    char *oname = NULL;
    int type = -1;
    herr_t ret = H5Gget_obj_info_idx(pid, gname,i, &oname, &type);

    if (ret < 0)  {
      string msg = "h5_dds handler: getting hdf5 object information error from ";
      msg += gname;
      throw InternalErr(msg, __FILE__, __LINE__);
    }

    switch (type) {

    case H5G_GROUP: {
      string full_path_name = string(gname) + string(oname) + "/";
      hid_t pgroup = H5Gopen(pid,gname);

      char *t_fpn = new char[full_path_name.length()+1];
      strcpy(t_fpn, full_path_name.c_str());
      try {
	depth_first(pgroup, t_fpn, dds, fname);
      }
      catch (Error &e) {
	delete [] t_fpn;
	throw;
      }
      delete [] t_fpn;
      break;
    }

    case H5G_DATASET: {
      string full_path_name = string(gname) + string(oname);
      hid_t dgroup= H5Gopen(pid,gname);
      if (dgroup <0) {
	string msg ="h5_dds handler: cannot open hdf5 group";
	msg +=gname;
	throw InternalErr(msg,__FILE__,__LINE__);
      }

      char *t_fpn = new char[full_path_name.length()+1];
      strcpy(t_fpn, full_path_name.c_str());
      /* obtain hdf5 dataset handle. */
      if ((get_dataset(dgroup, t_fpn, &dt_inst, Msgt)) < 0) {
	string msg = "h5_dds handler: get hdf5 dataset wrong for ";
	msg += t_fpn;
	msg += string("\n") + string(Msgt);
	delete [] t_fpn;
	throw InternalErr(msg, __FILE__, __LINE__);
      }
                    
      // Put hdf5 dataset structure into DODS dds.
      // read_objects throws InternalErr.
      try {
	read_objects(dds,t_fpn,fname);
      }
      catch (Error &e) {
	delete [] t_fpn;
	throw;
      }

      delete [] t_fpn;
      break;
    }	     
    case H5G_TYPE: 
      break;

    default:
      break;
    }

    type = -1;

  }
  return true;
}


/*-------------------------------------------------------------------------
 * Function:	return_type
 *
 * Purpose:	this function will get the text representation(string) of the 
                corresponding DODS datatype.
 *              DODS-HDF5 subclass method will use this function.
 * Errors:
 *
 * Return:     string
  *
 * In :	       datatype id
 *	     
 *
 *-------------------------------------------------------------------------
 */			
string
return_type(hid_t type) 
{
 
  switch (H5Tget_class(type)) {

  case H5T_INTEGER:

    if (H5Tequal(type, H5T_STD_I8BE)||
	H5Tequal(type, H5T_STD_I8LE)||
	H5Tequal(type, H5T_NATIVE_SCHAR))
      return BYTE; 

    else if (H5Tequal(type, H5T_STD_I16BE)||
	     H5Tequal(type, H5T_STD_I16LE)||
	     H5Tequal(type, H5T_NATIVE_SHORT))
      return INT16;

    else if (H5Tequal(type, H5T_STD_I32BE)||
	     H5Tequal(type, H5T_STD_I32LE)||
	     H5Tequal(type, H5T_NATIVE_INT))
      return INT32;
    
    else if (H5Tequal(type, H5T_STD_U16BE)||
	     H5Tequal(type, H5T_STD_U16LE)||
	     H5Tequal(type, H5T_NATIVE_USHORT))
      return UINT16;

    else if (H5Tequal(type, H5T_STD_U32BE)||
	     H5Tequal(type, H5T_STD_U32LE)||
	     H5Tequal(type, H5T_NATIVE_UINT))
      return UINT32;

#if 0
    //64-bit and 128-bit integers currently not supported by DODS.

    else if (H5Tequal(type, H5T_NATIVE_LONG))
      printf( "H5T_NATIVE_LONG");
    else if (H5Tequal(type, H5T_NATIVE_ULONG))
      printf( "H5T_NATIVE_ULONG");
    else if (H5Tequal(type, H5T_NATIVE_LLONG))
      printf( "H5T_NATIVE_LLONG");
    else if (H5Tequal(type, H5T_NATIVE_ULLONG))
      printf( "H5T_NATIVE_ULLONG");
   
#endif
    else 
      return INT_ELSE;
       
  case H5T_FLOAT: 

    if (H5Tget_size(type)==4) 
      return FLOAT32;
    else if (H5Tget_size(type)==8)
      return FLOAT64;
    else 
      return FLOAT_ELSE;

#if 0
    // 128-bit float point number not supported by DODS.
    //else if (H5Tequal(type, H5T_NATIVE_LDOUBLE))
    //printf( "H5T_NATIVE_LDOUBLE");
#endif  
	
  case H5T_STRING: 
    return STRING;


     
  }
}

/*-------------------------------------------------------------------------
 * Function:	Get_bt
 *
 * Purpose:	this function will create a new DODS object that corresponds 
                with  HDF5 dataset and return pointer of a new object of DODS 
		datatype.
 *             

 * Errors:
 *
 * Return:	pointer of base type. 
  *
 * In :	        datatype id, object name
 *	     
 *
 *-------------------------------------------------------------------------
 */			
BaseType *
Get_bt(string varname, hid_t datatype) 
{
  BaseType* temp_bt;
  //  cerr << varname << endl;
  switch (H5Tget_class(datatype)) {

  case H5T_INTEGER:
	
    if (H5Tequal(datatype, H5T_STD_I8BE)||
	H5Tequal(datatype, H5T_STD_I8LE)||
	H5Tequal(datatype, H5T_NATIVE_SCHAR)){
      temp_bt = NewByte(varname);	
      //   return (NewByte(varname)); 
    }
    else if (H5Tequal(datatype, H5T_STD_I16BE)||
	     H5Tequal(datatype, H5T_STD_I16LE)||
	     H5Tequal(datatype, H5T_NATIVE_SHORT)){
      temp_bt = NewInt16(varname);	
      // return (NewInt16(varname));
    }

    else  if (H5Tequal(datatype, H5T_STD_I32BE)||
	      H5Tequal(datatype, H5T_STD_I32LE)||
	      H5Tequal(datatype, H5T_NATIVE_INT)){
      temp_bt = NewInt32(varname);
      //return (NewInt32(varname));
    }

    else if (H5Tequal(datatype, H5T_STD_U16BE)||
	     H5Tequal(datatype, H5T_STD_U16LE)||
	     H5Tequal(datatype, H5T_NATIVE_USHORT)){
      temp_bt = NewUInt16(varname);
      //return (NewUInt16(varname));
    }

    else if (H5Tequal(datatype, H5T_STD_U32BE)||
	     H5Tequal(datatype, H5T_STD_U32LE)||
	     H5Tequal(datatype, H5T_NATIVE_UINT)){
      temp_bt = NewUInt32(varname);
      //return (NewUInt32(varname));
    }

    else {
      temp_bt = NULL;
      //return NULL;
    }       
    break;

  case H5T_FLOAT: 

    if (H5Tget_size(datatype)==4) {
      //return (NewFloat32(varname));
      temp_bt = NewFloat32(varname);
    }
    else if (H5Tget_size(datatype)==8){

      temp_bt = NewFloat32(varname);
    }
    else {
      temp_bt = NULL;

    }
    break;
         
  case H5T_STRING: 
    temp_bt = NewStr(varname);

    break;
  }

  if (!temp_bt) 
    return NULL;
  

  switch(temp_bt->type()){

  case dods_byte_c:

    (dynamic_cast<HDF5Byte *>(temp_bt))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5Byte *>(temp_bt))->set_tid(dt_inst.type);
    break;

  case dods_int16_c:

    (dynamic_cast<HDF5Int16 *>(temp_bt))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5Int16 *>(temp_bt))->set_tid(dt_inst.type);
    break;

  case dods_uint16_c:

    (dynamic_cast<HDF5UInt16 *>(temp_bt))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5UInt16 *>(temp_bt))->set_tid(dt_inst.type);
    break;

  case dods_int32_c:

    (dynamic_cast<HDF5Int32 *>(temp_bt))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5Int32 *>(temp_bt))->set_tid(dt_inst.type);
    break;

  case dods_uint32_c:
       
    (dynamic_cast<HDF5UInt32 *>(temp_bt))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5UInt32 *>(temp_bt))->set_tid(dt_inst.type);
    break;

  case dods_float32_c:
	
    (dynamic_cast<HDF5Float32 *>(temp_bt))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5Float32 *>(temp_bt))->set_tid(dt_inst.type);
    break;

  case dods_float64_c:

    (dynamic_cast<HDF5Float64 *>(temp_bt))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5Float64 *>(temp_bt))->set_tid(dt_inst.type);
    break;

  case dods_str_c:

    (dynamic_cast<HDF5Str *>(temp_bt))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5Str *>(temp_bt))->set_tid(dt_inst.type);
    (dynamic_cast<HDF5Str *>(temp_bt))->set_arrayflag(STR_NOFLAG);
    //   cerr <<"coming into dods_str_c" <<endl;
    break;
#if 0
  case dods_array_c:
  case dods_url_c:

    (dynamic_cast<HDF5Url *>(temp_bt))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5Url *>(temp_bt))->set_tid(dt_inst.type);

  case dods_list_c:

  case dods_structure_c:

  case dods_sequence_c:

  case dods_grid_c:
            
#endif

  default:    

    /*  sprintf(Msgt,"error in casting base datatype for object %s",temp_varname);
     *error =(string)Msgt;*/
    //return false;
    return NULL;
  }
  return temp_bt;
}


// Given a reference to an instance of class DDS and a filename that refers
// to a hdf5 file, read hdf5 file and extract all the dimensions of
// each of its variables. Add the variables and their dimensions to the
// instance of DDS.
//
// Returns: false if an error accessing the netcdf file was detected, true
// otherwise. 


/*-------------------------------------------------------------------------
 * Function:	read_objects
 *
 * Purpose:	this function will fill in information of a dataset
                (name,data type,data space)into one DDS table. 
		It will use dynamic cast to put necessary information into 
		subclass of dods datatype.
 *              

 * Errors:
 *
 * Return:	false, failed. otherwise,success.
  *
 * In :	        DDS object: reference
                varname: absolute name of either a dataset or a group
                error: a string of error message to the dods interface.
		filename: dataset name.
 *	     
 *
 *------------------------------------------------------------
 */			

/** This function will fill in information of a dataset (name,data
    type,data space)into one DDS table.  It will use dynamic cast to
    put necessary information into subclass of dods datatype.

    @return: false, failed. otherwise,success.
    @param dds_table Destination for the HDF5 objects. 
    @param varname Absolute name of either a dataset or a group
    @param error a string of error message to the dods interface.
    @param filename Added to the DDS (dds_table). */
void
read_objects(DDS &dds_table, const string &varname, const string &filename)
{
  Array *ar;
  Grid *gr;
  Part pr;

  int dim_index;
  size_t dimsize;
  hid_t dimtype;
  hid_t dimid;
  int num_dimelm;
  int num_dim;

  char* dimname;
  char* newdimname;
  char dimlab[DODS_MAX_RANK][6];

  char* cptr;
  char* newname;
  char ORI_SLASH = '/';
  char CHA_SLASH ='_';
  char *temp_varname = new char[varname.length()+1];
  varname.copy(temp_varname,string::npos);
  temp_varname[varname.length()]=0;
  //The following code will change "/" into "_"
#if 0
    while(strchr(temp_varname,ORI_SLASH)!=NULL){
      cptr = strchr(temp_varname,ORI_SLASH);
      *cptr = CHA_SLASH;
      }
#endif
    
  newname = strrchr(temp_varname, ORI_SLASH);
  newname++;

#if 0
  // make/test this mod sometime in the future.
  string::size_type pos = varname.rfind(ORI_SLASH);
  string newname = varname.substr(++pos, varname.end());
#endif

  dds_table.set_dataset_name(name_path(filename));
 
  BaseType *bt = Get_bt(newname,dt_inst.type);
  if (!bt) {
    delete [] temp_varname;
    // NB: We're throwing InternalErr even though it's possible that
    // someone might ask for a HDF5 varaible which this server cannot
    // handle (for example, an HDF5 reference).
    throw InternalErr("Unable to convert hdf5 datatype to dods basetype",
		      __FILE__, __LINE__);
#if 0
    *error = "unable to convert hdf5 datatype to dods  basetype";
    return false;
#endif
  }

  // first dealing with scalar data. 

  if (dt_inst.ndims == 0){
    dds_table.add_var(bt);
  }
	
  else {   
    // dealing with array data. 
    ar = NewArray(temp_varname);
 
    (dynamic_cast<HDF5Array *>(ar))->set_did(dt_inst.dset);
    (dynamic_cast<HDF5Array *>(ar))->set_tid(dt_inst.type);
    (dynamic_cast<HDF5Array *>(ar))->set_memneed(dt_inst.need);
    (dynamic_cast<HDF5Array *>(ar))->set_numdim(dt_inst.ndims);
    (dynamic_cast<HDF5Array *>(ar))->set_numelm((int)(dt_inst.nelmts));
    ar->add_var(bt);
	
#ifdef DODSGRID

    //check whether the dimension number matches.
    num_dim = get_dimnum(dt_inst.dset);

    if(num_dim==dt_inst.ndims) {
      // start building up the grid.
      //  gr = NewGrid(temp_varname);

      gr = NewGrid(newname);
      pr = array;

      for(dim_index = 0; dim_index <dt_inst.ndims; dim_index++){

	dimname = get_dimname(dt_inst.dset,dim_index);
	newdimname = correct_name(dimname);
	//  printf("dimname at array%s\n",newdimname);
	/* 1) obtain dimensional id, 
	   2) number of element in this dim.,
	   3) total dimensional size of this dimensional scale data
	   4) the HDF5 dimensional data type
	*/
	   
	dimid   = get_diminfo(dt_inst.dset,dim_index,&num_dimelm,&dimsize,&dimtype);

	// printf("num_dimelm at arrays%d\n",num_dimelm);
	ar->append_dim(num_dimelm,newdimname);
	free(dimname);
	free(newdimname);
      }

      gr->add_var(ar,pr);
      pr = maps;
      for(dim_index =0; dim_index <dt_inst.ndims;dim_index++){
	//get dimensional scale datasets. add to grid.
	dimname = get_dimname(dt_inst.dset,dim_index);
	newdimname = correct_name(dimname);
	dimid   = get_diminfo(dt_inst.dset,dim_index,&num_dimelm,&dimsize,&dimtype);
	bt = Get_bt(newdimname,dimtype);
	ar = NewArray(newdimname);
	(dynamic_cast<HDF5Array *>(ar))->set_did(dimid);
	(dynamic_cast<HDF5Array *>(ar))->set_tid(dimtype);
	(dynamic_cast<HDF5Array *>(ar))->set_memneed(dimsize);
	(dynamic_cast<HDF5Array *>(ar))->set_numdim(1);
	(dynamic_cast<HDF5Array *>(ar))->set_numelm(num_dimelm);
	ar->add_var(bt);
	//  ar->append_dim(dimsize,dimname);//add this later
	//    cout <<"dimname at maps"<<newdimname<<endl;
	//   cout <<"num_dimelm at maps"<<num_dimelm<<endl;
	//  cout <<"dimid at maps" <<dimid<<endl;
	ar->append_dim(num_dimelm,newdimname);
	gr->add_var(ar,pr);
	free(dimname);
	free(newdimname);
      }
      dds_table.add_var(gr);
    }
    else{
      for (int d = 0; d < dt_inst.ndims; ++d) {
	dimlab[d][0] ='d';
	dimlab[d][1]='i';
	dimlab[d][2]='m';
	if (d> 9) {
	  dimlab[d][3]=(char)(d/10+48);
	  dimlab[d][4]=(char)(d%10+49);
	  dimlab[d][5]='\0';
	}
	else {
	  dimlab[d][3]=(char)(d+49);
	  dimlab[d][4]='\0';
	}
	ar->append_dim(dt_inst.size[d],dimlab[d]);
      }
      dds_table.add_var(ar);
    }
	
#else 
    for (int d = 0; d < dt_inst.ndims; ++d) {
      dimlab[d][0] ='d';
      dimlab[d][1]='i';
      dimlab[d][2]='m';
      if (d> 9) {
	dimlab[d][3]=(char)(d/10+48);
	dimlab[d][4]=(char)(d%10+49);
	dimlab[d][5]='\0';
      }
      else {
	dimlab[d][3]=(char)(d+49);
	dimlab[d][4]='\0';
      }
      ar->append_dim(dt_inst.size[d],dimlab[d]);
    }
    dds_table.add_var(ar);
#endif
  }
  delete [] temp_varname;   
}   













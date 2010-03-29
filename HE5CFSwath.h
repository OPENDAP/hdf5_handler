// This file is part of hdf5_handler - a HDF5 file handler for the OPeNDAP
// data server.

// Authors: 
// Hyo-Kyung Lee <hyoklee@hdfgroup.org>
// Muqun Yang <myang6@hdfgroup.org> 

// Copyright (c) 2009 The HDF Group, Inc. and OPeNDAP, Inc.
//
// This is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// This software is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.
// You can contact The HDF Group, Inc. at 1901 South First Street,
// Suite C-2, Champaign, IL 61820  
#ifndef _HE5CFSWATH_H
#define _HE5CFSWATH_H

#include "config_hdf5.h"
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <InternalErr.h>
#include <debug.h>

using namespace std;
using namespace libdap;
/// A class for holding HDF-EOS5 swath information.
///
/// This class contains functions that generate CF-convention compliant output
/// for HDF-EOS5 Swath.
/// By default, hdf5 handler cannot generate the DAP ouput that OPeNDAP
/// visualization clients can display due to the discrepancy between the 
/// HDF-EOS5 model and the CF-1.x convention model. Most visualization
/// clients require an output that follows CF-convention in order to display
/// data directly on a map. 
/// 
///
/// @author Hyo-Kyung Lee <hyoklee@hdfgroup.org>
///
class HE5CFSwath {
  
private:
    bool _swath;                // Is it swath?
    bool _swath_2D;             // Is it 1-D or 2-D swath?
    string   _swath_lon_dimensions;   // The lon dimension names
    string   _swath_lat_dimensions;   // The lat dimension names
    string   _swath_lev_dimensions;   // The lev dimension names
    string   _swath_lon_variable;     // The name of lon variable
    string   _swath_lat_variable;     // The name of lat variable
    string   _swath_lev_variable;     // The name of lev variable

    map < string, int > _swath_dimension_size;
    map < string, string > _swath_variable_dimensions;
    vector < string >   _swath_dimension_list;
    vector < string >   _swath_variable_list;

  
public:
  
    HE5CFSwath();
    virtual ~HE5CFSwath();

    /// Checks if the current HDF5 file is a valid HDF-EOS5 Swath file.
    ///
    /// \return true if it has a set of correct meta data files.
    /// \return false otherwise  
    bool get_swath();

    /// Checks if the HDF-EOS5 Swath file has 2-D lat/lon.
    ///
    /// \return true if it has 2-D lat/lon coordinate variable.
    /// \return false otherwise  
    bool get_swath_2D();

    /// Returns the string representation of coordinate attirbute
    /// from lat / lon variable name.
    string  get_swath_coordinate_attribute();

    /// Returns true if \a varname matches the dimension names that 
    /// lat/lon variable has.
    bool get_swath_coordinate_dimension_match(string varname);

    /// Get the vector of all dimensions used in this HDF-EOS5 file.
    ///
    /// \param[in] tokens a vector to be fetched 
    void  get_swath_dimension_list(vector < string > &tokens);

    /// Get the size informationfrom the \a name dimension.
    ///
    /// \param name like nTimes
    /// \return the size of dimension in integer
    int  get_swath_dimension_size(string name);

    
    /// Checks if the argument string is a HDF-EOS5 SWATH dataset.
    ///
    /// \return true if it is a Swath dataset.
    /// \return false otherwise  
    bool get_swath_variable(string varname);

    /// Get dimension names of a swath variable.
    ///
    /// Checks if this class parsed the \a name as swath.
    /// Retrieve the dimension list from the \a name swath
    /// and tokenize the list into the string vector.
    /// \param[in] name a grid variable name
    /// \param[out] tokens a vector of ',' delimited dimension names
    void get_swath_variable_dimensions(string name, vector < string > &tokens);


    /// Clears all internal map variables and flags.
    void set();

    /// Sets the flag if HDF5 file is a valid NASA EOS SWATH file.
    void set_swath(bool flag);

    /// Checks if the NASA EOS SWATH file has valid 1-D or 2-D lat/lon.
    bool set_swath_2D();

    /// Remembers the dimension size of each dimension for a Swath variable.
    /// 
    /// It builds a map vector of dimension name and its size. It also checks
    /// for consistency among dimensions used in swath variables. For example,
    /// if a swath variable s1 has dimensions of  x=100 and y=200 and
    /// another swath variable s2 has dimensions of x=50 and y=100,
    /// this function throws an exception.
    ///    
    /// \param[in] dimension_name a name of dimension in a swath variable.
    /// \param[in] dimension a size of \a dimension_name dimension
    void set_swath_dimension_size(string dimension_name, int dimension);

    /// Remembers the full path of a Swath dataset variable.
    /// 
    /// It pushes the StructMetadata-parsed full path variable name
    /// into the vector for further processing.
    /// \param[in] full_path a full path information of a variable in metadata.
    void set_swath_variable_list(string full_path);

    /// Remebers the dimension names associated with a variable.
    ///
    /// It pushes the EOS-metadata-parsed dimension names list into
    /// map. The map keeps track of the association between variable
    /// and its dimension names for CF-compliant DDS/DAS generation.
    /// \param[in] full_path a full path information of a variable in metadata.
    /// \param[in] dimension_list a list of dimensions in metadata.
    void set_swath_variable_dimensions(string full_path,
                                       string dimension_list);
    
    /// Stores variables that are missing in the StructMetadata.
    ///
    /// It manually pushes the variable and its dimension names  into
    /// the maps. An OMI swath product like OMUVB has a missing entry
    /// for files that were created before 2007.
    void set_swath_missing_variable();
  
};

#endif
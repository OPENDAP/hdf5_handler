// This file is part of hdf5_handler a HDF5 file handler for the OPeNDAP
// data server.

// Author: Hyo-Kyung Lee <hyoklee@hdfgroup.org> and Muqun Yang
// <myang6@hdfgroup.org> 

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
#ifndef _H5CF_H
#define _H5CF_H

#include "config_hdf5.h"

#include "H5ShortName.h"
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <InternalErr.h>
#include <debug.h>

using namespace std;
using namespace libdap;
/// A class for generating CF convention compliant output.
///
/// This class contains functions that generate CF-convention compliant output.
/// By default, hdf5 handler cannot generate the DAP ouput that OPeNDAP
/// visualization clients can display due to the discrepancy between the 
/// HDF-EOS5 model and the model based on CF-convention. Most visualization
/// clients require an output that follows CF-convention in order to display
/// data directly on a map. 
/// 
///
/// @author Hyo-Kyung Lee <hyoklee@hdfgroup.org>
///
/// Copyright (c) 2009 The HDF Group
///
/// All rights reserved.
class H5CF: public H5ShortName {
  
private:
  
    bool _swath;
    map < string, int > _dimension_map_swath;
    vector < string >   _dimensions_swath;
    vector < string >   _full_data_paths_swath;
  
public:
    
    /// A flag for Level 2 OMI Swath data. Only OMI data has 2-D Grid.
    bool OMI; 
    /// A flag for checking whether shared dimension variables are generated
    /// or not.
    bool shared_dimension;        

    /// A look-up map for translating HDF-EOS5 convention to CF-1.x convention
    map < string, string > eos_to_cf_map;
    /// A reverse look-up map for translating CF-1.x convention to HDF-EOS5
    /// convention
    map < string, string > cf_to_eos_map;

  
    H5CF();
    virtual ~H5CF();
    
    /// Remembers the full path of a Swath dataset variable.
    /// 
    /// It pushes the StructMetadata-parsed full path variable name
    /// into the vector for further processing.
    /// \param[in] full_path a full path information of a variable in metadata.
    void        add_data_path_swath(string full_path);

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
    void        add_dimension_map_swath(string dimension_name, int dimension);

    /// Returns a character pointer to the string that matches CF-convention.
    const char* get_CF_name(char *eos_name);

    /// Returns a string that matches EOS-convention
    string      get_EOS_name(string cf_name);

    /// Returns whether shared dimension variables are generated or not.
    bool        is_shared_dimension_set();
    
    /// Checks if the current HDF5 file is a valid HDF-EOS5 Swath file.
    ///
    /// \return true if it has a set of correct meta data files.
    /// \return false otherwise  
    bool        is_swath();
    
    /// Checks if the argument string is a HDF-EOS5 SWATH dataset.
    ///
    /// \return true if it is a Swath dataset.
    /// \return false otherwise  
    bool        is_swath(string varname);

    /// Clears all internal map variables and flags.
    void        reset();

    /// Sets the shared_dimension flag true.
    void        set_shared_dimension();

    /// Sets the flag if HDF5 file is a valid NASA EOS SWATH file.
    void        set_swath(bool flag);

  
};

#endif

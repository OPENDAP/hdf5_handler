// This file is part of the hdf5_handler implementing for the CF-compliant
// Copyright (c) 2011 The HDF Group, Inc. and OPeNDAP, Inc.
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
// You can contact The HDF Group, Inc. at 1800 South Oak Street,
// Suite 203, Champaign, IL 61820  

////////////////////////////////////////////////////////////////////////////////
/// \file HDF5CFUInt32.cc
/// \brief The implementation of mapping HDF5 unsigned 32-bit integer to DAP uint32 for the CF option
///
/// In the future, this may be merged with the default option.
/// \author Muqun Yang <myang6@hdfgroup.org>
///
////////////////////////////////////////////////////////////////////////////////

#include "config_hdf5.h"


#include "InternalErr.h"
#include "HDF5CFUInt32.h"

HDF5CFUInt32::HDF5CFUInt32(const string &n, const string &d) : UInt32(n, d)
{
}

HDF5CFUInt32::~HDF5CFUInt32()
{
}
BaseType *HDF5CFUInt32::ptr_duplicate()
{
    return new HDF5CFUInt32(*this);
}

bool HDF5CFUInt32::read()
{
    throw InternalErr(__FILE__, __LINE__,
                      "Unimplemented read method called.");
}

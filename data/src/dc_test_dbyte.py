# Copyright (C) 2014 The HDF Group
#
# This examaple creates an HDF5 file d_dset_big.h5 and a 8G dataset /dset in it.
#
# It will take 2 hours to create this file.
#
import h5py
import random
import numpy as np
#
# Create a new file using defaut properties.
#
file = h5py.File('dc_test_dbyte.h5','w')
#
# Create a dataset under the Root group.
#
#dataset1 = file.create_dataset("dset",(1, 5392, 3200), h5py.h5t.STD_I8LE)
dataset1 = file.create_dataset("dset",(1, 10784, 3200), h5py.h5t.STD_I8LE)
#
# Initialize data object with 0.
#
data = np.zeros((1,10784,3200))
#
# Assign new values
#

for i in range(1):
       for k in range(10784):
          for l in range(3200):
               data[i][k][l]= random.randrange(-127,128)
#
# Write data
#
dataset1[...] = data

#
# Close the file before exiting
#
file.close()


#besstandalone -d"cerr,all" -c bes-testsuite/bes.conf -i bes-testsuite/h5.he5/grid_1_2d.h5.dmr.bescmd
#valgrind --leak-check=full besstandalone -c bes-testsuite/bes.conf -i bes-testsuite/h5.nasa/3A-MO.GPM.GMI.GRID2014R1.20140601-S000000-E235959.06.V03A.h5.dds.bescmd
#besstandalone -c /etc/bes/bes.conf -i bes-testsuite/h4.nasa1.with_hdfeos2/MOD09GA.A2007268.h10v08.005.2007272184810.hdf.das.bescmd1
#besstandalone -c bes-testsuite/bes.conf -i bes-testsuite/h4.nasa1.with_hdfeos2/MOD09GA.A2007268.h10v08.005.2007272184810.hdf.data.bescmd | getdap -M -
#valgrind besstandalone -c bes-testsuite/bes.conf -i bes-testsuite/h5.he5/grid_1_2d.h5.dmr.bescmd
#besstandalone -c bes-testsuite/bes.conf -i bes-testsuite/h5.he5/grid_1_2d.h5.dap.bescmd | getdap4 -D -M -
#valgrind -v besstandalone -c bes-testsuite/bes.conf -i bes-testsuite/h5.he5/grid_1_2d.h5.dap.bescmd
#besstandalone -c bes-testsuite/bes.conf -i bes-testsuite/h5.cf/grid_1_2d.h5.dds.bescmd
#besstandalone -c bes-testsuite/bes.default.conf -i bes-testsuite/h5.default/d_int.h5.das.bescmd
#besstandalone -c bes-testsuite/bes.conf -i bes-testsuite/h5.nasa/OMPS-NPP_LP-L2-O3-DAILY_v2.5_2019m1208_2019m1209t143730.h5.nc.bescmd>OMPS-NPP_LP-L2-O3-DAILY_v2.5_2019m1208_2019m1209t143730.nc
#besstandalone -d"cerr,fonc" -c bes-testsuite/bes.conf -i bes-testsuite/h5.nasa/OMPS-NPP_LP-L2-O3-DAILY_v2.5_2019m1208_2019m1209t143730.h5.nc.bescmd>OMPS-NPP_LP-L2-O3-DAILY_v2.5_2019m1208_2019m1209t143730.nc




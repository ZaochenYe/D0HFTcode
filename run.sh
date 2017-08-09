#!/bin/bash
cd default # raw yield from side-band method
rm -rf pic data
bash run.sh

cd ../fit # raw yield from gaus+pol1 fit method
rm -rf pic data
bash run.sh

cd ../ptCut1  # pT>0.6 ==> pT>0.3
rm -rf pic data
bash run.sh

cd ../ptCut2  # pT>0.6 ==> pT>0.3
rm -rf pic data
bash run.sh

cd ../topoCut1  # tight topo cuts
rm -rf pic data
bash run.sh

cd ../topoCut2  # loose topo cuts
rm -rf pic data
bash run.sh

cd ../DoubleCount # double count
rm -rf data
root -b -q rm_doubleCount.C

cd ../sys     # sys error summary
rm -rf data pic
root -b -q write_err.C
root -b -q plot_err.C

cd ../ptShift  # do pT shift
root -b -q doPtShift.C

cd ../RAA
rm -rf pic
root -b -q write_RAA_Integral.C # use RAA in integral pT bin,  write_RAA.C ==> use RAA at pT value after pT shift
root -b -q plot_RAA_1.C  # draw each cent
root -b -q plot_RAA.C    # draw 0-10%, 10-40%, 40-80% together

cd ../Rcp
root -b -q plot_Rcp.C

#### Get TOF match eff. ###
#cd ../tofMatch
#root -b -q get_tofMatch.C

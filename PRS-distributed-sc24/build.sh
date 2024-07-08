#!/bin/bash

cd kissat-inc
make clean
./configure
make -j
cd ..

cd mapleCOMSPS/m4ri-20140914
./configure
make clean
make -j
cd ../..

cd mapleCOMSPS
make clean
make -j
cd ..

cd yalsat
./configure.sh
make -j
cd ..

cd lingeling
./configure.sh
make clean
./configure.sh
make -j
cd ..

make clean
make -j
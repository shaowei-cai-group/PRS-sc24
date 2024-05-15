#!/bin/bash

INSTANCE=$1

mpirun --bind-to none -np 4 --allow-run-as-root ./PRS-distributed -i $INSTANCE --share=1 --threads=4 --times=10 --share_method=0
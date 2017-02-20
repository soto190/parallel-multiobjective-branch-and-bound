#!/bin/sh

#  submit-multiple.sh
#  ParallelBaB
#
#  Created by Carlos Soto on 20/02/17.
#  Copyright © 2017 Carlos Soto. All rights reserved.

#PBS -N parallel
#PBS -m ea
#PBS -M soto190@gmail.com
#PBS -o results/
#PBS -j oe

#Call this file with qsub -t 0-4 to run all the instances.

declare -a INSTANCE=(
"/home/csoto/instances/fjssp/kacem/Kacem-I1.txt"
"/home/csoto/instances/fjssp/kacem/Kacem-I2.txt"
"/home/csoto/instances/fjssp/kacem/Kacem-I3.txt"
"/home/csoto/instances/fjssp/kacem/Kacem-I4.txt"
)

/home/csoto/parallelBaB/branchAndBound FJSSP ${INSTANCE[${PBS_ARRAYID}]} ${INSTANCE[${PBS_ARRAYID}]} /home/csoto/parallelBaB/results/


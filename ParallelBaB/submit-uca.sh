#!/bin/bash
#SBATCH -n 15
#SBATCH -N 1
#SBATCH -J ParallelBB
#SBATCH -o parallelBB.%j.out
#SBATCH -e parallelBB.%j.out

~/parallel-multiobjective-branch-and-bound/ParallelBaB/branchAndBound 15 FJSSP ~/instances/fjssp/kacem/Kacem-I1.txt ~/instances/fjssp/kacem/Kacem-I0.txt ~/results/
~                                                                               

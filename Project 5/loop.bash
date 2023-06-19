#!/bin/bash
#SBATCH -J MonteCarlo
#SBATCH -A cs475-575
#SBATCH -p classgpufinal
#SBATCH --constraint=v100
#SBATCH --gres=gpu:1
#SBATCH -o MonteCarlo.out
#SBATCH -e MonteCarlo.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=deshpana@oregonstate.edu
for t in 1024 4096 16384 65536 262144 1048576 2097152
do
    for b in 8 32 64 128
    do
        /usr/local/apps/cuda/cuda-10.1/bin/nvcc -DNUMTRIALS=$t -DBLOCKSIZE=$b -o MonteCarlo  proj05.cu
        ./MonteCarlo
    done
done
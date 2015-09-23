#!/bin/bash

#SBATCH --time=00:20:00   # walltime
#SBATCH --ntasks=24   # number of processor cores (i.e. tasks)
#SBATCH --nodes=1   # number of nodes
#SBATCH --mem-per-cpu=10240M   # memory per CPU core
#SBATCH -J "Hotplate"   # job name
#SBATCH --mail-user=landen.barr@gmail.com   # email address
#SBATCH --mail-type=BEGIN
#SBATCH --mail-type=END
#SBATCH --mail-type=FAIL

# Compatibility variables for PBS. Delete if not needed.
export PBS_NODEFILE=`/fslapps/fslutils/generate_pbs_nodefile`
export PBS_JOBID=$SLURM_JOB_ID
export PBS_O_WORKDIR="$SLURM_SUBMIT_DIR"
export PBS_QUEUE=batch

# Set the max number of threads to use for programs using OpenMP. Should be <= ppn. Does nothing if the program doesn't use OpenMP.
export OMP_NUM_THREADS=$SLURM_CPUS_ON_NODE

# LOAD MODULES, INSERT CODE, AND RUN YOUR PROGRAMS HERE


OMP_NUM_THREADS=1
g++ -fopenmp -O3 -mcmodel=medium  -pthread hotplate.cpp -o hotplate1
./hotplate1


OMP_NUM_THREADS=2
g++ -fopenmp -O3 -mcmodel=medium  -pthread hotplate.cpp -o hotplate2
./hotplate2


OMP_NUM_THREADS=4
g++ -fopenmp -O3 -mcmodel=medium  -pthread hotplate.cpp -o hotplate3
./hotplate3


OMP_NUM_THREADS=8
g++ -fopenmp -O3 -mcmodel=medium  -pthread hotplate.cpp -o hotplate4
./hotplate4


OMP_NUM_THREADS=16
g++ -fopenmp -O3 -mcmodel=medium  -pthread hotplate.cpp -o hotplate5
./hotplate5


OMP_NUM_THREADS=24
g++ -fopenmp -O3 -mcmodel=medium  -pthread hotplate.cpp -o hotplate6
./hotplate6

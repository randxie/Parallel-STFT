#!/bin/bash
#SBATCH -J mpirunSTFT       # job name
#SBATCH -o mpirunSTFT.o%j   # output and error file name (%j expands to jobID)
#SBATCH -n 1000             # total number of mpi tasks requested
#SBATCH -p normal     		# queue (partition) -- normal, development, etc.
#SBATCH -t 00:20:00        	# run time (hh:mm:ss) - 1.5 hours
#SBATCH --mail-user=yxie@utexas.edu
#SBATCH --mail-type=begin  	# email me when the job starts
#SBATCH --mail-type=end    	# email me when the job finishes
ibrun -n 4 -o 1 mpirunSTFT
ibrun -n 8 -o 1 mpirunSTFT
ibrun -n 12 -o 1 mpirunSTFT

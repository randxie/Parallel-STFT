# Overview
The programs are designed for SDS394c Parallel Computing project. 
There are 3 folders "src", "data" and "doc". As their names suggest, "src" is used to store all the c++ code, including test cases, "data" is used to store relevant signal files and "doc" is used to store final report.

#Software requirement:
Before running programs, please use "make" to compile the programs.
Need omp, mpi, libpng and kissFFT
For libpng, Use "sudo apt-get install libpng++-dev" for installation; kissFFT has been put into folder lib

#Design Idea:
-Fast Fourier Transform (FFT)
 	 For convenience, I use kissFFT (open source library) for FFT and inverse FFT, as the main purpose is to parallelize whole TFD calculation. Also, FFT has been well implemented by many researchers. There is no need to repeat this work. However, a parallelized version of STFT (Short Time Fourier Transform) is not widely seen. 

 	 To speed up this part, better open source library can be used such as FFTW. 

#To run the code:
	Use make all to generate two executable "runSTFT" and "mpirunSTFT"
	For "runSTFT", just use "./runSTFT";
	For "mpirunSTFT", use "mpirun -n X mpirunSTFT", where X is the number of nodes you want

#IMPORTANT NOTE:
	All the tests are done on a local machine (Dell, 8 CPU, 16GB). Make sure you have "libpng++-dev" when compiling the code.
#Test results:
	Given in the report

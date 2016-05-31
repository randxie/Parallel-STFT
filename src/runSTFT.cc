#include <iostream>
#include "Signal.cc"
#include "STFT.cc"
using namespace std;

int main(int argc, char ** argv) 
{
	// Set up STFT parameters
	ParaSTFT* para = new ParaSTFT;

	// parameter for openMP parallelization
	para->nfft = 1024*2;
	para->hop = 2;

	//--- parameter for validation ---//
	//para->nfft = 512;
	//para->hop = 256;
	//-------------------------------//

	para->wlen = para->nfft;
	
	// Parallel parameter
	ParaParallel* para_par = new ParaParallel;
	para_par->nthread = 4;
	
	// Input file name and corresponding sampling frequency
	const char * fname = "../data/test_sig_chirp.txt";
	para->fs = 5000;

	// Read in signals
	Signal* data_in = new Signal;
	data_in->ReadInSignal(fname);

	// Set up STFT class
	STFT* SigTFD = new STFT(data_in, para, para_par);
	SigTFD->ZeroTfd();
	
	//----------------------------------------------------------------------------//
	// Sequential Algorithm and plotting

	double tstart_seq, tend_seq, elapsed_seq;

	// STFT
	tstart_seq = omp_get_wtime();
	SigTFD->Sequential(data_in);
	tend_seq = omp_get_wtime(); 
	elapsed_seq = tend_seq-tstart_seq;
	printf("Sequential Algorithm Calculation Takes %6.3f seconds\n", elapsed_seq);
	// Plotting
	tstart_seq = omp_get_wtime();
	SigTFD->PrintTfd();
	tend_seq = omp_get_wtime(); 
	elapsed_seq = tend_seq-tstart_seq;
	printf("Sequential Algorithm Plotting Takes %6.3f seconds\n", elapsed_seq);

	// Clear TFD in the class
	SigTFD->ZeroTfd();

	//----------------------------------------------------------------------------//
	// Parallel Algorithm and plotting (openMP)
	double tstart_par, tend_par, elapsed_par;

	tstart_par = omp_get_wtime();
	SigTFD->Parallel(data_in);
	tend_par = omp_get_wtime(); 
	elapsed_par = tend_par-tstart_par;
	printf("Parallel Algorithm Calculation Takes %6.3f seconds\n", elapsed_par);

	// Plotting
	tstart_par = omp_get_wtime();
	SigTFD->PrintTfdPar();
	tend_par = omp_get_wtime(); 
	elapsed_par = tend_par-tstart_par;
	printf("Parallel Algorithm Plotting Takes %6.3f seconds\n", elapsed_par);
	
	return 0;
}

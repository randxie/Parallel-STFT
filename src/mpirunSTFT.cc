#include <iostream>
#include "mpi.h"
#include "Signal.cc"
#include "STFT.cc"
using namespace std;

int main(int argc, char ** argv) 
{

	int rank, size;

	// Parameter for STFT
	ParaSTFT* para = new ParaSTFT;
	para->nfft = 1024*2;
	para->hop = 2;
	para->wlen = para->nfft;
	
	// Parallel parameter
	ParaParallel* para_par = new ParaParallel;
	para_par->nthread = 2;
	
	// Input file name and corresponding sampling frequency
	const char * fname = "../data/test_sig_chirp.txt";
	para->fs = 5000;

	//------------ MPI session ---------------------
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	
	Signal* data_in = new Signal();
	data_in->ReadInSignal(fname);
	
	// divide MPI jobs
	int idx_s, idx_e;
	int max_idx = floor((data_in->GetLength()-para->wlen)/para->hop);
	int seg_len = max_idx/size;

	// determine which node calculate corresponding time frequency distribution
	if(rank==size){
		idx_s = rank*seg_len;
		idx_e = max_idx;
	}
	else
	{
		idx_s = rank*seg_len;
		idx_e = (rank+1)*seg_len;
	}

	// Following http://scicomp.stackexchange.com/questions/1573/is-there-an-mpi-all-gather-operation-for-matrices
	// gather tfd
	int ncols, start;
    int *allncols, *allstarts;
    MPI_Datatype columnunsized, column;

    allncols = new int[rank];
    allstarts= new int[rank];

    /* create the data type for a column of data */
    int sizes[2]    = {para->nfft, max_idx};
    int subsizes[2] = {para->nfft, 1};
    int starts[2]   = {0,0};
    MPI_Type_create_subarray (2, sizes, subsizes, starts, MPI_ORDER_C, MPI_FLOAT, &columnunsized);
    MPI_Type_create_resized (columnunsized, 0, sizeof(float), &column);
    MPI_Type_commit(&column);

	// perform STFT calculation
	double tstart_par, tend_par, elapsed_par;
	double total_elapse = 0;
	float total_max_energy = 0;

	tstart_par = omp_get_wtime();

	STFT* SigTFD = new STFT(data_in, para, para_par);
	SigTFD->ParallelMPI(data_in, idx_s, idx_e);
	
	tend_par = omp_get_wtime(); 
	elapsed_par = tend_par-tstart_par;
	

	MPI_Reduce(&SigTFD->max_energy, &total_max_energy, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce(&elapsed_par, &total_elapse, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Allgather(&(SigTFD->tfd[0][idx_s]), idx_e-idx_s, column, &(SigTFD->tfd[0][idx_s]), idx_e-idx_s,column, MPI_COMM_WORLD);

	if(rank==0){
		printf("Parallel Algorithm Calculation (MPI) Takes %6.3f seconds on average\n", total_elapse/size);
		SigTFD->PrintTfdPar();
		printf("reduced maximum energy: %f\n", total_max_energy);
	}

	MPI_Finalize();

	return 0;
}

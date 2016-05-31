// time frequency analysis --- short time fourier transform
#ifndef STFT_H
#define STFT_H

#include <vector>
#include "Signal.h"
#include "./kissfft/kiss_fft.h"

// struct for storing FFT parameters
struct ParaSTFT{
	int nfft;		   // number of fft points(>=wlen)
	float fs;		   // sampling frequency
	int wlen;		   // window length
	int hop;		      // hop size (<=wlen)
	ParaSTFT(){nfft=1024; fs=1000; wlen=2048; hop=512;};
};

// struct for storing parallelization parameters
struct ParaParallel{
   int nthread;      // number of thread in openMP
   ParaParallel(){nthread = 4;};
};

class STFT
{
public:
	STFT(Signal*,ParaSTFT*,ParaParallel*);
	~STFT();
	void InitTfd();                                 // initialize vector<vector<float> > tfd
   void ZeroTfd();                                 // make tfd element being 0
   //-------------------------------------------------------------------------------//
   void Sequential(Signal*);                       // function for sequential calculation
	void GenLocalTfd(int, int, float*);             // generate tfd for a segment of signal   
	void PrintTfd();
   //-------------------------------------------------------------------------------//
   void Parallel(Signal*);                         // function for parallel calculation
   void GenLocalTfdPar(int, int, float*);          // generate tfd for a segment of signal
   void PrintTfdPar();
   //-------------------------------------------------------------------------------//
   void ParallelMPI(Signal* MySignal, int idx_s, int idx_e);   // function for parallel calculation MPI
   float max_energy;                               // maximum energy in the tfd
   vector<vector<float> > tfd;                     // time frequency distribution (tfd)

private:
	ParaSTFT* para; 		                           // parameter for STFT
   ParaParallel* para_par;                         // parameter for parallelization
	int time_length;	                              // time_length
};

//---------------- From Stackoverflow-------------------------------//
// http://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
// Transform gray scale image to RGB scale, credit to stackoverflow

typedef struct {
    double r,g,b;
} COLOUR;

// Input gray scale and give minimum and maximum
COLOUR GetColour(double v,double vmin,double vmax)
{
   COLOUR c = {1.0,1.0,1.0}; // white
   double dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25 * dv)) {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
   } else if (v < (vmin + 0.5 * dv)) {
      c.r = 0;
      c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
   } else if (v < (vmin + 0.75 * dv)) {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv;
      c.b = 0;
   } else {
      c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      c.b = 0;
   }

   return(c);
}
//-------------------------------------------------------------------------//
#endif
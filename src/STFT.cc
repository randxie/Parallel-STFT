// time frequency analysis --- short time fourier transform
#include <complex>
#include <vector>
#include <iostream>
#include <cmath>
#include <omp.h>
#include "mpi.h" 
#include "STFT.h"

// need kissFFT and libpng
#include "./kissfft/kiss_fft.c"
#include <png++/png.hpp>

using namespace std;

// load STFT parameters and initialize TFD
STFT::STFT(Signal* cal_sig,ParaSTFT* cal_para, ParaParallel* sys_para)
{
	time_length = cal_sig->GetLength();
	para = cal_para;
    para_par = sys_para;
    max_energy = 0.0;
	InitTfd();
}

STFT::~STFT()
{
    for (int i=0;i<tfd.size();i++)
    {
        tfd[i].clear();
    }
    tfd.clear();
}

// initialize time frequency matrix
void STFT::InitTfd()
{
    printf("Initializing TFD matrix\n");
    vector< float > ltfd (para->nfft,0);
    // only store certain time
    int num_pts = floor((time_length-para->wlen)/para->hop);
    tfd.resize(num_pts, ltfd);
    printf("TFD size: (%lu,%lu)\n", tfd.size(), tfd[0].size());
}

void STFT::ZeroTfd()
{
    for (int i=0;i<tfd.size();i++)
    {
        for (int j=0;j<tfd[0].size();j++){
            tfd[i][j] = 0.0;
        }
    }
    max_energy = 0.0;
}

//----------------------- Sequential Algorithm -----------------------------//
// generate TFD sequentially
void STFT::Sequential(Signal* MySignal)
{
    printf("\nSequential Calculation of STFT\n");

    // start from signal head
    int cur_pos = 0;
    // time index
    int tfd_idx = 0;

    float* sig_adr = MySignal->GetSigPtr();
    // process signal at a window length
    while (tfd_idx<tfd.size())
    {
        cur_pos = tfd_idx*para->hop;
        GenLocalTfd(tfd_idx, cur_pos, sig_adr);
        tfd_idx = tfd_idx + 1;
    }
}

// generate local time frequency distribution
void STFT::GenLocalTfd(int tfd_idx, int sig_pos, float* sig_adr)
{
    // initialize forward FFT object
    kiss_fft_cfg fwd = kiss_fft_alloc(para->nfft,0,NULL,NULL);

    vector< std::complex<float> > hann_sig(para->wlen, 0.0);

    // initialize local spectrum
    vector< std::complex<float> > ins_spectrum(para->nfft, 0.0);
    
    // Apply Hanning Window to Signal for improving frequency resolution
    if ((sig_pos+para->wlen)<time_length){
        // energy correction factor
        float corr_fact = 1.633;
        // hanning window formula: 0.5*(1-cos(2*pi*n/N)), N=L-1
        int N = para->wlen - 1;
        // apply hanning window
        for (int i=0; i<para->wlen; i++)
        {
            hann_sig[i] = std::complex<float>(corr_fact*0.5*(1-cos(2*3.1415926*i/N))*sig_adr[sig_pos+i],0.0);
            //hann_sig[i].i = 0;
        }
    }

    kiss_fft(fwd,(kiss_fft_cpx*)&hann_sig[0],(kiss_fft_cpx*)& ins_spectrum[0]);

    // get energy distribution
    for (int k=0;k<para->nfft;k++) {
        // Calculate Energy (1/N for Fourier Transform)
        tfd[tfd_idx][k] = (ins_spectrum[k]*conj(ins_spectrum[k])).real() / float(para->nfft);  
        if (tfd[tfd_idx][k] > max_energy)
        {
            max_energy = tfd[tfd_idx][k];
        }
    }

    kiss_fft_free(fwd);
    hann_sig.clear();
    ins_spectrum.clear();
    kiss_fft_cleanup();
}

// transform time frequency distribution into image for visualization purpose
void STFT::PrintTfd(){
    printf("Generating image\n");
    // x axis is time; y axis is frequency
    size_t img_width =tfd[0].size(); 
    size_t img_height = tfd.size();
    printf("height: %zu, width: %zu\n", img_height, img_width);
    printf("max energy: %f\n", max_energy);
    // choose gray scale image or colored image
    png::image< png::rgb_pixel > image(img_height, img_width);

    for (size_t y = 0; y < image.get_height(); ++y)
    {
       for (size_t x = 0; x < image.get_width(); ++x)
       {
            if(max_energy>0){
               double tmp = ((tfd[x][y])/(max_energy));
               COLOUR tmp_color = GetColour(tmp,0,1);
               image[y][x] = png::rgb_pixel(255*tmp_color.r, 255*tmp_color.g, 255*tmp_color.b);
            }
       }
    }
    image.write("tfd_seq.png");
}

//----------------------- Parallel Algorithm (openMP) -----------------------------//
void STFT::Parallel(Signal* MySignal)
{
    printf("\nParallel Calculation of STFT\n");
    // start from signal head
    int tfd_idx = 0;
    int cur_pos = 0;
    float* sig_adr = MySignal->GetSigPtr();
    // process signal at a window length in parallel
    #pragma omp parallel for schedule(guided,para->wlen) num_threads(para_par->nthread) 
        for (tfd_idx=0; tfd_idx<tfd.size(); tfd_idx++)
        {
            int cur_pos = tfd_idx*para->hop;
            GenLocalTfdPar(tfd_idx, cur_pos, sig_adr);  
        }
}

// generate local time frequency distribution
void STFT::GenLocalTfdPar(int tfd_idx, int sig_pos, float* sig_adr)
{
    // initialize forward FFT object
    kiss_fft_cfg fwd = kiss_fft_alloc(para->nfft,0,NULL,NULL);

    vector< std::complex<float> > hann_sig(para->wlen, 0.0);

    // initialize local spectrum
    vector< std::complex<float> > ins_spectrum(para->nfft, 0.0);
    
    // Apply Hanning Window to Signal for improving frequency resolution
    if ((sig_pos+para->wlen)<time_length){
        // energy correction factor
        float corr_fact = 1.633;
        // hanning window formula: 0.5*(1-cos(2*pi*n/N)), N=L-1
        int N = para->wlen - 1;
        // apply hanning window
        // this omp parallel harms the speed
        //#pragma omp for schedule(guided)
            for (int i=0; i<para->wlen; i++)
            {
                hann_sig[i] = std::complex<float>(corr_fact*0.5*(1-cos(2*3.1415926*i/N))*sig_adr[sig_pos+i],0.0);
            }
    }

    kiss_fft(fwd,(kiss_fft_cpx*)&hann_sig[0],(kiss_fft_cpx*)& ins_spectrum[0]);

    float max_val = 0;
    // get energy distribution
    // this omp parallel harms the speed as well
    //#pragma omp parallel for schedule(guided) reduction (max: max_val) //num_threads(para_par->nthread)
        for (int k=0;k<para->nfft;k++) {
            // Calculate Energy (1/N for Fourier Transform)
            tfd[tfd_idx][k] = (ins_spectrum[k]*conj(ins_spectrum[k])).real() / float(para->nfft);  
            if(tfd[tfd_idx][k]>max_val)
            {
                max_val = tfd[tfd_idx][k];
            }
        }

    // record maximum energy in the distribution
    #pragma omp critical
    {
        if (max_val > max_energy)
        {
            max_energy = max_val;
        }
    }

    // clear garbage
    kiss_fft_free(fwd);
    hann_sig.clear();
    ins_spectrum.clear();
    kiss_fft_cleanup();
}


// transform time frequency distribution into image for visualization purpose
void STFT::PrintTfdPar(){
    printf("Generating image\n");
    // x axis is time; y axis is frequency
    size_t img_width =tfd[0].size(); 
    size_t img_height = tfd.size();
    printf("height: %zu, width: %zu\n", img_height, img_width);
    //printf("max energy: %f\n", max_energy);
    // plot colored image
    png::image< png::rgb_pixel > imagePar(img_height, img_width);
    // combine two for loops into one, for image generation
    #pragma omp parallel for collapse(2) schedule(guided,para_par->nthread) num_threads(para_par->nthread) shared(imagePar)
        for (size_t y = 0; y < imagePar.get_height(); ++y)
        {
           for (size_t x = 0; x < imagePar.get_width(); ++x)
           {
                if(max_energy>0){
                    // normalized intensities
                    double tmp = ((tfd[x][y])/(max_energy));
                    // transform intensity into colormap
                    COLOUR tmp_color = GetColour(tmp,0,1);
                    imagePar[y][x] = png::rgb_pixel(255*tmp_color.r, 255*tmp_color.g, 255*tmp_color.b);
                }
           }
        }
    imagePar.write("tfd_par.png");
}

//----------------------- Parallel Algorithm (MPI) -----------------------------//
void STFT::ParallelMPI(Signal* MySignal, int idx_s, int idx_e)
{
    printf("\nParallel Calculation of STFT with MPI\n");
    // start from signal head
    int tfd_idx = 0;
    int cur_pos = 0;
    float* sig_adr = MySignal->GetSigPtr();
    // process signal at a window length in parallel
    #pragma omp parallel for schedule(guided,para->wlen) num_threads(para_par->nthread) 
        for (tfd_idx=idx_s; tfd_idx<idx_e; tfd_idx++)
        {
            int cur_pos = tfd_idx*para->hop;
            GenLocalTfdPar(tfd_idx, cur_pos, sig_adr);  
        }
}
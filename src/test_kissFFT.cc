// test kissFFT
#include "../lib/kissfft/kiss_fft.c"
#include <complex>
#include <iostream>
#include <vector>
using namespace std;


int main()
{
    const int nfft=1024;
    kiss_fft_cfg fwd = kiss_fft_alloc(nfft,0,NULL,NULL);
    kiss_fft_cfg inv = kiss_fft_alloc(nfft,1,NULL,NULL);

    vector< std::complex<float> > x(nfft, 0.0);
    vector< std::complex<float> > fx(nfft, 0.0);
    x[0] = 10;
    x[1] = 100;
    cout
        << x[0] << ","
        << x[1] << ","
        << x[2] << ","
        << x[3] << " ... " << endl;

    kiss_fft(fwd,(kiss_fft_cpx*)&x[0],(kiss_fft_cpx*)&fx[0]);
    /*
    for (int k=0;k<nfft;++k) {
        fx[k] = fx[k] * conj(fx[k]);
        fx[k] *= 1./nfft;
    }
    */
    kiss_fft(inv,(kiss_fft_cpx*)&fx[0],(kiss_fft_cpx*)&x[0]);
    std::complex<float> N(nfft,0.0);
    for (int i=0;i<nfft;i++){
    	x[i] = x[i]/N;
    }
    cout
        << x[0] << ","
        << x[1] << ","
        << x[2] << ","
        << x[3] << " ... " << endl;
    kiss_fft_free(fwd);
    kiss_fft_free(inv);
    return 0;
}

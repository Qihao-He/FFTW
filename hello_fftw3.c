/*
Created: 2/17/2018
desciption:
hello_fftw3.c with function of auto size and output to .csv
Author:Qihao He
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <fftw3.h>

char Usage[] =
    "Usage: hello_fftw3.bin log2_N [loops]\n"
    "log2_N = log2(FFT_length),       log2_N = 8...22\n"
    "loops  = number of test repeats, loops>0,       default 1\n";

unsigned Microseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1000000 + ts.tv_nsec/1000;
}

int main(){
    fftw_complex *in, *out; //in, out buffer
    fftw_plan p; //fftw_plan prepare
    ...
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    ...
    fftw_execute(p); /* repeat as needed */
    ...
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    return 0;
}

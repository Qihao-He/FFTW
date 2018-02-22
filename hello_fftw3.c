/*
Created: 2/17/2018
desciption:
hello_fftw3.c with function of auto size and output to .csv
Author:Qihao He
*/

#include <stdlib.h>
// #include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <fftw3.h>

char Usage[] =
    "Usage: hello_fftw3.bin log2_N [log2_M [loops [RMS_C]]]\n"
    "log2_N = log2(FFT_length),       log2_N = 8...22\n"
    "log2_M = log2(FFT_length),       log2_M > log2_N\n"
    "loops  = number of test repeats, loops>0,       default 1\n";
    "RMS_C  = number of test repeats, T(1),F(0),     default 0\n";

// not sure about the time profiling for the RPI function
unsigned Microseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1000000 + ts.tv_nsec/1000;
}

int main(int argc, char *argv[]){
  int i, loops, freq, log2_N, log2_M, N, RMS_C, span_log2_N;

    fftw_complex *in, *out; //in, out buffer
    fftw_plan p; //fftw_plan prepare

    log2_N = argc>1? atoi(argv[1]) : 12; // 8 <= log2_N <= 22
    log2_M = argc>2? atoi(argv[2]) : log2_N + 1; // 8 <= log2_N <= 22
    loops  = argc>3? atoi(argv[3]) : 1;  // test repetitions
    RMS_C  = argc>4? atoi(argv[4]) : 1;  // RMS_controller

    if (!(argc>=2 && argc<=5) || loops<1 || !(RMS_C>=0 && RMS_C<=1) ||
    log2_N >= log2_M) {
        printf(Usage);
        return -1;
    }

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(p); /* repeat as needed */

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    return 0;
}

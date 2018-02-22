/*
Created: 2/17/2018
desciption:
hello_fftw3.c with function of auto size and output to .csv
Author:Qihao He
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <fftw3.h>
// #include <unistd.h>

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

    span_log2_N = log2_M - log2_N;
    double time_elapsed[span_log2_N][loops][4]; //3D array
    for(i=0; i<span_log2_N; i++){
        for(j=0; j<loops; j++){
            for(k=0; k<4; k++){
                time_elapsed[i][j][k] = 0;
            }
        }
    }
    double REL_RMS_ERR[span_log2_N][loops]; //2D array
    for(i=0; i<span_log2_N; i++){
        for(j=0; j<loops; j++){
            REL_RMS_ERR[i][j] = 0;
        }
    }

    printf("log2_N,N,Init_T,FFT_T,RMS_T,Total_T\n");

    for (k=0; k<loops; k++) {
        t[0] = Microseconds();
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

        t[1] = Microseconds();
        fftw_execute(p); /* repeat as needed */
        t[2] = Microseconds();

        if(RMS_C == 1){
            tsq[0]=tsq[1]=0;
            for (j=0; j<jobs; j++) {
                base = fft->out + j*fft->step; // output buffer
                freq = j+1;
                for (i=0; i<N; i++) {
                    double re = cos(2*GPU_FFT_PI*freq*i/N);
                    tsq[0] += pow(re, 2);
                    tsq[1] += pow(re - base[i].re, 2) + pow(base[i].im, 2);
                }
                REL_RMS_ERR[l][k] = sqrt(tsq[1] / tsq[0]);
            }
        }
        t[3] = Microseconds();
        printf("%i,%i,%d,%d,%d,%d\n",log2_N + l,N,t[1] - t[0],t[2] - t[1],
        t[3] - t[2],t[3] - t[0]);
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
    return 0;
}

/*
Created: 2/27/2018
desciption:
hello_fftw3.c with function of auto size and output to .csv
Author:Qihao He
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "fftw3.h"

#define REAL 0
#define IMAG 1

char Usage[] =
    "Usage: hello_fftw3.bin log2_N [log2_M [loops [RMS_C]]]\n"
    "log2_N = log2(FFT_length),       log2_N = 8...22\n"
    "log2_M = log2(FFT_length),       log2_M > log2_N\n"
    "loops  = number of test repeats, loops>0,       default 1\n"
    "RMS_C  = RMS_controller, T(1),F(0),     default 0\n";

fftwf_complex *in, *out; //allocate arrays of in, out buffer
fftwf_plan p; //fftwf_plan prepare
double **REL_RMS_ERR;

unsigned Microseconds(void);
void RMS_malloc(int span_log2_N, int loops);
void REL_RMS_ERR_init(int span_log2_N, int loops, double **REL_RMS_ERR);
void input_buffer(fftwf_complex* in, int N);
void output_RMS(fftwf_complex *out, int span_log2_N, double **REL_RMS_ERR,
  int N, int j, int k);
void print_RMS(int span_log2_N, int loops, int log2_N, double **REL_RMS_ERR);

int main(int argc, char *argv[]){
    int i, j, k, l, loops, freq, log2_N, log2_M, log2_P, N, RMS_C, span_log2_N,
    sizeofblock;
    unsigned t[4];

    log2_N = argc>1? atoi(argv[1]) : 12; // 8 <= log2_N <= 22
    log2_M = argc>2? atoi(argv[2]) : log2_N + 1; // 8 <= log2_N <= 22
    loops  = argc>3? atoi(argv[3]) : 1;  // test repetitions
    RMS_C  = argc>4? atoi(argv[4]) : 0;  // RMS_controller

    if (!(argc >= 2 && argc <= 6) || loops < 1 || !(RMS_C >= 0 && RMS_C <= 1)
    || !(log2_N >= 8 && log2_N <= 11 && log2_M <= 12)){
        printf(Usage);
        return -1;
    }

    span_log2_N = log2_M - log2_N;
    RMS_malloc(span_log2_N, loops);
    // initializing 2D, 3D array to 0
    REL_RMS_ERR_init(span_log2_N, loops, (double **)REL_RMS_ERR);
    // print out lables for .csv file
    printf("log2_N,Init_T,FFT_T,RMS_T,Total_T\n");

    for(l = 0; l < span_log2_N; l++){
        log2_P = log2_N + l;
        N = 1 << log2_P; // initializing FFT length: N
        sizeofblock = N * N;
        in = (fftwf_complex *) fftwf_malloc(sizeofblock * sizeof(fftwf_complex));
        out = (fftwf_complex *) fftwf_malloc(sizeofblock * sizeof(fftwf_complex));

        p = fftwf_plan_dft_2d(N, N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);

        for (k = 0; k < loops; k++) {
            t[0] = Microseconds();
            input_buffer(in, sizeofblock);

            usleep(1);
            t[1] = Microseconds();
            fftwf_execute(p); /* repeat as needed */
            t[2] = Microseconds();

            if(RMS_C == 1) output_RMS(out, span_log2_N, REL_RMS_ERR,
              sizeofblock, l, k);
            t[3] = Microseconds();
            printf("%i,%d,%d,%d,%d\n",log2_P,t[1] - t[0],t[2] - t[1],
            t[3] - t[2],t[3] - t[0]); //print for .csv file
        }
        fftwf_destroy_plan(p);
        fftwf_free(in);
        fftwf_free(out);
    }
    if(RMS_C == 1) print_RMS(span_log2_N, loops, log2_N, REL_RMS_ERR);
    return 0;
}

unsigned Microseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

void RMS_malloc(int span_log2_N, int loops){
    int i;
    REL_RMS_ERR = (double **)malloc(span_log2_N * sizeof(double *));
    if(REL_RMS_ERR == NULL){
        printf("Malloc failed\n");
        exit(-1);
    }
    for (i = 0; i < span_log2_N; i++){
        REL_RMS_ERR[i] = (double *)malloc(loops * sizeof(double));
        if(REL_RMS_ERR[i] == NULL){
           printf("Malloc failed on loop %d",i);
           exit(-1);
        }
    }
}

void REL_RMS_ERR_init(int span_log2_N, int loops, double **REL_RMS_ERR){
    int i, j;
    for(i = 0; i < span_log2_N; i++){
        for(j = 0; j < loops; j++){
            REL_RMS_ERR[i][j] = 0;
        }
    }
}
// input buffer
void input_buffer(fftwf_complex *in, int sizeofblock){
    int i;
    for (i = 0; i < sizeofblock; i++) in[i][REAL] = in[i][IMAG] = 0;
    in[0][REAL] = 1;
}
// output REL_RMS_ERR
void output_RMS(fftwf_complex *out, int span_log2_N, double **REL_RMS_ERR,
  int sizeofblock, int j, int k){
    int i;
    double tsq[2], a;
    tsq[0] = tsq[1] = 0;
    tsq[0] = sizeofblock;
    for (i = 0; i < sizeofblock; i++){
        tsq[1] += pow(1 - out[i][REAL], 2) + pow(out[i][IMAG], 2);
    }
    REL_RMS_ERR[j][k] = sqrt(tsq[1] / tsq[0]);
}
// print out REL_RMS_ERR
void print_RMS(int span_log2_N, int loops, int log2_N, double **REL_RMS_ERR){
    int i,j;
    for (i = 0; i < span_log2_N; i++) {
      printf("REL_RMS_ERR for log2_N:%d\n", log2_N + i);
        for (j = 0; j < loops; j++) {
          printf("%.10e,", REL_RMS_ERR[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

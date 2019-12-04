#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ERROR 1.e-20    // Acceptable precision
#define MAX_VAL 5       // Random values are [0, MAX_VAL]

// Matrix and vector sizes
#define N 1000

// Reference computation kernel (do not touch)
void matmat_reference(double C[N][N], double A[N][N], double B[N][N]) {
    size_t i, j, k;

    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            C[i][j] = 0.;
            for(k = 0; k < N; k++) { C[i][j] += A[i][k] * B[k][j]; }
        }
    }
}

// Computation kernel (to parallelize)
void matmat_kernel(double C[N][N], double A[N][N], double B[N][N]) {
    size_t i, j, k;
    double sum;

    #pragma omp parallel for collapse(2)
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++){
            sum = 0;
            #pragma omp parallel for reduction(+:sum)
            for(k = 0; k < N; k++){
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

int main() {
    double* A   = malloc(N * N * sizeof(double));
    double* B   = malloc(N * N * sizeof(double));
    double* C   = malloc(N * N * sizeof(double));
    double* ref = malloc(N * N * sizeof(double));
    double  time_reference, time_kernel;

    // Initialization by random values
    srand((unsigned int) time(NULL));
    for(size_t i = 0; i < N * N; i++) {
        A[i] = (double) rand() / (double) (RAND_MAX / MAX_VAL);
        B[i] = (double) rand() / (double) (RAND_MAX / MAX_VAL);
    }
    time_reference = omp_get_wtime();
    matmat_reference((double(*)[N]) ref, (double(*)[N]) A, (double(*)[N]) B);
    time_reference = omp_get_wtime() - time_reference;
    printf("Reference time : %3.5lf s\n", time_reference);

    time_kernel = omp_get_wtime();
    matmat_kernel((double(*)[N]) C, (double(*)[N]) A, (double(*)[N]) B);
    time_kernel = omp_get_wtime() - time_kernel;
    printf("Kernel time    : %3.5lf s\n", time_kernel);

    printf("Speedup        : %3.5lf\n", time_reference / time_kernel);


    // Check if the result differs from the reference
    for(size_t i = 0; i < N * N; i++) {
        if(ref[i] != C[i]) {
            printf("Bad results :-(((\n");
            exit(1);
        }
    }
    printf("OK results :-)\n");

    free(A);
    free(B);
    free(C);
    free(ref);
    return 0;
}
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_VAL 5    // Random values are [0, MAX_VAL]

double f(double x, double y, unsigned int time) {
    sleep(time);
    return x + 2 * y;
}

// Reference computation kernel (do not touch)
void dag_reference(double r1, double r2, double r3, double* r) {
    double d1, d2, d3, d4, d5, d6;

    d1 = f(r1, r2, 1);
    d2 = f(r2, r3, 1);
    d3 = f(d1, d2, 1);
    d4 = f(r1, r3, 2);
    d5 = f(r2, d2, 1);
    d6 = f(d5, d4 + d3, 1);

    *r = d6;
}

// Computation kernel (to parallelize)
void dag_kernel(double r1, double r2, double r3, double* r) {
    double d1, d2, d3, d4, d5, d6;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            #pragma omp task
            d1 = f(r1, r2, 1);

            #pragma omp task
            d2 = f(r2, r3, 1);
            
            #pragma omp taskwait

            #pragma omp task
            d3 = f(d1, d2, 1);

            #pragma omp task
            d5 = f(r2, d2, 1);
        }
        
        #pragma omp section
        d4 = f(r1, r3, 2);

    }
    
    d6 = f(d5, d4 + d3, 1);

    *r = d6;
}

int main() {
    double val_ref, val_ker;
    double time_reference, time_kernel;
    // Initialization by random values
    srand((unsigned int) time(NULL));
    double val1 = (double) rand() / (double) (RAND_MAX / MAX_VAL);
    double val2 = (double) rand() / (double) (RAND_MAX / MAX_VAL);
    double val3 = (double) rand() / (double) (RAND_MAX / MAX_VAL);

    time_reference = omp_get_wtime();
    dag_reference(val1, val2, val3, &val_ref);
    time_reference = omp_get_wtime() - time_reference;
    printf("Reference time : %3.5lf s\n", time_reference);

    time_kernel = omp_get_wtime();
    dag_kernel(val1, val2, val3, &val_ker);
    time_kernel = omp_get_wtime() - time_kernel;
    printf("Kernel time    : %3.5lf s\n", time_kernel);

    printf("Speedup        : %3.5lf\n", time_reference / time_kernel);

    // Check if the result differs from the reference
    if(val_ref != val_ker) {
        printf("Bad results :-(((\n");
        exit(1);
    }
    printf("OK results :-)\n");

    return 0;
}
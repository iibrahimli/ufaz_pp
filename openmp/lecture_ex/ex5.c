#include <stdio.h>
#include <omp.h>

#define N 1000000

int a[N];

int main(){
    // initialize the array
    for(int i=0; i<N; ++i){
        a[i] = 0.2 * i;
    }

    int sum = 0;
    double seq_time, par_time;

    double t1 = omp_get_wtime();
    // sequential implementation
    for(int i=0; i<N; ++i){
        sum += a[i];
    }
    seq_time = omp_get_wtime() - t1;

    printf("sequential sum:   %d\n", sum);
    printf("sequential time:  %lf\n", par_time);

    sum = 0;

    t1 = omp_get_wtime();
    // parallel implementation
    #pragma omp parallel for reduction(+:sum)
    for(int i=0; i<N; ++i){
        sum += a[i];
    }
    par_time = omp_get_wtime() - t1;

    printf("parallel sum:     %d\n", sum);
    printf("parallel time:    %lf\n", par_time);

    return 0;
}
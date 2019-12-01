#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 64


// compare function: compare two integer values (pointed by parameters
// p1 and p2), in a way compatible with C's library qsort function
int compare(const void* p1, const void* p2) {
    int v1 = *(int*) p1;
    int v2 = *(int*) p2;

    if(v1 < v2)
        return -1;
    else if(v2 < v1)
        return 1;
    else
        return 0;
}


// Return 1 if a number "n" is a power of two, 0 otherwise.
int is_power_of_two(int n) {
    if(n == 0) return 0;
    while(n != 1) {
        if(n % 2 != 0) return 0;
        n = n / 2;
    }
    return 1;
}


void _merge(int *arr, int l, int m, int r){
    int temp_size = r - l + 1;
    int *temp = malloc(temp_size * sizeof *temp);
    int li = l, ri = m+1, ti = 0;

    while(li <= m && ri <= r){
        if(arr[li] <= arr[ri])
            temp[ti++] = arr[li++];
        else
            temp[ti++] = arr[ri++];
    }

    while(li <= m)
        temp[ti++] = arr[li++];

    while(ri <= r)
        temp[ti++] = arr[ri++];

    for(int i = 0; i < temp_size; ++i)
        arr[l+i] = temp[i];
}


void _merge_sort(int *arr, int l, int r){
    if(l >= r) return;

    int m = l + (r-l)/2;

    _merge_sort(arr, l, m);
    _merge_sort(arr, m + 1, r);

    _merge(arr, l, m, r);
}


void merge_sort(int *arr, int size){
    _merge_sort(arr, 0, size-1);
}


int array_init[N], merged_init[N];


int main(int argc, char *argv[]){

    int*   array  = array_init;
    int*   merged = merged_init;
    double t1 = 0., t2 = 0.;
    int    rank, size;

    // initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // check whether size is a power of 2 which divides N
    if(!is_power_of_two(size) || N % size != 0) {
        if(rank == 0) {
            fprintf(stderr, "[!] error: size must divide N and be a power of 2\n");
        }
        MPI_Finalize();
        return 1;
    }

    // process 0 initializes the arrays
    if(rank == 0) {
        srand(time(0) + getpid());
        for(int i = 0; i < N; i++) { array[i] = rand() % N; }
        check_init(array);
        t1 = MPI_Wtime();
    }

    return 0;
}
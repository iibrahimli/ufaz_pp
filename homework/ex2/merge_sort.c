#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 65536


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


// return 1 if a number "n" is a power of two, 0 otherwise.
int is_power_of_two(int n) {
    if(n == 0) return 0;
    while(n != 1) {
        if(n % 2 != 0) return 0;
        n = n / 2;
    }
    return 1;
}


// return 1 if arr is sorted, 0 otherwise
int is_sorted(int *arr, int size){
    for(int i = 1; i < size; ++i){
        if(arr[i] < arr[i-1])
            return 0;
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


// merge function: merge two sorted arrays (array1 of size1 and
// array2 of size2) merged sorted array of size1 + size2
void merge(int* merged, int* array1, int size1, int* array2, int size2){
    int i1 = 0, i2 = 0, ir = 0;

    while(i1 < size1 && i2 < size2) {
        if(array2[i2] < array1[i1])
            merged[ir++] = array2[i2++];
        else
            merged[ir++] = array1[i1++];
    }

    while(i1 < size1) merged[ir++] = array1[i1++];

    while(i2 < size2) merged[ir++] = array2[i2++];
}


// reserve space for arrays
int *array_init[N], *merged_init[N];


int main(int argc, char *argv[]){

    int*   array = array_init;    // big array
    int*   merged = merged_init;  // array that holds the result for each process
    double t1 = 0., t2 = 0.;
    int    rank, size;

    // initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // check whether size is a power of 2 which divides N
    if(!is_power_of_two(size) || N % size != 0) {
        if(rank == 0)
            fprintf(stderr, "[!] error: size must divide N and be a power of 2\n");
        MPI_Finalize();
        return 1;
    }

    // process 0 initializes the arrays
    if(rank == 0){
        // srand(time(0));
        for(int i = 0; i < N; i++)
            array[i] = N - i;
            // array[i] = rand() % N;
        t1 = MPI_Wtime();
    }

    // distribute parts of the array to processes
    MPI_Scatter(array, N / size, MPI_INT, array, N / size, MPI_INT, 0, MPI_COMM_WORLD);

    // each process sorts the array using quicksort
    qsort(array, N / size, sizeof(int), compare);

    // merge
    int step = 1;
    while(step < size){
        if(rank % (2 * step) == step){
            MPI_Ssend(array, step * N / size, MPI_INT, rank - step, 0, MPI_COMM_WORLD);
        }
        else if(rank % (2 * step) == 0){
            MPI_Recv(array + N / 2, step * N / size, MPI_INT, rank + step, 0,
                     MPI_COMM_WORLD, NULL);
            merge(merged, array, step * N / size, array + N / 2,
                  step * N / size);
            
            // swap array and merged
            int* tmp;
            tmp    = merged;
            merged = array;
            array  = tmp;
        }
        step *= 2;
    }

    merged = array;
    // merged is now sorted

    // check the output array
    if(rank == 0){
        t2 = MPI_Wtime();
        if(!is_sorted(merged, N)){
            printf("[!] error: the output array is not sorted\n");
        }
        printf("processing time: %lf s\n", t2 - t1);
        // printf("sorted array: ");
        // for(int i = 0; i < N; ++i)
            // printf("%d ", merged[i]);
        // printf("\n");
    }

    MPI_Finalize();
    return 0;
}
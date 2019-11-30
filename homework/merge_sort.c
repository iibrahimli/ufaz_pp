#include <stdlib.h>
#include <stdio.h>


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


int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Pass an array as cmd arg\n");
        return 1;
    }

    int arr_size = argc - 1;
    int *array = malloc(arr_size * sizeof *array);

    printf("input array:  ");
    for(int i = 1; i < argc; ++i){
        array[i - 1] = atoi(argv[i]);
        printf("%3d ", array[i - 1]);
    }
    printf("\n");

    merge_sort(array, arr_size);

    printf("sorted array: ");
    for(int i = 0; i < arr_size; ++i){
        printf("%3d ", array[i]);
    }
    printf("\n");

    return 0;
}
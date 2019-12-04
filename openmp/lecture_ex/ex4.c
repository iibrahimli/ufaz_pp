#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {    
    int val = 42;
    
    #pragma omp parallel private(val)
    {
        val = rand();
        sleep(1);
        printf("My val : %d\n", val);
    }

    return 0;
}
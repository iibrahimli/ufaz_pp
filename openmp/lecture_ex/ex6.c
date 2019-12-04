#include <stdio.h>

int main() {
    int fin = 0;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            while(!fin){
                #pragma omp critical
                {
                    if(!fin)
                        printf("Not Done\n");
                }
            }
        }
        #pragma omp section
        {
            #pragma omp critical
            {
                fin = 1;
                printf("Done\n");
            }
        }
    }
    return 0;
}
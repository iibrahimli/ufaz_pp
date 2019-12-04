#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PRIME_MIN 3
#define PRIME_MAX 100000



// Reference computation kernel (do not touch)
void prime_reference(size_t primes[], size_t* ptr_nb_primes) {
    size_t nb_primes = 0;
    size_t divisor;
    bool   is_prime;

    for(size_t i = PRIME_MIN; i < PRIME_MAX; i += 2) {
        is_prime = true;
        divisor  = PRIME_MIN;

        while((divisor < i) && is_prime) {
            if((i % divisor) == 0) is_prime = false;
            divisor += 2;
        }

        if(is_prime) {
            primes[nb_primes] = i;
            nb_primes++;
        }
    }

    *ptr_nb_primes = nb_primes;
}

// Computation kernel (to parallelize)
void prime_kernel(size_t primes[], size_t* ptr_nb_primes){
    size_t nb_primes = 0;
    size_t divisor;
    bool   is_prime;

    #pragma omp parallel for private(divisor, is_prime) ordered schedule(dynamic)
    for(size_t i = PRIME_MIN; i < PRIME_MAX; i += 2){
        is_prime = true;
        divisor  = PRIME_MIN;

        while((divisor < i) && is_prime) {
            if((i % divisor) == 0)
                is_prime = false;
            divisor += 2;
        }

        #pragma omp ordered
        #pragma omp critical
        if(is_prime) {
            primes[nb_primes] = i;
            nb_primes++;
        }
    }

    *ptr_nb_primes = nb_primes;
}

void print_sample(size_t tab[], size_t size, size_t sample_length) {
    if(size <= 2 * sample_length) {
        for(size_t i = 0; i < size; i++) printf("%zu ", tab[i]);
    } else {
        for(size_t i = 0; (i < size) && (i < sample_length); i++)
            printf("%zu ", tab[i]);
        printf("... ");
        for(size_t i = size - sample_length; i < size; i++)
            printf("%zu ", tab[i]);
    }
    printf("\n");
}

int main() {
    size_t* primes_ref = malloc(PRIME_MAX / 2 * sizeof(size_t));
    size_t* primes     = malloc(PRIME_MAX / 2 * sizeof(size_t));
    size_t  nb_primes_ref;
    size_t  nb_primes;
    double  time_reference, time_kernel;

    time_reference = omp_get_wtime();
    prime_reference(primes_ref, &nb_primes_ref);
    time_reference = omp_get_wtime() - time_reference;
    printf("Reference time : %3.5lf s\n", time_reference);

    time_kernel = omp_get_wtime();
    prime_kernel(primes, &nb_primes);
    time_kernel = omp_get_wtime() - time_kernel;
    printf("Kernel time    : %3.5lf s\n", time_kernel);

    printf("Speedup        : %3.5lf\n", time_reference / time_kernel);

    print_sample(primes_ref, nb_primes_ref, 5);
    print_sample(primes, nb_primes, 5);

    // Check if the result differs from the reference
    if(nb_primes_ref != nb_primes) {
        printf("Bad results (wrong number of prime numbers) :-(((\n");
        exit(1);
    }
    for(size_t i = 0; i < nb_primes; i++) {
        if(primes_ref[i] != primes[i]) {
            printf("Bad results (prime numbers do not correspond) :-(((\n");
            exit(1);
        }
    }
    printf("OK results :-)\n");

    free(primes_ref);
    free(primes);
    return 0;
}
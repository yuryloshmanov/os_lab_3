#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#include "vector.h"
#include "polynomial.h"


long threads_count;


struct args {
    vector_t **vector;
    size_t start;
    size_t finish;
};


void *multiply_part_polynomials(void *args) {
    vector_t **vector = ((struct args *)args)->vector;
    size_t start = ((struct args *)args)->start;
    size_t finish = ((struct args *)args)->finish;

    for (size_t i = start; i + 1 < finish ; i++) {
        polynomial_t *tmp = multiply_polynomials((*vector)->data[i], (*vector)->data[i + 1]);
        if (!tmp) {
            fprintf(stderr, "multiply_polynomials error\n");
            return (void *)-1;
        }
        polynomial_destroy((*vector)->data[i]);
        polynomial_destroy((*vector)->data[i + 1]);
        (*vector)->data[i] = NULL;
        (*vector)->data[i + 1] = tmp;
    }

    free(args);
    return NULL;
}


void *multiply_polynomials_args(void *args) {
    vector_t **vector = ((struct args *)args)->vector;
    size_t i = ((struct args *)args)->start;
    size_t j = ((struct args *)args)->finish;

    polynomial_t *tmp = multiply_polynomials((*vector)->data[i], (*vector)->data[j]);
    if (!tmp) {
        fprintf(stderr, "multiply_polynomials error\n");
        return (void *)-1;
    }
    polynomial_destroy((*vector)->data[i]);
    polynomial_destroy((*vector)->data[j]);
    (*vector)->data[i] = NULL;
    (*vector)->data[j] = tmp;

    free(args);
    return NULL;
}


polynomial_t *multiply_polynomials_in_vector(vector_t **vector) {
    size_t polynomials_per_thread = (*vector)->size / (size_t)threads_count;
    if (polynomials_per_thread < 2) {
        polynomials_per_thread = 2;
        threads_count = (long)((*vector)->size / (size_t)polynomials_per_thread);
    }

    printf("Using threads = %ld\n", threads_count);

    pthread_t threads[threads_count - 1];

    // launching threads
    for (long i = 0; i < threads_count - 1; i++) {
        struct args *args = (struct args *)malloc(sizeof(struct args));
        if (!args) {
            fprintf(stderr, "malloc error\n");
            return NULL;
        }
        args->vector = vector;
        args->start = i * polynomials_per_thread;
        args->finish = (i + 1) * polynomials_per_thread;
        if (pthread_create(&threads[i], NULL, multiply_part_polynomials, args) != 0) {
            fprintf(stderr, "pthread_create error\n");
            return NULL;
        }
    }

    // main thread
    {
        struct args *args = (struct args *) malloc(sizeof(struct args));
        if (!args) {
            fprintf(stderr, "malloc error\n");
            return NULL;
        }

        args->vector = vector;
        args->start = (threads_count - 1) * polynomials_per_thread;
        args->finish = (*vector)->size;
        if (multiply_part_polynomials(args) != NULL) {
            fprintf(stderr, "multiply_part_polynomials error\n");
            return NULL;
        }
    }

    // waiting threads
    for (long i = 0; i < threads_count - 1; i++) {
        void *status;
        pthread_join(threads[i], &status);
        if (status != (void *)0) {
            fprintf(stderr, "multiply_part_polynomials error\n");
            return NULL;
        }
    }

    // multithreading multiplication
    while (threads_count > 1) {
        size_t current_threads_count = 0;
        for (size_t i = 0; i < threads_count; i++) {
            int flag = 0;

            size_t j1 = (i + 1) * polynomials_per_thread - 1;
            size_t j2 = (i + 2) * polynomials_per_thread - 1;

            if (j2 > (*vector)->size - 1) {
                j2 = (*vector)->size - 1;
                flag = 1;
            }

            if (i == threads_count - 1) {
                j1 = j2 = (*vector)->size - 1;
                flag = 1;
            }

            if (i == threads_count - 2) {
                j2 = (*vector)->size - 1;
                flag = 1;
            }

            if (j2 + polynomials_per_thread > (*vector)->size - 1) {
                j2 = (*vector)->size - 1;
                flag = 1;
            }

            if (j1 != j2) {
                struct args *args = (struct args *)malloc(sizeof(struct args));

                if (!args) {
                    fprintf(stderr, "malloc error\n");
                    return NULL;
                }

                args->vector = vector;
                args->start = j1;
                args->finish = j2;
                if (pthread_create(&threads[current_threads_count], NULL, multiply_polynomials_args, args) != 0) {
                    fprintf(stderr, "pthread_create error\n");
                    return NULL;
                }
                current_threads_count++;
            }
            if (flag) {
                break;
            }
            i++;
        }

        for (size_t i = 0; i < current_threads_count; i++) {
            void *status;
            pthread_join(threads[i], &status);
            if (status != (void *)0) {
                fprintf(stderr, "multiply_polynomials_args error\n");
                return NULL;
            }
        }

        polynomials_per_thread *= 2;
        if (threads_count % 2 == 1) {
            threads_count++;
        }
        threads_count /= 2;
    }

    // returning result
    polynomial_t *result = (*vector)->data[(*vector)->size - 1];
    (*vector)->data[(*vector)->size - 1] = NULL;
    return result;
}


int main(int argc, char *argv[]) {
    threads_count = sysconf(_SC_NPROCESSORS_ONLN);
    threads_count = (threads_count * threads_count * threads_count) / 2;
    FILE *input = (argc > 2 && strcmp(argv[1], "-f") == 0) ? (fopen(argv[2], "r")) : (stdin);

    if (!input) {
        fprintf(stderr, "Can't open file");
        exit(1);
    }

    if (argc > 4 && strcmp(argv[3], "-t") == 0) {
        long num = strtol(argv[4], NULL, 0);
        if (num < 1) {
            fprintf(stderr, "Invalid threads count");
            exit(3);
        }
        threads_count = threads_count < num ? threads_count : num;
    }

    vector_t *vector = parse_polynomials(input);

    if (!vector) {
        fprintf(stderr, "parse_polynomials error\n");
        exit(4);
    }

    polynomial_t *result = multiply_polynomials_in_vector(&vector);

    if (!result) {
        fprintf(stderr, "multiply_polynomials_in_vector error\n");
        exit(5);
    }

    print_polynomial(&result);

    polynomial_destroy(result);
    vector_destroy(vector);

    fclose(input);
    return 0;
}

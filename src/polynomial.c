#include <stdlib.h>
#include <string.h>

#include "polynomial.h"
#include "vector.h"


#define DATA_SIZE 1024


vector_t *parse_polynomials(FILE *input) {
    vector_t *vector = vector_create();

    if (!vector) {
        return NULL;
    }

    char buff[DATA_SIZE];
    while (1) {
        for (int i = 0; i < DATA_SIZE; i++) {
            buff[i] = '\0';
        }
        if (fscanf(input, "%[^\n]\n", buff) == EOF) {
            break;
        }
        char *start = buff;
        char *end;
        polynomial_t *polynomial = polynomial_create((size_t)(DATA_SIZE / 2));
        if (!polynomial) {
            return NULL;
        }
        while (1) {
            int64_t d = strtoull(start, &end, 0);
            if (polynomial_insert_digit(&polynomial, d) != 0) {
                return NULL;
            }
            if (!*end || strcmp(end, " ") == 0) {
                break;
            }
            char *tmp = end;
            end = start;
            start = tmp;

        }
        vector_push_back(&vector, polynomial);
    }
    return vector;
}

polynomial_t *polynomial_create(size_t size) {
    polynomial_t *polynomial = (polynomial_t *)malloc(sizeof(polynomial_t));

    if (!polynomial) {
        return NULL;
    }

    polynomial->degree = 0;
    polynomial->data = (int64_t *)malloc(sizeof(int64_t) * size);

    if (!polynomial->data) {
        return NULL;
    }

    for (size_t i = 0; i < size; i++) {
        polynomial->data[i] = 0;
    }

    return polynomial;
}

int polynomial_insert_digit(polynomial_t **polynomial, int64_t digit) {
    if ((*polynomial)->degree == DATA_SIZE) {
        return -1;
    }

    (*polynomial)->data[(*polynomial)->degree++] = digit;

    return 0;
}

polynomial_t *multiply_polynomials(polynomial_t *a, polynomial_t *b) {
    int m = a->degree, n = b->degree;
    polynomial_t *result = polynomial_create(m + n - 1);

    if (!result) {
        return NULL;
    }

    result->degree = m + n - 1;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            result->data[i + j] += (a->data[i]) % INT32_MAX * (b->data[j]) % INT32_MAX;
        }
    }

    return result;
}


int print_polynomial(polynomial_t **polynomial) {
    if (polynomial == NULL || (*polynomial) == NULL || (*polynomial)->data == NULL) {
        return -1;
    }

    for (size_t i = 0; i < (*polynomial)->degree; i++) {
        int64_t value = (*polynomial)->data[i];
        printf("%llu ", value);

    }
    printf("\n");

    return 0;
}


void polynomial_destroy(polynomial_t *polynomial) {
    free(polynomial->data);
    free(polynomial);
}

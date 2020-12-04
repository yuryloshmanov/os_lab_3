#ifndef LAB3_POLYNOMIAL_H
#define LAB3_POLYNOMIAL_H


#include <stdio.h>
#include <stddef.h>
#include <stdint.h>


typedef struct vector vector_t;


typedef struct polynomial {
    size_t degree;
    int64_t *data;
} polynomial_t;


vector_t *parse_polynomials(FILE *input);
polynomial_t *polynomial_create(size_t size);
int polynomial_insert_digit(polynomial_t **polynomial, int64_t digit);
polynomial_t *multiply_polynomials(polynomial_t *a, polynomial_t *b);
int print_polynomial(polynomial_t **p);
void polynomial_destroy(polynomial_t *polynomial);


#endif //LAB3_POLYNOMIAL_H

#ifndef LAB3_VECTOR_H
#define LAB3_VECTOR_H


#include "polynomial.h"


typedef polynomial_t* vector_type_t;


typedef struct vector {
    size_t size;
    size_t capacity;
    vector_type_t *data;
} vector_t;


vector_t *vector_create(void);
int vector_push_back(vector_t **vector, vector_type_t value);
int vector_destroy(vector_t *vector);


#endif //LAB3_VECTOR_H

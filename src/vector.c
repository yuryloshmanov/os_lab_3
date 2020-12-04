#include <stdlib.h>


#include "vector.h"


vector_t *vector_create(void) {
    vector_t *vector = (vector_t *)malloc(sizeof(vector_t));
    if (vector == NULL) {
        fprintf(stderr, "malloc error\n");
        return NULL;
    }

    vector->size = 0;
    vector->capacity = 1000;
    vector->data = (vector_type_t *)malloc(sizeof(vector_type_t) * vector->capacity);
    if (!vector->data) {
        fprintf(stderr, "malloc error\n");
        return NULL;
    }

    return vector;
}


int vector_push_back(vector_t **vector, vector_type_t value) {
    if ((*vector)->size == (*vector)->capacity) {
        (*vector)->capacity *= 2;
        (*vector)->data = (vector_type_t *)realloc((*vector)->data, sizeof(vector_type_t) * (*vector)->capacity);
        if (!(*vector)->data) {
            fprintf(stderr, "realloc error\n");
            return -1;
        }
    }
    (*vector)->data[(*vector)->size++] = value;
    return 0;
}


int vector_destroy(vector_t *vector) {
    free(vector->data);
    free(vector);
    return 0;
}

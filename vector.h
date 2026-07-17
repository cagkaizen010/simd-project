#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef struct{
    double *data;
    size_t size;
    size_t capacity;
} Vector;

void vector_init(Vector *v, size_t initial_capacity);
void vector_push_back(Vector *v, double value);
double vector_get(Vector *v, size_t index);
void vector_free(Vector *v);

#endif
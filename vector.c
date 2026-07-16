#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

typedef struct {
    double *data;
    size_t size;
    size_t capacity;
} Vector;

void vector_init(Vector *v, size_t initial_capacity){
    v->data = (double*)malloc(initial_capacity * sizeof(double));
    v->size = 0;
    v->capacity = initial_capacity;
}

void vector_push_back(Vector *v, int value){
    if( v->size >= v-> capacity){
        v->capacity *=2;
        v->data = (double*)realloc(v->data, v->capacity * sizeof(double));
    }

    v->data[v->size] = value;
    v->size++;
}

int vector_get(Vector *v, size_t index) {
    if (index >= v->size) {
        printf("Index out off bounds!\n");
        exit(1);
    }
    return v->data[index];
}

void vector_free(Vector *v){
    free(v->data);
    v->data = NULL;
    v->size = 0;
}
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

void vector_init(Vector *v, size_t initial_capacity){
    if (initial_capacity >0) {

        v->data = (double*)malloc(initial_capacity * sizeof(double));
    }else {
        v->data = NULL;
    }

    v->size = 0;
    v->capacity = initial_capacity;
}

void vector_push_back(Vector *v, double value){
    if( v->size >= v-> capacity){
        v->capacity = (v->capacity ==0) ? 1 : v->capacity *2;

        v->data = (double*)realloc(v->data, v->capacity * sizeof(double));

        if (v->data == NULL) {
            printf("Memory alllocation failed!\n");
            exit(1);
        }
    }


    v->data[v->size] = value;
    v->size++;
}

double vector_get(Vector *v, size_t index) {
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
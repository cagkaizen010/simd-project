#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vector.h"

void kernel(int n, Vector* b) {
    printf("%lf\n", vector_get(b, 1));
}

double get_random_double(double min, double max){
    double scale = (double) rand() / (double) RAND_MAX;
    return min + scale*(max-min);
}

int main() {
    srand(time(NULL));  // So that we can actually get random numbers

    // value is 2^n, where n is the input
    int n= 4;
    double min = 0.0;
    double max= 20.0;
    

    size_t size = (int) exp2(n);
    Vector b;
    vector_init(&b, size);

    for(size_t i = 0; i < size; i++){
        vector_push_back(&b, get_random_double(min, max));
    }

    kernel(size, &b);
    
    vector_free(&b);
    return 0;
}

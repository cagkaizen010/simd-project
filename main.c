#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vector.h"

typedef struct {
    double ESUM;
    double OSUM;
}   KernelResult;

KernelResult kernel(int n, Vector* b) {
    KernelResult result;
    result.ESUM = 0;
    result.OSUM = 0;

    for(size_t i = 0; i < n; i++){
        // printf("%lf\n", vector_get(b, i));
        if(i %2 == 0) result.ESUM += vector_get(b,i);
        else result.OSUM += vector_get(b,i);

    }
    return result;
}

double get_random_double(double min, double max){
    double scale = (double) rand() / (double) RAND_MAX;
    return min + scale*(max-min);
}

int main() {
    srand(time(NULL));  // So that we can actually get random numbers

    // value is 2^n, where n is the input
    int n= 27;
    double min = 0.0;
    double max= 20.0;
    

    size_t size = (int) exp2(n);
    Vector b;
    vector_init(&b, size);

    for(size_t i = 0; i < size; i++){
        vector_push_back(&b, get_random_double(min, max));
    }

    KernelResult result = kernel(size, &b);
    printf("Even Sum: %lf\nOdd Sum: %lf\n", result.ESUM, result.OSUM);
    
    vector_free(&b);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vector.h"

typedef struct {
    double ESUM;
    double OSUM;
} KernelResult;

extern KernelResult compute_sums(size_t n, Vector* b);
extern KernelResult simd_compute_sums(size_t n, Vector* b);
extern KernelResult simd_ymm_compute_sums(size_t n, Vector* b);

KernelResult kernel(int n, Vector* b) {
    KernelResult result;
    result.ESUM = 0;
    result.OSUM = 0;

    for(size_t i = 0; i < n; i++){
        if(i % 2 == 0) result.ESUM += vector_get(b,i);
        else result.OSUM += vector_get(b,i);
    }
    return result;
}

double get_random_double(double min, double max){
    double scale = (double) rand() / (double) RAND_MAX;
    return min + scale*(max-min);
}

int main() {
    srand(time(NULL));  
    struct timespec start, end;

    int n = 27;
    double min = 0.0;
    double max = 20.0;

    size_t size = (size_t) exp2(n);
    Vector b;
    vector_init(&b, size);

    printf("Populating vector with %zu elements...\n", size);
    for(size_t i = 0; i < size; i++){
        vector_push_back(&b, get_random_double(min, max));
    }

    // benchmarking C Kernel 
    timespec_get(&start, TIME_UTC);
    KernelResult c_result = kernel(size, &b);
    timespec_get(&end, TIME_UTC);

    double c_kernel_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_nsec - start.tv_nsec)/1000000.0;

    // benchmarking ASM Kernel ---
    timespec_get(&start, TIME_UTC);
    KernelResult asm_result = compute_sums(size, &b);
    timespec_get(&end, TIME_UTC);

    double asm_kernel_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_nsec - start.tv_nsec)/1000000.0;

    // benchmark ASM Kernel utilizing AVX2 and xmm registers
    timespec_get(&start, TIME_UTC);
    KernelResult simd_asm_result = simd_compute_sums(size, &b);
    timespec_get(&end, TIME_UTC);

    double simd_asm_kernel_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_nsec - start.tv_nsec)/1000000.0;

    // benchmark ASM Kernel utilizing AVX2 and ymm registers
    timespec_get(&start, TIME_UTC);
    KernelResult simd_ymm_asm_result = simd_ymm_compute_sums(size, &b);
    timespec_get(&end, TIME_UTC);

    double simd_ymm_asm_kernel_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_nsec - start.tv_nsec)/1000000.0;

    // Printing results
    printf("\n--- C Kernel Results ---\n");
    printf("Even Sum: %lf\nOdd Sum: %lf\n", c_result.ESUM, c_result.OSUM);
    printf("C Kernel Time: %lf ms\n", c_kernel_time);

    printf("\n--- ASM Kernel Results ---\n");
    printf("Even Sum: %lf\nOdd Sum: %lf\n", asm_result.ESUM, asm_result.OSUM);
    printf("ASM Kernel Time: %lf ms\n", asm_kernel_time);

    printf("\n--- ASM XMMD Kernel Results ---\n");
    printf("Even Sum: %lf\nOdd Sum: %lf\n", simd_asm_result.ESUM, simd_asm_result.OSUM);
    printf("ASM Kernel Time: %lf ms\n", simd_asm_kernel_time);

    printf("\n--- ASM YMMD Kernel Results ---\n");
    printf("Even Sum: %lf\nOdd Sum: %lf\n", simd_ymm_asm_result.ESUM, simd_ymm_asm_result.OSUM);
    printf("ASM Kernel Time: %lf ms\n", simd_ymm_asm_kernel_time);

    vector_free(&b);
    return 0;
}
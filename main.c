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

void test_vector_size(size_t size, const char* label) {
    struct timespec start, end;
    double min = 0.0, max = 20.0;
    int iterations = 30;

    Vector b;
    vector_init(&b, size);

    printf("\n==================================================\n");
    printf("Testing %s (Size: %zu elements)\n", label, size);
    printf("==================================================\n");

    printf("Populating vector...\n");
    for(size_t i = 0; i < size; i++){
        vector_push_back(&b, get_random_double(min, max));
    }

    // --- C Kernel ---
    printf("Running C kernel (30 iterations)...\n");
    double total_c_time = 0;
    KernelResult c_result;
    for(int i = 0; i < iterations; i++) {
        timespec_get(&start, TIME_UTC);
        c_result = kernel(size, &b);
        timespec_get(&end, TIME_UTC);
        total_c_time += (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_nsec - start.tv_nsec)/1000000.0;
    }

    // --- ASM Kernel ---
    printf("Running ASM kernel (30 iterations)...\n");
    double total_asm_time = 0;
    KernelResult asm_result;
    for(int i = 0; i < iterations; i++) {
        timespec_get(&start, TIME_UTC);
        asm_result = compute_sums(size, &b);
        timespec_get(&end, TIME_UTC);
        total_asm_time += (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_nsec - start.tv_nsec)/1000000.0;
    }

    // --- SIMD XMM Kernel ---
    printf("Running SIMD XMM kernel (30 iterations)...\n");
    double total_xmm_time = 0;
    KernelResult xmm_result;
    for(int i = 0; i < iterations; i++) {
        timespec_get(&start, TIME_UTC);
        xmm_result = simd_compute_sums(size, &b);
        timespec_get(&end, TIME_UTC);
        total_xmm_time += (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_nsec - start.tv_nsec)/1000000.0;
    }

    // --- SIMD YMM Kernel ---
    printf("Running SIMD YMM kernel (30 iterations)...\n");
    double total_ymm_time = 0;
    KernelResult ymm_result;
    for(int i = 0; i < iterations; i++) {
        timespec_get(&start, TIME_UTC);
        ymm_result = simd_ymm_compute_sums(size, &b);
        timespec_get(&end, TIME_UTC);
        total_ymm_time += (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_nsec - start.tv_nsec)/1000000.0;
    }
    
    printf("\n--- Average Execution Times (over %d runs) ---\n", iterations);
    printf("C Kernel Time      : %lf ms\n", total_c_time / iterations);
    printf("ASM Kernel Time    : %lf ms\n", total_asm_time / iterations);
    printf("SIMD XMM Time      : %lf ms\n", total_xmm_time / iterations);
    printf("SIMD YMM Time      : %lf ms\n", total_ymm_time / iterations);
    
    printf("\n--- Correctness Check (ESUM | OSUM) ---\n");
    printf("C Kernel   : %lf | %lf\n", c_result.ESUM, c_result.OSUM);
    printf("ASM Kernel : %lf | %lf\n", asm_result.ESUM, asm_result.OSUM);
    printf("SIMD XMM   : %lf | %lf\n", xmm_result.ESUM, xmm_result.OSUM);
    printf("SIMD YMM   : %lf | %lf\n", ymm_result.ESUM, ymm_result.OSUM);

    vector_free(&b);
}

int main() {
    srand(time(NULL));  
    
    size_t size_20 = 1ULL << 20; // 2^20
    size_t size_26 = 1ULL << 26; // 2^26
    size_t size_30 = 1ULL << 30; // 2^30

    // Vector size tests
    test_vector_size(size_20, "2^20");
    test_vector_size(size_26, "2^26");
    test_vector_size(size_30, "2^30");

    // Boundary handling test
    test_vector_size(1003, "Boundary Test (1003 elements)");

    return 0;
}
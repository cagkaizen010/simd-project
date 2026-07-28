# SIMD Project 
## Input 
We are using the example input outlined in the specifications document. </br>
```
for (i = 0; i < n; i++){
    B[i] = i+1;
}
```

## Results 
### Execution time for all cases  </br>
![Execution Time](./images/execution_time.png)

### Correctness check for all cases  </br>
![Correctness Check](./images/correctness_check.png)

### Comparative Table
|Vector Size|C (ms)         |x86-64 (ms)| SIMD XMM (ms) |  SIMD YMM (ms)| 
| --------- | --------      | --------  | --------      | --------      |
| 2^20      | 1.240257      | 0.311618  | 0.305951      | 0.160788      |
| 2^26      | 78.020333     | 19.921683 | 19.779483     | 11.415362     |
| 2^30      | 1230.636197   | 323.475674| 312.385642| 181.412945    |
#### Analysis 
* **C vs. Scalar Assembly:**
    * The C kernel is the slowest execution times across all vector sizes.
    * This bottleneck is primarily caused by the `if(i % 2 == 0)` conditional statement, which introduces significant branching overhead evaluated at every single loop iteration.
    * The baseline x86-64 assembly implementation (`asmfunc1`) provides a massive speedup by eliminating this conditional branch entirely and sequentially unrolling the additions, running **~3.8x faster** (1230.636 ms vs. 323.475 ms) for the 2^30 dataset.
* **Scalar vs. SIMD XMM:**
    * The 128-bit SIMD XMM implementation (`asmfunc2`) processes two double-precision floats simultaneously.
    * The performance gain of SIMD XMM over the Scalar assembly is minimal, executing **~3.9x faster** than the C kernel (312.385 ms for 2^30).
    * This suggests that processing only two elements at a time is bottlenecked by memory bandwidth limits and any vectorized gains are slightly offset by the overhead of unpacking the upper and lower bits at the end of the function.
* **AVX2 Superiority:**
    * The 256-bit SIMD YMM implementation (`asmfunc3`) yields the best performance by processing four elements at once.
    * By doubling the data throughput of the XMM kernel and heavily reducing the total number of loop iterations, it runs **~6.8x faster** than the baseline C kernel (181.412 ms for 2^30) and takes roughly half the time of the XMM kernel for larger datasets.
* **Linear Data Scaling ($O(n)$ Complexity):**
    * When scaling the dataset from 2^26 to 2^30 elements, the data size increases by a factor of 16.
    * Across all kernels, the execution time also predictably scales by a factor of ~15.7x to 16.2x.
    * This confirms that all implementations maintain strict $O(n)$ linear time complexity and that the memory bandwidth scales proportionally without cache thrashing.


### Boundary Check
![Boundary Check](./images/boundary_check.png)

### Discussion
* **Methodology for Specifications:**
    * The objective was to segregate the sum of even indices into ESUM and odd indices into OSUM.
    * Instead of filtering these conditionally inside a loop, our SIMD kernels load sequential data chunks directly into vector registers, accumulating the odds and evens concurrently in alternating lanes.
* **Precise Kernel Timing (Methodology):**
    * To adhere strictly to the project specifications, we utilized the `timespec_get` function to isolate the performance benchmarking.
    * The timer is started immediately before the kernel call and stopped immediately after it returns, ensuring that the overhead from vector initialization, memory allocation (`malloc`/`realloc`), and terminal I/O printing does not skew the average execution times.
* **The Extraction Challenge (Problem & Solution):**
    * A major hurdle was isolating OSUM and ESUM from the vectorized accumulators before returning the final struct to the C program.
    * For the YMM kernel, this required using the `vextractf128` instruction to safely collapse the upper 128 bits into an XMM register, adding the halves together, and finally unpacking the 64-bit doubles into standard return registers.
* **Boundary Handling Strategy:**
    * To satisfy the requirement of handling leftover elements (as proven by the 1003 element boundary check), the SIMD loops use bitwise AND operations (`and r9, -2` and `and r9, -4`) to dynamically find the largest safe multiple to process.
    * Any remaining elements are then processed via a sequential scalar loop at the tail end of the function.
* **AHA Moment- State Penalties:**
    * A key optimization breakthrough was including the `vzeroupper` instruction at the end of the YMM kernel.
    * Explicitly clearing the upper 128 bits prevents a severe CPU performance penalty that occurs when the processor transitions from AVX execution back to legacy SSE state for the subsequent C code.
* **AHA Moment - Floating-Point Non-Associativity:**
    * In the correctness check for $2^{30}$ elements, the SIMD YMM OSUM result (`288230376420147200.000000`) slightly deviates from the baseline C kernel (`288230376285929536.000000`).
    * This is a demonstration that floating-point addition is not perfectly associative.
    * Because the YMM kernel accumulates large floats in parallel lanes (changing the order of additions compared to the C loop's strict sequential summing), the 53-bit mantissa experiences slightly different precision loss.

## Authors
Kaizen Edwin Rodriguez </br>
Frances Danielle Solis

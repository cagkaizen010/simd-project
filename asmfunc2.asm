; Inputs:
;   rdi = n (size of vector)
;   rsi = b (pointer to Vector struct)
; Outputs:
;   xmm0 = ESUM
;   xmm1 = OSUM

global simd_compute_sums

section .text

simd_compute_sums:
    ; extract b->data from the Vector struct, no offset
    mov r8, [rsi]       
    
    ; Initialize our SIMD accumulator to [0.0, 0.0]
    ; xmm2 will hold OSUM and ESUM 
    pxor xmm2, xmm2     
    
    ; Initialize loop counter (rcx = 0)
    xor rcx, rcx        

    ; To safely process 2 elements at a time, we find the largest even 
    ; number <= n. We do this by clearing the lowest bit of n.

    ; Clear the lowest bit of n
    ; Find the largest even number <= n, so that we can safely process two elements at a time. 
    mov r9, rdi
    and r9, -2          

.loop_start:
    ; process all pairs check
    cmp rcx, r9
    jge .tail_check

    ; Load 2 doubles (aka 16 bytes) at once into xmm3. 
    ; xmm3 now holds: [ b[rcx+1], b[rcx] ] -> [ Odd, Even ]
    movupd xmm3, [r8 + rcx*8]

    ; xmm2 = xmm2 + xmm3
    addpd xmm2, xmm3

    ; increment index by 2 because we processed two elements
    add rcx, 2
    jmp .loop_start

.tail_check:
    ; if n was an odd number, there will be exactly 1 element left over
    ; Because we incremented by 2 starting from 0, the leftover index is even.
    cmp rcx, rdi
    jge .done

    ; Load the single remaining double and add it to the lower half (ESUM)
    movsd xmm3, [r8 + rcx*8]
    addsd xmm2, xmm3

.done:

    ; 1. Copy the lower 64 bits (ESUM) into xmm0
    movapd xmm0, xmm2

    ; 2. Extract the upper 64 bits (OSUM) and move them to the lower 64 bits of xmm1
    movhlps xmm1, xmm2

    ret
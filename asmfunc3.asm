; Inputs:
;   rdi = n (size of vector)
;   rsi = b (pointer to Vector struct)
; Outputs:
;   xmm0 = ESUM
;   xmm1 = OSUM

global simd_ymm_compute_sums

section .text


simd_ymm_compute_sums:
    mov r8, [rsi]       
    
    ; initialize YMM accumulator to zero
    vxorpd ymm2, ymm2, ymm2     
    
    ; initialize loop counter (rcx = 0)
    xor rcx, rcx        

    ; to safely process 4 elements at a time, we find the largest multiple 
    ; of 4 that is <= n by clearing the lowest two bits of n
    mov r9, rdi
    and r9, -4          ; r9 = n & ~3

.loop_start:
    cmp rcx, r9
    jge .reduce

    ; Load 4 doubles (aka 32 bytes) at once into ymm3. 
    ; ymm3 now holds: [ b[rcx+3], b[rcx+2], b[rcx+1], b[rcx] ]
    vmovupd ymm3, [r8 + rcx*8]

    vaddpd ymm2, ymm2, ymm3

    ; Increment index by 4 because we processed 4 elements
    add rcx, 4
    jmp .loop_start

.reduce:
    ; collapse 256-bit register into a 128-bit register.
    ; Extract the upper 128 bits of ymm2 into xmm3
    ; xmm3 = [Odd_high, Even_high]
    vextractf128 xmm3, ymm2, 1  
    
    ; Add the upper half (xmm3) to the lower half (xmm2)
    ; Now xmm2 = [Odd_high + Odd_low, Even_high + Even_low] -> [OSUM, ESUM]
    vaddpd xmm2, xmm2, xmm3

    ; Move ESUM (lower 64 bits) into xmm0
    vmovapd xmm0, xmm2
    
    ; Move OSUM (upper 64 bits) into the lower 64 bits of xmm1
    vunpckhpd xmm1, xmm2, xmm2

.tail_loop:
    ; Since we processed by 4s, we might have 0, 1, 2, or 3 elements left over.
    ; We process any remaining elements individually.
    cmp rcx, rdi
    jge .done

    ; check if current index (rcx) is even or odd
    test rcx, 1
    jnz .odd_tail

.even_tail:
    ; add to ESUM (xmm0) using scalar addition
    vaddsd xmm0, xmm0, qword [r8 + rcx*8]
    inc rcx
    jmp .tail_loop

.odd_tail:
    ; add to OSUM (xmm1) using scalar addition
    vaddsd xmm1, xmm1, qword [r8 + rcx*8]
    inc rcx
    jmp .tail_loop

.done:
    ; dani, someone said to do this, i forgot what video it was from >_>
    vzeroupper
    ret
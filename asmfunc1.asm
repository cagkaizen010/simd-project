; Inputs:
;   rdi = n (size of vector)
;   rsi = b (pointer to Vector struct)
; Outputs:
;   xmm0 = ESUM
;   xmm1 = OSUM

global compute_sums

section .text
compute_sums:
    mov r8, [rsi]       
    
    pxor xmm0, xmm0
    pxor xmm1, xmm1
    
    xor rcx, rcx

.loop_start:
    cmp rcx, rdi
    jge .done

    ; Even Index 
    addsd xmm0, qword [r8 + rcx*8]
    inc rcx

    ; if (i >= n) break; (In case n is an odd number)
    cmp rcx, rdi
    jge .done

    ; Odd Index 
    addsd xmm1, qword [r8 + rcx*8]
    inc rcx

    jmp .loop_start

.done:
    ret
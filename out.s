    .global _start
    .section
    .text

_start:
    mov %r8d, a
    mov %r9d, b
    cmp %r8d, %r9d
    sete %r8d
    cmp %r8d, $0
    jz _LBL0

    mov %r8d, a
    mov %r9d, b
    cmp %r8d, %r9d
    sete %r8d
    cmp %r8d, $0
    jz _LBL1

_LOOP0:
    mov %r8d, a
    mov %r9d, b
    cmp %r8d, %r9d
    sete %r8d
    cmp %r8d, $0
    jz _LBL2

    mov %r8d, d
    mov c, %r8d
    jmp _LOOP0

_LBL2: 
    mov %r8d, e
    mov %r9d, f
    sub	%r8d, %r9d
    mov e, %r8d

_LBL1: 
    mov %r8d, n
    mov m, %r8d

_LBL0: 

    mov %ebx, %eax
    mov %eax, $1
    int $0x80

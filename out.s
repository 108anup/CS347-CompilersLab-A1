    mov t0, a
    mov t1, b
    cmp t0, t1
    sete t0
    cmp t0, $0
    jz _LBL0

    mov t0, a
    mov t1, b
    cmp t0, t1
    sete t0
    cmp t0, $0
    jz _LBL1

_LOOP0:
    mov t0, a
    mov t1, b
    cmp t0, t1
    sete t0
    cmp t0, $0
    jz _LBL2

    mov t0, d
    mov c, t0
    jmp _LOOP0

_LBL2: 
    mov t0, e
    mov t1, f
    sub	t0, t1
    mov e, t0

_LBL1: 
    mov t0, n
    mov m, t0

_LBL0: 

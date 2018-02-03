_LOOP0:
    mov t0, a
    mov t1, b
    cmp t0, t1
    setg t0
    cmp t0, $0
    jz _LBL0

    mov t0, e
    mov d, t0
    mov t0, a
    mov t1, b
    cmp t0, t1
    sete t0
    cmp t0, $0
    jz _LBL1

    mov t0, e
    mov d, t0

_LBL1: 
    mov t0, e
    mov f, t0
    jmp _LOOP0

_LBL0: 
    mov t0, a
    mov t1, b
    cmp t0, t1
    setl t0
    cmp t0, $0
    jz _LBL2

    mov t0, e
    mov d, t0

_LBL2: 
    mov t0, g
    mov t1, m
    cmp t0, t1
    sete t0
    mov h, t0

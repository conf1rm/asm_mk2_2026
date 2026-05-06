.386

MTXBASE    EQU 20000
FEOF       EQU 0FFh

FAC_RD     EQU 0
FAC_WR     EQU 1

FSK_START  EQU 0
FSK_CUR    EQU 1
FSK_END    EQU 2

a1 EQU 4
a2 EQU 6
a3 EQU 8
a4 EQU 10

v1 EQU -2
v2 EQU -4
v3 EQU -6
v4 EQU -8

stack segment para stack use16
db 65530 dup(?)
stack ends

data segment para public use16

    err_unkop   db "Unknown operation!",0
    err_dimm    db "Matrix dimensions are incompatible for this operation!", 0
    e00         db " ", 0
    e01         db "Invalid function", 0dh, 0ah, 0
    e02         db "File not found", 0dh, 0ah, 0
    e03         db "Path not found", 0dh, 0ah, 0
    e04         db "Too many open files", 0dh, 0ah, 0
    e05         db "Access denied", 0dh, 0ah, 0
    e06         db "Invalid handle", 0dh, 0ah, 0
    e07         db "Memory control blocks destroyed", 0dh, 0ah, 0
    e08         db "Insufficient memory", 0dh, 0ah, 0
    e09         db "Invalid memory block address", 0dh, 0ah, 0
    e10         db "Invalid environment", 0dh, 0ah, 0
    e11         db "Invalid format", 0dh, 0ah, 0
    e12         db "Invalid access code", 0dh, 0ah, 0
    e13         db "Invalid data", 0dh, 0ah, 0
    e14         db "Reserved error", 0dh, 0ah, 0
    e15         db "Invalid drive", 0dh, 0ah, 0
    e16         db "Cannot remove current directory", 0dh, 0ah, 0
    e17         db "Not same device", 0dh, 0ah, 0
    e18         db "No more files", 0dh, 0ah, 0
    e19         db "Write protected", 0dh, 0ah, 0
    e20         db "Unknown unit", 0dh, 0ah, 0
    e21         db "Drive not ready", 0dh, 0ah, 0
    e22         db "Unknown command", 0dh, 0ah, 0
    e23         db "CRC error", 0dh, 0ah, 0
    e24         db "Bad request structure length", 0dh, 0ah, 0
    e25         db "Seek error", 0dh, 0ah, 0
    e26         db "Unknown media type", 0dh, 0ah, 0
    e27         db "Sector not found", 0dh, 0ah, 0
    e28         db "Printer out of paper", 0dh, 0ah, 0
    e29         db "Invalid device request", 0dh, 0ah, 0
    e30         db "Read fault", 0dh, 0ah, 0
    e31         db "General failure", 0dh, 0ah, 0
    e32         db "Unknown DOS error", 0dh, 0ah, 0

    evec dw offset e00, offset e01, offset e02, offset e03, offset e04
         dw offset e05, offset e06, offset e07, offset e08, offset e09
         dw offset e10, offset e11, offset e12, offset e13, offset e14
         dw offset e15, offset e16, offset e17, offset e18, offset e19
         dw offset e20, offset e21, offset e22, offset e23, offset e24
         dw offset e25, offset e26, offset e27, offset e28, offset e29
         dw offset e30, offset e31

    newln db 10, 13

    fn1       db 200 dup(0)
    fn2       db 200 dup(0)
    fn3       db 200 dup(0)

    oper dw 0

    ask1 db "enter matrix_1: ", 0
    ask2 db "enter matrix_2: ", 0
    ask3 db "enter res: ", 0
    askop db "enter operation: ", 0

    tbuf db 3 dup (0)

data ends

mat1_seg segment para public use16
    m1dat DW 10000 DUP (?)
    m1r   DW 0
    m1c   DW 0
mat1_seg ends

mat2_seg segment para public use16
    m2dat DW 10000 DUP (?)
    m2r   DW 0
    m2c   DW 0
mat2_seg ends

code segment para public use16

assume cs:code,ds:data,ss:stack, es:data

include strings.inc
include memory.inc
include matrix.inc
include func.inc

inp_fn proc near
    push bp
    mov bp, sp

    push offset ask1
    call pstr
    add sp, 2

    push offset fn1
    push 200
    call gstr
    add sp, 4

    push offset ask2
    call pstr
    add sp, 2

    push offset fn2
    push 200
    call gstr
    add sp, 4

    push offset ask3
    call pstr
    add sp, 2

    push offset fn3
    push 200
    call gstr
    add sp, 4

    mov sp, bp
    pop bp
    ret
inp_fn endp

selop proc near
    push bp
    mov bp, sp

    mov ax, word ptr [bp+a1]

    cmp al, '+'
    je selop_plus
    cmp al, '-'
    je selop_minus
    cmp al, '*'
    je selop_mul

    push offset err_unkop
    call pstr
    add sp, 2

    mov sp, bp
    pop bp
    stc
    ret

selop_plus:
    mov word ptr [oper], offset madd
    jmp selop_done

selop_minus:
    mov word ptr [oper], offset msub
    jmp selop_done

selop_mul:
    mov word ptr [oper], offset mmul
    jmp selop_done

selop_done:
    mov sp, bp
    pop bp
    clc
    ret
selop endp

calcop proc near
    push bp
    mov bp, sp
    sub sp, 4

    mov ax, mat1_seg
    mov es, ax

    push FAC_RD
    push offset fn1
    call fopn
    add sp, 4
    mov word ptr [bp+v1], ax

    push word ptr [bp+v1]
    call rmat
    add sp, 2

    push word ptr [bp+v1]
    call fcls
    add sp, 2

    mov ax, mat2_seg
    mov es, ax

    push FAC_RD
    push offset fn2
    call fopn
    add sp, 4
    mov word ptr [bp+v1], ax

    push word ptr [bp+v1]
    call rmat
    add sp, 2

    push word ptr [bp+v1]
    call fcls
    add sp, 2

    mov ax, mat1_seg
    mov ds, ax
    mov ax, mat2_seg
    mov es, ax

    push FAC_WR
    push offset fn3
    call fopn
    add sp, 4
    mov word ptr [bp+v1], ax

    push word ptr [bp+v1]
    call word ptr [oper]
    add sp, 2

    push word ptr [bp+v1]
    call fcls
    add sp, 2

calcop_done:
    mov sp, bp
    pop bp
    ret
calcop endp

mainp proc near
    push bp
    mov bp, sp

    call inp_fn

    push offset askop
    call pstr
    add sp, 2

    call gchr
    xor ah, ah
    push ax
    call selop
    jc main_done

    call calcop

main_done:
    mov sp, bp
    pop bp
    ret
mainp endp

start:
    mov ax, data
    mov ds, ax
    mov es, ax
    mov ax, stack
    mov ss, ax
    nop

    push offset endcode
    push cs
    push es
    call meminit
    add sp, 6

    call mainp

    call quit0
endcode:
code ends

end start
putchar proc near
    push bp
    mov bp, sp
    mov dx, word ptr [bp+4]
    mov ah, 02h
    int 21h
    mov sp, bp
    pop bp
    ret
putchar endp

getchar proc near
    push bp
    mov bp, sp
    mov ah, 01h
    int 21h
    mov sp, bp
    pop bp
    ret
getchar endp

putstr proc near
    push bp
    mov bp, sp
    pushregs cx, dx, bx, ax
    push word ptr [bp+4]
    call strlen
    add sp, 2
    mov cx, ax
    mov dx, word ptr [bp+4]
    mov ah, 40h
    mov bx, 1
    int 21h
    popregs ax, bx, dx, cx
    mov sp, bp
    pop bp
    ret
putstr endp

getstr proc near
    push bp
    mov bp, sp
    dec word ptr [bp+6]
    mov bx, word ptr [bp+4]
    mov si, 0
getscyc:
    cmp si, word ptr [bp+6]
    je getsret
    call getchar
    cmp al, 10
    je getsret
    cmp al, 13
    je getsret
    mov byte ptr [bx], al
    inc bx
    inc si
    jmp getscyc
getsret:
    mov byte ptr [bx], 0
    call putnewline
    mov sp, bp
    pop bp
    ret
getstr endp

putnewline proc near
    push bp
    mov bp, sp
    mov dx, 10
    push dx
    call putchar
    add sp, 2
    mov dx, 13
    push dx
    call putchar
    add sp, 2
    mov sp, bp
    pop bp
    ret
putnewline endp

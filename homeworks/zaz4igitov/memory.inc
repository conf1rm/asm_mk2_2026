meminit proc near
    push bp
    mov bp, sp
    push es
    mov bx, 0fff0h
    mov ax, [bp+a3]
    add ax, 0fh
    and bx, ax
    shr bx, 4
    mov ax, [bp+a2]
    add bx, ax
    mov ax, [bp+a1]
    sub bx, ax
    mov ah, 4ah
    int 21h
    jnc minit_ok
    xor ax, ax
    jmp minit_end
minit_ok:
    mov ax, 1
minit_end:
    pop es
    mov sp, bp
    pop bp
    ret
meminit endp

memalc proc near
    _memalc_localSize equ 2
    push bp
    mov bp, sp
    sub sp, _memalc_localSize
    mov bx, 0fff0h
    mov ax, [bp+a1]
    add ax, 0fh
    and bx, ax
    shr bx, 4
    mov [bp+v1], bx
    mov bx, 0FFFFh
    mov ah, 48h
    int 21h
    cmp bx, [bp+v1]
    jb malc_fail
    mov ah, 48h
    mov bx, [bp+v1]
    int 21h
    jnc malc_end
malc_fail:
    mov ax, 0
malc_end:
    add sp, _memalc_localSize
    mov sp, bp
    pop bp
    ret
memalc endp

memfre proc near
    push bp
    mov bp, sp
    push es
    mov es, [bp+a1]
    mov ah, 49h
    int 21h
    jnc mfre_end
    stc
mfre_end:
    pop es
    mov sp, bp
    pop bp
    ret
memfre endp

memrea proc near
    _memrea_localSize equ 2
    push bp
    mov bp, sp
    sub sp, _memrea_localSize
    push es
    mov bx, 0fff0h
    mov ax, [bp+a2]
    add ax, 0fh
    and bx, ax
    shr bx, 4
    mov [bp+v1], bx
    mov es, [bp+a1]
    mov ah, 4ah
    int 21h
    jnc mrea_end
mrea_end:
    pop es
    add sp, _memrea_localSize
    mov sp, bp
    pop bp
    ret
memrea endp
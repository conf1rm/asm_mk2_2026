exit proc near
    push bp
    mov bp, sp
    mov ax, word ptr [bp+4]
    mov ah, 4ch
    int 21h
    mov sp, bp
    pop bp
    ret
exit endp

exit0 proc near
    push bp
    mov bp, sp
    mov dx, 0
    push dx
    call exit
    add sp, 2
    mov sp, bp
    pop bp
    ret
exit0 endp

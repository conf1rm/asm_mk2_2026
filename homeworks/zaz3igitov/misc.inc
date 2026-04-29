; void exit(int code)
; завершает работу программы с кодом, переданным в качетве аргумента (кодом является младший байт аргумента)  
exit proc near
    push bp
    mov bp, sp
    
    mov ax, word ptr [bp + 4]
    mov ah, 4ch
	int 21h
    
    mov sp, bp
    pop bp
    ret
exit endp
    
; void exit0()
; завершает работу программы с кодом 0 
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
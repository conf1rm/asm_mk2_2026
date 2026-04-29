; void putchar(int c)
; выводит символ на экран (младший байт переданного аргумента)
putchar proc near
    push bp
    mov bp, sp
    
    mov dx, word ptr [bp + 4]
    mov ah, 02h
    int 21h
    
    mov sp, bp
    pop bp
    ret
putchar endp    

; int getchar()
; читает символ с клавиатуры и возвращает его (считанный символ - младший байт (al) регистра ax)
getchar proc near
    push bp
    mov bp, sp
    
    mov ah, 01h
    int 21h
    
    mov sp, bp
    pop bp
    ret
getchar endp


; void putstr(const char *str)
; выводит строку на экран (до завершающего нуля), адрес которой передан в качестве аргумента
putstr proc near
    push bp
    mov bp, sp
    pushregs cx, dx, bx, ax

    ; находим длину строки
    push word ptr [bp + 4] 
    call strlen
    add sp, 2
    
    ; выводим строку
    mov cx, ax
    mov dx, word ptr [bp + 4]
    mov ah, 40h
    mov bx, 1
    int 21h

    popregs ax, bx, dx, cx
    mov sp, bp
    pop bp
    ret
putstr endp

; void getstr(const char *str, int max_len)
; читает строку с клавиатуры (либо max_len - 1 байт, либо до перевода строки) и сохраняет её в память, 
; при этом дописывает в конец строки завершающий 0
getstr proc near
    push bp
    mov bp, sp
    
    dec word ptr [bp + 6] ; уменьшаем требуемую длину на 1 (max_len - 1)
    mov bx, word ptr [bp + 4] ; адрес начала строки
    mov si, 0 ; счётчик
 
getscyc: 
    cmp si, word ptr [bp + 6]
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


; void putnewline()
; выводит на экран возврат каретки (\r) и перевод строки (\n), т.е. переводит вывод на новую строку
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
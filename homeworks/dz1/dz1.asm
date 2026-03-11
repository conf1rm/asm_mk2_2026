stack segment para stack
    db 256 dup(?)
stack ends

data segment
data ends

code segment
    assume cs:code, ds:data, ss:stack

Start:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    
    mov ah, 01h
    int 21h
    
    mov bl, al
    mov dl, bl
    mov ah, 02h
    int 21h
    
    mov al, 0
    mov ah, 4ch
    int 21h

code ends
end Start
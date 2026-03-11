stack segment para stack
    db 256 dup(?)
stack ends

data segment
    InputBuffer db 242
                 db 0
                 db 242 dup('$')
data ends

code segment
    assume cs:code, ds:data, ss:stack

Start:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    
    mov dx, offset InputBuffer
    mov ah, 0Ah
    int 21h
    
    lea si, InputBuffer+2
    mov cl, InputBuffer+1
    mov ch, 0
    
    lea bx, [si+cx]
    mov byte ptr [bx], '$'
    
    mov dx, si
    mov ah, 09h
    int 21h
    
    mov al, 0
    mov ah, 4ch
    int 21h

code ends
end Start
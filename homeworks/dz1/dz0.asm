stack segment para stack
    db 256 dup(?)
stack ends

data segment
    String db 'Hello, asm!'
data ends

code segment
    assume cs:code, ds:data, ss:stack

Start:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax
    
    lea bx, String      
    
    lea si, [bx+3]      
    mov byte ptr [si], 'X'  
    
    lea si, [bx+4]      
    mov dl, [si]        
    mov ah, 02h
    int 21h
    
    mov dl, ' '
    mov ah, 02h
    int 21h
    
    mov dx, offset String
    mov ah, 09h
    int 21h
    
    mov al, 0
    mov ah, 4ch
    int 21h

code ends
end Start
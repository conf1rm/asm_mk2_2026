; access_mode = 
;   0000 - read access
;   0001 - write access
;   0010 - read/write access
; short fopen(short access_mode, const char* filename)
; cf flag - as error and return -1
fopen proc near 
    push bp
    mov bp, sp
    pushregs cx, dx

    mov cx, word ptr[bp+arg1]

    mov dx, word ptr[bp+arg2] ; filename
    mov al, cl                ; access mode
    mov ah, 3Dh               ; function code: 3dh - open file
    int 21h
    jc fopen_error

fopen_done:
    popregs dx, cx
    mov sp, bp 
    pop bp
    ret

fopen_error:
    jmp fopen_done
fopen endp


; bool fclose(short d)
; cf - flag and return -1
fclose proc near
    push bp
    mov bp, sp
    pushregs bx

    mov ah, 3eh
    mov bx, word ptr [bp+arg1]
    int 21
    jc fclose_error

    xor ax, ax 
fclose_done:
    popregs bx
    mov sp, bp
    pop bp
    ret

fclose_error:
    mov ax, -1
    jmp fclose_done

fclose endp

; rel = 
; 0 - from file start
; 1 - current pos
; 2 - end of file
; int fseek(short d, short offset_high_word, short offset_low_words, short rel)
; dx:ax - new file pos
; on error cf flag
lseek proc near 
    push bp
    mov bp, sp
    pushregs cx, bx

    mov bx, word ptr [bp+arg4]; BX = file handle
    mov dx, word ptr[bp+arg2] ; offset
    mov cx, word ptr[bp+arg3] ; offset

    mov ah, 42h
    mov al, byte ptr[bp+arg1]

    int 21h

    popregs bx, cx
    mov sp, bp
    pop bp
    ret
lseek endp

; short fsize(int d)
fsize proc near 
    push bp
    mov bp, sp

    ; Get file size
    push word ptr [bp+arg1]
    push 0
    push 0
    push SEEK_END
    call lseek
    jc fsize_error
    add sp, 8

    pushregs dx, ax

    ; Restore original position
    push word ptr [bp+arg1]
    push 0
    push 0
    push SEEK_START
    call lseek
    add sp, 8

    popregs ax, dx

fsize_done:
    mov sp, bp
    pop bp
    ret

fsize_error:
    add sp, 8
    mov ax, -1
    cwd ; DX:AX = -1
    jmp fsize_done

fsize endp

; ds:buffer
; short fread(short d, char* buffer, const short len)
fread proc near 
    push bp
    mov bp, sp
    pushregs bx, dx, cx, si
    
    mov cx, word ptr [bp+arg3]   ; Load CX with the number of bytes to read (arg3)
    mov ah, 3Fh                  ; Set DOS function 3Fh (read from file/device)
    mov bx, word ptr [bp+arg1]   ; Load BX with the file handle (arg1)
    mov dx, word ptr [bp+arg2]   ; Load DX with the offset of the buffer (arg2)
    int 21h                      ; Call DOS interrupt to perform the read operation
    
    jc fread_failed

    mov si, word ptr [bp+arg2]
    add si, ax ; SI += bytes_read 
    mov byte ptr[si], 0 
fread_done:
    popregs si, cx, dx, bx

    mov sp, bp
    pop bp
    ret
fread_failed: 
    mov ax, 0
    jmp fread_done 
fread endp

; ds:buffer
; short fwrite(short d, char* buffer, short size) 
fwrite proc near
    push bp
    mov bp, sp
    
    mov ah, 40h
    mov bx, word ptr[bp+arg1]
    mov cx, word ptr[bp+arg3]
    mov dx, word ptr[bp+arg2]
    int 21h

    mov sp, bp
    pop bp
    ret
fwrite endp

; fread to string
; char* freadtos(short d, short count_of_bytes)
freadtos proc near
    push bp
    mov bp, sp
    pushregs ds, bx, cx, dx

    mov bx, word ptr [bp+arg1]

    inc bx
    push bx 
    call AllocMem
    jc freadtos_alloc_failed
    add sp, 2

    mov es, ax

    push ds
    push es
    pop ds ; ds = es

    push word ptr [bp+arg1]
    push 0 ; es:[0]
    push word ptr [bp+arg2]
    call fread
    add sp, 6
    jc freadtos_read_failed

    pop ds ; ds = ds
    mov ax, es
    clc
freadtos_done:
    popregs dx, cx, bx, ds
    mov sp, bp
    pop bp
    ret
freadtos_read_failed:
    pop ds
    push es
    call FreeMem
    add sp, 2
    xor ax, ax
    stc
    jmp freadtos_done

freadtos_alloc_failed:
    add sp, 2
    xor ax, ax
    stc
    jmp freadtos_done
freadtos endp

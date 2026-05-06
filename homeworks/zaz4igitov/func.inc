pushregs macro r1, r2, r3, r4, r5, r6, r7, r8, r9, r10
    ifnb <r1>
        push r1
        pushregs r2, r3, r4, r5, r6, r7, r8, r9, r10
    endif
endm

popregs macro r1, r2, r3, r4, r5, r6, r7, r8, r9, r10
    ifnb <r1>
        pop r1
        popregs r2, r3, r4, r5, r6, r7, r8, r9, r10
    endif
endm

pchr proc near
    push bp
    mov bp, sp
    mov dx, word ptr [bp + 4]
    mov ah, 02h
    int 21h
    mov sp, bp
    pop bp
    ret
pchr endp

gchr proc near
    push bp
    mov bp, sp
    mov ah, 01h
    int 21h
    mov ah, 0
    mov sp, bp
    pop bp
    ret
gchr endp

pnl proc near
    push bp
    mov bp, sp
    mov dx, 10
    push dx
    call pchr
    add sp, 2
    mov dx, 13
    push dx
    call pchr
    add sp, 2
    mov sp, bp
    pop bp
    ret
pnl endp

pstr proc near
    push bp
    mov bp, sp
    pushregs cx, dx, bx, ax
    push word ptr [bp + 4]
    call slen
    add sp, 2
    mov cx, ax
    mov dx, word ptr [bp + 4]
    mov ah, 40h
    mov bx, 1
    int 21h
    popregs ax, bx, dx, cx
    mov sp, bp
    pop bp
    ret
pstr endp

gstr proc near
    push bp
    mov bp, sp
    push si
    dec word ptr [bp + 6]
    mov bx, word ptr [bp + 4]
    mov si, 0
gstr_loop:
    cmp si, word ptr [bp + 6]
    je gstr_ret
    call gchr
    cmp al, 10
    je gstr_ret
    cmp al, 13
    je gstr_ret
    mov byte ptr [bx], al
    inc bx
    inc si
    jmp gstr_loop
gstr_ret:
    mov byte ptr [bx], 0
    call pnl
    pop si
    mov sp, bp
    pop bp
    ret
gstr endp

quit proc near
    push bp
    mov bp, sp
    mov ax, word ptr [bp + 4]
    mov ah, 4ch
    int 21h
    mov sp, bp
    pop bp
    ret
quit endp

quit0 proc near
    push bp
    mov bp, sp
    mov dx, 0
    push dx
    call quit
    add sp, 2
    mov sp, bp
    pop bp
    ret
quit0 endp

ferr proc near
    push bp
    mov bp, sp
    mov bx, word ptr [bp+a1]
    shl bx, 1
    mov dx, word ptr [evec + bx]
    push dx
    call pstr
    add sp, 2
    push 2
    call quit
    add sp, 2
    mov sp, bp
    pop bp
    ret
ferr endp

fopn proc near
    push bp
    mov bp, sp
    pushregs cx, dx
    mov dx, word ptr[bp+a1]
    mov cx, word ptr[bp+a2]
    mov al, cl
    mov ah, 3Dh
    int 21h
    jc fopn_fail
fopn_ok:
    popregs dx, cx
    mov sp, bp
    pop bp
    ret
fopn_fail:
    mov ax, -1
    jmp fopn_ok
fopn endp

fcls proc near
    push bp
    mov bp, sp
    pushregs bx
    mov ah, 3eh
    mov bx, word ptr [bp+a1]
    int 21h
    jc fcls_fail
    xor ax, ax
fcls_ok:
    popregs bx
    mov sp, bp
    pop bp
    ret
fcls_fail:
    mov ax, -1
    jmp fcls_ok
fcls endp

fsek proc near
    push bp
    mov bp, sp
    pushregs cx, bx
    mov bx, word ptr [bp+a4]
    mov dx, word ptr[bp+a2]
    mov cx, word ptr[bp+a3]
    mov ah, 42h
    mov al, byte ptr[bp+a1]
    int 21h
    popregs bx, cx
    mov sp, bp
    pop bp
    ret
fsek endp

fsze proc near
    push bp
    mov bp, sp
    push word ptr [bp+a1]
    push 0
    push 0
    push FSK_END
    call fsek
    jc fsze_fail
    add sp, 8
    pushregs dx, ax
    push word ptr [bp+a1]
    push 0
    push 0
    push FSK_START
    call fsek
    add sp, 8
    popregs ax, dx
fsze_ok:
    mov sp, bp
    pop bp
    ret
fsze_fail:
    add sp, 8
    mov ax, -1
    cwd
    jmp fsze_ok
fsze endp

frd proc near
    push bp
    mov bp, sp
    pushregs bx, dx, cx
    mov cx, word ptr [bp+a3]
    mov ah, 3Fh
    mov bx, word ptr [bp+a1]
    mov dx, word ptr [bp+a2]
    int 21h
    jc frd_fail
frd_ok:
    popregs cx, dx, bx
    mov sp, bp
    pop bp
    ret
frd_fail:
    mov ax, 0
    jmp frd_ok
frd endp

fwrt proc near
    push bp
    mov bp, sp
    mov ah, 40h
    mov bx, word ptr[bp+a1]
    mov cx, word ptr[bp+a3]
    mov dx, word ptr[bp+a2]
    int 21h
    mov sp, bp
    pop bp
    ret
fwrt endp

frdstr proc near
    push bp
    mov bp, sp
    pushregs ds, bx, cx, dx
    mov bx, word ptr [bp+a1]
    inc bx
    push bx
    call memalc
    jc frdstr_alc_fail
    add sp, 2
    mov es, ax
    push ds
    push es
    pop ds
    push word ptr [bp+a1]
    push 0
    push word ptr [bp+a2]
    call frd
    add sp, 6
    jc frdstr_rd_fail
    pop ds
    mov ax, es
    clc
frdstr_ok:
    popregs dx, cx, bx, ds
    mov sp, bp
    pop bp
    ret
frdstr_rd_fail:
    pop ds
    push es
    call memfre
    add sp, 2
    xor ax, ax
    stc
    jmp frdstr_ok
frdstr_alc_fail:
    add sp, 2
    xor ax, ax
    stc
    jmp frdstr_ok
frdstr endp

fgch proc near
    push bp
    mov bp, sp
    sub sp, 2
    push ds
    mov ax, ss
    mov ds, ax
    mov ah, 3Fh
    mov bx, [bp+a1]
    mov cx, 1
    lea dx, [bp-2]
    int 21h
    pop ds
    jc fgch_err
    cmp ax, 1
    jne fgch_err
    mov al, [bp-2]
    mov ah, 0
    jmp fgch_done
fgch_err:
    mov al, 0FFh
    mov ah, 0
fgch_done:
    mov sp, bp
    pop bp
    ret
fgch endp
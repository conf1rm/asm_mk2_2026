rmat proc near
    push bp
    mov bp, sp
    sub sp, 34
    push si
    push di
    mov di, word ptr [bp+4]
r_skip1:
    push di
    call fgch
    pop cx
    mov byte ptr [bp-1], al
    cmp byte ptr [bp-1], FEOF
    je r_err
    cmp byte ptr [bp-1], ' '
    je r_skip1
    cmp byte ptr [bp-1], 10
    je r_skip1
    cmp byte ptr [bp-1], 13
    je r_skip1
    xor si, si
r_rows_loop:
    cmp byte ptr [bp-1], ' '
    je r_end_rows
    cmp byte ptr [bp-1], 10
    je r_end_rows
    cmp byte ptr [bp-1], 13
    je r_end_rows
    cmp byte ptr [bp-1], FEOF
    je r_err
    cmp si, 19
    jge r_skip_st1
    mov al, byte ptr [bp-1]
    mov byte ptr [bp+si-34], al
    inc si
r_skip_st1:
    push di
    call fgch
    pop cx
    mov byte ptr [bp-1], al
    jmp r_rows_loop
r_end_rows:
    mov byte ptr [bp+si-34], 0
    push ds
    mov ax, ss
    mov ds, ax
    push 10
    lea ax, word ptr [bp-4]
    push ax
    lea ax, word ptr [bp-34]
    push ax
    call str2int
    add sp, 6
    mov word ptr [bp-6], ax
    pop ds
r_skip2:
    push di
    call fgch
    pop cx
    mov byte ptr [bp-1], al
    cmp byte ptr [bp-1], FEOF
    je r_err
    cmp byte ptr [bp-1], ' '
    je r_skip2
    cmp byte ptr [bp-1], 10
    je r_skip2
    cmp byte ptr [bp-1], 13
    je r_skip2
    xor si, si
r_cols_loop:
    cmp byte ptr [bp-1], ' '
    je r_end_cols
    cmp byte ptr [bp-1], 10
    je r_end_cols
    cmp byte ptr [bp-1], 13
    je r_end_cols
    cmp byte ptr [bp-1], FEOF
    je r_err
    cmp si, 19
    jge r_skip_st2
    mov al, byte ptr [bp-1]
    mov byte ptr [bp+si-34], al
    inc si
r_skip_st2:
    push di
    call fgch
    pop cx
    mov byte ptr [bp-1], al
    jmp r_cols_loop
r_end_cols:
    push ds
    mov ax, ss
    mov ds, ax
    mov byte ptr [bp+si-34], 0
    push 10
    lea ax, word ptr [bp-4]
    push ax
    lea ax, word ptr [bp-34]
    push ax
    call str2int
    add sp, 6
    pop ds
    mov word ptr [bp-8], ax
    cmp word ptr [bp-6], 0
    jle r_err
    cmp word ptr [bp-6], 100
    jg r_err
    cmp word ptr [bp-8], 0
    jle r_err
    cmp word ptr [bp-8], 100
    jg r_err
    push di
    mov di, 20000
    mov ax, word ptr [bp-6]
    mov word ptr es:[di], ax
    mov ax, word ptr [bp-8]
    mov word ptr es:[di+2], ax
    pop di
    mov word ptr [bp-10], 0
    jmp r_check_i
r_outer:
    mov word ptr [bp-12], 0
    jmp r_check_j
r_inner:
r_skip3:
    push di
    call fgch
    pop cx
    mov byte ptr [bp-1], al
    cmp byte ptr [bp-1], FEOF
    je r_done_ok
    cmp byte ptr [bp-1], ' '
    je r_skip3
    cmp byte ptr [bp-1], 10
    je r_skip3
    cmp byte ptr [bp-1], 13
    je r_skip3
    xor si, si
r_val_loop:
    cmp byte ptr [bp-1], ' '
    je r_end_val
    cmp byte ptr [bp-1], 10
    je r_end_val
    cmp byte ptr [bp-1], 13
    je r_end_val
    cmp byte ptr [bp-1], FEOF
    je r_end_val
    cmp si, 19
    jge r_skip_st3
    mov al, byte ptr [bp-1]
    mov byte ptr [bp+si-34], al
    inc si
r_skip_st3:
    push di
    call fgch
    pop cx
    mov byte ptr [bp-1], al
    jmp r_val_loop
r_end_val:
    push ds
    mov ax, ss
    mov ds, ax
    mov byte ptr [bp+si-34], 0
    push 10
    lea ax, word ptr [bp-4]
    push ax
    lea ax, word ptr [bp-34]
    push ax
    call str2int
    add sp, 6
    mov word ptr [bp-14], ax
    pop ds
    mov ax, word ptr [bp-10]
    mov dx, word ptr [bp-8]
    shl dx, 1
    mul dx
    mov dx, word ptr [bp-12]
    shl dx, 1
    add ax, dx
    mov bx, ax
    mov dx, word ptr [bp-14]
    push di
    mov di, 20000
    add di, 4
    add di, bx
    mov word ptr es:[di], dx
    pop di
    inc word ptr [bp-12]
r_check_j:
    mov ax, word ptr [bp-12]
    cmp ax, word ptr [bp-8]
    jl r_inner
    inc word ptr [bp-10]
r_check_i:
    mov ax, word ptr [bp-10]
    cmp ax, word ptr [bp-6]
    jl r_outer
r_done_ok:
    xor ax, ax
    jmp r_done
r_err:
    mov ax, 0FFFFh
r_done:
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
rmat endp

mprt proc near
    push bp
    mov bp, sp
    sub sp, 16
    push si
    push di
    mov byte ptr [bp-1], 32
    mov byte ptr [bp-4], 10
    mov byte ptr [bp-3], 13
    mov byte ptr [bp-2], 0
    xor di, di
    jmp mprt_i_tst
mprt_i_bdy:
    xor si, si
    jmp mprt_j_tst
mprt_j_bdy:
    lea ax, word ptr [bp-16]
    push ax
    mov ax, di
    mov dx, word ptr es:[20002]
    shl dx, 1
    mul dx
    mov dx, si
    shl dx, 1
    add ax, dx
    mov bx, 20000
    add bx, 4
    add bx, ax
    push word ptr es:[bx]
    call near ptr stoa
    pop cx
    pop cx
    lea ax, word ptr [bp-16]
    push ax
    call near ptr slen
    pop cx
    push ax
    lea ax, word ptr [bp-16]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    mov ax, word ptr es:[20002]
    dec ax
    cmp ax, si
    jle mprt_j_nxt
    push ds
    mov ax, ss
    mov ds, ax
    mov ax, 1
    push ax
    lea ax, word ptr [bp-1]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop ds
mprt_j_nxt:
    inc si
mprt_j_tst:
    cmp si, word ptr es:[20002]
    jl mprt_j_bdy
    push ds
    mov ax, ss
    mov ds, ax
    mov ax, 1
    push ax
    lea ax, word ptr [bp-4]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop ds
    inc di
mprt_i_tst:
    cmp di, word ptr es:[20000]
    jl mprt_i_bdy
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
mprt endp

madd proc near
    push bp
    mov bp, sp
    sub sp, 16
    push si
    push di
    mov byte ptr [bp-13], ' '
    mov byte ptr [bp-16], 13
    mov byte ptr [bp-15], 10
    mov byte ptr [bp-14], 0
    mov ax, word ptr ds:[20002]
    cmp ax, word ptr es:[20002]
    jne madd_dim
    mov ax, word ptr ds:[20000]
    cmp ax, word ptr es:[20000]
    je madd_go
madd_dim:
    mov ax, -1
    jmp madd_ret
madd_go:
    push ds
    push es
    pop ds
    pop es
    xor di, di
madd_r_loop:
    xor si, si
madd_c_loop:
    mov ax, di
    mov dx, word ptr es:[20002]
    shl dx, 1
    mul dx
    mov dx, si
    shl dx, 1
    add ax, dx
    mov bx, 20000
    add bx, 4
    add bx, ax
    mov ax, word ptr es:[bx]
    push ax
    mov ax, di
    mov dx, word ptr ds:[20002]
    shl dx, 1
    mul dx
    mov dx, si
    shl dx, 1
    add ax, dx
    mov bx, 20000
    add bx, 4
    add bx, ax
    pop ax
    add ax, word ptr ds:[bx]
    mov word ptr [bp-2], ax
    push ds
    push es
    mov ax, ss
    mov ds, ax
    lea ax, word ptr [bp-12]
    push ax
    push word ptr [bp-2]
    call near ptr stoa
    pop cx
    pop cx
    lea ax, word ptr [bp-12]
    push ax
    call near ptr slen
    pop cx
    mov word ptr [bp-4], ax
    push word ptr [bp-4]
    lea ax, word ptr [bp-12]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop es
    pop ds
    inc si
    cmp si, word ptr es:[20002]
    jge madd_endr
    push ds
    push es
    mov ax, ss
    mov ds, ax
    push 1
    lea ax, word ptr [bp-13]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop es
    pop ds
    jmp madd_c_loop
madd_endr:
    push ds
    push es
    mov ax, ss
    mov ds, ax
    push 2
    lea ax, word ptr [bp-16]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop es
    pop ds
    inc di
    cmp di, word ptr es:[20000]
    jl madd_r_loop
    xor ax, ax
madd_ret:
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
madd endp

msub proc near
    push bp
    mov bp, sp
    sub sp, 16
    push si
    push di
    mov byte ptr [bp-13], ' '
    mov byte ptr [bp-16], 13
    mov byte ptr [bp-15], 10
    mov byte ptr [bp-14], 0
    mov ax, word ptr ds:[20002]
    cmp ax, word ptr es:[20002]
    jne msub_dim
    mov ax, word ptr ds:[20000]
    cmp ax, word ptr es:[20000]
    je msub_go
msub_dim:
    mov ax, -1
    jmp msub_ret
msub_go:
    xor di, di
msub_r_loop:
    xor si, si
msub_c_loop:
    mov ax, di
    mov dx, word ptr ds:[20002]
    shl dx, 1
    mul dx
    mov dx, si
    shl dx, 1
    add ax, dx
    mov bx, 20000
    add bx, 4
    add bx, ax
    mov ax, word ptr ds:[bx]
    push ax
    mov ax, di
    mov dx, word ptr es:[20002]
    shl dx, 1
    mul dx
    mov dx, si
    shl dx, 1
    add ax, dx
    mov bx, 20000
    add bx, 4
    add bx, ax
    pop ax
    sub ax, word ptr es:[bx]
    mov word ptr [bp-2], ax
    push ds
    mov ax, ss
    mov ds, ax
    lea ax, word ptr [bp-12]
    push ax
    push word ptr [bp-2]
    call near ptr stoa
    pop cx
    pop cx
    pop ds
    push ds
    mov ax, ss
    mov ds, ax
    lea ax, word ptr [bp-12]
    push ax
    call near ptr slen
    pop cx
    mov word ptr [bp-4], ax
    pop ds
    push ds
    mov ax, ss
    mov ds, ax
    push word ptr [bp-4]
    lea ax, word ptr [bp-12]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop ds
    inc si
    cmp si, word ptr ds:[20002]
    jge msub_endr
    push ds
    mov ax, ss
    mov ds, ax
    push 1
    lea ax, word ptr [bp-13]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop ds
    jmp msub_c_loop
msub_endr:
    push ds
    mov ax, ss
    mov ds, ax
    push 2
    lea ax, word ptr [bp-16]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop ds
    inc di
    cmp di, word ptr ds:[20000]
    jl msub_r_loop
    xor ax, ax
msub_ret:
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
msub endp

mmul proc near
    push bp
    mov bp, sp
    sub sp, 18
    push si
    push di
    mov byte ptr [bp-15], ' '
    mov byte ptr [bp-18], 13
    mov byte ptr [bp-17], 10
    mov byte ptr [bp-16], 0
    mov ax, word ptr ds:[20002]
    cmp ax, word ptr es:[20000]
    je mmul_go
    mov ax, -1
    jmp mmul_exit
mmul_go:
    xor di, di
    jmp mmul_chk_r
mmul_r_loop:
    xor si, si
    jmp mmul_chk_c
mmul_c_loop:
    mov word ptr [bp-2], 0
    xor bx, bx
    jmp mmul_chk_k
mmul_k_loop:
    mov ax, di
    mov dx, word ptr ds:[20002]
    shl dx, 1
    mul dx
    mov dx, bx
    shl dx, 1
    add ax, dx
    push si
    mov si, 20000
    add si, 4
    add si, ax
    mov ax, word ptr ds:[si]
    mov cx, ax
    pop si
    mov ax, bx
    mov dx, word ptr es:[20002]
    shl dx, 1
    mul dx
    mov dx, si
    shl dx, 1
    add ax, dx
    push di
    mov di, 20000
    add di, 4
    add di, ax
    mov ax, word ptr es:[di]
    pop di
    imul cx
    add word ptr [bp-2], ax
    inc bx
mmul_chk_k:
    cmp bx, word ptr ds:[20002]
    jl mmul_k_loop
    push ds
    mov ax, ss
    mov ds, ax
    lea ax, word ptr [bp-10]
    push ax
    push word ptr [bp-2]
    call near ptr stoa
    pop cx
    pop cx
    pop ds
    push ds
    mov ax, ss
    mov ds, ax
    lea ax, word ptr [bp-10]
    push ax
    call near ptr slen
    pop cx
    mov word ptr [bp-4], ax
    pop ds
    push ds
    mov ax, ss
    mov ds, ax
    push word ptr [bp-4]
    lea ax, word ptr [bp-10]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop ds
    inc si
mmul_chk_c:
    cmp si, word ptr es:[20002]
    jge mmul_endc
    push ds
    mov ax, ss
    mov ds, ax
    push 1
    lea ax, word ptr [bp-15]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop ds
    jmp mmul_c_loop
mmul_endc:
    push ds
    mov ax, ss
    mov ds, ax
    push 2
    lea ax, word ptr [bp-18]
    push ax
    push word ptr [bp+4]
    call near ptr fwrt
    add sp, 6
    pop ds
    inc di
mmul_chk_r:
    cmp di, word ptr ds:[20000]
    jl mmul_r_loop
    xor ax, ax
mmul_exit:
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
mmul endp
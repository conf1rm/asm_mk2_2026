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

slen proc near
    push bp
    mov bp, sp
    pushregs cx, di
    mov di, word ptr [bp + 4]
    xor ax, ax
slen_loop:
    mov cl, byte ptr [di]
    test cl, cl
    jz slen_ret
    inc ax
    inc di
    jmp slen_loop
slen_ret:
    popregs di, cx
    mov sp, bp
    pop bp
    ret
slen endp

str2int proc near
    push bp
    mov bp, sp
    sub sp, 10
    pushregs di, si, cx, bx, dx
    mov word ptr [bp-2], 1
    mov word ptr [bp-6], 0
    mov byte ptr [bp-8], 0
    mov di, word ptr [bp+a1]
    mov si, word ptr [bp+a2]
    mov cx, word ptr [bp+a3]
    mov [bp-10], cx
str2int_ws_loop:
    mov bl, byte ptr [di]
    xor bh, bh
    test bl, bl
    jz str2int_empty
    cmp bl, ' '
    je str2int_ws_found
    cmp bl, 09h
    je str2int_ws_found
    cmp bl, 0Ah
    je str2int_ws_found
    cmp bl, 0Bh
    je str2int_ws_found
    cmp bl, 0Ch
    je str2int_ws_found
    cmp bl, 0Dh
    je str2int_ws_found
    jmp str2int_sign
str2int_ws_found:
    inc di
    jmp str2int_ws_loop
str2int_sign:
    mov bl, byte ptr [di]
    cmp bl, '-'
    je str2int_neg
    cmp bl, '+'
    je str2int_pos
str2int_sign_end:
    mov cx, [bp-10]
    test cx, cx
    jz str2int_base0
    cmp cx, 16
    jz str2int_base16
str2int_base_chk_done:
    cmp cx, 2
    jb str2int_inv_base
    mov word ptr [bp-4], di
str2int_cnv:
    mov bl, byte ptr [di]
    test bl, bl
    jz str2int_nodig
    cmp bl, '0'
    jb str2int_chk_up
    cmp bl, '9'
    ja str2int_chk_up
    sub bl, '0'
    jmp str2int_dig_ok
str2int_chk_up:
    cmp bl, 'A'
    jb str2int_chk_lo
    cmp bl, 'Z'
    ja str2int_chk_lo
    sub bl, 'A'
    add bl, 10
    jmp str2int_dig_ok
str2int_chk_lo:
    cmp bl, 'a'
    jb str2int_nodig
    cmp bl, 'z'
    ja str2int_nodig
    sub bl, 'a'
    add bl, 10
str2int_dig_ok:
    cmp bl, cl
    jae str2int_inv_dig
    cmp byte ptr [bp-8], 0
    jne str2int_skip_acc
    mov ax, [bp-6]
    mul cx
    test dx, dx
    jnz str2int_ovf_set
    add ax, bx
    jc str2int_ovf_set
    cmp word ptr [bp-2], -1
    je str2int_neglim
    cmp ax, 32767
    ja str2int_ovf_set
    jmp str2int_store
str2int_neglim:
    cmp ax, 32768
    ja str2int_ovf_set
str2int_store:
    mov [bp-6], ax
    jmp str2int_next_dig
str2int_ovf_set:
    mov byte ptr [bp-8], 1
str2int_skip_acc:
str2int_next_dig:
    inc di
    jmp str2int_cnv
str2int_nodig:
    jmp str2int_nodig_common
str2int_nodig_common:
    mov ax, [bp-4]
    cmp di, ax
    je str2int_nodigit
    cmp byte ptr [bp-8], 0
    jne str2int_ovf_res
    mov ax, [bp-6]
    cmp word ptr [bp-2], -1
    jne str2int_posres
    neg ax
str2int_posres:
    mov bx, [bp+a2]
    mov [bx], di
    jmp str2int_done
str2int_ovf_res:
    cmp word ptr [bp-2], -1
    je str2int_negovf
    mov ax, 32767
    jmp str2int_setend
str2int_negovf:
    mov ax, -32768
str2int_setend:
    mov bx, [bp+a2]
    mov [bx], di
    jmp str2int_done
str2int_nodigit:
    xor ax, ax
    mov bx, [bp+a2]
    mov cx, [bp+a1]
    mov [bx], cx
    jmp str2int_done
str2int_inv_dig:
    mov ax, word ptr [bp-6]
    mov word ptr [si], di
    jmp str2int_done
str2int_inv_base:
str2int_empty:
    xor ax, ax
    mov di, word ptr [bp+a1]
    mov word ptr [si], di
    jmp str2int_done
str2int_neg:
    inc di
    mov word ptr [bp-2], -1
    jmp str2int_sign_end
str2int_pos:
    inc di
    jmp str2int_sign_end
str2int_base0:
    mov bl, byte ptr [di]
    test bl, bl
    jz str2int_empty
    cmp bl, '0'
    jne str2int_base0_skip
    inc di
    mov bl, byte ptr [di]
    test bl, bl
    jz str2int_base0_ret
    cmp bl, 'x'
    je str2int_base0_16
    mov cx, 8
    mov [bp-10], cx
    jmp str2int_base_chk_done
str2int_base0_skip:
    mov cx, 10
    mov [bp-10], cx
    jmp str2int_base_chk_done
str2int_base0_16:
    inc di
    mov cx, 16
    mov [bp-10], cx
    jmp str2int_base_chk_done
str2int_base0_ret:
    dec di
    mov cx, 10
    mov [bp-10], cx
    jmp str2int_base_chk_done
str2int_base16:
    cmp byte ptr [di], '0'
    jne str2int_base16_skip
    cmp byte ptr [di+1], 'x'
    je str2int_base16_pfx_skip
str2int_base16_skip:
    jmp str2int_base_chk_done
str2int_base16_pfx_skip:
    add di, 2
    jmp str2int_base_chk_done
str2int_done:
    popregs dx, bx, cx, si, di
    mov sp, bp
    pop bp
    ret
str2int endp

stoa proc near
    push bp
    mov bp, sp
    sub sp, 14
    push si
    push di
    mov di, word ptr [bp+4]
    mov word ptr [bp-2], 0
    xor si, si
    cmp di, 0
    jne stoa_check_neg
    mov byte ptr [bp-14], 48
    inc si
    jmp stoa_nonegsign
stoa_check_neg:
    or di, di
    jge stoa_cvt
    mov word ptr [bp-2], 1
    mov ax, di
    neg ax
    mov di, ax
stoa_cvt:
    mov ax, di
    mov bx, 10
    cwd
    idiv bx
    add dl, 48
    mov byte ptr [bp+si-14], dl
    inc si
    mov ax, di
    mov bx, 10
    cwd
    idiv bx
    mov di, ax
    or di, di
    jg stoa_cvt
    cmp word ptr [bp-2], 0
    je stoa_nonegsign
    mov byte ptr [bp+si-14], 45
    inc si
stoa_nonegsign:
    xor cx, cx
    jmp stoa_rev_tst
stoa_rev_body:
    dec si
    mov al, byte ptr [bp+si-14]
    mov bx, word ptr [bp+6]
    add bx, cx
    mov byte ptr [bx], al
    inc cx
stoa_rev_tst:
    or si, si
    jg stoa_rev_body
    mov bx, word ptr [bp+6]
    add bx, cx
    mov byte ptr [bx], 0
    mov ax, word ptr [bp+6]
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
stoa endp
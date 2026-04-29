; void print_result(char* message, bool res)
print_result proc near
    push bp
    mov bp, sp
    pusha

    mov dx, word ptr [bp+6]      
    push dx
    call putstr
    add sp, 2
    
    mov bx, word ptr [bp+4] 
    cmp bx, 0
    jne pr_fail
    mov dx, offset msg_pass
    jmp pr_print
pr_fail:
    mov dx, offset msg_fail
pr_print:
    push dx
    call putstr
    add sp, 2

    popa
    mov sp, bp
    pop bp
    ret
print_result endp

; ========== STRLEN ==========
test_strlen proc near
    push bp
    mov bp, sp
    sub sp, 2

    push offset test_str4
    call strlen
    add sp, 2
    cmp ax, 0
    jne tstrlen_fail

    push offset test_str1
    call strlen
    add sp, 2
    cmp ax, 5
    jne tstrlen_fail

    push offset test_str2
    call strlen
    add sp, 2
    cmp ax, 13
    jne tstrlen_fail

    mov word ptr [bp-2], 0
    jmp tstrlen_end
tstrlen_fail:
    mov word ptr [bp-2], 1
tstrlen_end:
    push offset msg_strlen
    push word ptr [bp-2]
    call print_result
    add sp, 4
    mov sp, bp
    pop bp
    ret
test_strlen endp

; ========== STRCHR ==========
test_strchr proc near
    push bp
    mov bp, sp
    sub sp, 2

    push 'l'
    push offset test_str1
    call strchr
    add sp, 4
    cmp ax, offset test_str1 + 2
    jne tschr_fail

    push 'x'
    push offset test_str1
    call strchr
    add sp, 4
    test ax, ax
    jnz tschr_fail

    push 0
    push offset test_str1
    call strchr
    add sp, 4
    cmp ax, offset test_str1 + 5
    jne tschr_fail

    mov word ptr [bp-2], 0
    jmp tschr_end
tschr_fail:
    mov word ptr [bp-2], 1
tschr_end:
    push offset msg_strchr
    push word ptr [bp-2]
    call print_result
    add sp, 4
    mov sp, bp
    pop bp
    ret
test_strchr endp

; ========== STRCPY ==========
test_strcpy proc near
    push bp
    mov bp, sp
    sub sp, 2

    push offset test_str1
    push offset test_buffer
    call strcpy
    add sp, 4

    push offset test_str1
    push offset test_buffer
    call strcmp
    add sp, 4
    cmp ax, 0
    jne tstrcpy_fail

    mov word ptr [bp-2], 0
    jmp tstrcpy_end
tstrcpy_fail:
    mov word ptr [bp-2], 1
tstrcpy_end:
    push offset msg_strcpy
    push word ptr [bp-2]
    call print_result
    add sp, 4
    mov sp, bp
    pop bp
    ret
test_strcpy endp

; ========== STRCAT ==========
test_strcat proc near
    push bp
    mov bp, sp
    sub sp, 2

    mov byte ptr test_buffer, 0
    
    push offset test_str1
    push offset test_buffer
    call strcpy
    add sp, 4
    
    push offset test_str6
    push offset test_buffer
    call strcat
    add sp, 4
    
    push offset test_str_cat_res
    push offset test_buffer
    call strcmp
    add sp, 4
    cmp ax, 0
    jne tstrcat_fail

    mov word ptr [bp-2], 0
    jmp tstrcat_end
tstrcat_fail:
    mov word ptr [bp-2], 1
tstrcat_end:
    push offset msg_strcat
    push word ptr [bp-2]
    call print_result
    add sp, 4
    mov sp, bp
    pop bp
    ret
test_strcat endp

; ========== STRCMP ==========
test_strcmp proc near
    push bp
    mov bp, sp
    sub sp, 2

    push offset test_str5
    push offset test_str5
    call strcmp
    add sp, 4
    cmp ax, 0
    jne tstrcmp_fail

    push offset test_str6
    push offset test_str5
    call strcmp
    add sp, 4
    cmp ax, 0
    je tstrcmp_fail

    mov word ptr [bp-2], 0
    jmp tstrcmp_end
tstrcmp_fail:
    mov word ptr [bp-2], 1
tstrcmp_end:
    push offset msg_strcmp
    push word ptr [bp-2]
    call print_result
    add sp, 4
    mov sp, bp
    pop bp
    ret
test_strcmp endp

; ========== STRICMP ==========
test_stricmp proc near
    push bp
    mov bp, sp
    sub sp, 2

    push offset test_str5
    push offset test_str5
    call stricmp
    add sp, 4
    cmp ax, 0
    jne tstricmp_fail

    push offset test_str_upper
    push offset test_str5
    call stricmp
    add sp, 4
    cmp ax, 0
    jne tstricmp_fail

    mov word ptr [bp-2], 0
    jmp tstricmp_end
tstricmp_fail:
    mov word ptr [bp-2], 1
tstricmp_end:
    push offset msg_stricmp
    push word ptr [bp-2]
    call print_result
    add sp, 4
    mov sp, bp
    pop bp
    ret
test_stricmp endp

; ========== STRSTR ==========
test_strstr proc near
    push bp
    mov bp, sp
    sub sp, 2

    push offset test_str1
    push offset test_str4
    call strstr
    add sp, 4
    cmp ax, offset test_str1
    jne tstrstr_fail

    push offset test_str2
    push offset test_str1
    call strstr
    add sp, 4
    cmp ax, offset test_str2
    jne tstrstr_fail

    mov word ptr [bp-2], 0
    jmp tstrstr_end
tstrstr_fail:
    mov word ptr [bp-2], 1
tstrstr_end:
    push offset msg_strstr
    push word ptr [bp-2]
    call print_result
    add sp, 4
    mov sp, bp
    pop bp
    ret
test_strstr endp

; ========== strtol ==========
test_strtol proc near
    push bp
    mov bp, sp
    sub sp, 2

    push 10
    push offset test_end_ptr
    push offset test_strtol1
    call strtol
    add sp, 6
    cmp ax, 123
    jne tstrtol_fail

    push 10
    push offset test_end_ptr
    push offset test_strtol2
    call strtol
    add sp, 6
    cmp ax, -456
    jne tstrtol_fail

    mov word ptr [bp-2], 0
    jmp tstrtol_end
tstrtol_fail:
    mov word ptr [bp-2], 1
tstrtol_end:
    push offset msg_strtol
    push word ptr [bp-2]
    call print_result
    add sp, 4
    mov sp, bp
    pop bp
    ret
test_strtol endp

; ========== STRDUP ==========
test_strdup proc near
    push bp
    mov bp, sp
    sub sp, 2

    push offset test_str1
    call strdup
    add sp, 2
    test ax, ax
    jz tstrdup_fail
    
    push ax
    mov word ptr [bp-2], 0
    jmp tstrdup_end
    
tstrdup_fail:
    mov word ptr [bp-2], 1
    xor ax, ax
    push ax
    
tstrdup_end:
    pop ax
    test ax, ax
    jz tstrdup_skip
    push ax
    call FreeMem
    add sp, 2
tstrdup_skip:
    push offset msg_strdup
    push word ptr [bp-2]
    call print_result
    add sp, 4
    mov sp, bp
    pop bp
    ret
test_strdup endp
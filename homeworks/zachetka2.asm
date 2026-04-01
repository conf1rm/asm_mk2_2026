.386

arg1 equ 4
arg2 equ 6
arg3 equ 8

stack segment para stack
db 65530 dup(?)
stack ends

data segment para public

inp_buf db 255, 0, 256 dup(0)
base_buf db 3, 0, 4 dup(0)

msg_base_ask db "base (d/h): $"
msg_expr_ask db "expression: $"
msg_res_dec db "dec: $"
msg_res_hex db "hex: $"
msg_err_fmt db "invalid format$"
msg_err_op db "invalid operator$"
msg_err_div0 db "division by zero$"
msg_err_range db "number out of range$"
msg_err_base db "invalid base$"
msg_err_dov db "division overflow$"

temp_buf db 32 dup(0)

ERR_FORMAT equ 1
ERR_OPERATOR equ 2
ERR_DIV_ZERO equ 3
ERR_RANGE equ 4
ERR_BASE equ 5
ERR_DIV_OVER equ 6

val1 dw ?
val2 dw ?
oper db ?
num_base db ?
res_low dw ?
res_high dw ?
is_32bit db ?

data ends

code segment para public use16
assume cs:code, ds:data, ss:stack

_putstr:
    push bp
    mov bp, sp
    mov dx, [bp+arg1]
    mov ah, 09h
    int 21h
    pop bp
    ret

_newline:
    push bp
    mov bp, sp
    mov dl, 13
    mov ah, 02h
    int 21h
    mov dl, 10
    int 21h
    pop bp
    ret

_getstr:
    push bp
    mov bp, sp
    mov dx, [bp+arg1]
    mov ah, 0Ah
    int 21h
    mov bx, [bp+arg1]
    xor cx, cx
    mov cl, byte ptr [bx+1]
    add bx, 2
    add bx, cx
    mov byte ptr [bx], 0
    pop bp
    ret

_read_base:
    push bp
    mov bp, sp

    push offset msg_base_ask
    call _putstr
    add sp, 2

    push offset base_buf
    call _getstr
    add sp, 2

    call _newline

    cmp byte ptr [base_buf+1], 1
    jne base_err

    mov al, byte ptr [base_buf+2]
    cmp al, 'd'
    je base_ok
    cmp al, 'h'
    je base_ok

base_err:
    mov ax, ERR_BASE
    stc
    jmp base_exit

base_ok:
    mov byte ptr [num_base], al
    clc

base_exit:
    pop bp
    ret

_str_to_dec:
    push bp
    mov bp, sp
    push cx
    push dx
    push di

    mov si, [bp+arg1]
    xor ax, ax
    xor di, di

    cmp byte ptr [si], '-'
    jne dec_check_digits
    mov di, 1
    inc si

dec_check_digits:
    cmp byte ptr [si], '0'
    jb dec_err
    cmp byte ptr [si], '9'
    ja dec_err

dec_loop:
    mov dl, byte ptr [si]
    cmp dl, ' '
    je dec_done
    cmp dl, 0
    je dec_done

    cmp dl, '0'
    jb dec_err
    cmp dl, '9'
    ja dec_err

    sub dl, '0'
    xor dh, dh
    xor cx, cx
    mov cx, dx

    cmp di, 0
    jne dec_check_neg

    cmp ax, 3276
    ja dec_range
    jne dec_safe_mul
    cmp cl, 7
    ja dec_range
    jmp dec_safe_mul

dec_check_neg:
    cmp ax, 3276
    ja dec_range
    jne dec_safe_mul
    cmp cl, 8
    ja dec_range

dec_safe_mul:
    imul ax, 10
    add ax, cx
    inc si
    jmp dec_loop

dec_done:
    cmp di, 0
    je dec_ok
    neg ax

dec_ok:
    clc
    jmp dec_exit

dec_err:
    mov ax, ERR_FORMAT
    stc
    jmp dec_exit

dec_range:
    mov ax, ERR_RANGE
    stc

dec_exit:
    pop di
    pop dx
    pop cx
    pop bp
    ret

_str_to_hex:
    push bp
    mov bp, sp
    push bx
    push cx
    push di

    mov si, [bp+arg1]
    xor ax, ax
    xor di, di

    cmp byte ptr [si], '-'
    jne hex_check_char
    mov di, 1
    inc si

hex_check_char:
    mov cl, byte ptr [si]
    cmp cl, '0'
    jb hex_err
    cmp cl, '9'
    jbe hex_loop_start
    cmp cl, 'A'
    jb hex_err
    cmp cl, 'F'
    jbe hex_loop_start
    cmp cl, 'a'
    jb hex_err
    cmp cl, 'f'
    jbe hex_loop_start
    jmp hex_err

hex_loop_start:
    mov cl, byte ptr [si]
    cmp cl, ' '
    je hex_done
    cmp cl, 0
    je hex_done

    cmp cl, '0'
    jb hex_err
    cmp cl, '9'
    jbe hex_digit_09
    cmp cl, 'A'
    jb hex_try_lower
    cmp cl, 'F'
    jbe hex_digit_AF

hex_try_lower:
    cmp cl, 'a'
    jb hex_err
    cmp cl, 'f'
    ja hex_err
    sub cl, 20h

hex_digit_AF:
    sub cl, 'A'
    add cl, 10
    jmp hex_apply

hex_digit_09:
    sub cl, '0'

hex_apply:
    xor ch, ch

    cmp di, 0
    jne hex_check_neg_ov

    cmp ax, 2048
    jae hex_range
    jmp hex_safe

hex_check_neg_ov:
    cmp ax, 2048
    ja hex_range
    jb hex_safe
    cmp cl, 0
    ja hex_range

hex_safe:
    shl ax, 4
    add ax, cx
    inc si
    jmp hex_loop_start

hex_done:
    cmp di, 0
    je hex_ok
    neg ax

hex_ok:
    clc
    jmp hex_exit

hex_err:
    mov ax, ERR_FORMAT
    stc
    jmp hex_exit

hex_range:
    mov ax, ERR_RANGE
    stc

hex_exit:
    pop di
    pop cx
    pop bx
    pop bp
    ret

_parse_dec:
    push bp
    mov bp, sp
    push bx
    push cx
    push dx

    mov si, [bp+arg1]

    push si
    call _str_to_dec
    pop dx
    jc parse_err
    mov word ptr [val1], ax

    cmp byte ptr [si], ' '
    jne parse_fmt_err
    inc si

    mov al, byte ptr [si]
    cmp al, '+'
    je parse_op_ok
    cmp al, '-'
    je parse_op_ok
    cmp al, '*'
    je parse_op_ok
    cmp al, '/'
    je parse_op_ok
    cmp al, '%'
    je parse_op_ok

    mov ax, ERR_OPERATOR
    stc
    jmp parse_err

parse_op_ok:
    mov byte ptr [oper], al
    inc si

    cmp byte ptr [si], ' '
    jne parse_fmt_err
    inc si

    push si
    call _str_to_dec
    pop dx
    jc parse_err
    mov word ptr [val2], ax

    cmp byte ptr [si], 0
    jne parse_fmt_err

    clc
    jmp parse_exit

parse_fmt_err:
    mov ax, ERR_FORMAT
    stc

parse_err:
parse_exit:
    pop dx
    pop cx
    pop bx
    pop bp
    ret

_parse_hex:
    push bp
    mov bp, sp
    push bx
    push cx
    push dx

    mov si, [bp+arg1]

    push si
    call _str_to_hex
    pop dx
    jc parse_hex_err
    mov word ptr [val1], ax

    cmp byte ptr [si], ' '
    jne parse_hex_fmt_err
    inc si

    mov al, byte ptr [si]
    cmp al, '+'
    je parse_hex_op_ok
    cmp al, '-'
    je parse_hex_op_ok
    cmp al, '*'
    je parse_hex_op_ok
    cmp al, '/'
    je parse_hex_op_ok
    cmp al, '%'
    je parse_hex_op_ok

    mov ax, ERR_OPERATOR
    stc
    jmp parse_hex_err

parse_hex_op_ok:
    mov byte ptr [oper], al
    inc si

    cmp byte ptr [si], ' '
    jne parse_hex_fmt_err
    inc si

    push si
    call _str_to_hex
    pop dx
    jc parse_hex_err
    mov word ptr [val2], ax

    cmp byte ptr [si], 0
    jne parse_hex_fmt_err

    clc
    jmp parse_hex_exit

parse_hex_fmt_err:
    mov ax, ERR_FORMAT
    stc

parse_hex_err:
parse_hex_exit:
    pop dx
    pop cx
    pop bx
    pop bp
    ret

_int_to_dec:
    push bp
    mov bp, sp
    push ax
    push bx
    push cx
    push dx
    push si
    push di

    mov ax, [bp+arg1]
    mov bx, [bp+arg2]
    xor di, di

    cmp ax, 0
    jge itod_not_neg
    mov di, 1
    neg ax

itod_not_neg:
    cmp ax, 0
    jne itod_not_zero
    mov byte ptr [bx], '0'
    mov byte ptr [bx+1], '$'
    jmp itod_end

itod_not_zero:
    mov si, bx
    add si, 10
    mov byte ptr [si], '$'

itod_conv:
    xor dx, dx
    mov cx, 10
    div cx
    add dl, '0'
    dec si
    mov byte ptr [si], dl
    cmp ax, 0
    jne itod_conv

    cmp di, 0
    je itod_copy
    dec si
    mov byte ptr [si], '-'

itod_copy:
    cmp si, bx
    je itod_end

itod_copy_loop:
    mov cl, byte ptr [si]
    mov byte ptr [bx], cl
    inc bx
    inc si
    cmp byte ptr [si], '$'
    jne itod_copy_loop
    mov byte ptr [bx], '$'

itod_end:
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    pop bp
    ret

_int_to_hex16:
    push bp
    mov bp, sp
    push ax
    push bx
    push cx
    push dx
    push si

    mov ax, [bp+arg1]
    mov si, [bp+arg2]

    cmp ax, 0
    jge ith16_pos
    mov byte ptr [si], '-'
    inc si
    neg ax

ith16_pos:
    mov dx, ax
    mov cx, 4
    xor bx, bx

ith16_loop:
    rol dx, 4
    mov ax, dx
    and ax, 000Fh

    cmp bx, 0
    jne ith16_write
    cmp ax, 0
    je ith16_skip
    mov bx, 1

ith16_write:
    cmp al, 10
    jb ith16_below
    add al, 'A' - 10
    jmp ith16_store
ith16_below:
    add al, '0'
ith16_store:
    mov byte ptr [si], al
    inc si

ith16_skip:
    loop ith16_loop

    cmp bx, 0
    jne ith16_done
    mov byte ptr [si], '0'
    inc si

ith16_done:
    mov byte ptr [si], '$'

    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    pop bp
    ret

_int32_to_dec:
    push bp
    mov bp, sp
    push ax
    push bx
    push cx
    push dx
    push si
    push di

    mov ax, [bp+arg1]
    mov dx, [bp+arg2]
    mov di, [bp+arg3]

    cmp ax, 0
    jne i32d_not_zero
    cmp dx, 0
    jne i32d_not_zero
    mov byte ptr [di], '0'
    mov byte ptr [di+1], '$'
    jmp i32d_end

i32d_not_zero:
    xor cx, cx
    test dx, 8000h
    jz i32d_pos
    not ax
    not dx
    add ax, 1
    adc dx, 0
    mov cx, 1

i32d_pos:
    push cx
    push di
    add di, 16
    mov byte ptr [di], '$'

i32d_divide:
    push di
    xor si, si
    mov cx, 32
    mov di, ax
    mov bx, dx
    xor ax, ax
    xor dx, dx

i32d_div_loop:
    shl di, 1
    rcl bx, 1
    rcl si, 1
    cmp si, 10
    jb i32d_below
    sub si, 10
    shl ax, 1
    rcl dx, 1
    or ax, 1
    jmp i32d_next

i32d_below:
    shl ax, 1
    rcl dx, 1

i32d_next:
    loop i32d_div_loop

    mov cx, si
    pop di
    add cl, '0'
    dec di
    mov byte ptr [di], cl

    or dx, dx
    jnz i32d_divide
    or ax, ax
    jnz i32d_divide

    pop si
    pop cx
    cmp cx, 1
    jne i32d_copy
    dec di
    mov byte ptr [di], '-'

i32d_copy:
    xor bx, bx
i32d_copy_loop:
    mov bl, byte ptr [di]
    mov byte ptr [si], bl
    inc si
    inc di
    cmp byte ptr [di], '$'
    jne i32d_copy_loop
    mov byte ptr [si], '$'

i32d_end:
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    pop bp
    ret

_int32_to_hex:
    push bp
    mov bp, sp
    push ax
    push bx
    push cx
    push dx
    push si
    push di

    mov ax, [bp+arg1]
    mov dx, [bp+arg2]
    mov di, [bp+arg3]

    xor cx, cx
    test dx, 8000h
    jz i32h_pos
    mov byte ptr [di], '-'
    inc di
    not ax
    not dx
    add ax, 1
    adc dx, 0

i32h_pos:
    cmp dx, 0
    jne i32h_nonzero
    cmp ax, 0
    jne i32h_nonzero
    mov byte ptr [di], '0'
    inc di
    jmp i32h_finish

i32h_nonzero:
    mov si, ax
    mov bx, dx
    xor cx, cx

    push si
    mov dx, bx
    mov si, 4

i32h_hi_loop:
    rol dx, 4
    mov ax, dx
    and ax, 000Fh

    cmp cx, 0
    jne i32h_hi_write
    cmp ax, 0
    je i32h_hi_skip
    mov cx, 1

i32h_hi_write:
    cmp al, 10
    jb i32h_hi_below
    add al, 'A' - 10
    jmp i32h_hi_store
i32h_hi_below:
    add al, '0'
i32h_hi_store:
    mov byte ptr [di], al
    inc di

i32h_hi_skip:
    dec si
    jnz i32h_hi_loop
    pop si

    mov dx, si
    mov si, 4

    cmp cx, 0
    je i32h_lo_loop

i32h_lo_loop_full:
    rol dx, 4
    mov ax, dx
    and ax, 000Fh
    cmp al, 10
    jb i32h_lo_below
    add al, 'A' - 10
    jmp i32h_lo_store
i32h_lo_below:
    add al, '0'
i32h_lo_store:
    mov byte ptr [di], al
    inc di
    dec si
    jnz i32h_lo_loop_full
    jmp i32h_finish

i32h_lo_loop:
    rol dx, 4
    mov ax, dx
    and ax, 000Fh

    cmp cx, 0
    jne i32h_lo_write
    cmp ax, 0
    je i32h_lo_skip
    mov cx, 1

i32h_lo_write:
    cmp al, 10
    jb i32h_lo_below
    add al, 'A' - 10
    jmp i32h_lo_store
i32h_lo_below:
    add al, '0'
i32h_lo_store:
    mov byte ptr [di], al
    inc di

i32h_lo_skip:
    dec si
    jnz i32h_lo_loop

    cmp cx, 0
    jne i32h_finish
    mov byte ptr [di], '0'
    inc di

i32h_finish:
    mov byte ptr [di], '$'

    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
    pop bp
    ret

_calc_op:
    push bp
    mov bp, sp
    push bx

    mov ax, [bp+arg1]
    mov bx, [bp+arg2]
    mov cx, [bp+arg3]

    cmp cl, '+'
    je op_add
    cmp cl, '-'
    je op_sub
    cmp cl, '*'
    je op_mul
    cmp cl, '/'
    je op_div
    cmp cl, '%'
    je op_mod

    mov ax, ERR_OPERATOR
    stc
    jmp op_exit

op_add:
    add ax, bx
    jo op_overflow
    mov word ptr [res_low], ax
    mov word ptr [res_high], 0
    mov byte ptr [is_32bit], 0
    clc
    jmp op_exit

op_sub:
    sub ax, bx
    jo op_overflow
    mov word ptr [res_low], ax
    mov word ptr [res_high], 0
    mov byte ptr [is_32bit], 0
    clc
    jmp op_exit

op_mul:
    imul bx
    mov word ptr [res_low], ax
    mov word ptr [res_high], dx
    mov byte ptr [is_32bit], 1
    clc
    jmp op_exit

op_div:
    cmp bx, 0
    je op_div_zero
    cmp ax, 8000h
    jne op_div_safe
    cmp bx, 0FFFFh
    je op_div_over

op_div_safe:
    cwd
    idiv bx
    mov word ptr [res_low], ax
    mov word ptr [res_high], 0
    mov byte ptr [is_32bit], 0
    clc
    jmp op_exit

op_mod:
    cmp bx, 0
    je op_div_zero
    cmp ax, 8000h
    jne op_mod_safe
    cmp bx, 0FFFFh
    jne op_mod_safe
    mov word ptr [res_low], 0
    mov word ptr [res_high], 0
    mov byte ptr [is_32bit], 0
    clc
    jmp op_exit

op_mod_safe:
    cwd
    idiv bx
    mov word ptr [res_low], dx
    mov word ptr [res_high], 0
    mov byte ptr [is_32bit], 0
    clc
    jmp op_exit

op_overflow:
    mov ax, ERR_RANGE
    stc
    jmp op_exit

op_div_zero:
    mov ax, ERR_DIV_ZERO
    stc
    jmp op_exit

op_div_over:
    mov ax, ERR_DIV_OVER
    stc

op_exit:
    pop bx
    pop bp
    ret

_print_res:
    push bp
    mov bp, sp

    push offset msg_res_dec
    call _putstr
    add sp, 2

    cmp byte ptr [is_32bit], 1
    je pr32_dec

    push offset temp_buf
    push word ptr [res_low]
    call _int_to_dec
    add sp, 4
    jmp pr_dec_out

pr32_dec:
    push offset temp_buf
    push word ptr [res_high]
    push word ptr [res_low]
    call _int32_to_dec
    add sp, 6

pr_dec_out:
    push offset temp_buf
    call _putstr
    add sp, 2

    call _newline

    push offset msg_res_hex
    call _putstr
    add sp, 2

    cmp byte ptr [is_32bit], 1
    je pr32_hex

    push offset temp_buf
    push word ptr [res_low]
    call _int_to_hex16
    add sp, 4
    jmp pr_hex_out

pr32_hex:
    push offset temp_buf
    push word ptr [res_high]
    push word ptr [res_low]
    call _int32_to_hex
    add sp, 6

pr_hex_out:
    push offset temp_buf
    call _putstr
    add sp, 2

    pop bp
    ret

_print_err:
    push bp
    mov bp, sp

    mov ax, [bp+arg1]

    cmp ax, ERR_FORMAT
    je pe_fmt
    cmp ax, ERR_OPERATOR
    je pe_op
    cmp ax, ERR_DIV_ZERO
    je pe_div
    cmp ax, ERR_RANGE
    je pe_rng
    cmp ax, ERR_BASE
    je pe_base
    cmp ax, ERR_DIV_OVER
    je pe_dov
    jmp pe_fmt

pe_fmt:
    push offset msg_err_fmt
    jmp pe_print
pe_op:
    push offset msg_err_op
    jmp pe_print
pe_div:
    push offset msg_err_div0
    jmp pe_print
pe_rng:
    push offset msg_err_range
    jmp pe_print
pe_base:
    push offset msg_err_base
    jmp pe_print
pe_dov:
    push offset msg_err_dov

pe_print:
    call _putstr
    add sp, 2

    pop bp
    ret

_calc:
    push bp
    mov bp, sp

    call _read_base
    jc calc_error

    push offset msg_expr_ask
    call _putstr
    pop dx

    push offset inp_buf
    call _getstr
    pop dx

    call _newline

    lea ax, [inp_buf+2]

    cmp byte ptr [num_base], 'h'
    je calc_hex

    push ax
    call _parse_dec
    pop dx
    jc calc_error
    jmp calc_do

calc_hex:
    push ax
    call _parse_hex
    pop dx
    jc calc_error

calc_do:
    mov al, byte ptr [oper]
    xor ah, ah
    push ax
    push word ptr [val2]
    push word ptr [val1]
    call _calc_op
    pop dx
    pop dx
    pop dx
    jc calc_error

    call _print_res
    jmp calc_done

calc_error:
    push ax
    call _print_err
    pop dx

calc_done:
    call _newline
    pop bp
    ret

_exit0:
    mov ax, 4C00h
    int 21h

start:
    mov ax, data
    mov ds, ax
    mov ax, stack
    mov ss, ax

    call _calc
    call _exit0

code ends
end start
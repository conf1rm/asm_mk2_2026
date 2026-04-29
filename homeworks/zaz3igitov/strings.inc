; Функции для работы со строками

; char* strcat(char* dest, const char* src)
strcat proc near
    push bp
    mov bp, sp
    push di
    push si

    mov di, word ptr [bp+arg1]
    xor al, al
    mov cx, 0FFFFh
    repne scasb
    dec di ; указывает на нулевой байт конца dest

    mov si, word ptr [bp+arg2]
strcat_loop:
    mov al, byte ptr [si]
    mov byte ptr [di], al
    test al, al
    jz strcat_done
    inc si
    inc di
    jmp strcat_loop

strcat_done:
    mov ax, word ptr [bp+arg1]
    pop si
    pop di
    mov sp, bp
    pop bp
    ret
strcat endp

; int strcmp_idx(const char* s1_seg, const char* s1_off, const char* s2_seg, const char* s2_off)
; Возвращает индекс первого несовпадающего символа или -1, если строки равны
; int strcmp_idx(word seg1, word off1, word seg2, word off2)
; Возвращает индекс первого различия или -1 если строки равны
strcmp_idx proc near
    push bp
    mov bp, sp
    push si
    push di
    push ds
    push es

    mov ax, word ptr [bp+arg1] ; seg1
    mov dx, word ptr [bp+arg3] ; seg2
    mov si, word ptr [bp+arg2] ; off1
    mov di, word ptr [bp+arg4] ; off2
    xor cx, cx                  ; индекс

strcmp_idx_loop:
    mov ds, ax
    mov es, dx
    mov bl, byte ptr [si]
    mov bh, byte ptr es:[di]
    
    cmp bl, bh
    jne strcmp_idx_found
    
    test bl, bl
    jz strcmp_idx_equal
    
    inc si
    inc di
    inc cx
    jmp strcmp_idx_loop

strcmp_idx_found:
    mov ax, cx
    jmp strcmp_idx_done

strcmp_idx_equal:
    mov ax, -1

strcmp_idx_done:
    pop es
    pop ds
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
strcmp_idx endp

; word split_to_lines(word seg_str)
; Разбивает текст на строки по \n (и \r\n). Возвращает сегмент массива указателей (сегментов) на строки.
; В CX возвращается количество строк.
split_to_lines proc near
    push bp
    mov bp, sp
    sub sp, 10
    pushregs di, si, ds, es

    mov word ptr [bp-2], 0   ; line_count
    mov word ptr [bp-4], 0   ; start_idx
    mov word ptr [bp-6], 0   ; curr_idx

    mov dx, word ptr [bp+arg1] ; seg_str
    mov ds, dx

    ; Подсчет количества строк
    xor si, si
count_lines_loop:
    mov al, byte ptr [si]
    test al, al
    jz count_lines_done
    cmp al, 0Ah
    jne count_lines_skip
    inc word ptr [bp-2]
count_lines_skip:
    inc si
    jmp count_lines_loop
count_lines_done:
    ; Добавляем 1, если последний символ не \n
    cmp si, 0
    je count_lines_zero
    mov al, byte ptr [si-1]
    cmp al, 0Ah
    je count_lines_zero
    inc word ptr [bp-2]
count_lines_zero:

    mov cx, word ptr [bp-2]
    test cx, cx
    jz split_empty

    ; Выделение памяти под массив указателей (по 2 байта на строку)
    shl cx, 1
    push cx
    call AllocMem
    add sp, 2
    test ax, ax
    jz split_empty
    mov es, ax
    mov word ptr [bp-8], ax  ; arr_seg
    mov word ptr [bp-10], 0  ; arr_idx

    ; Второй проход: заполнение массива и копирование строк
    mov ds, dx
    xor si, si
    mov word ptr [bp-4], 0   ; start_idx = 0

split_parse_loop:
    mov al, byte ptr [si]
    test al, al
    jz split_end_str
    cmp al, 0Ah
    je split_end_str

    ; Пропускаем \r
    cmp al, 0Dh
    jne split_parse_next
    inc si
    jmp split_parse_loop

split_parse_next:
    inc si
    jmp split_parse_loop

split_end_str:
    ; Рассчитываем длину строки (без \r и \n)
    mov cx, si
    sub cx, word ptr [bp-4] ; len = curr - start
    test cx, cx
    jz split_skip_empty_line

    ; Выделяем память под строку
    inc cx
    push cx
    call AllocMem
    add sp, 2
    test ax, ax
    jz split_alloc_fail
    mov dx, ax ; сохраняем новый сегмент строки во временный dx

    ; Копируем строку
    push ds
    mov ds, word ptr [bp+arg1]
    mov es, ax
    xor di, di
    mov si, word ptr [bp-4]
copy_line_loop:
    mov al, byte ptr [si]
    cmp al, 0Dh
    je skip_cr_copy
    mov byte ptr es:[di], al
    inc di
skip_cr_copy:
    inc si
    loop copy_line_loop
    mov byte ptr es:[di], 0
    pop ds

    ; Записываем сегмент в массив
    mov di, word ptr [bp-10]
    mov es, word ptr [bp-8]
    mov word ptr es:[di], dx
    add word ptr [bp-10], 2
    inc word ptr [bp-2] ; увеличиваем счетчик реальных строк (если были пустые)

split_skip_empty_line:
    ; Обновляем start_idx
    mov ax, si
    cmp byte ptr [si], 0Ah
    jne split_check_null
    inc ax
split_check_null:
    mov word ptr [bp-4], ax
    mov si, ax
    
    cmp byte ptr [si-1], 0
    jne split_parse_loop

split_done:
    mov cx, word ptr [bp-2]
    mov ax, word ptr [bp-8]
    jmp split_return

split_empty:
    xor ax, ax
    xor cx, cx
    jmp split_return

split_alloc_fail:
    xor ax, ax
    xor cx, cx
    jmp split_return

split_return:
    popregs es, ds, si, di
    mov sp, bp
    pop bp
    ret
split_to_lines endp

; char* strdup(char* string)
strdup proc near
    push bp
    mov bp, sp
    push di
    push si
    push bx
    push es

    push word ptr [bp+arg1]
    call strlen
    add sp, 2

    test ax, ax
    jz strdup_done

    inc ax
    push ax
    call AllocMem
    add sp, 2

    test ax, ax 
    jz strdup_failed
    mov es, ax

    mov si, word ptr [bp+arg1]
    xor di, di
strdup_loop:
    mov bl, byte ptr [si]
    mov byte ptr es:[di], bl
    inc si
    inc di
    test bl, bl
    jnz strdup_loop

    mov ax, es

strdup_done:
    pop es
    pop bx
    pop si
    pop di
    mov sp, bp
    pop bp
    ret

strdup_failed:
    xor ax, ax
    jmp strdup_done
strdup endp

; bool is_white_space(char ch)
is_white_space proc near
    push bp
    mov bp, sp
    
    mov al, byte ptr [bp+arg1]
    cmp al, ' '
    je is_white_space_detected
    cmp al, 09h
    je is_white_space_detected
    cmp al, 0Ah
    je is_white_space_detected
    cmp al, 0Dh
    je is_white_space_detected

    mov ax, 0
    jmp is_white_space_done

is_white_space_detected:
    mov ax, 1

is_white_space_done:
    mov sp, bp
    pop bp
    ret
is_white_space endp

; short strtol (const char* string, char** end_ptr, short base)
strtol proc near
    push bp
    mov bp, sp
    sub sp, 8
    pushregs di, si, cx, bx, dx

    mov word ptr [bp-2], 1        ; знак (1 = положительное)
    mov word ptr [bp-4], 0        ; результат
    mov word ptr [bp-6], 0        ; флаг переполнения

    mov si, word ptr [bp+arg1]    ; исходная строка
    mov di, word ptr [bp+arg2]    ; end_ptr
    mov cx, word ptr [bp+arg3]    ; основание

    ; Пропускаем пробелы
strtol_skip_ws:
    mov al, byte ptr [si]
    test al, al
    jz strtol_empty
    
    cmp al, ' '
    je strtol_ws_next
    cmp al, 9      ; табуляция
    je strtol_ws_next
    cmp al, 10     ; перевод строки
    je strtol_ws_next
    cmp al, 13     ; возврат каретки
    je strtol_ws_next
    jmp strtol_check_sign
    
strtol_ws_next:
    inc si
    jmp strtol_skip_ws

    ; Проверяем знак
strtol_check_sign:
    mov al, byte ptr [si]
    cmp al, '-'
    jne strtol_check_plus
    mov word ptr [bp-2], -1
    inc si
    jmp strtol_check_base
    
strtol_check_plus:
    cmp al, '+'
    jne strtol_check_base
    inc si

    ; Определяем основание если не задано
strtol_check_base:
    cmp cx, 0
    jne strtol_base_ready
    
    mov al, byte ptr [si]
    cmp al, '0'
    jne strtol_base_10
    
    mov al, byte ptr [si+1]
    cmp al, 'x'
    je strtol_base_16
    cmp al, 'X'
    je strtol_base_16
    
    mov cx, 8
    jmp strtol_base_ready
    
strtol_base_16:
    add si, 2
    mov cx, 16
    jmp strtol_base_ready
    
strtol_base_10:
    mov cx, 10

strtol_base_ready:
    cmp cx, 2
    jb strtol_invalid
    cmp cx, 16
    ja strtol_invalid
    
    mov word ptr [bp-8], si       ; сохраняем начало числа

    ; Основной цикл преобразования
strtol_convert:
    mov al, byte ptr [si]
    test al, al
    jz strtol_done
    
    ; Определяем значение цифры
    cmp al, '0'
    jb strtol_not_digit
    cmp al, '9'
    jbe strtol_is_digit
    
    cmp al, 'A'
    jb strtol_not_digit
    cmp al, 'F'
    jbe strtol_is_upper
    
    cmp al, 'a'
    jb strtol_not_digit
    cmp al, 'f'
    jbe strtol_is_lower
    
    jmp strtol_not_digit

strtol_is_digit:
    sub al, '0'
    jmp strtol_check_digit
    
strtol_is_upper:
    sub al, 'A'
    add al, 10
    jmp strtol_check_digit
    
strtol_is_lower:
    sub al, 'a'
    add al, 10

strtol_check_digit:
    cmp al, cl
    jae strtol_not_digit
    
    ; Умножаем результат на основание
    movzx bx, al
    mov ax, word ptr [bp-4]
    mul cx
    
    ; Проверяем переполнение
    cmp dx, 0
    jne strtol_overflow
    
    add ax, bx
    jc strtol_overflow
    
    cmp word ptr [bp-2], 1
    jne strtol_check_neg_limit
    
    ; Проверка для положительных
    cmp ax, 32767
    ja strtol_overflow
    jmp strtol_store
    
strtol_check_neg_limit:
    ; Проверка для отрицательных
    cmp ax, 32768
    ja strtol_overflow

strtol_store:
    mov word ptr [bp-4], ax
    inc si
    jmp strtol_convert

strtol_not_digit:
    cmp si, word ptr [bp-8]
    je strtol_no_digits
    jmp strtol_done

strtol_overflow:
    mov word ptr [bp-6], 1
    inc si
    jmp strtol_convert

strtol_no_digits:
    xor ax, ax
    mov word ptr [bp-4], ax
    mov si, word ptr [bp+arg1]    ; возвращаем исходный указатель

strtol_done:
    ; Устанавливаем end_ptr
    mov bx, word ptr [bp+arg2]
    mov word ptr [bx], si
    
    ; Получаем результат
    mov ax, word ptr [bp-4]
    
    ; Проверяем переполнение
    cmp word ptr [bp-6], 1
    jne strtol_apply_sign
    
    ; Возвращаем предельное значение
    cmp word ptr [bp-2], 1
    je strtol_overflow_pos
    mov ax, 32768
    neg ax
    jmp strtol_return
    
strtol_overflow_pos:
    mov ax, 32767
    jmp strtol_return

strtol_apply_sign:
    cmp word ptr [bp-2], -1
    jne strtol_return
    neg ax

strtol_return:
    popregs dx, bx, cx, si, di
    mov sp, bp
    pop bp
    ret

strtol_invalid:
strtol_empty:
    xor ax, ax
    mov bx, word ptr [bp+arg2]
    mov word ptr [bx], si
    jmp strtol_return
    
strtol endp

; int stricmp (const char* string1, const char* string2)
stricmp proc near
    push bp
    mov bp, sp
    push si
    push di

    mov si, word ptr [bp+arg1]
    mov di, word ptr [bp+arg2]

stricmp_loop:
    mov al, byte ptr [si]
    mov ah, byte ptr [di]
    
    ; Преобразуем AL в верхний регистр
    cmp al, 'a'
    jb stricmp_check_ah
    cmp al, 'z'
    ja stricmp_check_ah
    sub al, 20h
    
stricmp_check_ah:
    ; Преобразуем AH в верхний регистр
    cmp ah, 'a'
    jb stricmp_compare
    cmp ah, 'z'
    ja stricmp_compare
    sub ah, 20h
    
stricmp_compare:
    cmp al, ah
    jne stricmp_diff
    test al, al
    jz stricmp_equal
    inc si
    inc di
    jmp stricmp_loop

stricmp_diff:
    movzx ax, al
    movzx bx, ah
    sub ax, bx
    jmp stricmp_done

stricmp_equal:
    xor ax, ax

stricmp_done:
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
stricmp endp

; char* strchr(char* string, int c)
strchr proc near
    push bp
    mov bp, sp
    push di
    mov di, word ptr [bp+arg1]
    mov al, byte ptr [bp+arg2]
strchr_loop:
    cmp byte ptr [di], al
    je strchr_found
    cmp byte ptr [di], 0
    je strchr_not_found
    inc di
    jmp strchr_loop
strchr_found:
    mov ax, di
    jmp strchr_done
strchr_not_found:
    xor ax, ax
strchr_done:
    pop di
    mov sp, bp
    pop bp
    ret
strchr endp

; char* strstr(char* string, char* needle)
strstr proc near
    push bp
    mov  bp, sp
    pushregs 
    push si
    push di
    push bx

    mov  di, word ptr [bp+arg2]        ; DI = string
    mov  si, word ptr [bp+arg1]        ; SI = needle
    mov  bx, di        

    cmp  byte ptr [di], 0
    je   strstr_di_zero
    cmp  byte ptr es:[si], 0 
    jne  strstr_not_empty
    mov  ax, di        
    jmp  strstr_done

strstr_not_empty:
strstr_search_loop:
    mov  di, bx          
    mov  si, word ptr [bp+arg1]      

strstr_compare:
    mov  cl, byte ptr [di]   
    mov  ch, byte ptr es:[si]     

    test ch, ch
    jz   strstr_success 

    cmp  cl, ch
    jne  strstr_next_position    

    test cl, cl         
    jz   strstr_success

    inc  di  
    inc  si 
    jmp  strstr_compare

strstr_next_position:
    inc  bx 
    cmp  byte ptr [bx], 0
    jnz  strstr_search_loop

    xor  ax, ax 
    jmp  strstr_done

strstr_success:
    mov  ax, bx  

strstr_done:
    pop  bx
    pop  di
    pop  si
    mov  sp, bp
    pop  bp
    ret

strstr_di_zero:
    mov ax, di
    jmp strstr_done
strstr endp

; int strcmp(const char* string1, const char* string2)
strcmp proc near
    push bp
    mov bp, sp
    push si
    push di
    mov si, word ptr [bp+arg1]
    mov di, word ptr [bp+arg2]
strcmp_loop:
    mov al, byte ptr [si]
    mov ah, byte ptr [di]
    cmp al, ah
    jne strcmp_diff
    test al, al
    jz strcmp_equal
    inc si
    inc di
    jmp strcmp_loop
strcmp_diff:
    movzx ax, al
    movzx bx, ah
    sub ax, bx
    jmp strcmp_done
strcmp_equal:
    xor ax, ax
strcmp_done:
    pop di
    pop si
    mov sp, bp
    pop bp
    ret
strcmp endp

; char* strcpy(char* dest, const char* src)
strcpy proc near
    push bp
    mov bp, sp
    push di
    push si
    mov di, word ptr [bp+arg1]
    mov si, word ptr [bp+arg2]
strcpy_loop:
    mov dl, byte ptr [si]
    mov byte ptr [di], dl
    test dl, dl
    jz strcpy_done
    inc si
    inc di
    jmp strcpy_loop
strcpy_done:
    mov ax, word ptr [bp+arg1]
    pop si
    pop di
    mov sp, bp
    pop bp
    ret
strcpy endp

; int strlen(const char *str)
strlen proc near
    push bp
    mov bp, sp
    push di
    mov di, word ptr [bp+arg1]
    xor ax, ax
strlen_loop:
    cmp byte ptr [di], 0
    jz strlen_done
    inc ax
    inc di
    jmp strlen_loop
strlen_done:
    pop di
    mov sp, bp
    pop bp
    ret
strlen endp
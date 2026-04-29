; Макросы для сохранения/восстановления регистров
pushregs macro reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9, reg10
    ifnb <reg1>
        push reg1
        pushregs reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9, reg10
    endif
endm

popregs macro reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9, reg10
    ifnb <reg1>
        pop reg1
        popregs reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9, reg10
    endif
endm

; Макросы для аргументов и локальных переменных
arg1 equ 4
arg2 equ 6
arg3 equ 8
arg4 equ 10
var1 equ -2
var2 equ -4
var3 equ -6
var4 equ -8
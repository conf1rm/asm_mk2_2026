InitMem proc near 
	push bp
	mov bp, sp
	push es
	
	mov bx, 0fff0h
	mov ax, [bp+arg3]
	add ax, 0fh
	and bx, ax
	shr bx, 4
	
	mov ax, [bp+arg2]
	add bx, ax
	mov ax, [bp+arg1]
	sub bx, ax
	mov ah, 4ah
	int 21h
	jnc _IM_success

	_IM_fail:
	xor ax, ax
	jmp _IM_exit
	
	_IM_success:
	mov ax, 1
	_IM_exit:
	pop es
	mov sp, bp
	pop bp
	ret
InitMem endp

AllocMem proc near	
	_AllocMem_localSize equ 1*2
	push bp
	mov bp, sp
	sub sp, _AllocMem_localSize
	
	mov bx, 0fff0h
	mov ax, [bp+arg1]
	add ax, 0fh
	and bx, ax
	shr bx, 4
	mov [bp+var1], bx
	
	mov bx, 0FFFFh
	mov ah, 48h
	int 21h
	cmp bx, [bp+var1]
	jb _AM_fail
	
	mov ah, 48h
	mov bx, [bp+var1]
	int 21h
	jnc _AM_exit

	_AM_fail:
	mov ax, 0
	_AM_exit:
	add sp, _AllocMem_localSize
	mov sp, bp
	pop bp
	ret
AllocMem endp

FreeMem proc near
	push bp
	mov bp, sp
	push es
	
	mov es, [bp+arg1]
	mov ah, 49h
	int 21h
	jnc _FM_exit

	stc
	
_FM_exit:
	pop es
	mov sp, bp
	pop bp
	ret
FreeMem endp

ReallocMem proc near
	_ReallocMem_localSize equ 1*2
	push bp
	mov bp, sp
	sub sp, _ReallocMem_localSize
	push es
	
	mov bx, 0fff0h
	mov ax, [bp+arg2]
	add ax, 0fh
	and bx, ax
	shr bx, 4
	
	mov [bp+var1], bx
	mov es, [bp+arg1]
	mov ah, 4ah
	int 21h
	jnc _RM_exit
	
_RM_exit:
	pop es
	add sp, _ReallocMem_localSize
	mov sp, bp
	pop bp
	ret
ReallocMem endp

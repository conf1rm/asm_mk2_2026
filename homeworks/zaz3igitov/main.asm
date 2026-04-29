; Функции для ввода-вывода строк/символов (используется соглашение cdecl)
.386

ACCESS_READ     EQU 0

SEEK_START equ 0
SEEK_CURRENT_POS equ 1
SEEK_END equ 2

arg1 equ 4
arg2 equ 6
arg3 equ 8
arg4 equ 10

var1 equ -2
var2 equ -4
var3 equ -6
var4 equ -8

SHRT_MAX equ 32767
SHRT_MIN equ -32768

stack segment para stack use16
db 65530 dup(?)
stack ends

data segment para public use16

; errors
    ERROR_ALLOC equ 0
    ERROR_OPEN equ 1
    ERROR_LSEEK equ 2

    err_alloc db "failed to allocate memory", 0dh, 0ah, 0
    err_open db "failed to open file", 0dh, 0ah, 0
    err_lseek db "failed to lseek file", 0dh, 0ah, 0
    err_vec dw offset err_alloc, offset err_open, offset err_lseek

    fio_err_no_error        db " ", 0
    fio_err_invalid_func    db "The specified function is not supported", 0dh, 0ah, 0
    fio_err_not_found       db "The requested file could not be located", 0dh, 0ah, 0
    fio_err_path_not_found  db "The specified directory path does not exist", 0dh, 0ah, 0
    fio_err_too_many_open   db "The system has reached the limit of simultaneously open files", 0dh, 0ah, 0
    fio_err_access_denied   db "You do not have permission to perform this operation", 0dh, 0ah, 0
    fio_err_invalid_handle  db "The provided file handle is not recognized", 0dh, 0ah, 0
    fio_err_mcb_destroyed   db "The internal memory control structure has been corrupted", 0dh, 0ah, 0
    fio_err_insufficient_mem db "There is not enough free memory to complete this request", 0dh, 0ah, 0
    fio_err_invalid_mem_block db "The memory block address supplied is incorrect", 0dh, 0ah, 0
    fio_err_invalid_env     db "The program environment block appears to be invalid", 0dh, 0ah, 0
    fio_err_invalid_format  db "The file or data format is not recognized", 0dh, 0ah, 0
    fio_err_invalid_access  db "The specified access mode code is invalid", 0dh, 0ah, 0
    fio_err_invalid_data    db "The provided data is malformed or corrupted", 0dh, 0ah, 0
    fio_err_reserved        db "A reserved system error has occurred", 0dh, 0ah, 0
    fio_err_invalid_drive   db "The drive letter you specified is not valid", 0dh, 0ah, 0
    fio_err_remove_cur_dir  db "It is not possible to delete the current working directory", 0dh, 0ah, 0
    fio_err_not_same_device db "The source and destination paths refer to different devices", 0dh, 0ah, 0
    fio_err_no_more_files   db "There are no additional files matching the search criteria", 0dh, 0ah, 0
    fio_err_write_protected db "The target disk or media is currently write-protected", 0dh, 0ah, 0
    fio_err_unknown_unit    db "The requested disk drive or unit does not exist", 0dh, 0ah, 0
    fio_err_drive_not_ready db "The drive is not ready; please check the disk and try again", 0dh, 0ah, 0
    fio_err_unknown_cmd     db "The device does not recognize the command that was sent", 0dh, 0ah, 0
    fio_err_crc_error       db "A cyclic redundancy check (CRC) error has been detected", 0dh, 0ah, 0
    fio_err_bad_req_len     db "The length of the request structure is invalid", 0dh, 0ah, 0
    fio_err_seek_error      db "Unable to reposition the file pointer to the desired location", 0dh, 0ah, 0
    fio_err_unknown_media   db "The type of media in the drive cannot be determined", 0dh, 0ah, 0
    fio_err_sector_not_found db "The requested disk sector was not found on the media", 0dh, 0ah, 0
    fio_err_printer_out_paper db "The printer is out of paper and cannot continue", 0dh, 0ah, 0
    fio_err_invalid_device_req db "The requested operation is not supported for this device type", 0dh, 0ah, 0
    fio_err_read_fault      db "A critical error occurred while attempting to read data", 0dh, 0ah, 0
    fio_err_general_failure db "An unspecified general hardware or system failure has occurred", 0dh, 0ah, 0
    fio_err_unknown         db "An unrecognized DOS error condition has been encountered", 0dh, 0ah, 0

    fio_err_vec dw offset fio_err_no_error
                dw offset fio_err_invalid_func
                dw offset fio_err_not_found
                dw offset fio_err_path_not_found
                dw offset fio_err_too_many_open
                dw offset fio_err_access_denied
                dw offset fio_err_invalid_handle
                dw offset fio_err_mcb_destroyed
                dw offset fio_err_insufficient_mem
                dw offset fio_err_invalid_mem_block
                dw offset fio_err_invalid_env
                dw offset fio_err_invalid_format
                dw offset fio_err_invalid_access
                dw offset fio_err_invalid_data
                dw offset fio_err_reserved
                dw offset fio_err_invalid_drive
                dw offset fio_err_remove_cur_dir
                dw offset fio_err_not_same_device
                dw offset fio_err_no_more_files
                dw offset fio_err_write_protected
                dw offset fio_err_unknown_unit
                dw offset fio_err_drive_not_ready
                dw offset fio_err_unknown_cmd
                dw offset fio_err_crc_error
                dw offset fio_err_bad_req_len
                dw offset fio_err_seek_error
                dw offset fio_err_unknown_media
                dw offset fio_err_sector_not_found
                dw offset fio_err_printer_out_paper
                dw offset fio_err_invalid_device_req
                dw offset fio_err_read_fault
                dw offset fio_err_general_failure
   

	str1 db 256 dup(?)
	str2 db "Hello, World!", 0

    test_str1       db "Hello", 0
    test_str2       db "Hello, World!", 0
    test_str3       db "world", 0
    test_str4       db 0
    test_str5       db "abc", 0
    test_str6       db "abd", 0
    test_str7       db "ab", 0
    test_str8       db "abcdef", 0
    test_str9       db "cde", 0
    test_buffer     db 256 dup(?)

    test_str_upper db "ABC", 0
    test_str_apple db "apple", 0
    test_str_banana db "BANANA", 0
	test_str_cat_res db "Helloabd", 0
    

    test_strtol1   db "123",0
    test_strtol2   db "-456",0
    test_strtol3   db "  +789",0
    test_strtol4   db "FF",0
    test_strtol5   db "0x10",0
    test_strtol6   db "077",0
    test_strtol7   db "10",0
    test_strtol8   db "123abc",0
    test_strtol9   db "abc",0
    test_strtol10  db "32767",0
    test_strtol11  db "-32768",0
    test_end_ptr   dw ?


    msg_pass        db " PASS", 13, 10, 0
    msg_fail        db " FAIL", 13, 10, 0
    msg_strlen      db "Test strlen", 0
    msg_strchr      db "Test strchr", 0
    msg_strstr      db "Test strstr", 0
    msg_strcmp      db "Test strcmp", 0
    msg_strcpy      db "Test strcpy", 0
    msg_stricmp     db "Test stricmp", 0
    msg_strtol      db "Test strtol ", 0
    msg_strdup      db "Test strdup", 0
	msg_strcat      db "Test strcat", 0

    msg_loaded_str  db "Loaded string from file: ", 13, 10, 0
    msg_filed_to_find_delimiter  db "failed to find delimiter '|' in string!", 13, 10, 0



data ends

code segment para public use16

assume cs:code,ds:data,ss:stack, es:data

include macro.inc

include strings.inc
include memory.inc
include misc.inc

include io.inc
include fio.inc
include error.inc

include tests.inc

_test proc near
    push bp
    mov bp, sp   

    call putnewline

    call test_strtol
    call test_strlen
    call test_strchr
    call test_strstr
    call test_strcmp
    call test_stricmp
    call test_strcpy
	call test_strcat 
    call putnewline


    mov sp, bp
    pop bp
    ret
_test endp
 
start:
    mov ax, data
    mov ds, ax
    mov es, ax
    mov ax, stack
    mov ss, ax
	nop

    push offset end_code_seg
    push cs
    push es
    call InitMem
    add sp, 6 

    call _test
    
    call exit0
end_code_seg:
code ends

end start
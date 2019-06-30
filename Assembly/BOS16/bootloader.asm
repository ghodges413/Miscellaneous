;
;   The bootloader is loaded from a very specific part of memory
;
;   Compile this file with the command:
;       nasm.exe bootloader.asm -f bin -o bootloader.bin
;       nasm.exe kernel.asm -f bin -o kernel.bin
;
;   Concatenate multiple files together with the copy command:
;       copy /b bootloader.bin+kernel.bin bos16.bin
;
;   Run the binary in bochs x86 emulator by loading the bochsrc.bxrc file
;
;   To run from usb stick use the command:
;       dd if=c:\my files\boot.bin of=\\.\z: bs=512 count=1
;   to ensure the copy to usb will put the file at the right address.
;   http://www.chrysocome.net/dd
;

[bits 16]               ; Designate that this file should be compiled to 16-bit assembly
[org 0x7c00]            ; This should designate to the processor that we expect the program to be run at 0x7c00.

mov ah, 0x0e            ; int 10/ah = 0eh -> scrolling teletype BIOS routine
mov al, [message]       ; This loads the value stored at the address into al
int 0x10                ; Print the value in AL

mov bx, HELLO_MSG       ; Print the Hello World message from print.asm
call print_string

mov dx, 0xbadf
call print_hex
mov dx, 0x0000

mov bx, loading_msg
call print_string

; Read some sectors from the boot disk using our disk_read function
mov [BOOT_DRIVE], dl ; BIOS stores our boot drive in DL , so it ’s
                    ; best to remember this for later.
mov bp, 0x8000      ; Here we set our stack safely out of the
mov sp, bp          ; way, at 0x8000
mov bx, 0x9000      ; Load 5 sectors to 0x0000(ES):0x9000(BX)
mov dh, 5           ; from the boot disk.
mov dl, [BOOT_DRIVE]
call disk_load
mov dx, [0x9000]    ; Print out the first loaded word, which
call print_hex      ; we expect to be 0xdada, stored
                    ; at address 0x9000
mov dx, [0x9000 + 512]  ; Also , print the first word from the
call print_hex          ; 2nd loaded sector: should be 0xface

jmp 0x9400 ; + 1024     ; The kernel should be in the third loaded sector
jmp $

%include "utilities/print.asm"
%include "utilities/diskio.asm"

; Global variables
BOOT_DRIVE:     db 0
message:        db "X"
loading_msg:    db " Loading OS ", 10, 13, 0
failure_msg:    db "FAILED TO LOAD OS", 10, 13, 0
my_msg:         db "my message", 10, 13, 0

times 510-($-$$) db 0   ; This program needs to fit into 512 bytes, with the last two bytes being the bootloader's magic number.
                        ; "times x instruction" tells NASM to repeat the instruction x times.  It's NASM's version of DUP in MASM.
                        ; "db" means "declare bytes of data" which informs the assembler to just write bytes into the compiled file.

dw 0xaa55               ; This is the magic number.  This will be stored in the last 2 bytes of the 512 byte file, so the BIOS
                        ; will know that this is the bootloader.

; Place added data in this file so that we can test reading from disk                        
times 256 dw 0xdada
times 256 dw 0xface

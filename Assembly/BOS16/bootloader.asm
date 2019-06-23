;
;   The bootloader is loaded from a very specific part of memory
;
;   Compile this file with the command: nasm.exe bootloader.asm -f bin -o bootloader.bin
;
;   Run the binary in bochs x86 emulator by loading the bochsrc.bxrc file
;
;   To run from usb stick use the command: dd if=c:\my files\boot.bin of=\\.\z: bs=512 count=1
;   to ensure the copy to usb will put the file at the right address.
;   http://www.chrysocome.net/dd
;

; Memory layout after the power on selft test (POST)
;
; 0x000000 - Interrupt Vector Table ( 1 KB )
; 0x000400 - BIOS Data Area ( 256 bytes )
; 0x000500 - Free
; 0x007C00 - Loaded Boot Sector ( 512 bytes )
; 0x007E00 - Free
; 0x09FC00 - Extended BIOS Data Area ( 639 KB )
; 0x0A0000 - Video Memory ( 128 KB )
; 0x0C0000 - BIOS ( 256 KB )
; 0x100000 - Free for the rest of memory

; ax - Accumulator register (AX). Used in arithmetic operations
; bx - Base register (BX). Used as a pointer to data (located in segment register DS, when in segmented mode).
; cx - Counter register (CX). Used in shift/rotate instructions and loops.
; dx - Data register (DX). Used in arithmetic operations and I/O operations.

; si - Source Index register (SI). Used as a pointer to a source in stream operations.
; di - Destination Index register (DI). Used as a pointer to a destination in stream operations.

; bp is the base pointer, and it points to the base address of the stack
; sp is the stack pointer, and it grows down (decrements) from the stack's base pointer
; This means it makes sense to start the stack at the end of avialable memory
; and let it grow "downwards".

; Segment registers.  These allow us to access more than 64kb of memory when in 16-bit real mode
; cs    - code segment - offset for code
; ds    - data segment - this is the offset used for addressing general memory (such as strings that we have stored)
; ss    - stack segment - this is used to modify the stack base pointer (bp)
; es    - extra segment
; fs    - another extra segment - not available in 16-bit real mode
; gs    - another extra segment - not available in 16-bit real mode

[bits 16]               ; Designate that this file should be compiled to 16-bit assembly
[org 0x7c00]            ; This should designate to the processor that we expect the program to be run at 0x7c00.

mov ah, 0x0e            ; int 10/ah = 0eh -> scrolling teletype BIOS routine
mov al, [message]       ; This loads the value stored at the address into al
int 0x10                ; Print the value in AL

mov bx, HELLO_MSG       ; Print the Hello World message from print.asm
call print_string

mov dx, 0xbadf
call print_hex

loop:                   ; Defines a label, we'll call it "loop" so that we can call it
jmp loop                ; jump to loop label, which takes us back to this location and hits the jmp instruction again (infinite loop)

%include "utilities/print.asm"

message:    db "X"

times 510-($-$$) db 0   ; This program needs to fit into 512 bytes, with the last two bytes being the bootloader's magic number.
                        ; "times x instruction" tells NASM to repeat the instruction x times.  It's NASM's version of DUP in MASM.
                        ; "db" means "declare bytes of data" which informs the assembler to just write bytes into the compiled file.

dw 0xaa55               ; This is the magic number.  This will be stored in the last 2 bytes of the 512 byte file, so the BIOS
                        ; will know that this is the bootloader.
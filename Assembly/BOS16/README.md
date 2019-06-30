# BOS16

A very simple 16-bit operating system.  Nothing special.
Just go through the basic boot loading process.  And then,
see if we can load some programs on disk and run them.

Some useful resources:

https://appusajeev.wordpress.com/2011/01/27/writing-a-16-bit-real-mode-os-nasm/

"Writing a Simple Operating System from Scratch" by Nick Blundell

https://linuxgazette.net/82/raghu.html

https://github.com/cfenollosa/os-tutorial









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

;
;   Registers
;

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

;
;   Basic Commands
;

;   db = declare bytes of data
;   dw = declare word
;   dd = declare double word
;
;   mov x, y = move the value y into x
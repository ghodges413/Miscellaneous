;
;   The bootloader is loaded from a very specific part of memory
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
; es    - extra segment - no idea what this is for yet
; fs    - another extra segment - not available in 16-bit real mode
; gs    - another extra segment - not available in 16-bit real mode

loop:                   ; Defines a label, we'll call it "loop" so that we can call it
jmp loop                ; jump to loop label, which takes us back to this location and hits the jmp instruction again (infinite loop)

times 510-($-$$) db 0   ; This program needs to fit into 512 bytes, with the last two bytes being the bootloader's magic number.
                        ; "times x instruction" tells NASM to repeat the instruction x times.  It's NASM's version of DUP in MASM.
                        ; "db" means "declare bytes of data" which informs the assembler to just write bytes into the compiled file.

dw 0xaa55               ; This is the magic number.  This will be stored in the last 2 bytes of the 512 byte file, so the BIOS
                        ; will know that this is the bootloader.
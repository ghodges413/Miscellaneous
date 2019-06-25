;
;   kernel
;
[bits 16]
;[org 0]
[org 0x9400]

;================================
; main
; This is the entry point into the operating system
;================================
main:   ; TODO: We should probably setup stack pointers and segments here

        mov ah, 0x0e    ; int 10/ah = 0eh -> scrolling teletype BIOS routine
        mov al, 'b'     ; This loads the value stored at the address into al
        int 0x10        ; Print the value in AL

        mov     bx, WELCOME_MSG
		call    print_string
        mov     bx, DAS_MSG
        call    print_string
        mov     ah, 0           ; read keyboard interrupt
        int     16h             ; read keyboard interrupt

game_loop:  mov     bx, REPEAT_MSG
            call    print_string
            mov     ah, 0           ; read keyboard interrupt
		    int     16h             ; read keyboard interrupt
            jmp     game_loop

%include "utilities/print.asm"

WELCOME_MSG:    db "Welcome to your OS", 10, 13, 0       ; Null terminated string
REPEAT_MSG:     db "Repeated", 10, 13, 0
DAS_MSG:        db "press something to continue", 10, 13, 0
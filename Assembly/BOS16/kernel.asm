;
;   kernel
;
[bits 16]
;[org 0]
[org 0x9400]

;================================
;   main
;   This is the entry point into the operating system
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

game_loop:  mov     bx, SHELL_PROMPT
            call    print_string
            push    USER_COMMAND
			call    read_user_input
            jmp     game_loop

;================================
;   read_user_input
;   Reads in whatever the user types and stores it
;================================
read_user_input:    pusha
                    mov bp, sp
                    cld         ; clear direction flag
                    mov byte  [CHARACTER_COUNT], 0
                    mov di, [bp + 64]
                    mov bx, 0
                    mov es, bx  ; can't directly set ES, so we set BX and then set ES

input_loop: mov     ah, 0       ; read keyboard interrupt
            int     16h         ; read keyboard interrupt
            cmp     al, 0x0d    ; check for carriage return (0dh)
            jz      echo_input   ; stop looping if user hit enter
            mov     ah, 0x0e    ; int 10/ah = 0eh -> scrolling teletype BIOS routine
            mov     bx, 0       ; not sure why bx needs to be zero
            int     10h         ; print the value in al
            stosb               ; store string (Store AL at address ES:(E)DI.)
            inc     byte [CHARACTER_COUNT]    ; increment the character count
            jmp     input_loop	
            
echo_input: mov     al, 0           ; make sure there's a null terminating character
            stosb                   ; make sure there's a null terminating character
            mov     bx, ECHO_MSG
            call    print_string
            mov     bx, [bp + 64]   ; print what the user input
            call    print_string   ; print what the user input
            mov     sp, bp
            popa
            ret

%include "utilities/print.asm"

WELCOME_MSG:    db "Welcome to your OS", 10, 13, 0       ; Null terminated string
REPEAT_MSG:     db "Repeated", 10, 13, 0
DAS_MSG:        db "press something to continue", 10, 13, 0

SHELL_PROMPT:   db 10, 13, "$ ", 0
USER_COMMAND:   times 64 db 0
ECHO_MSG:       db 10,13, "echo ", 0
CHARACTER_COUNT:    dw 0
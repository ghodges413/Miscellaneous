;
;   print
;

;================================
; print_string
;================================
print_string:   push    ax
                mov     ah, 0x0e    ; int 10/ ah = 0eh -> scrolling teletype BIOS routine
print_loop:     mov     al, [bx]    ; dereference the address stored in bx and store in al
                int     0x10        ; print al
                add     bx, 0x01    ; increment the address stored in bx
                cmp     al, 0       ; check for the null terminator
                jg      print_loop  ; loop if we haven't hit a null terminator yet
                pop     ax
                ret

;
;   Data
;
HELLO_MSG:      db "Hello, World!", 0       ; Null terminated string
GOODBYE_MSG:    db "Goodbye!", 0            ; Null terminated string
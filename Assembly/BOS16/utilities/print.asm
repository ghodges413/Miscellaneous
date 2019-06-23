;
;   print
;

;================================
; print_string
; print the string that's pointed to by register bx
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

;================================
; hex_to_string
; print the value stored in dx
;================================
hex_to_string:  mov     cx, 0           ; the index variable
hex_loop:       mov     ax, dx
                and     ax, 0x000f      ; mask out the first 12 bits
                cmp     al, 0x0a
                jl      std_inc
                add     al, 7           ; the letters a-f need an extra 7 to get to the correct ASCII value
std_inc:        add     al, 0x30        ; increment the value to the appropriate ASCII value
                mov     bx, HEX_OUT + 5 ; set the address for the output
                sub     bx, cx          ; offset the address by the current index
                mov     [bx], al        ; store the value in al at the memory address pointed to by bx
                ror     dx, 4           ; rotate values right, similar to shr: 0x1234 -> 0x4123
                add     cx, 1           ; increment the index
                cmp     cx, 4           ; check if we should continue looping
                jl      hex_loop
                ret

;================================
; print_hex
; print the value stored in dx
;================================
print_hex:  pusha
            call    hex_to_string
end:        mov     bx, HEX_OUT
            call    print_string
            popa
            ret

;
;   Data
;
HELLO_MSG:      db "Hello, World!", 0       ; Null terminated string
GOODBYE_MSG:    db "Goodbye!", 0            ; Null terminated string
HEX_OUT:        db "0x0000", 0
HEX_TABLE:      db "0123456789ABCDEF", 0
HEX_STRING:     resb  50                    ; reserve 50 bytes for the string
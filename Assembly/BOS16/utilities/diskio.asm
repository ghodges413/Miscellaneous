;
;   diskio
;

; For loading sectors off the disk to RAM, bios provides an interrupt,
; INT 0x13 and the parameters to set are
; AH = 2
; DL = drive,  DL = 0x80 for hard disk and this applies to our pen drive
; DH = head number
; CH = track number
; CL= sector number of the sector to be loaded
; AL = number of sectors to load
; ES = segment to load the sector
; BX = offset from ES where the sector is to be loaded

%define drive		0x80

;================================
; loadsector
; This one is from appsuajeev's blog post
;================================
loadsector: mov     bx, 0
		    mov     dl, drive   ; drive
		    mov     dh, 0       ; head
		    mov     ch, 0       ; track
		    mov     ah, 2
		    int     0x13
		    jc      err
		    ret
err:        mov     bx, error_loading
		    call    print_string
		    ret

error_loading db "Error loading sector ", 10, 13, 0

;================================
; disk_load
; this is from nick blundell's book
;================================
; load DH sectors to ES:BX from drive DL
disk_load:  push    dx  ; Store DX on stack so later we can recall
                        ; how many sectors were request to be read ,
                        ; even if it is altered in the meantime
            mov     ah, 0x02    ; BIOS read sector function
            mov     al, dh      ; Read DH sectors
            mov     ch, 0x00    ; Select cylinder 0
            mov     dh, 0x00    ; Select head 0
            mov     cl, 0x02    ; Start reading from second sector ( i.e. after the boot sector )
            int     0x13        ; BIOS interrupt
            jc      disk_error0 ; Jump if error ( i.e. carry flag set )
            pop     dx          ; Restore DX from the stack
            cmp     dh, al      ; if AL ( sectors read ) != DH ( sectors expected )
            jne     disk_error1 ; display error message
            ret
disk_error0:    mov     bx, DISK_ERROR_MSG0
                call    print_string
                jmp     $
disk_error1:    mov     bx, DISK_ERROR_MSG1
                call    print_string
                jmp     $

; Variables
DISK_ERROR_MSG0 db "Disk read error! Carry flag was set", 10, 13, 0
DISK_ERROR_MSG1 db "Disk read error! Number of sectors read was unexpected", 10, 13, 0


%include "src/boot/macros.asm"

gdt64:
.null:
    dq 0
.code: equ $ - gdt64
    dw 0xffff                                       ; Limit 0-15
    dw 0                                            ; Base 16-31
    db 0                                            ; Base 32-39
    db GDT_AB_RW | GDT_AB_E | GDT_AB_S | GDT_AB_P   ; Access Byte 40-47
    db 0x0f | (GDT_FLAG_L<<4)                       ; Limit (48-51) and Flags (52-55)
    db 0                                            ; Base 56-64
.data: equ $ - gdt64
    dw 0xffff                                       ; Limit 0-15
    dw 0                                            ; Base 16-31
    db 0                                            ; Base 32-39
    db GDT_AB_RW | GDT_AB_S | GDT_AB_P              ; Access Byte 40-47
    db 0x0f | (0<<4)                                ; Limit (48-51) and Flags (52-55)
    db 0                                            ; Base 56-64
.pointer:
    dw .pointer - gdt64 - 1
    dq gdt64

gdt32:
.null:
    dq 0
.code32: equ $ - gdt32
    dw 0xffff                                       ; limit 0-15
    dw 0                                            ; base 16-31
    db 0                                            ; base 32-39
    db GDT_AB_RW | GDT_AB_E | GDT_AB_S | GDT_AB_P   ; Access Byte 40-47
    db 0x00 | ((GDT_FLAG_DB | GDT_FLAG_G)<<4)       ; Limit (48-51) and Flags (52-22)
    db 0                                            ; Base 56-64
.data32: equ $ - gdt32
    dw 0xffff                                       ; limit 0-15
    dw 0                                            ; base 16-31
    db 0                                            ; base 32-39
    db GDT_AB_RW | GDT_AB_S | GDT_AB_P              ; Access Byte 40-47
    db 0x00 | ((GDT_FLAG_DB | GDT_FLAG_G)<<4)       ; Limit (48-51) and Flags (52-22)
    db 0                                            ; Base 56-64
.code16: equ $ - gdt32
    dw 0xffff                                       ; limit 0-15
    dw 0                                            ; base 16-31
    db 0                                            ; base 32-39
    db GDT_AB_RW | GDT_AB_E | GDT_AB_S | GDT_AB_P   ; Access Byte 40-47
    db 0x0f | (0<<4)                                ; Limit (48-51) and Flags (52-22)
    db 0                                            ; Base 56-64
.data16: equ $ - gdt32
    dw 0xffff                                        ; limit 0-15
    dw 0                                             ; base 16-31
    db 0                                             ; base 32-39
    db GDT_AB_RW | GDT_AB_S | GDT_AB_P               ; Access Byte 40-47
    db 0x0f | (0<<4)                                 ; Limit (48-51) and Flags (52-22)
    db 0                                             ; Base 56-64
.pointer:
    dw .pointer - gdt32 - 1                          ; size of gdt minus 1
    dq gdt32                                         ; offset

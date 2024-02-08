%ifndef __GDT
%define __GDT

%include "src/boot/macros.asm"

global _gdt
section .vmm
_gdt:
.null:
    dq 0
.code64: equ $ - _gdt
    dw 0xffff                                       ; Limit 0-15
    dw 0                                            ; Base 16-31
    db 0                                            ; Base 32-39
    db GDT_AB_A | GDT_AB_RW | GDT_AB_E | GDT_AB_S | GDT_AB_P   ; Access Byte 40-47
    db 0x0f | (GDT_FLAG_L<<4)                       ; Limit (48-51) and Flags (52-55)
    db 0                                            ; Base 56-64
.data64: equ $ - _gdt
    dw 0xffff                                       ; Limit 0-15
    dw 0                                            ; Base 16-31
    db 0                                            ; Base 32-39
    db GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P              ; Access Byte 40-47
    db 0x0f | (0<<4)                                ; Limit (48-51) and Flags (52-55)
    db 0                                            ; Base 56-64
.code32: equ $ - _gdt
    dw 0xffff                                       ; limit 0-15
    dw 0                                            ; base 16-31
    db 0                                            ; base 32-39
    db GDT_AB_A | GDT_AB_RW | GDT_AB_E | GDT_AB_S | GDT_AB_P   ; Access Byte 40-47
    db 0x0f | ((GDT_FLAG_G | GDT_FLAG_DB)<<4)                      ; Limit (48-51) and Flags (52-22)
    db 0                                            ; Base 56-64
.data32: equ $ - _gdt
    dw 0xffff                                       ; limit 0-15
    dw 0                                            ; base 16-31
    db 0                                            ; base 32-39
    db GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P              ; Access Byte 40-47
    db 0x0f | ((GDT_FLAG_G | GDT_FLAG_DB)<<4)                      ; Limit (48-51) and Flags (52-22)
    db 0                                            ; Base 56-64
.code16: equ $ - _gdt
    dw 0xffff                                       ; limit 0-15
    dw 0                                            ; base 16-31
    db 0                                            ; base 32-39
    db GDT_AB_A | GDT_AB_RW | GDT_AB_E | GDT_AB_S | GDT_AB_P   ; Access Byte 40-47
    db 0x0f | (GDT_FLAG_G<<4)                                ; Limit (48-51) and Flags (52-22)
    db 0                                            ; Base 56-64
.data16: equ $ - _gdt
    dw 0xffff                                       ; limit 0-15
    dw 0                                            ; base 16-31
    db 0                                            ; base 32-39
    db GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P              ; Access Byte 40-47
    db 0x0f | (GDT_FLAG_G<<4)                                ; Limit (48-51) and Flags (52-22)
    db 0                                            ; Base 56-64
.pointer:
    dw .pointer - _gdt - 1                          ; size of gdt minus 1
    dq _gdt                                         ; offset

%endif

%define DAP_ADDRESS 0x2fef
%define DRIVE_IDX_ADDRESS 0x2fff

global ReadDisk
global JumpToGuest

section .text

bits 16
JumpToGuest:
    ; vmcall
    ; println "here"
    ; in al, 0x70

    ; mov al, 0xb5
    ; mov ebx, 0xf7d2a
    ; mov ecx, 0x1234
    ; out 0xb2, al
    ; nop
    ; hlt

    ; mov ax, 0xbb00
    ; int 0x1a
    ; push 0xf000
    ; push 0xd009
    ; push 0xcb00
    ; push 0x0018
    ; retf

    ; println "Success!"
    ; hlt

    ; mov ebx, 0xffe6e
    ; mov byte [ebx], 0xcb ; retf
    ; push word 0x0000
    ; push word REAL_ADDR(JumpToGuest.here)
    ; push word 0xf000
    ; push word 0xfe6e
    ; retf
    ; .here:
    ; println "Passed!"
    ; hlt
    ; println "here"
    mov dl, byte [DRIVE_IDX_ADDRESS]
    UpdateSelectorsAX 0
    jmp 0:0x7c00

bits 16
ReadDisk:
    mov ah, 0x42
    mov dl, byte [DRIVE_IDX_ADDRESS]
    mov si, DAP_ADDRESS
    int 0x13
    jc ReadDisk.problem
    ret

    .problem:
        println "Error while reading from disk"
        mov byte [DRIVE_IDX_ADDRESS], ah
    ret


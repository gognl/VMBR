
%define DAP_ADDRESS 0x2fef
%define DRIVE_IDX_ADDRESS 0x2fff

global ReadDisk
global JumpToGuest

section .text

bits 16
JumpToGuest:
    ; println "Before"
    ; in al, 0x64
    ; println "After"
    println "Here"
    pop di
    sub di, 0x7c83
    add di, 0x4370
    push di
    ret
    ; mov dl, byte [DRIVE_IDX_ADDRESS]
    ; UpdateSelectorsAX 0
    ; jmp 0:0x7c00

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


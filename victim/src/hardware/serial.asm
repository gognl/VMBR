
%define DAP_ADDRESS 0x2fef
%define DRIVE_IDX_ADDRESS 0x2fff

global ReadDisk
global JumpToGuest
global LoadGuestVmcall
global __invept

section .text

bits 16
LoadGuestVmcall:
    mov ax, 0x1234
    mov bx, 0xabcd
    vmcall

section .vmm

bits 64
__invept:
    invept rsi, oword [rdi]
    ret

section .text

bits 16
JumpToGuest:
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

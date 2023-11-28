; %include "src/boot/macros.asm"

global VmExitHandler
extern vmexit_handler

VmExitHandler:

    ; mov rcx, 0x1234
    mov qword [fs:0], rax
    mov qword [fs:8], rbx
    mov qword [fs:16], rcx
    mov qword [fs:24], rdx
    mov qword [fs:32], rdi
    mov qword [fs:40], rsi
    mov qword [fs:48], rbp
    mov qword [fs:56], r8
    mov qword [fs:64], r9
    mov qword [fs:72], r10
    mov qword [fs:80], r11
    mov qword [fs:88], r12
    mov qword [fs:96], r13
    mov qword [fs:104], r14
    mov qword [fs:112], r15

    call vmexit_handler

    mov rax, qword [fs:0]
    mov rbx, qword [fs:8]
    mov rcx, qword [fs:16]
    mov rdx, qword [fs:24]
    mov rdi, qword [fs:32]
    mov rsi, qword [fs:40]
    mov rbp, qword [fs:48]
    mov r8, qword [fs:56]
    mov r9, qword [fs:64]
    mov r10, qword [fs:72]
    mov r11, qword [fs:80]
    mov r12, qword [fs:88]
    mov r13, qword [fs:96]
    mov r14, qword [fs:104]
    mov r15, qword [fs:112]

    vmresume

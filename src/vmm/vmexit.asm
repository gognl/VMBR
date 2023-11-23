
global VmExitHandler
extern vmexit_handler

%define VMCS_GUEST_RIP 0x0000681e
%define VMCS_GUEST_RIP 0x0000681c

VmExitHandler:
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

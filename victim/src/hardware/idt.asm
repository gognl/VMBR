
global IsrWrapper_NIC
extern nic_handler

%macro pushaq 0
    push rax
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    mov rax, ds
    push rax
    mov rax, es
    push rax
    mov rax, fs
    push rax
    mov rax, gs
    push rax
    mov rax, 0x10
    mov ds, rax
    mov es, rax
    mov fs, rax
    mov gs, rax
%endmacro

%macro popaq 0
    pop rax
    mov gs, rax
    pop rax
    mov fs, rax
    pop rax
    mov es, rax
    pop rax
    mov ds, rax
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rax
%endmacro

section .vmm
bits 64
IsrWrapper_NIC:
    pushaq
    cld
    call nic_handler
    popaq
    iretq

extern exception_handler
%macro isr_err_stub 1
isr_stub_%+%1:
    pushaq
    cld
    mov rdi, %1
    mov rsi, rsp
    add rsi, 8*13
    call exception_handler
    popaq
    add rsp, 8
    iretq
%endmacro

extern exception_handler
%macro isr_no_err_stub 1
isr_stub_%+%1:
    pushaq
    cld
    mov rdi, %1
    mov rsi, rsp
    add rsi, 8*13
    call exception_handler
    popaq
    iretq
%endmacro

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep 32 
    dq isr_stub_%+i
%assign i i+1
%endrep


global _start

%define ESER_MSR 0xC0000080
%define MEM_SIZE 512

section .bss

align 4096

p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096

    resb 8192
_sys_stack:

section .rodata
gdt64:
    dq 0    ; gdt zero entry
.code: equ $ - gdt64    ; gdt code segment
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53)
.data: equ $ - gdt64    ; gdt data segment
    dq (1<<44) | (1<<47) | (1<<41)
.pointer:
    dw .pointer - gdt64 - 1
    dq gdt64

section .text
bits 32
_start:

    mov esp, _sys_stack  ; stack initialization

    ; Point the first entry in the p4 table to the p3 table
    mov eax, p3_table
    or eax, (1 << 0) | (1 << 1)    ; set the present & writeable bits
    mov dword [p4_table+0], eax

    ; Point the first entry in the p3 table to the p2 table
    mov eax, p2_table
    or eax, (1 << 0) | (1 << 1)    ; set the present & writeable bits
    mov dword [p3_table+0], eax

    ; Map the p2 table 
    xor ecx, ecx
    .map_p2_table:
        mov eax, 0x200000   ; 2MB
        mul ecx    ; now eax=eax*ecx, meaning eax=2MB*idx
        or eax, (1 << 0) | (1 << 1) | (1 << 7)    ; present, writeable & huge page bits
        mov [p2_table + 8*ecx], eax

        inc ecx
        cmp ecx, MEM_SIZE
        jne .map_p2_table

    ; Move p4 address to cr3
    mov eax, p4_table
    mov cr3, eax

    ; Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Set long mode
    mov ecx, ESER_MSR
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, ((1 << 16) | (1 << 31))
    mov cr0, eax

    ; Load GDT
    lgdt [gdt64.pointer]

    ; Update selectors
    mov ax, gdt64.data
    mov ss, ax
    mov ds, ax
    mov es, ax

    jmp gdt64.code:main

    bits 64
    long_mode_start:

    extern main
    call main
    hlt

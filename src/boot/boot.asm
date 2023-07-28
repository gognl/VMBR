global _start

extern gdt64.code, gdt64.data, gdt64.pointer

%define MEM_SIZE 512

; -------------- Page Table Entry Flags ---------------------------------------
%define PTE_P 1<<0          ; Present bit
%define PTE_W 1<<1          ; Writeable bit
%define PTE_PS 1<<7         ; Huge Page bit (2MB)
; -----------------------------------------------------------------------------

; -------------- CR4 bits -----------------------------------------------------
%define CR4_PAE 1<<5        ; PAE bit (Physical Address Extension)
; -----------------------------------------------------------------------------

section .bss

align 0x1000

p4_table:
    resb 0x1000
p3_table:
    resb 0x1000
p2_table:
    resb 0x1000

    resb 0x2000
_sys_stack:

section .text
bits 32

InitializePageTables:
    push eax
    push ecx
    push edx

    ; Point the first entry in the p4 table to the p3 table
    mov eax, p3_table
    or eax, PTE_P | PTE_W 
    mov dword [p4_table+0], eax

    ; Point the first entry in the p3 table to the p2 table
    mov eax, p2_table
    or eax, PTE_P | PTE_W
    mov dword [p3_table+0], eax

    ; Map the p2 table 
    xor ecx, ecx
    .map_p2_table:
        mov eax, 0x200000   ; 2MB
        mul ecx    ; now eax=eax*ecx, meaning eax=2MB*idx
        or eax, PTE_P | PTE_W | PTE_PS
        mov [p2_table + 8*ecx], eax

        inc ecx
        cmp ecx, MEM_SIZE
        jne .map_p2_table

    ; Move p4 address to cr3
    mov eax, p4_table
    mov cr3, eax

    ; Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, CR4_PAE
    mov cr4, eax
    
    pop edx
    pop ecx
    pop eax
    ret

_start:
    ; protected mode

    mov esp, _sys_stack  ; stack initialization

    call InitializePageTables

    extern ProtectedToCompatibility
    call ProtectedToCompatibility

    extern CompatibilityToLong
    call CompatibilityToLong
bits 64
    extern main:function
    call main
    hlt

%include "src/boot/macros.asm"

%define p4_table 0x60000
%define p3_table 0x61000
%define p2_table 0x62000

section .text
bits 32
InitializePageTables:

    ; Point the first entry in the p4 table to the p3 table
    mov eax, p3_table
    or eax, PTE_P | PTE_W 
    mov dword [p4_table+0], eax
    mov dword [p4_table+4], 0

    ; Point the first entry in the p3 table to the p2 table
    mov eax, p2_table
    mov edi, p3_table
    mov ecx, MEM_SIZE
    .map_p3_table:
        mov edx, eax
        or edx, PTE_P | PTE_W
        mov dword [edi], edx
        mov dword [edi + 4], 0
        add eax, 0x1000
        add edi, 8
        loop .map_p3_table

    mov ecx, MEM_SIZE
    shl ecx, 9                  ; multiply by 512
    xor eax, eax                ; start address is 0
    mov ebx, PTE_P | PTE_W | PTE_PS
    mov edi, p2_table
    .map_p2_table:
        or eax, ebx
        mov dword [edi], eax
        mov dword [edi + 4], 0
        add edi, 8
        add eax, 0x200000
        loop .map_p2_table

    ret
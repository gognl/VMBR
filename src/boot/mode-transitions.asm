section .data

ivt:
    dw 0x03ff             ; limit 0-15
    dd 0                  ; base 16-48

section .text

bits 32
ProtectedToLong:
    pop esi
    
    call InitializePageTables

    ; Move p4 address to cr3
    mov eax, p4_table
    mov cr3, eax

    ; Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, CR4_PAE
    mov cr4, eax

    ; Set long mode
    mov ecx, ESER_MSR
    rdmsr
    or eax, ESER_LM  
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, CR0_PG
    mov cr0, eax

    lgdt [gdt64.pointer]

    jmp gdt64.code:ProtectedToLong.long_mode
bits 64
    .long_mode:
    
    UpdateSelectorsAX gdt64.data

    and rsi, 0xFFFFFFFF
    push rsi
    ret

%include "src/boot/macros.asm"

section .rodata
%include "src/boot/gdt.asm"

section .text
bits 32
ivt:
    dw 0x03ff             ; limit 0-15
    dq 0                  ; base 16-48

bits 32
ProtectedToLong:
    pop esi

    ; Move p4 address to cr3
    mov eax, p4_table
    mov cr3, eax

    ; Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, CR4_PAE
    mov cr4, eax

    ; Set long mode
    mov ecx, EFER_MSR
    rdmsr
    or eax, EFER_LM  
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, CR0_PG
    mov cr0, eax

    lgdt [_gdt.pointer]

    jmp _gdt.code64:ProtectedToLong.long_mode
bits 64
    .long_mode:
    
    UpdateSelectorsAX _gdt.data64

    and rsi, 0xFFFFFFFF
    push rsi
    ret

bits 64
LongToProtected:
    pop rsi
    push _gdt.code32
    push REAL_ADDR(LongToProtected.compatibility_mode)
    retfq

bits 32
    .compatibility_mode:

    UpdateSelectorsAX _gdt.data32

    ; Disable paging
    mov eax, cr0
    and eax, ~CR0_PG
    mov cr0, eax

    ; Disable long mode
    mov ecx, EFER_MSR
    rdmsr
    and eax, ~EFER_LM  
    wrmsr

    ; Disable PAE (Physical Address Extension)
    mov eax, cr4
    and eax, ~CR4_PAE
    mov cr4, eax

    push esi
    ret


bits 32
ProtectedToReal:
    pop esi

    cli 
    jmp _gdt.code16:REAL_ADDR(ProtectedToReal.protected16)
bits 16
    .protected16:

    UpdateSelectorsAX _gdt.data16

    lidt [REAL_ADDR(ivt)]

    mov eax, cr0
    and eax, ~(CR0_PE)
    mov cr0, eax

    jmp 0:REAL_ADDR(ProtectedToReal.real_mode)
    .real_mode:

    UpdateSelectorsAX 0

    push si
    ret


bits 16
RealToProtected:
    pop si

    cli

    mov eax, cr0
    or eax, CR0_PE
    mov cr0, eax

    jmp _gdt.code32:REAL_ADDR(RealToProtected.protected_mode)
bits 32
    .protected_mode:

    UpdateSelectorsAX _gdt.data32

    and esi, 0xFFFF
    push esi
    ret

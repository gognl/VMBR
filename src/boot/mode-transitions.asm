global RealToProtected
global ProtectedToCompatibility

global gdt64.code, gdt64.data, gdt64.pointer

%define ESER_MSR 0xC0000080

; -------------- ESER MSR bits ------------------------------------------------
%define ESER_LM 1<<8        ; Long Mode enabled bit
; -----------------------------------------------------------------------------

; -------------- CR0 bits -----------------------------------------------------
%define CR0_PE 1<<0         ; Protected mode Enabled bit
%define CR0_PG 1<<31        ; Paging enabled bit
; -----------------------------------------------------------------------------

; -------------- GDT Segment Descriptor Access Byte bits & Flags --------------
%define SEG_DSCR_RW 1<<41   ; R/W bit - R for code segments, W for data segments
%define SEG_DSCR_DC 1<<42   ; Direction bit (0 for growing up, 1 for growing down)
%define SEG_DSCR_E 1<<43    ; Executable bit
%define SEG_DSCR_S 1<<44    ; S bit (type) - 1 if code/data segment
%define SEG_DSCR_P 1<<47    ; Present bit
%define SEG_DSCR_L 1<<53    ; Long mode code flag
; -----------------------------------------------------------------------------

section .rodata
gdt64:
.null:
    dq 0    ; gdt zero entry
.code: equ $ - gdt64    ; gdt code segment
    dq  SEG_DSCR_RW | SEG_DSCR_E | SEG_DSCR_S | SEG_DSCR_P | SEG_DSCR_L
.data: equ $ - gdt64    ; gdt data segment
    dq SEG_DSCR_RW | SEG_DSCR_S | SEG_DSCR_P
.pointer:
    dw .pointer - gdt64 - 1    ; size of gdt minus 1
    dq gdt64    ; offset is 8-bytes in 64bit mode

section .text

bits 32
ProtectedToCompatibility:
    push eax
    push ecx

    ; Set long mode
    mov ecx, ESER_MSR
    rdmsr
    or eax, ESER_LM  
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, CR0_PG
    mov cr0, eax

    pop ecx
    pop eax
    ret

bits 32
RealToProtected:
    push eax
    
    cli

    lgdt [gdt64.pointer]

    mov eax, cr0
    or eax, CR0_PE
    mov cr0, eax

    pop eax
    ret

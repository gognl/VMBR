%ifndef MACROS.ASM
%define MACROS.ASM

; -------------- Real Address Macro -------------------------------------------
%define REAL_START 0x2000
%define REAL_ADDR(addr) (addr + REAL_START - low_functions_start)
; -----------------------------------------------------------------------------

; -------------- ESER MSR bits ------------------------------------------------
%define ESER_LM (1<<8)        ; Long Mode enabled bit
; -----------------------------------------------------------------------------

; -------------- CR0 bits -----------------------------------------------------
%define CR0_PE (1<<0)         ; Protected mode Enabled bit
%define CR0_PG (1<<31)        ; Paging enabled bit
; -----------------------------------------------------------------------------

; -------------- GDT Segment Descriptor Access Byte bits & Flags --------------
%define SEG_DSCR_RW (1<<41)   ; R/W bit - R for code segments, W for data segments
%define SEG_DSCR_DC (1<<42)   ; Direction bit (0 for growing up, 1 for growing down)
%define SEG_DSCR_E (1<<43)    ; Executable bit
%define SEG_DSCR_S (1<<44)    ; S bit (type) - 1 if code/data segment
%define SEG_DSCR_P (1<<47)    ; Present bit
%define SEG_DSCR_L (1<<53)    ; Long mode code flag

%define GDT_AB_RW (1<<1)      ; R/W bit - R for code segments, W for data segments
%define GDT_AB_DC (1<<2)      ; Direction bit (0 for growing up, 1 for growing down)
%define GDT_AB_E (1<<3)       ; Executable bit
%define GDT_AB_S (1<<4)       ; S bit (type) - 1 if code/data segment
%define GDT_AB_P (1<<7)       ; Present bit
%define GDT_FLAG_L (1<<1)     ; Long mode code flag
%define GDT_FLAG_DB (1<<2)    ; Size flag (0 for 16/64bit, 1 for 32bit)
%define GDT_FLAG_G (1<<3)     ; Granularity flag
; -----------------------------------------------------------------------------

; -------------- Page Table Entry Flags ---------------------------------------
%define PTE_P (1<<0)          ; Present bit
%define PTE_W (1<<1)          ; Writeable bit
%define PTE_PS (1<<7)         ; Huge Page bit (2MB)
; -----------------------------------------------------------------------------

; -------------- CR4 bits -----------------------------------------------------
%define CR4_PAE (1<<5)        ; PAE bit (Physical Address Extension)
; -----------------------------------------------------------------------------

%define ESER_MSR 0xC0000080

%macro UpdateSelectorsAX 1
    mov ax, %1
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
%endmacro

%endif
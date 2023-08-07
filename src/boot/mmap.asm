; https://wiki.osdev.org/Detecting_Memory_(x86)#BIOS_Function:_INT_0x15.2C_EAX_.3D_0xE820

%include "src/boot/macros.asm"

global LoadMemoryMap

section .text

bits 16
LoadMemoryMap:

    mov di, MMAP_TABLE+4        ; The first dword is for the length of the table
    xor ebx, ebx
    xor ebp, ebp
    mov edx, E820_MAGIC
    mov eax, 0xe820
    mov ecx, 24
    mov dword [es:di + 20], 1   ; force a valid ACPI 3.X entry
    int 0x15
    jc .failed                  ; carry on first call means "unsupported function"
    mov edx, E820_MAGIC
    cmp eax, edx
    jne .failed
    test ebx, ebx		        ; ebx = 0 implies list is only 1 entry long
    je .failed
    jmp .test_entry

.load_loop:
    mov eax, 0xe820
    mov dword [es:di + 20], 1	; force a valid ACPI 3.X entry
    mov ecx, 24
    int 0x15
    jc .finished
    mov edx, E820_MAGIC
.test_entry:
    jcxz .skip_entry            ; skip length 0 entries
    cmp cl, 20
    jbe .valid_entry
    test byte [es:di + 20], 1   ; "ignore the data"
    je .skip_entry
.valid_entry:
    mov ecx, [es:di + 8]
    or ecx, [es:di + 12]
    jz .skip_entry              ; length is zero
    ;println "Found a good entry at ", di, dword [es:di + 16]
    inc ebp                      ; got a good entry
    add di, 24
.skip_entry:
    test ebx, ebx               ; done if ebx=0
    jne .load_loop
.finished:
    println "Loaded memory map!"
    mov dword [MMAP_TABLE], ebp
    clc
    ret
.failed:
    println "Failed to load memory map"
    stc
    ret

%include "src/boot/macros.asm"

global low_functions_start
global low_functions_end
global CallReal

section .text
bits 64

CallReal:
    ; As per the calling convention of System V AMD64 ABI,
    ; which is the standard on most UNIX systems, the first
    ; int/pointer arguments is passed on rdi.
    
    ; Passing a register to a macro is not possible

    ; sub rdi, low_functions_start
    ; add rdi, REAL_START
    mov rsi, REAL_ADDR(LongToProtected)
    call rsi
    bits 32
    mov esi, REAL_ADDR(ProtectedToReal)
    call esi
    bits 16
    pusha
    call di
    popa
    mov si, REAL_ADDR(RealToProtected)
    call si
    bits 32
    mov esi, REAL_ADDR(ProtectedToLong)
    call esi
    bits 64
    
    ret

low_functions_start:
%include "src/boot/mode-transitions.asm"
%include "src/boot/mmap.asm"
%include "src/hardware/apic.asm"
low_functions_end:

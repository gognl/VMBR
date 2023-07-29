;global low_functions_start

section .text
bits 64
low_functions_start:
%include "src/boot/mode-transitions.asm"
low_functions_end:

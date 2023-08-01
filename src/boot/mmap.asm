global LoadMemoryMap

%macro disp 1
    mov ah, 0x0e
    mov al, %1
    int 10h
%endmacro

section .text
LoadMemoryMap:

    disp('H')
    disp('e')
    disp('l')
    disp('l')
    disp('o')

    ret

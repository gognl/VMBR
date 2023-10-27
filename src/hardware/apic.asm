global InitializeSingleCore
global InitializeSingleCore_end
global cores_semaphore

section .real
cores_semaphore dd 0

section .text

%macro dispChar 1
    mov al, %1
    mov ah, 0x0e
    int 0x10
%endmacro

%macro print 1+
    jmp %%endstr
    %%str: db %1
    %%endstr:
    %assign i 0
    %rep %%endstr-%%str
        dispChar byte [%%str+i]
        %assign i i+1
    %endrep
%endmacro

%macro println 1+
    print %1, 10, 13
%endmacro

bits 16
InitializeSingleCore:
    println "Initializing"

    ; call REAL_ADDR(RealToProtected)
    ; bits 32
    ; call REAL_ADDR(ProtectedToLong)
    ; bits 64

    mov eax, 1
    mov dword [cores_semaphore], eax

    hlt
    ret

InitializeSingleCore_end:

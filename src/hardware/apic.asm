global InitializeSingleCore
global InitializeSingleCore_end
global cores_semaphore
global AcquireLock
global ReleaseLock
global guest_ap

extern prepare_vmm_ap

section .text
cores_semaphore db 0

bits 16
InitializeSingleCore:

    cli

    mov eax, cr0
    or eax, CR0_PE
    mov cr0, eax

    lgdt [_gdt.pointer]

    jmp _gdt.code32:(InitializeSingleCore.protected_mode-InitializeSingleCore+0x3000)
bits 32
    .protected_mode:

    UpdateSelectorsAX _gdt.data32

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

    jmp _gdt.code64:(InitializeSingleCore.long_mode-InitializeSingleCore+0x3000)
bits 64
    .long_mode:
    
    UpdateSelectorsAX _gdt.data64

    ; Setup temporary stack for calling allocate_memory
    xor rbx, rbx
    mov eax, 1
    xor ecx, ecx
    cpuid
    shr ebx, 24     ; now ebx = core id
    shl ebx, 12     ; now ebx = core id * 0x1000
    mov rsp, 0x70000
    sub rsp, rbx

    mov rax, prepare_vmm_ap
    call rax

bits 16
guest_ap:

    lock add byte [cores_semaphore], 1

    cli
    hlt


bits 64
InitializeSingleCore_end:

section .vmm

AcquireLock:
    lock bts dword [rdi], 0 ; Attempt to acquire the lock (in case lock is uncontended)
    jc AcquireLock.spin_with_pause
    ret
 
.spin_with_pause:
    pause                    ; Tell CPU we're spinning
    test dword [rdi], 1      ; Is the lock free?
    jnz AcquireLock.spin_with_pause     ; no, wait
    jmp AcquireLock          ; retry
 
ReleaseLock:
    mov dword [rdi], 0
    ret

section .text

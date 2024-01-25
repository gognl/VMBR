#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <vmm/vmm.h>
#include <vmm/hooks.h>
#include <boot/addresses.h>

#define LOWER_DWORD(x) ((x) & 0xffffffffull)
#define UPPER_DWORD(x) ((x) >> 32)

void initialize_vmexit_data(vmexit_data_t *data){

    data->registers = (guest_registers_t*)__read_fs();

    // Intel SDM 25.9.1, 28.2.1
    data->exit_reason = (BASIC_EXIT_REASON)__vmread(RODATA_EXIT_REASON);
    data->exit_qual = (exit_qualification_t)__vmread(RODATA_EXIT_QUALIFICATION);
    data->guest_linear_address = __vmread(RODATA_GUEST_LINEAR_ADDRESS);
    data->guest_physical_address = __vmread(RODATA_GUEST_PHYSICAL_ADDRESS);

    // Intel SDM 25.9.2
    data->interruption_info.value = __vmread(RODATA_VMEXIT_INTERRUPTION_INFO);
    data->interruption_errorcode = __vmread(RODATA_VMEXIT_INTERRUPTION_ERRORCODE);
    
    // Intel SDM 25.9.3
    data->idt_info.value = __vmread(RODATA_IDT_VECTORING_INFO);
    data->idt_errorcode = __vmread(RODATA_IDT_VECTORING_ERRORCODE);
    
    // Intel SDM 25.9.4
    data->instr_length = __vmread(RODATA_VMEXIT_INSTRUCTION_LENGTH);
    data->instr_info.value = __vmread(RODATA_VMEXIT_INSTRUCTION_INFO); // 28.2.5
    
    // Intel SDM 25.9.5
    data->vmx_error = __vmread(RODATA_VM_INSTRUCTION_ERROR);
}

void vmexit_handler(){
    
    vmexit_data_t state;
    initialize_vmexit_data(&state);

    if (state.vmx_error)
        LOG_ERROR("VMX ERROR %d\n", state.vmx_error);

    switch (state.exit_reason){
        case EXIT_REASON_WRMSR:
            qword_t msr = LOWER_DWORD(state.registers->rcx);
            qword_t value = LOWER_DWORD(state.registers->rax) | (state.registers->rdx<<32);
            LOG_DEBUG("WRMSR VMEXIT (%x, %x)\n", msr, value);
            __wrmsr(msr, value);
            break;
        case EXIT_REASON_RDMSR:
            msr = LOWER_DWORD(state.registers->rcx);
            LOG_DEBUG("RDMSR VMEXIT (%x, rcx=%x)\n", msr, state.registers->rcx);
            value = __rdmsr(msr);
            state.registers->rdx = (state.registers->rdx>>32<<32) | value >> 32;
            state.registers->rax = LOWER_DWORD(value);
            break;
        case EXIT_REASON_CPUID:
            LOG_DEBUG("CPUID VMEXIT\n");
            qword_t eax_in, ecx_in, eax, ebx, ecx, edx;
            eax_in = LOWER_DWORD(state.registers->rax);
            ecx_in = LOWER_DWORD(state.registers->rcx);
            __cpuid(eax_in, ecx_in, &eax, &ebx, &ecx, &edx);
            state.registers->rax = (qword_t)eax;
            state.registers->rcx = (qword_t)ecx;
            state.registers->rbx = (qword_t)ebx;
            state.registers->rdx = (qword_t)edx;

            if (eax_in == 1){
                // state.registers->rcx &= ~(1<<31);   // hypervisor present bit
                state.registers->rcx &= ~(1<<5);    // no VMX support (todo nested virtualization)
            }
            LOG_DEBUG("rcx is %x\n", (qword_t)state.registers->rcx);
            break;
        case EXIT_REASON_GETSEC:
            LOG_DEBUG("GETSEC VMEXIT\n");
            break;
        case EXIT_REASON_INVD:
            LOG_DEBUG("INVD VMEXIT\n");
            break;
        case EXIT_REASON_XSETBV:
            LOG_DEBUG("XSETBV VMEXIT\n");
            break;
        case EXIT_REASON_INVEPT:
            LOG_DEBUG("INVEPT VMEXIT\n");
            break;
        case EXIT_REASON_INVVPID:
            LOG_DEBUG("INVVPID VMEXIT\n");
            break;
        case EXIT_REASON_VMCALL:
            LOG_DEBUG("VMCALL VMEXIT\n");
            if (__vmread(GUEST_RIP) == HOOK_INT15H){
                if (state.registers->rax == 0xe820){
                    handle_int15h_hook(&state);
                } else {
                    __vmwrite(GUEST_RIP, shared_cores_data.int15h_offset);
                    __vmwrite(GUEST_CS, shared_cores_data.int15h_segment);
                    __vmwrite(GUEST_CS_BASE, shared_cores_data.int15h_segment<<4);
                }
            }
            return;
        case EXIT_REASON_VMCLEAR:
            LOG_DEBUG("VMCLEAR VMEXIT\n");
            break;
        case EXIT_REASON_VMLAUNCH:
            LOG_DEBUG("VMLAUNCH VMEXIT\n");
            break;
        case EXIT_REASON_VMPTRLD:
            LOG_DEBUG("VMPTRLD VMEXIT\n");
            break;
        case EXIT_REASON_VMPTRST:
            LOG_DEBUG("VMPTRST VMEXIT\n");
            break;
        case EXIT_REASON_VMRESUME:
            LOG_DEBUG("VMRESUME VMEXIT\n");
            break;
        case EXIT_REASON_VMXOFF:
            LOG_DEBUG("VMXOFF VMEXIT\n");
            break;
        case EXIT_REASON_VMXON:
            LOG_DEBUG("VMXON VMEXIT\n");
            break;
        default:     
            LOG_DEBUG("Unknown VMEXIT (%x, %x)\n", (BASIC_EXIT_REASON)__vmread(RODATA_EXIT_REASON), __vmread(RODATA_VM_INSTRUCTION_ERROR));
            LOG_DEBUG("Qual: %x\n\tInterruption info: %x (%x)\n\tIDT info: %x (%x)\n", state.exit_qual.value, (qword_t)state.interruption_info.value, (qword_t)state.interruption_errorcode, (qword_t)state.idt_info.value, (qword_t)state.idt_errorcode);
            LOG_DEBUG("GUEST_RIP: %x\n\tNEXT_GUEST_RIP: %x\n\tINSTR_LENGTH: %x\n", __vmread(GUEST_RIP), __vmread(GUEST_RIP)+(qword_t)state.instr_length, (qword_t)state.instr_length);
            while(1);
    }

    LOG_DEBUG("GUEST_RIP: %x\n\tNEXT_GUEST_RIP: %x\n\tINSTR_LENGTH: %x\n", __vmread(GUEST_RIP), __vmread(GUEST_RIP)+(qword_t)state.instr_length, (qword_t)state.instr_length);
    __vmwrite(GUEST_RIP, __vmread(GUEST_RIP)+(qword_t)state.instr_length);
}

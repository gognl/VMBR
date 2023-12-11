#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <vmm/vmm.h>

#define LOWER_DWORD(x) ((x) & 0xffffffffull)
#define UPPER_DWORD(x) ((x) >> 32)
// TODO ept and memory types
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
    
    vmexit_data_t vmexit_data;
    initialize_vmexit_data(&vmexit_data);

    if (vmexit_data.vmx_error)
        LOG_ERROR("VMX ERROR %d\n", vmexit_data.vmx_error);

    switch (vmexit_data.exit_reason){
        case EXIT_REASON_WRMSR:
            qword_t msr = LOWER_DWORD(vmexit_data.registers->rcx);
            qword_t value = LOWER_DWORD(vmexit_data.registers->rax) | (vmexit_data.registers->rdx<<32);
            LOG_DEBUG("WRMSR VMEXIT (%x, %x)\n", msr, value);
            __wrmsr(msr, value);
            break;
        case EXIT_REASON_RDMSR:
            msr = LOWER_DWORD(vmexit_data.registers->rcx);
            LOG_DEBUG("RDMSR VMEXIT (%x, rcx=%x)\n", msr, vmexit_data.registers->rcx);
            value = __rdmsr(msr);
            vmexit_data.registers->rdx = (vmexit_data.registers->rdx>>32<<32) | value >> 32;
            vmexit_data.registers->rax = LOWER_DWORD(value);
            break;
        case EXIT_REASON_CPUID:
            LOG_DEBUG("CPUID VMEXIT\n");
            dword_t eax_in, ecx_in, eax, ebx, ecx, edx;
            eax_in = LOWER_DWORD(vmexit_data.registers->rax);
            ecx_in = LOWER_DWORD(vmexit_data.registers->rcx);
            __cpuid(eax_in, ecx_in, &eax, &ebx, &ecx, &edx);
            vmexit_data.registers->rax = (qword_t)eax;
            vmexit_data.registers->rbx = (qword_t)ebx;
            vmexit_data.registers->rcx = (qword_t)ecx;
            vmexit_data.registers->rdx = (qword_t)edx;
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
            break;
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
            LOG_DEBUG("Qual: %x\n\tInterruption info: %x (%x)\n\tIDT info: %x (%x)\n", vmexit_data.exit_qual.value, (qword_t)vmexit_data.interruption_info.value, (qword_t)vmexit_data.interruption_errorcode, (qword_t)vmexit_data.idt_info.value, (qword_t)vmexit_data.idt_errorcode);
    }

    LOG_DEBUG("GUEST_RIP: %x\n\tNEXT_GUEST_RIP: %x\n\tINSTR_LENGTH: %x\n", __vmread(GUEST_RIP), __vmread(GUEST_RIP)+(qword_t)vmexit_data.instr_length, (qword_t)vmexit_data.instr_length);
    __vmwrite(GUEST_RIP, __vmread(GUEST_RIP)+(qword_t)vmexit_data.instr_length);
}

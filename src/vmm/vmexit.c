#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <vmm/vmm.h>

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

    if (!vmexit_data.vmx_error)
        LOG_ERROR("VMX ERROR %d\n", vmexit_data.vmx_error);

    switch (vmexit_data.exit_reason){
        case EXIT_REASON_INIT:
            LOG_DEBUG("INIT VMEXIT\n");
            break;
        case EXIT_REASON_SIPI:
            LOG_DEBUG("SIPI VMEXIT\n");
            break;
        case EXIT_REASON_CPUID:
            LOG_DEBUG("CPUID VMEXIT\n");
            break;
        default:
            LOG_DEBUG("Unknown VMEXIT (%q)\n", (BASIC_EXIT_REASON)__vmread(RODATA_EXIT_REASON));
    }

}

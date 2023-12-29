#include <hardware/rsdt.h>
#include <lib/types.h>
#include <vmm/vmm.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <lib/msr.h>
#include <hardware/apic.h>
#include <boot/mmap.h>
#include <vmm/hooks.h>

shared_cores_data_t shared_cores_data = {0};

void prepare_vmxon(byte_t *vmxon_region_ptr){
    qword_t ecx, tmp;
    __cpuid(1, 0, &tmp, &tmp, &ecx, &tmp);
    if (!(ecx & CPUID_VMXON))
        LOG_ERROR("No VMX support in cpuid\n");
    __write_cr0((__read_cr0() | __rdmsr(IA32_VMX_CR0_FIXED0) | CR0_NE) & __rdmsr(IA32_VMX_CR0_FIXED1));
    __write_cr4((__read_cr4() | __rdmsr(IA32_VMX_CR4_FIXED0) | CR4_VMXE) & __rdmsr(IA32_VMX_CR4_FIXED1));
    *(dword_t*)vmxon_region_ptr = (dword_t)__rdmsr(IA32_VMX_BASIC);  // revision identifier
}

void prepare_vmcs(vmcs_t *vmcs_ptr){
    vmcs_ptr->revision_id = (dword_t)__rdmsr(IA32_VMX_BASIC);
    vmcs_ptr->shadow_vmcs_indicator = FALSE;
}

void vmentry_handler(){
    LOG_INFO("Entered the VM Entry handler\n");

    load_guest();

    LOG_INFO("Exited the VM Entry handler\n");
    for(;;);
}

void prepare_vmm(){

    byte_t *vmxon_region_ptr = allocate_memory(0x1000);   // 4kb aligned. size should actually be read from IA32_VMX_BASIC[32:44], but it's 0x1000 max.
    prepare_vmxon(vmxon_region_ptr);
    __vmxon(vmxon_region_ptr);

    vmcs_t* vmcs_ptr = (vmcs_t*)allocate_memory(0x1000);   // 4kb aligned. size should actually be read from IA32_VMX_BASIC[32:44], but it's 0x1000 max.
    prepare_vmcs(vmcs_ptr);
    __vmclear(vmcs_ptr);
    __vmptrld(vmcs_ptr);

    initialize_vmcs();
    LOG_INFO("Done initializing VMCS fields\n");

    setup_int15h_hook();
}

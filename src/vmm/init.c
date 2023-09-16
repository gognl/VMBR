/*
Sources:
    OSDev - VMX page
*/

#include <rsdt.h>
#include <types.h>
#include <real.h>
#include <system.h>
#include <init.h>
#include <vmcs.h>

void prepare_vmxon(BYTE *vmxon_region_ptr){
    // TODO check that vmx is available in cpuid. Also take care of edge cases (intel manual vol. 3c, section 23.8)
    __write_cr0((__read_cr0() | __read_msr(IA32_VMX_CR0_FIXED0) | CR0_NE) & __read_msr(IA32_VMX_CR0_FIXED1));
    __write_cr4((__read_cr4() | __read_msr(IA32_VMX_CR4_FIXED0) | CR4_VMXE) & __read_msr(IA32_VMX_CR4_FIXED1));
    *(DWORD*)vmxon_region_ptr = (DWORD)__read_msr(IA32_VMX_BASIC);  // revision identifier
}

void prepare_vmcs(vmcs *vmcs_ptr){
    vmcs_ptr->revision_id = (DWORD)__read_msr(IA32_VMX_BASIC);  // revision identifier
    vmcs_ptr->revision_id &= ~(1<<31);                          // no shadow vmcs
    
}

void initialize_vmcs(){
    
}

void init_vmm(){
    puts("basic is %d\n", (DWORD)__read_msr(IA32_VMX_BASIC));

    UINT32 cpu_count = get_cpu_count();
    puts("Found cpu count (%d)\n", cpu_count);

    BYTE *vmxon_region_ptr = allocate_memory(0x1000);   // 4kb aligned. size should actually be read from IA32_VMX_BASIC[32:44], but it's 0x1000 max.
    prepare_vmxon(vmxon_region_ptr);
    puts("Prepared for vmxon\n");
    __vmxon(vmxon_region_ptr);
    puts("Entered VMX root operation!\n");

    vmcs* vmcs_ptr = (vmcs*)allocate_memory(0x1000);   // 4kb aligned. size should actually be read from IA32_VMX_BASIC[32:44], but it's 0x1000 max.
    prepare_vmcs(vmcs_ptr);
    __vmclear(vmcs_ptr);
    __vmptrld(vmcs_ptr);
    puts("VMCS is now loaded\n");

    initialize_vmcs();
}

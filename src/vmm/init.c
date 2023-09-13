/*
Sources:
    OSDev - VMX page
*/

#include <rsdt.h>
#include <types.h>
#include <real.h>
#include <system.h>
#include <init.h>

void prepare_vmxon(BYTE *vmxon_region){
    // TODO check that vmx is available in cpuid
    __write_cr0((__read_cr0() | __read_msr(IA32_VMX_CR0_FIXED0) | CR0_NE) & __read_msr(IA32_VMX_CR0_FIXED1));
    __write_cr4((__read_cr4() | __read_msr(IA32_VMX_CR4_FIXED0) | CR4_VMXE) & __read_msr(IA32_VMX_CR4_FIXED1));
    *(DWORD*)vmxon_region = (DWORD)__read_msr(IA32_VMX_BASIC);
}

void init_vmm(){
    UINT32 cpu_count = get_cpu_count();
    BYTE *vmxon_region = allocate_memory(0x1000);   // 4kb aligned
    puts("Allocated region and found cpu count (%d)\n", cpu_count);
    prepare_vmxon(vmxon_region);
    puts("Prepared for vmxon\n");
    __vmxon(vmxon_region);
    puts("Entered VMX root operation!\n");

}

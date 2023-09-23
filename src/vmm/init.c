/*
Sources:
    OSDev - VMX page
*/

#include <rsdt.h>
#include <types.h>
#include <init.h>
#include <vmcs.h>
#include <instr.h>

extern byte_t *_sys_stack(void);


void prepare_vmxon(byte_t *vmxon_region_ptr){
    // TODO check that vmx is available in cpuid. Also take care of edge cases (intel manual vol. 3c, section 23.8)
    if (!(__get_cpuid() & CPUID_VMXON))
        LOG_ERROR("No VMX support in cpuid\n");
    __write_cr0((__read_cr0() | __read_msr(IA32_VMX_CR0_FIXED0) | CR0_NE) & __read_msr(IA32_VMX_CR0_FIXED1));
    __write_cr4((__read_cr4() | __read_msr(IA32_VMX_CR4_FIXED0) | CR4_VMXE) & __read_msr(IA32_VMX_CR4_FIXED1));
    *(dword_t*)vmxon_region_ptr = (dword_t)__read_msr(IA32_VMX_BASIC);  // revision identifier
}

void prepare_vmcs(vmcs_t *vmcs_ptr){
    vmcs_ptr->revision_id = (dword_t)__read_msr(IA32_VMX_BASIC);  // revision identifier
    vmcs_ptr->revision_id &= ~(1<<31);                          // no shadow vmcs
    
}

void vmentry_handler(){
    __hlt();
    LOG_INFO("Entered the VM Entry handler\n");
}

void vmexit_handler(){
    LOG_INFO("Entered the VM Exit handler\n");
}

void initialize_vmcs(){

    // qword_t basic_bit_55 = ;

    __vmwrite(HOST_CR0, __read_cr0());
    __vmwrite(HOST_CR3, __read_cr3());  // todo initialize VMM paging, hidden from VM
    __vmwrite(HOST_CR4, __read_cr4());
    __vmwrite(HOST_RIP, vmexit_handler);
    __vmwrite(HOST_RSP, _sys_stack);    // todo create a new VMM stack, hidden from VM
    __vmwrite(HOST_ES, __read_es());
    __vmwrite(HOST_CS, __read_cs());
    __vmwrite(HOST_SS, __read_ss());
    __vmwrite(HOST_DS, __read_ds());
    __vmwrite(HOST_FS, 0);
    __vmwrite(HOST_GS, 0);
    __vmwrite(GUEST_CR0, __read_cr0());
    __vmwrite(GUEST_CR3, __read_cr3());
    __vmwrite(GUEST_CR4, __read_cr4());
    __vmwrite(GUEST_DR7, __read_dr7());
    __vmwrite(GUEST_RIP, vmentry_handler);
    __vmwrite(GUEST_RSP, 0);
    __vmwrite(GUEST_RFLAGS, __read_rflags());
    __vmwrite(GUEST_ES, __read_es() & 0xf8);
    __vmwrite(GUEST_ES_BASE, 0);
    __vmwrite(GUEST_ES_LIMIT, 0xfffff);
    __vmwrite(GUEST_ES_ACCESS_RIGHTS, GDT_AB_RW | GDT_AB_S | GDT_AB_P);
    __vmwrite(GUEST_CS, __read_cs() & 0xf8);
    __vmwrite(GUEST_CS_BASE, 0);
    __vmwrite(GUEST_CS_LIMIT, 0xfffff);
    __vmwrite(GUEST_CS_ACCESS_RIGHTS, GDT_AB_RW | GDT_AB_E | GDT_AB_S | GDT_AB_P);
    __vmwrite(GUEST_SS, __read_ss() & 0xf8);
    __vmwrite(GUEST_SS_BASE, 0);
    __vmwrite(GUEST_SS_LIMIT, 0xfffff);
    __vmwrite(GUEST_SS_ACCESS_RIGHTS, GDT_AB_RW | GDT_AB_S | GDT_AB_P);
    __vmwrite(GUEST_DS, __read_ds() & 0xf8);
    __vmwrite(GUEST_DS_BASE, 0);
    __vmwrite(GUEST_DS_LIMIT, 0xfffff);
    __vmwrite(GUEST_DS_ACCESS_RIGHTS, GDT_AB_RW | GDT_AB_S | GDT_AB_P);
    __vmwrite(GUEST_FS, __read_fs() & 0xf8);
    __vmwrite(GUEST_FS_BASE, 0);
    __vmwrite(GUEST_FS_LIMIT, 0xfffff);
    __vmwrite(GUEST_FS_ACCESS_RIGHTS, GDT_AB_RW | GDT_AB_S | GDT_AB_P);
    __vmwrite(GUEST_GS, __read_gs() & 0xf8);
    __vmwrite(GUEST_GS_BASE, 0);
    __vmwrite(GUEST_GS_LIMIT, 0xfffff);
    __vmwrite(GUEST_GS_ACCESS_RIGHTS, GDT_AB_RW | GDT_AB_S | GDT_AB_P);
    gdtr_t gdtr;
    __read_gdtr(&gdtr);
    __vmwrite(GUEST_GDTR_BASE, gdtr.base);
    __vmwrite(GUEST_GDTR_LIMIT, gdtr.limit);
    idtr_t idtr;
    __read_idtr(&idtr);
    __vmwrite(GUEST_IDTR_BASE, idtr.base);
    __vmwrite(GUEST_IDTR_LIMIT, idtr.limit);
    __vmwrite(GUEST_ACTIVITY_STATE, 0);


    // Intel Manual Appendix A
    if (__read_msr(IA32_VMX_BASIC) & (1<<55)){
        LOG_DEBUG("IA32_VMX_BASIC:55 is set\n");
        __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, (dword_t)__read_msr(IA32_VMX_TRUE_PINBASED_CTLS) | (1<<1) | (1<<2) | (1<<4));
        __vmwrite(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, (dword_t)__read_msr(IA32_VMX_TRUE_PROCBASED_CTLS) | (1<<1) | (1<<4) | (1<<5) | (1<<6) | (1<<8) | (1<<13) | (1<<14) | (1<<15) | (1<<16) | (1<<26));
        __vmwrite(CONTROL_PRIMARY_VMEXIT_CONTROLS, (dword_t)__read_msr(IA32_VMX_TRUE_EXIT_CTLS) | (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) | (1<<8) | (1<<10) | (1<<11) | (1<<13) | (1<<14) | (1<<16) | (1<<17));
        __vmwrite(CONTROL_VMENTRY_CONTROLS, (dword_t)__read_msr(IA32_VMX_TRUE_ENTRY_CTLS) | (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) | (1<<8) | (1<<12));
    } else {
        LOG_DEBUG("IA32_VMX_BASIC:55 is not set\n");
        __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, (dword_t)__read_msr(IA32_VMX_PINBASED_CTLS));
        __vmwrite(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, (dword_t)__read_msr(IA32_VMX_PROCBASED_CTLS));
        __vmwrite(CONTROL_PRIMARY_VMEXIT_CONTROLS, (dword_t)__read_msr(IA32_VMX_EXIT_CTLS));
        __vmwrite(CONTROL_VMENTRY_CONTROLS, (dword_t)__read_msr(IA32_VMX_ENTRY_CTLS));
    }
}

void init_vmm(){

    uint32_t cpu_count = get_cpu_count();
    LOG_INFO("Found cpu count (%d)\n", cpu_count);

    byte_t *vmxon_region_ptr = allocate_memory(0x1000);   // 4kb aligned. size should actually be read from IA32_VMX_BASIC[32:44], but it's 0x1000 max.
    prepare_vmxon(vmxon_region_ptr);
    LOG_INFO("Prepared for vmxon\n");
    __vmxon(vmxon_region_ptr);
    LOG_INFO("Entered VMX root operation!\n");

    vmcs_t* vmcs_ptr = (vmcs_t*)allocate_memory(0x1000);   // 4kb aligned. size should actually be read from IA32_VMX_BASIC[32:44], but it's 0x1000 max.
    prepare_vmcs(vmcs_ptr);
    __vmclear(vmcs_ptr);
    __vmptrld(vmcs_ptr);
    LOG_INFO("VMCS is now loaded\n");

    initialize_vmcs();
    LOG_INFO("Done initializing VMCS fields\n");

    __vmlaunch();
}

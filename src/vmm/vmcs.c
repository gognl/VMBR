#include <vmm/vmcs.h>
#include <vmm/paging.h>
#include <vmm/vmm.h>
#include <lib/msr.h>
#include <lib/instr.h>
#include <boot/mmap.h>
#include <hardware/apic.h>
#include <boot/addresses.h>
#include <lib/util.h>
#include <hooks/hooking.h>

void set_msr_bitmap_write(uint32_t msr, byte_t *bitmap){
    bitmap += 2048;
    if (msr >= 0xc0000000){
        bitmap += 1024;
        msr -= 0xc0000000;
    }

    if (msr >= 0x2000) return;

    bitmap[msr/8] |= (1<<(msr%8));

}

__attribute__((section(".vmm"))) void clear_msr_bitmap_write(uint32_t msr, byte_t *bitmap){
    bitmap += 2048;
    if (msr >= 0xc0000000){
        bitmap += 1024;
        msr -= 0xc0000000;
    }

    if (msr >= 0x2000) return;

    bitmap[msr/8] &= ~(1<<(msr%8));

}

__attribute__((section(".vmm"))) void set_msr_bitmap_read(uint32_t msr, byte_t *bitmap){
    if (msr >= 0xc0000000){
        bitmap += 1024;
        msr -= 0xc0000000;
    }

    if (msr >= 0x2000) return;

    bitmap[msr/8] |= (1<<(msr%8));

}

extern void VmExitHandlerEnd();
void initialize_vmcs(){

    __vmwrite(HOST_CR0, __read_cr0());
    __vmwrite(HOST_CR3, shared_cores_data.pml4);
    __vmwrite(HOST_CR4, __read_cr4());
    __vmwrite(HOST_RIP, VmExitHandler);
    __vmwrite(HOST_RSP, allocate_memory(0x4000)+0x4000);
    __vmwrite(HOST_ES, __read_es());
    __vmwrite(HOST_CS, __read_cs());
    __vmwrite(HOST_SS, __read_ss());
    __vmwrite(HOST_DS, __read_ds());
    __vmwrite(HOST_TR, __read_ds());
    __vmwrite(HOST_FS, REGISTERS_ADDRESS+sizeof(guest_registers_t)*get_current_core_id());
    __vmwrite(HOST_GS, 0);
    __vmwrite(HOST_FS_BASE, REGISTERS_ADDRESS+sizeof(guest_registers_t)*get_current_core_id());
    __vmwrite(HOST_GS_BASE, CANONICAL_ADDRESS);
    __vmwrite(HOST_TR_BASE, CANONICAL_ADDRESS);
    __vmwrite(HOST_IA32_SYSENTER_EIP, CANONICAL_ADDRESS);
    __vmwrite(HOST_IA32_SYSENTER_ESP, CANONICAL_ADDRESS);
    __vmwrite(HOST_IA32_SYSENTER_CS, 0xff);
    __vmwrite(HOST_IA32_EFER, __rdmsr(0xC0000080));

    __vmwrite(HOST_IDTR_BASE, idt);
    
    __vmwrite(GUEST_CR0, __read_cr0());
    __vmwrite(GUEST_CR3, __read_cr3());
    __vmwrite(GUEST_CR4, __read_cr4());
    __vmwrite(GUEST_DR7, __read_dr7());
    __vmwrite(GUEST_RIP, vmentry_handler);
    __vmwrite(GUEST_RSP, 0);
    __vmwrite(GUEST_RFLAGS, __read_rflags());
    // CS
    __vmwrite(GUEST_CS, __read_cs() & 0x00f8);
    __vmwrite(GUEST_CS_BASE, 0ull);
    __vmwrite(GUEST_CS_LIMIT, 0xffffffff);
    __vmwrite(GUEST_CS_ACCESS_RIGHTS,  GDT_AB_A | GDT_AB_RW | GDT_AB_E | GDT_AB_S | GDT_AB_P | GDT_AB_L | GDT_AB_G);
    // ES
    __vmwrite(GUEST_ES, __read_es() & 0x00f8);
    __vmwrite(GUEST_ES_BASE, 0ull);
    __vmwrite(GUEST_ES_LIMIT, 0xffffffff);
    __vmwrite(GUEST_ES_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // SS (the selector)
    __vmwrite(GUEST_SS, __read_ss() & 0x00f8);
    __vmwrite(GUEST_SS_BASE, 0ull);
    __vmwrite(GUEST_SS_LIMIT, 0xffffffff);
    __vmwrite(GUEST_SS_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // DS
    __vmwrite(GUEST_DS, __read_ds() & 0x00f8);
    __vmwrite(GUEST_DS_BASE, 0ull);
    __vmwrite(GUEST_DS_LIMIT, 0xffffffff);
    __vmwrite(GUEST_DS_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // FS
    __vmwrite(GUEST_FS, __read_fs() & 0x00f8);
    __vmwrite(GUEST_FS_BASE, 0ull);
    __vmwrite(GUEST_FS_LIMIT, 0xffffffff);
    __vmwrite(GUEST_FS_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // GS
    __vmwrite(GUEST_GS, __read_gs() & 0x00f8);
    __vmwrite(GUEST_GS_BASE, 0ull);
    __vmwrite(GUEST_GS_LIMIT, 0xffffffff);
    __vmwrite(GUEST_GS_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // TR
    __vmwrite(GUEST_TR, __read_ds() & 0x00f8);
    __vmwrite(GUEST_TR_BASE, 0ull);
    __vmwrite(GUEST_TR_LIMIT, 0xffffffff);
    __vmwrite(GUEST_TR_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_E | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // GDTR
    gdtr_t gdtr;
    __read_gdtr(&gdtr);
    __vmwrite(GUEST_GDTR_BASE, gdtr.base);
    __vmwrite(GUEST_GDTR_LIMIT, gdtr.limit);
    __vmwrite(HOST_GDTR_BASE, gdtr.base);
    // IDTR
    // Default IVT (at 0x00)
    __vmwrite(GUEST_IDTR_BASE, 0);
    __vmwrite(GUEST_IDTR_LIMIT, 0x3ff);
    // LDTR
    __vmwrite(GUEST_LDTR, 0);
    __vmwrite(GUEST_LDTR_BASE, 0);
    __vmwrite(GUEST_LDTR_LIMIT, 0xff);
    __vmwrite(GUEST_LDTR_ACCESS_RIGHTS, UNUSABLE_SELECTOR);
    
    __vmwrite(GUEST_ACTIVITY_STATE, 0ull);
    __vmwrite(GUEST_IA32_SYSENTER_EIP, 0xffff);
    __vmwrite(GUEST_IA32_SYSENTER_ESP, 0xffff);
    __vmwrite(GUEST_IA32_SYSENTER_CS, 8);
    __vmwrite(GUEST_VMCS_LINK_PTR, -1ull);
    shared_cores_data.msr_bitmaps = allocate_memory(0x1000);
    set_msr_bitmap_write(LSTAR_MSR, shared_cores_data.msr_bitmaps);
    __vmwrite(CONTROL_MSR_BITMAPS, shared_cores_data.msr_bitmaps);
    __vmwrite(GUEST_IA32_EFER, __rdmsr(0xC0000080ull));


    // Intel Manual Appendix A
    pin_based_ctls_t pin_based_ctls = {0};
    proc_based_ctls_t proc_based_ctls = {0};
    proc_based_ctls2_t proc_based_ctls2 = {0};
    vmexit_ctls_t vmexit_ctls = {0};
    vmentry_ctls_t vmentry_ctls = {0};

    proc_based_ctls.activate_secondary_controls = TRUE;
    proc_based_ctls.use_msr_bitmaps = TRUE;
    proc_based_ctls2.enable_xsaves_xrstors = TRUE;
    proc_based_ctls2.enable_ept = TRUE;
    proc_based_ctls2.unrestricted_guest = TRUE;
    proc_based_ctls2.enable_rdtscp = TRUE;
    proc_based_ctls2.enable_invpcid = TRUE;
    vmexit_ctls.host_address_space_size = TRUE;
    vmexit_ctls.save_ia32_efer = TRUE;
    vmexit_ctls.load_ia32_efer = TRUE;
    vmentry_ctls.ia32_mode_guest = TRUE;
    vmentry_ctls.load_ia32_efer = TRUE;


    if (__rdmsr(IA32_VMX_BASIC) & (1ull<<55)){
        __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, __rdmsr(IA32_VMX_TRUE_PINBASED_CTLS) | pin_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, __rdmsr(IA32_VMX_TRUE_PROCBASED_CTLS) | proc_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_VMEXIT_CONTROLS, __rdmsr(IA32_VMX_TRUE_EXIT_CTLS) | vmexit_ctls.value);
        __vmwrite(CONTROL_VMENTRY_CONTROLS, __rdmsr(IA32_VMX_TRUE_ENTRY_CTLS) | vmentry_ctls.value);
    } else {
        __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, __rdmsr(IA32_VMX_PINBASED_CTLS) | pin_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, __rdmsr(IA32_VMX_PROCBASED_CTLS) | proc_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_VMEXIT_CONTROLS, __rdmsr(IA32_VMX_EXIT_CTLS) | vmexit_ctls.value);
        __vmwrite(CONTROL_VMENTRY_CONTROLS, __rdmsr(IA32_VMX_ENTRY_CTLS) | vmentry_ctls.value);
    }

    eptp_t eptp = {0};
    shared_cores_data.ept_pml4 = initialize_ept();
    eptp.ept_pml4 = shared_cores_data.ept_pml4;
    eptp.page_walk_length_m1 = 4-1;
    eptp.memory_type = WRITEBACK;

    __vmwrite(CONTROL_SECONDARY_EXECUTION_CONTROLS, proc_based_ctls2.value);
    __vmwrite(CONTROL_EPTP, eptp.value);

    __vmwrite(CONTROL_XSS_EXITING_BITMAP, 0);

    // __vmwrite(CONTROL_EXCEPTION_BITMAP, (1<<6));
    __vmwrite(CONTROL_EXCEPTION_BITMAP, (1<<INT3));

}

extern void guest_ap();
void initialize_vmcs_ap(){
    __vmwrite(HOST_CR0, __read_cr0());
    __vmwrite(HOST_CR3, shared_cores_data.pml4);
    __vmwrite(HOST_CR4, __read_cr4());
    __vmwrite(HOST_RIP, VmExitHandler);
    __vmwrite(HOST_RSP, allocate_memory(0x4000)+0x4000);
    __vmwrite(HOST_ES, __read_es());
    __vmwrite(HOST_CS, __read_cs());
    __vmwrite(HOST_SS, __read_ss());
    __vmwrite(HOST_DS, __read_ds());
    __vmwrite(HOST_TR, __read_ds());
    __vmwrite(HOST_FS, REGISTERS_ADDRESS+sizeof(guest_registers_t)*get_current_core_id());
    __vmwrite(HOST_GS, 0);
    __vmwrite(HOST_FS_BASE, REGISTERS_ADDRESS+sizeof(guest_registers_t)*get_current_core_id());
    __vmwrite(HOST_GS_BASE, CANONICAL_ADDRESS);
    __vmwrite(HOST_TR_BASE, CANONICAL_ADDRESS);
    __vmwrite(HOST_IA32_SYSENTER_EIP, CANONICAL_ADDRESS);
    __vmwrite(HOST_IA32_SYSENTER_ESP, CANONICAL_ADDRESS);
    __vmwrite(HOST_IA32_SYSENTER_CS, 0xff);
    __vmwrite(HOST_IA32_EFER, __rdmsr(0xC0000080));

    __vmwrite(HOST_IDTR_BASE, idt);

    __vmwrite(GUEST_CR0, __read_cr0() & (~CR0_PE) & (~CR0_PG));
    __vmwrite(GUEST_CR3, 0);
    __vmwrite(GUEST_CR4, __read_cr4() & (~CR4_PAE));
    __vmwrite(GUEST_DR7, __read_dr7());
    __vmwrite(GUEST_RIP, guest_ap);
    __vmwrite(GUEST_RSP, 0);
    __vmwrite(GUEST_RFLAGS, __read_rflags());
    // CS
    __vmwrite(GUEST_CS, 0);
    __vmwrite(GUEST_CS_BASE, 0ull);
    __vmwrite(GUEST_CS_LIMIT, 0xffffull);
    __vmwrite(GUEST_CS_ACCESS_RIGHTS,  GDT_AB_A | GDT_AB_RW | GDT_AB_E | GDT_AB_S | GDT_AB_P | GDT_AB_L | GDT_AB_G);
    // ES
    __vmwrite(GUEST_ES, 0);
    __vmwrite(GUEST_ES_BASE, 0ull);
    __vmwrite(GUEST_ES_LIMIT, 0xffffull);
    __vmwrite(GUEST_ES_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // SS (the selector)
    __vmwrite(GUEST_SS, 0);
    __vmwrite(GUEST_SS_BASE, 0ull);
    __vmwrite(GUEST_SS_LIMIT, 0xffffull);
    __vmwrite(GUEST_SS_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // DS
    __vmwrite(GUEST_DS, 0);
    __vmwrite(GUEST_DS_BASE, 0ull);
    __vmwrite(GUEST_DS_LIMIT, 0xffffull);
    __vmwrite(GUEST_DS_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // FS
    __vmwrite(GUEST_FS, 0);
    __vmwrite(GUEST_FS_BASE, 0ull);
    __vmwrite(GUEST_FS_LIMIT, 0xffffull);
    __vmwrite(GUEST_FS_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // GS
    __vmwrite(GUEST_GS, 0);
    __vmwrite(GUEST_GS_BASE, 0ull);
    __vmwrite(GUEST_GS_LIMIT, 0xffffull);
    __vmwrite(GUEST_GS_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_S | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // TR
    __vmwrite(GUEST_TR, 0);
    __vmwrite(GUEST_TR_BASE, 0ull);
    __vmwrite(GUEST_TR_LIMIT, 0xffffull);
    __vmwrite(GUEST_TR_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_E | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // GDTR
    gdtr_t gdtr;
    __read_gdtr(&gdtr);
    __vmwrite(GUEST_GDTR_BASE, gdtr.base);
    __vmwrite(GUEST_GDTR_LIMIT, gdtr.limit);
    __vmwrite(HOST_GDTR_BASE, gdtr.base);
    // IDTR
    __vmwrite(GUEST_IDTR_BASE, 0);
    __vmwrite(GUEST_IDTR_LIMIT, 0x3ff);
    // LDTR
    __vmwrite(GUEST_LDTR, 0);
    __vmwrite(GUEST_LDTR_BASE, 0);
    __vmwrite(GUEST_LDTR_LIMIT, 0xff);
    __vmwrite(GUEST_LDTR_ACCESS_RIGHTS, UNUSABLE_SELECTOR);
    
    __vmwrite(GUEST_ACTIVITY_STATE, 0ull);
    __vmwrite(GUEST_IA32_SYSENTER_EIP, 0xffff);
    __vmwrite(GUEST_IA32_SYSENTER_ESP, 0xffff);
    __vmwrite(GUEST_IA32_SYSENTER_CS, 8);
    __vmwrite(GUEST_VMCS_LINK_PTR, -1ull);
    __vmwrite(CONTROL_MSR_BITMAPS, shared_cores_data.msr_bitmaps);
    __vmwrite(GUEST_IA32_EFER, 0);
    // LOG_DEBUG("efer is %b\n", __vmread(GUEST_IA32_EFER));


    // Intel Manual Appendix A
    pin_based_ctls_t pin_based_ctls = {0};
    proc_based_ctls_t proc_based_ctls = {0};
    proc_based_ctls2_t proc_based_ctls2 = {0};
    vmexit_ctls_t vmexit_ctls = {0};
    vmentry_ctls_t vmentry_ctls = {0};

    proc_based_ctls.activate_secondary_controls = TRUE;
    proc_based_ctls.use_msr_bitmaps = TRUE;
    proc_based_ctls2.enable_xsaves_xrstors = TRUE;
    proc_based_ctls2.enable_ept = TRUE;
    proc_based_ctls2.unrestricted_guest = TRUE;
    proc_based_ctls2.enable_rdtscp = TRUE;
    proc_based_ctls2.enable_invpcid = TRUE;
    vmexit_ctls.host_address_space_size = TRUE;
    vmexit_ctls.save_ia32_efer = TRUE;
    vmexit_ctls.load_ia32_efer = TRUE;
    vmentry_ctls.load_ia32_efer = TRUE;


    if (__rdmsr(IA32_VMX_BASIC) & (1ull<<55)){
        __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, __rdmsr(IA32_VMX_TRUE_PINBASED_CTLS) | pin_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, __rdmsr(IA32_VMX_TRUE_PROCBASED_CTLS) | proc_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_VMEXIT_CONTROLS, __rdmsr(IA32_VMX_TRUE_EXIT_CTLS) | vmexit_ctls.value);
        __vmwrite(CONTROL_VMENTRY_CONTROLS, __rdmsr(IA32_VMX_TRUE_ENTRY_CTLS) | vmentry_ctls.value);
    } else {
        __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, __rdmsr(IA32_VMX_PINBASED_CTLS) | pin_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, __rdmsr(IA32_VMX_PROCBASED_CTLS) | proc_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_VMEXIT_CONTROLS, __rdmsr(IA32_VMX_EXIT_CTLS) | vmexit_ctls.value);
        __vmwrite(CONTROL_VMENTRY_CONTROLS, __rdmsr(IA32_VMX_ENTRY_CTLS) | vmentry_ctls.value);
    }

    eptp_t eptp = {0};
    eptp.ept_pml4 = shared_cores_data.ept_pml4;
    eptp.page_walk_length_m1 = 4-1;
    eptp.memory_type = WRITEBACK;

    __vmwrite(CONTROL_SECONDARY_EXECUTION_CONTROLS, proc_based_ctls2.value);
    __vmwrite(CONTROL_EPTP, eptp.value);

    __vmwrite(CONTROL_XSS_EXITING_BITMAP, 0);

    __vmwrite(CONTROL_EXCEPTION_BITMAP, (1<<INT3));
    // __vmwrite(CONTROL_EXCEPTION_BITMAP, 0xffffffff);
}

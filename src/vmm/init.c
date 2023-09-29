/*
Sources:
    OSDev - VMX page
*/

#include <rsdt.h>
#include <types.h>
#include <init.h>
#include <vmcs.h>
#include <instr.h>
#include <ept.h>

extern byte_t *_sys_stack(void);

qword_t initialize_host_paging(){
    qword_t *pml4 = (qword_t*)allocate_memory(PAGE_SIZE);                   // page map level 4
    qword_t *pdpt = (qword_t*)allocate_memory(PAGE_SIZE*1);                 // page directory pointer table
    qword_t *pd = (qword_t*)allocate_memory(PAGE_SIZE*COMPUTER_RAM);        // page directory; points to pages and not page tables bc size=2mb.

    // connect pml4 to pdpt
    pml4[0] = (qword_t)pdpt | PTE_P | PTE_W;

    // connect pdpt to pd
    for(uint32_t i = 0; i<COMPUTER_RAM; i++){
        pdpt[i] = (qword_t)(&pd[i*512]) | PTE_P | PTE_W;
    }

    // connect pd to pages
    for(uint32_t i = 0; i<COMPUTER_RAM*512; i++){
        pd[i] = (i*LARGE_PAGE_SIZE) | PTE_P | PTE_W | PTE_PS;
    }

    return (qword_t)pml4;
}


qword_t initialize_ept(){
    ept_pml4e_t *ept_pml4 = (ept_pml4e_t*)allocate_memory(PAGE_SIZE);
    ept_pdpte_t *ept_pdpt = (ept_pdpte_t*)allocate_memory(PAGE_SIZE*1);
    ept_pde_t *ept_pd = (ept_pde_t*)allocate_memory(PAGE_SIZE*COMPUTER_RAM);
    ept_pte_t *ept_pt = (ept_pte_t*)allocate_memory(PAGE_SIZE*(COMPUTER_RAM*PAGE_SIZE/sizeof(ept_pde_t)));

    ept_pml4[0].next_pd = (qword_t)ept_pdpt;
    ept_pml4[0].read_access = TRUE;
    ept_pml4[0].write_access = TRUE;
    ept_pml4[0].execute_access = TRUE;

    for(uint32_t i = 0; i<COMPUTER_RAM; i++){
        ept_pdpt[i].next_pd = (qword_t)&ept_pd[i*512];
        ept_pdpt[i].read_access = TRUE;
        ept_pdpt[i].write_access = TRUE;
        ept_pdpt[i].execute_access = TRUE;
    }

    for(uint32_t i = 0; i<COMPUTER_RAM*(PAGE_SIZE/sizeof(ept_pde_t)); i++){
        ept_pd[i].next_pd = (qword_t)&ept_pt[i*512];
        ept_pd[i].read_access = TRUE;
        ept_pd[i].write_access = TRUE;
        ept_pd[i].execute_access = TRUE;
    }

    for(uint64_t i = 0; i<COMPUTER_RAM*(PAGE_SIZE/sizeof(ept_pde_t))*(PAGE_SIZE/sizeof(ept_pte_t)); i++){
        ept_pt[i].page = i*PAGE_SIZE;
        ept_pt[i].read_access = TRUE;
        ept_pt[i].write_access = TRUE;
        ept_pt[i].execute_access = TRUE;
        ept_pt[i].memory_type = WRITEBACK;
    }

    return (qword_t)ept_pml4;
}

void prepare_vmxon(byte_t *vmxon_region_ptr){
    // TODO check that vmx is available in cpuid. Also take care of edge cases (intel manual vol. 3c, section 23.8)
    if (!(__get_cpuid() & CPUID_VMXON))
        LOG_ERROR("No VMX support in cpuid\n");
    __write_cr0((__read_cr0() | __read_msr(IA32_VMX_CR0_FIXED0) | CR0_NE) & __read_msr(IA32_VMX_CR0_FIXED1));
    __write_cr4((__read_cr4() | __read_msr(IA32_VMX_CR4_FIXED0) | CR4_VMXE) & __read_msr(IA32_VMX_CR4_FIXED1));
    *(dword_t*)vmxon_region_ptr = (dword_t)__read_msr(IA32_VMX_BASIC);  // revision identifier
}

void prepare_vmcs(vmcs_t *vmcs_ptr){
    vmcs_ptr->revision_id = (dword_t)__read_msr(IA32_VMX_BASIC);
    vmcs_ptr->shadow_vmcs_indicator = FALSE;
}

void vmentry_handler(){
    LOG_INFO("Entered the VM Entry handler\n");
    for(;;);
}

void vmexit_handler(){
    LOG_INFO("Entered the VM Exit handler\n");
    LOG_DEBUG("Exit information: %d\n", __vmread(RODATA_EXIT_REASON));
    LOG_DEBUG("Exit qualification: %d\n", __vmread(RODATA_EXIT_QUALIFICATION));
    LOG_DEBUG("Exit interruption information: %d (%d)\n", __vmread(RODATA_VMEXIT_INTERRUPTION_INFO), __vmread(RODATA_VMEXIT_INTERRUPTION_INFO) & 0xff);
    LOG_DEBUG("Exit interruption code: %d\n", __vmread(RODATA_VMEXIT_INTERRUPTION_ERRORCODE));
}

void initialize_vmcs(){

    __vmwrite(HOST_CR0, __read_cr0());
    __vmwrite(HOST_CR3, initialize_host_paging());
    __vmwrite(HOST_CR4, __read_cr4());
    __vmwrite(HOST_RIP, vmexit_handler);
    __vmwrite(HOST_RSP, _sys_stack);    // todo create a new VMM stack, hidden from VM
    __vmwrite(HOST_ES, __read_es());
    __vmwrite(HOST_CS, __read_cs());
    __vmwrite(HOST_SS, __read_ss());
    __vmwrite(HOST_DS, __read_ds());
    __vmwrite(HOST_TR, __read_ds());
    __vmwrite(HOST_FS, 0);
    __vmwrite(HOST_GS, 0);
    __vmwrite(HOST_FS_BASE, CANONICAL_ADDRESS);
    __vmwrite(HOST_GS_BASE, CANONICAL_ADDRESS);
    __vmwrite(HOST_GDTR_BASE, CANONICAL_ADDRESS);
    __vmwrite(HOST_IDTR_BASE, CANONICAL_ADDRESS);
    __vmwrite(HOST_TR_BASE, CANONICAL_ADDRESS);
    __vmwrite(HOST_IA32_SYSENTER_EIP, CANONICAL_ADDRESS);
    __vmwrite(HOST_IA32_SYSENTER_ESP, CANONICAL_ADDRESS);
    
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
    __vmwrite(GUEST_TR, __read_ds());
    __vmwrite(GUEST_TR_BASE, 0ull);
    __vmwrite(GUEST_TR_LIMIT, 0xffffffff);
    __vmwrite(GUEST_TR_ACCESS_RIGHTS, GDT_AB_A | GDT_AB_RW | GDT_AB_E | GDT_AB_P | GDT_AB_DB | GDT_AB_G);
    // GDTR
    gdtr_t gdtr;
    __read_gdtr(&gdtr);
    __vmwrite(GUEST_GDTR_BASE, gdtr.base);
    __vmwrite(GUEST_GDTR_LIMIT, gdtr.limit);
    // IDTR
    idtr_t idtr;
    __read_idtr(&idtr);
    __vmwrite(GUEST_IDTR_BASE, idtr.base);
    __vmwrite(GUEST_IDTR_LIMIT, idtr.limit);
    // LDTR
    __vmwrite(GUEST_LDTR, 0);
    __vmwrite(GUEST_LDTR_BASE, 0);
    __vmwrite(GUEST_LDTR_LIMIT, 0xff);
    __vmwrite(GUEST_LDTR_ACCESS_RIGHTS, UNUSABLE_SELECTOR);

    __vmwrite(GUEST_ACTIVITY_STATE, 0ull);
    __vmwrite(GUEST_IA32_SYSENTER_EIP, CANONICAL_ADDRESS);
    __vmwrite(GUEST_IA32_SYSENTER_ESP, CANONICAL_ADDRESS);
    __vmwrite(GUEST_VMCS_LINK_PTR, -1ll);

    // Intel Manual Appendix A
    pin_based_ctls_t pin_based_ctls = {0};
    proc_based_ctls_t proc_based_ctls = {0};
    proc_based_ctls2_t proc_based_ctls2 = {0};
    vmexit_ctls_t vmexit_ctls = {0};
    vmentry_ctls_t vmentry_ctls = {0};

    proc_based_ctls.activate_secondary_controls = TRUE;
    proc_based_ctls2.enable_ept = TRUE;
    vmexit_ctls.host_address_space_size = TRUE;
    vmentry_ctls.ia32_mode_guest = TRUE;

    if (__read_msr(IA32_VMX_BASIC) & (1ull<<55)){
        __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, __read_msr(IA32_VMX_TRUE_PINBASED_CTLS) | DEFAULT_PINBASED_CTLS | pin_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, __read_msr(IA32_VMX_TRUE_PROCBASED_CTLS) | DEFAULT_PROCBASED_CTLS | proc_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_VMEXIT_CONTROLS, __read_msr(IA32_VMX_TRUE_EXIT_CTLS) | DEFAULT_EXIT_CTLS | vmexit_ctls.value);
        __vmwrite(CONTROL_VMENTRY_CONTROLS, __read_msr(IA32_VMX_TRUE_ENTRY_CTLS) | DEFAULT_ENTRY_CTLS | vmentry_ctls.value);
    } else {
        __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, __read_msr(IA32_VMX_PINBASED_CTLS) | pin_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS, __read_msr(IA32_VMX_PROCBASED_CTLS) | proc_based_ctls.value);
        __vmwrite(CONTROL_PRIMARY_VMEXIT_CONTROLS, __read_msr(IA32_VMX_EXIT_CTLS) | vmexit_ctls.value);
        __vmwrite(CONTROL_VMENTRY_CONTROLS, __read_msr(IA32_VMX_ENTRY_CTLS) | vmentry_ctls.value);
    }

    eptp_t eptp = {0};
    eptp.ept_pml4 = initialize_ept();
    eptp.page_walk_length_m1 = 4-1;
    eptp.memory_type = WRITEBACK;

    __vmwrite(CONTROL_SECONDARY_EXECUTION_CONTROLS, proc_based_ctls2.value);
    __vmwrite(CONTROL_EPTP, eptp.value);
    __vmwrite(CONTROL_EXCEPTION_BITMAP, 0xffffffff);


}

void init_vmm(){

    // LOG_DEBUG("physical_address_width is %q\n", __get_physical_address_width());
    // LOG_DEBUG("IA32_VMX_EPT_VPID_CAP is %b\n", __read_msr(IA32_VMX_EPT_VPID_CAP));
    // LOG_DEBUG("CR0 is %b\n", __read_cr0());
    // __hlt();

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

    __vmwrite(GUEST_RSP, __read_rsp());
    __vmlaunch();
}

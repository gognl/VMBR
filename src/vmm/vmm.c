#include <hardware/rsdt.h>
#include <lib/types.h>
#include <vmm/vmm.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <lib/msr.h>
#include <hardware/apic.h>
#include <boot/mmap.h>
#include <vmm/hooks.h>
#include <lib/util.h>
#include <vmm/paging.h>

shared_cores_data_t __attribute__((section(".vmm"))) shared_cores_data = {0};

void prepare_vmxon(byte_t *vmxon_region_ptr){
    qword_t ecx, tmp;
    __cpuid(1, 0, &tmp, &tmp, &ecx, &tmp);
    if (!(ecx & CPUID_VMXON))
        LOG_ERROR("No VMX support in cpuid\n");
    __write_cr0((__read_cr0() | __rdmsr(IA32_VMX_CR0_FIXED0) | CR0_NE) & __rdmsr(IA32_VMX_CR0_FIXED1));
    __write_cr4((__read_cr4() | __rdmsr(IA32_VMX_CR4_FIXED0) | CR4_VMXE | CR4_OSXSAVE) & __rdmsr(IA32_VMX_CR4_FIXED1));
    *(dword_t*)vmxon_region_ptr = (dword_t)__rdmsr(IA32_VMX_BASIC);  // revision identifier
}

void prepare_vmcs(vmcs_t *vmcs_ptr){
    vmcs_ptr->revision_id = (dword_t)__rdmsr(IA32_VMX_BASIC);
    vmcs_ptr->shadow_vmcs_indicator = FALSE;
}

void protect_vmm_memory(){

    extern byte_t vmm_start[];
    extern byte_t vmm_end[];
    extern byte_t vmbr_end[];
    LOG_DEBUG("vmbr_end is %x\n", vmbr_end);

    qword_t aligned_bottom = ALIGN_DOWN((qword_t)vmm_start, PAGE_SIZE);
    qword_t aligned_top = ALIGN_UP((qword_t)vmm_end, PAGE_SIZE);

    ept_pte_t *pte;
    
    for (uint64_t current_page = aligned_bottom; current_page < aligned_top; current_page += PAGE_SIZE){
        pte = get_ept_pte_from_guest_address(current_page);
        qword_t new_page = allocate_memory(PAGE_SIZE);
        if (current_page == 0xf000){    // for the vmcall hooks
            memcpy(new_page, current_page, PAGE_SIZE);
        }
        LOG_DEBUG("Mapped %x to %x\n", current_page, new_page);
        modify_pte_page(pte, new_page);
    }

    // Also keep 0x3000 page for extra uses
    pte = get_ept_pte_from_guest_address(0x3000);
    qword_t new_page = allocate_memory(PAGE_SIZE);
    modify_pte_page(pte, new_page);

    // And protect the allocated pages
    uint64_t bottom_allocation = get_bottom_allocation(), top_allocation = get_top_allocation();
    for (uint64_t current_page = bottom_allocation; current_page<top_allocation; current_page += PAGE_SIZE){
        pte = get_ept_pte_from_guest_address(current_page);
        // LOG_DEBUG("Protected %x\n", current_page);
        modify_pte_access(pte, 0, 0, 0);   
    }

    invept_descriptor_t descriptor;
    descriptor.eptp = (eptp_t)__vmread(CONTROL_EPTP);
    descriptor.zeros = 0;
    __invept(&descriptor, 1);

}

void vmentry_handler(){

    // activate_x2apic();
    // __wrmsr(IA32_APIC_BASE, __rdmsr(IA32_APIC_BASE) | X2APIC_ENABLE | XAPIC_GLOBAL_ENABLE);
    // broadcast_init_ipi();
    // sleep();
    // broadcast_sipi_ipi();
    // init_cores_apic();
    // broadcast_init_ipi_apic();
    // sleep();
    // broadcast_sipi_ipi_apic();
    load_guest();

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

void prepare_vmm_ap(){
    byte_t *vmxon_region_ptr = allocate_memory(0x1000);   // 4kb aligned. size should actually be read from IA32_VMX_BASIC[32:44], but it's 0x1000 max.
    prepare_vmxon(vmxon_region_ptr);
    __vmxon(vmxon_region_ptr);

    vmcs_t* vmcs_ptr = (vmcs_t*)allocate_memory(0x1000);   // 4kb aligned. size should actually be read from IA32_VMX_BASIC[32:44], but it's 0x1000 max.
    prepare_vmcs(vmcs_ptr);
    __vmclear(vmcs_ptr);
    __vmptrld(vmcs_ptr);

    initialize_vmcs_ap();
    LOG_DEBUG("Core %d initialized.\n", get_current_core_id());
    __vmwrite(GUEST_RSP, __read_rsp());
    __vmlaunch();
}

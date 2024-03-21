#include <hooks/keyboard.h>
#include <vmm/vmm.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <vmm/paging.h>

__attribute__((section(".vmm"))) uint64_t guest_virtual_to_physical(uint64_t addr, uint64_t cr3){
    
    qword_t *pml4 = cr3, *pml3, *pml2, *pml1;
    qword_t pml4e, pml3e, pml2e, pml1e;

    pml4e = pml4[PML4E_IDX(addr)];
    if (!(pml4e & PAGING_P)) return 0;

    pml3 = pml4e & (~0xfff);
    pml3e = pml3[PML3E_IDX(addr)];
    if (!(pml3e & PAGING_P)) return 0;
    if (pml3e & PAGING_PS) return (pml3e & (~0xfff)) + PAGING_OFFSET_1GB(addr);

    pml2 = pml3e & (~0xfff);
    pml2e = pml2[PML2E_IDX(addr)];
    if (!(pml2e & PAGING_P)) return 0;
    if (pml2e & PAGING_PS) return (pml2e & (~0xfff)) + PAGING_OFFSET_2MB(addr);

    pml1 = pml2e & (~0xfff);
    pml1e = pml1[PML1E_IDX(addr)];
    if (!(pml1e & PAGING_P)) return 0;
    return (pml1e & (~0xfff)) + PAGING_OFFSET_4KB(addr);

}

__attribute__((section(".vmm"))) void hook_function(byte_t *func){
    *func = 0xcc;
    __vmwrite(CONTROL_EXCEPTION_BITMAP, __vmread(CONTROL_EXCEPTION_BITMAP) | (1<<3));
}

__attribute__((section(".vmm"))) void handle_lstar_write(uint64_t lstar){

    // LSTAR is either KiSystemCall64Shadow or KiSystemCall64. Try both.
    if (!((lstar-NTOSKRNL_KiSystemCall64Shadow_OFFSET) & 0xfff))
        shared_cores_data.ntoskrnl = lstar-NTOSKRNL_KiSystemCall64Shadow_OFFSET;
    else if (!((lstar-NTOSKRNL_KiSystemCall64_OFFSET) & 0xfff))
        shared_cores_data.ntoskrnl = lstar-NTOSKRNL_KiSystemCall64_OFFSET;
    else
        LOG_ERROR("FAILED TO FIND NTOSKRNL (LSTAR=%x)\n", lstar);

    uint64_t MiDriverLoadSucceeded = shared_cores_data.ntoskrnl + NTOSKRNL_MiDriverLoadSucceeded_OFFSET;
    uint64_t ntoskrnl_phys = guest_virtual_to_physical(shared_cores_data.ntoskrnl, __vmread(GUEST_CR3));
    uint64_t MiDriverLoadSucceeded_phys = guest_virtual_to_physical(MiDriverLoadSucceeded, __vmread(GUEST_CR3));
    
    clear_msr_bitmap_write(LSTAR_MSR, shared_cores_data.msr_bitmaps);

    // hook_function(guest_virtual_to_physical(shared_cores_data.ntoskrnl+NTOSKRNL_MiDriverLoadSucceeded_OFFSET));
}


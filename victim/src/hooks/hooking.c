#include <hooks/hooking.h>
#include <lib/instr.h>
#include <lib/util.h>
#include <vmm/paging.h>
#include <hooks/pe.h>

__attribute__((section(".vmm"))) uint64_t guest_virtual_to_physical(uint64_t addr){
    qword_t *pml4 = __vmread(GUEST_CR3), *pml3, *pml2, *pml1;
    qword_t pml4e, pml3e, pml2e, pml1e;

    pml4e = pml4[PML4E_IDX(addr)];
    if (!(pml4e & PAGING_P)) return 0;

    pml3 = pml4e & PAGING_ENTRY_MASK;
    pml3e = pml3[PML3E_IDX(addr)];
    if (!(pml3e & PAGING_P)) return 0;
    if (pml3e & PAGING_PS) return (pml3e & PAGING_ENTRY_MASK) + PAGING_OFFSET_1GB(addr);

    pml2 = pml3e & PAGING_ENTRY_MASK;
    pml2e = pml2[PML2E_IDX(addr)];
    if (!(pml2e & PAGING_P)) return 0;
    if (pml2e & PAGING_PS) return (pml2e & PAGING_ENTRY_MASK) + PAGING_OFFSET_2MB(addr);

    pml1 = pml2e & PAGING_ENTRY_MASK;
    pml1e = pml1[PML1E_IDX(addr)];
    if (!(pml1e & PAGING_P)) return 0;
    return (pml1e & PAGING_ENTRY_MASK) + PAGING_OFFSET_4KB(addr);

}

__attribute__((section(".vmm"))) void hook_function(byte_t *func, byte_t **x_page, byte_t *rw_page){

    *x_page = ALIGN_DOWN((qword_t)func, PAGE_SIZE);
    memcpy(rw_page, *x_page, PAGE_SIZE);
    *func = INT3_OPCODE;
    modify_pte_access(get_ept_pte_from_guest_address(*x_page), 0, 0, 1);

    invept_descriptor_t descriptor;
    descriptor.eptp = (eptp_t)__vmread(CONTROL_EPTP);
    descriptor.zeros = 0;
    __invept(&descriptor, 1);

}

__attribute__((section(".vmm"))) uint64_t find_signature(uint64_t addr, byte_t *sign, uint32_t sign_len){
    uint32_t module_len = get_image_size(addr);

    uint64_t current_virt = addr;
    byte_t *current_phys;

    while (current_virt < addr + module_len){
        if (current_virt % PAGE_SIZE == 0) current_phys = guest_virtual_to_physical(current_virt);

        if (memcmp(current_phys, sign, sign_len)) return current_virt;

        current_virt++;
        current_phys++;
    }

    return 0;

}

#include <lib/types.h>
#include <vmm/paging.h>
#include <boot/mmap.h>
#include <lib/msr.h>
#include <lib/instr.h>

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

void update_ept_caching_policy(ept_pte_t *ept_pt){

    // Fixed-Ranged MTRRs 
    if (((ia32_mtrr_def_type_t)__rdmsr(IA32_MTRR_DEF_TYPE)).fixed_enabled && ((ia32_mtrrcap_t)__rdmsr(IA32_MTRRCAP)).fix){
        uint64_t memory_type = ((ia32_mtrr_def_type_t)__rdmsr(IA32_MTRR_DEF_TYPE)).type;
        for (uint32_t i = 0; i<0x100000; i += PAGE_SIZE){
            if (i < 0x80000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX64K_00000), 0, 0x10000);
            else if (i < 0xa0000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX16K_80000), 0x80000, 0x4000);
            else if (i < 0xc0000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX16K_A0000), 0xa0000, 0x4000);
            else if (i < 0xc8000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX4K_C0000), 0xc0000, 0x1000);
            else if (i < 0xd0000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX4K_C8000), 0xc8000, 0x1000);
            else if (i < 0xd8000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX4K_D0000), 0xd0000, 0x1000);
            else if (i < 0xe0000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX4K_D8000), 0xd8000, 0x1000);
            else if (i < 0xe8000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX4K_E0000), 0xe0000, 0x1000);
            else if (i < 0xf0000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX4K_E8000), 0xe8000, 0x1000);
            else if (i < 0xf8000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX4K_F0000), 0xf0000, 0x1000);
            else if (i < 0x10000)
                memory_type = GET_TYPE_RANGED(i, __rdmsr(IA32_MTRR_FIX4K_F8000), 0xf8000, 0x1000);
            ept_pt[i/PAGE_SIZE].memory_type = memory_type;
        }
    }

    // Variable Range MTRRs
    qword_t mtrr_count = ((ia32_mtrrcap_t)__rdmsr(IA32_MTRRCAP)).vcnt;
    for (qword_t i = 0; i<mtrr_count; i++){

        ia32_mtrr_physbase_t base = {__rdmsr(IA32_MTRR_PHYSBASE0+2*i)};
        ia32_mtrr_physmask_t mask = {__rdmsr(IA32_MTRR_PHYSMASK0+2*i)};
        
        if (!mask.valid) continue;

        qword_t base_address = base.page_idx * PAGE_SIZE;
        qword_t length = GET_ENTRY_LENGTH(mask.page_idx*PAGE_SIZE);

        for (qword_t j = base_address/PAGE_SIZE; j<(base_address+length)/PAGE_SIZE; j++){
            ept_pt[j].memory_type = base.type;
        }
    }
}

qword_t initialize_ept(){

    qword_t default_caching_type = ((ia32_mtrr_def_type_t)__rdmsr(IA32_MTRR_DEF_TYPE)).type;

    if (!(__rdmsr(IA32_VMX_EPT_VPID_CAP) & ((1ull<<6) | (1ull<<14))))
        LOG_ERROR("Some necessary features of EPT are not supported.\n");

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

    for(uint64_t i = 0; i<(COMPUTER_RAM*(PAGE_SIZE/sizeof(ept_pde_t))*(PAGE_SIZE/sizeof(ept_pte_t))); i++){
        ept_pt[i].page = i*PAGE_SIZE;
        ept_pt[i].read_access = TRUE;
        ept_pt[i].write_access = TRUE;
        ept_pt[i].execute_access = TRUE;
        ept_pt[i].memory_type = default_caching_type;
    }

    update_ept_caching_policy(ept_pt);

    return (qword_t)ept_pml4;
}

ept_pte_t *get_ept_pte_from_guest_address(qword_t address){
    ept_pml4e_t* pml4 = ((eptp_t)__vmread(CONTROL_EPTP)).ept_pml4 & ~(0xfff);

    ept_pdpte_t* pdpt = pml4[ADDRMASK_EPT_PML4_INDEX(address)].next_pd & ~(0xfff);
    ept_pde_t* pd = pdpt[ADDRMASK_EPT_PDPT_INDEX(address)].next_pd & ~(0xfff);
    ept_pde_t* pt = pd[ADDRMASK_EPT_PD_INDEX(address)].next_pd & ~(0xfff);

    return &pt[ADDRMASK_EPT_PT_INDEX(address)];

}

void modify_pte_page(ept_pte_t *pte, qword_t page){
    pte->page &= 0xff00000000000fff;
    pte->page |= page;
}

void modify_pte_access(ept_pte_t *pte, uint8_t read, uint8_t write, uint8_t execute){
    pte->read_access = read;
    pte->write_access = write;
    pte->execute_access = execute;
}

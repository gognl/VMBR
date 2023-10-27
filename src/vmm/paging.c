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

qword_t initialize_ept(){

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

    for(uint64_t i = 0; i<COMPUTER_RAM*(PAGE_SIZE/sizeof(ept_pde_t))*(PAGE_SIZE/sizeof(ept_pte_t)); i++){
        ept_pt[i].page = i*PAGE_SIZE;
        ept_pt[i].read_access = TRUE;
        ept_pt[i].write_access = TRUE;
        ept_pt[i].execute_access = TRUE;
        ept_pt[i].memory_type = WRITEBACK;
    }

    return (qword_t)ept_pml4;
}
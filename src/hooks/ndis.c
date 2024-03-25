#include <hooks/ndis.h>
#include <vmm/vmcs.h>
#include <vmm/vmm.h>
#include <lib/instr.h>
#include <hooks/hooking.h>
#include <lib/util.h>

__attribute__((section(".vmm"))) void handle_NdisSendNetBufferLists_hook(vmexit_data_t *state){

    LOG_DEBUG("In NdisSendNetBufferLists\n");

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack);
    *(uint64_t*)guest_stack_phys = state->registers->rbp;

    uint64_t PNetBufferLists = state->registers->rdx;
    if (*(uint64_t*)guest_virtual_to_physical(NET_BUFFER_LIST_Next(PNetBufferLists)) != 0) return;
    uint64_t PNetBuffer = *(uint64_t*)guest_virtual_to_physical(NET_BUFFER_LIST_FirstNetBuffer(PNetBufferLists));
    if (*(uint64_t*)guest_virtual_to_physical(NET_BUFFER_Next(PNetBuffer)) != 0) return;
    uint64_t PMdl = *(uint64_t*)guest_virtual_to_physical(NET_BUFFER_CurrentMdl(PNetBuffer));
    if (*(uint64_t*)guest_virtual_to_physical(MDL_Next(PMdl)) != 0) return;

    // Now left only with single of each (one-node lists only).

    if (*(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer)) < 202) return;

    // Now left only with packets which have 202 bytes of free space in their buffer

    LOG_DEBUG("Has data offset of %d\n", *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer)));

    uint64_t pkt_pfn = *(uint64_t*)guest_virtual_to_physical(MDL_PhysicalPage(PMdl));
    uint32_t byte_offset = *(uint32_t*)guest_virtual_to_physical(MDL_ByteOffset(PMdl));
    uint32_t data_offset = *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer));
    uint32_t data_length = *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataLength(PNetBuffer));

    byte_t *physical_pkt_addr  = pkt_pfn*PAGE_SIZE + byte_offset + data_offset;

    *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer)) = 0;   // Change DataOffset to 0
    *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_CurrentMdlOffset(PNetBuffer)) = 0;   // Change CurrentMdlOffset to 0
    *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataLength(PNetBuffer)) = 256;       // Change DataLength to 256
    
    byte_t *new_pkt = pkt_pfn*PAGE_SIZE + byte_offset;

    new_pkt[0] = 0x11;
    new_pkt[1] = 0x11;
    new_pkt[2] = 0x11;
    new_pkt[3] = 0x11;
    new_pkt[4] = 0x11;
    new_pkt[5] = 0x11;

}

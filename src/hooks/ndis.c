#include <hooks/ndis.h>
#include <vmm/vmcs.h>
#include <vmm/vmm.h>
#include <lib/instr.h>

__attribute__((section(".vmm"))) void handle_NdisSendNetBufferLists_hook(vmexit_data_t *state){

    LOG_DEBUG("In NdisSendNetBufferLists\n");

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack, __vmread(GUEST_CR3));
    *(uint64_t*)guest_stack_phys = state->registers->rbp;

}

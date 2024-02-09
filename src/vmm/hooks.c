#include <vmm/hooks.h>
#include <lib/util.h>
#include <vmm/vmm.h>
#include <boot/mmap.h>
#include <lib/instr.h>
#include <boot/addresses.h>

#define LOWER_WORD(x) ((x) & 0xffffull)

void __attribute__((section(".vmm"))) emulate_iret(vmexit_data_t *state){

    // 16bit operand mode. https://www.felixcloutier.com/x86/iret:iretd:iretq

    word_t ip = *(word_t*)(__vmread(GUEST_SS_BASE) + __vmread(GUEST_RSP));   // pop ip
    word_t cs = *(word_t*)(__vmread(GUEST_SS_BASE) + __vmread(GUEST_RSP) + 2);   // pop cs
    word_t flags = *(word_t*)(__vmread(GUEST_SS_BASE) + __vmread(GUEST_RSP) + 4);   // pop flags

    __vmwrite(GUEST_RIP, (qword_t)ip);
    __vmwrite(GUEST_CS, cs);
    __vmwrite(GUEST_CS_BASE, cs<<4);
    __vmwrite(GUEST_RFLAGS, (__vmread(GUEST_RFLAGS) & ~(0xffff)) | (qword_t)flags);
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)+6); // for the pops
}

void setup_int15h_hook(){
    ivt_entry_t *ivt = (ivt_entry_t*)0;

    shared_cores_data.int15h_segment = ivt[0x15].segment;
    shared_cores_data.int15h_offset = ivt[0x15].offset;
    
    byte_t vmcall_opcode[] = {0x0f, 0x01, 0xc1};
    memcpy((byte_t*)HOOK_INT15H, vmcall_opcode, 3);
    
    ivt[0x15].segment = 0;
    ivt[0x15].offset = HOOK_INT15H;
}


void __attribute__((section(".vmm"))) handle_int15h_hook(vmexit_data_t *state){
    mmap_table_t *mmap = (mmap_table_t*)MMAP_TABLE;

    mmap_entry_t* address = (mmap_entry_t*)(__vmread(GUEST_ES_BASE) + LOWER_WORD(state->registers->rdi));
    qword_t idx = state->registers->rbx;

    if (idx >= mmap->length){
        *(word_t*)(__vmread(GUEST_SS_BASE) + __vmread(GUEST_RSP) + 4) |= RFLAGS_CARRY;  // will be clear in iret
        emulate_iret(state);
        return;
    }

    // load the entry
    address->base_addr = mmap->entries[idx].base_addr;
    address->length = mmap->entries[idx].length;
    address->type = mmap->entries[idx].type;
    
    state->registers->rbx++;    // increment idx
    state->registers->rax = E820_MAGIC;
    state->registers->rcx = (state->registers->rcx & ~(0xffull)) | 20;  // cl=20
    
    if (state->registers->rbx == mmap->length){
        state->registers->rbx = 0;
    }

    *(word_t*)(__vmread(GUEST_SS_BASE) + __vmread(GUEST_RSP) + 4) &= ~RFLAGS_CARRY;     // will be clear in iret
    emulate_iret(state);

}

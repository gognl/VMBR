#include <hardware/idt.h>
#include <lib/instr.h>

__attribute__((section(".vmmdata"), aligned(0x10))) idt_entry_t idt[256];

void set_idt_entry(void (*isr)(), uint8_t id){
    uint64_t offset = (uint64_t)isr;

    idt[id].offset_low16 = offset & 0xffffull;
    idt[id].offset_mid16 = (offset >> 16) & 0xffffull;
    idt[id].offset_high32 = (offset >> 32) & 0xffffffffull;
    idt[id].cs = GDT_CS_64;
    idt[id].ist = 0;
    idt[id].gate_type = IDT_GATE_TYPE_INTERRUPT;
    idt[id].p = 1;

}

__attribute__((section(".vmm"))) void exception_handler(uint64_t interrupt, interrupt_data_t *data){
    LOG_DEBUG("Exception %x: ERROR=%x, RIP=%x, CS=%x, RFLAGS=%x, RSP=%x\n",
                interrupt, data->error, data->rip, data->cs, data->rflags, data->rsp);
}

void init_idt(){

    for (uint32_t i = 0; i<32; i++){
        set_idt_entry(isr_stub_table[i], i);
    }
    
    idtr_t idtr = {.offset = idt, .size = 4096-1};
    __write_idtr(&idtr);
    __sti();
}

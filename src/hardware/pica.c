#include <hardware/pica.h>
#include <lib/instr.h>

void init_pic(){

    // Initialize PIC1 and PIC2 (ICW1)
    pic_icw1_t icw1 = {.ic4=1, .sngl=0, .adi=0, .ltim=0, .init=1};
    __outb(PIC1_COMMAND, icw1.value);
    __outb(PIC2_COMMAND, icw1.value);

    // Map PIC1 to IRQ32-39 and PIC2 to IRQ40-47 (ICW2)
    __outb(PIC1_DATA, 0x20);
    __outb(PIC2_DATA, 0x28);

    // Tell the PICs which IRQ to use when communicating with each other (ICW3)
    __outb(PIC1_DATA, 0b100);  // Use IRQ2
    __outb(PIC2_DATA, 0x2);  // Use IRQ2

    // Set x86 mode (ICW4)
    __outb(PIC1_DATA, 1);
    __outb(PIC2_DATA, 1);

    // Do not service timer interrupts
    __outb(PIC1_DATA, 1);
    __outb(PIC2_DATA, 0);
}

void pic_ack(uint8_t irq){
    if (irq >= 0x28)
        __outb(PIC2_COMMAND, PIC_EOI);
    __outb(PIC1_COMMAND, PIC_EOI);
}

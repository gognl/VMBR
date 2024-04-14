#ifndef __IDT_H
#define __IDT_H

#include <lib/types.h>

#define GDT_CS_64 0x8

#define IDT_GATE_TYPE_INTERRUPT 0xE

typedef struct {
    uint16_t size;
    uint64_t offset;
} __attribute__((__packed__)) idtr_t;

typedef struct {
    uint16_t offset_low16;
    uint16_t cs;
    uint8_t ist;
    uint16_t gate_type : 4;
    uint16_t : 1;
    uint16_t dpl : 2;
    uint16_t p : 1;
    uint16_t offset_mid16;
    uint32_t offset_high32;
    uint32_t : 32;
} __attribute__((__packed__)) idt_entry_t;
STATIC_ASSERT(sizeof(idt_entry_t) == 2*sizeof(uint64_t));

typedef struct __attribute__((__packed__)) {
    word_t offset;
    word_t segment;
} ivt_entry_t;
STATIC_ASSERT(sizeof(ivt_entry_t) == 2*sizeof(uint16_t));

typedef struct {
    uint64_t error;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
} __attribute__((__packed__)) interrupt_data_t;

extern uint64_t isr_stub_table[];

extern idt_entry_t idt[256];
extern void set_idt_entry(void (*isr)(), uint8_t id);
extern void init_idt();
#endif
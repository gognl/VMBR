#ifndef __PIC_H
#define __PIC_H

#include <lib/types.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define PIC_EOI 0x20

typedef union {
    uint8_t value;
    struct {
        uint8_t ic4 : 1;
        uint8_t sngl : 1;
        uint8_t adi : 1;
        uint8_t ltim : 1;
        uint8_t init : 1;
        uint8_t : 3;
    } __attribute__((__packed__));
} pic_icw1_t;
STATIC_ASSERT(sizeof(pic_icw1_t) == sizeof(uint8_t));

extern void init_pic();

#endif
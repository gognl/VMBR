#ifndef __APIC_H
#define __APIC_H

#include <lib/types.h>

#define CPUID_X2APIC (1<<21)

#define X2APIC_ENABLE (1<<10)
#define XAPIC_GLOBAL_ENABLE (1<<11)

typedef union {
    uint64_t value;
    struct {
        uint64_t vector : 8;                // 0-7
        uint64_t delivery_mode : 3;         // 8-10
        #define INIT 5
        #define SIPI 6
        uint64_t destination_mode : 1;      // 11
        uint64_t : 2;                       // 12-13
        uint64_t level : 1;                 // 14
        #define ASSERT 1
        uint64_t trigger_mode : 1;          // 15
        uint64_t : 2;                       // 16-17
        uint64_t destination_shorthand : 2; // 18-19
        uint64_t : 12;                      // 20-31
        uint64_t destination_field : 32;    // 32-63
    };
} x2apic_icr_t;

extern void init_cores();
extern void init_ap(byte_t apic_id, uint8_t page_idx);
extern uint8_t get_current_core_id();
// extern BOOL is_bsp();

#endif
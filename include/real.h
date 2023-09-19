#ifndef __REAL_H
#define __REAL_H

#include <types.h>

extern void init_mmap(void);
extern void init_real(void);
extern byte_t *allocate_memory(uint64_t length);

typedef struct {
    qword_t base_addr;
    qword_t length;
    dword_t type;
    #define E820_USABLE 1
    #define E820_RESERVED 2
    #define E820_ACPI_RECLAIMABLE 3
    #define E820_ACPI_NVS 4
    #define E820_BAD 5
    dword_t acpi_ea;
} __attribute__((__packed__)) mmap_entry_t;


typedef struct {
    dword_t length;
    mmap_entry_t entries[];
} __attribute__((__packed__)) mmap_table_t;

#endif
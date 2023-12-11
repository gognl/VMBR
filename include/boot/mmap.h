#ifndef __MMAP_H
#define __MMAP_H

#include <lib/types.h>

extern void init_mmap(void);
extern void init_real(void);
extern byte_t *allocate_memory(uint64_t length);

#define REAL_START 0x4000
#define MMAP_TABLE 0x5000

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
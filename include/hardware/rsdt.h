#ifndef __RSDT_H
#define __RSDT_H

#include <lib/types.h>

#define RSDP_MAGIC "RSD PTR "
#define EBDA_PTR_ADDR 0x040e
#define MADT_SIGNATURE "APIC"

typedef struct __attribute__((__packed__)) {
    char_t magic[8];
    byte_t checksum;
    char_t oem_id[6];
    byte_t revision;
    uint32_t rsdt_addr;
} rsdp_t;

typedef struct __attribute__((__packed__)) {
    rsdp_t original;
    uint32_t length;
    uint64_t xsdt_addr;
    byte_t extended_checksum;
    byte_t reserved[3];
} rsdp2_t;

typedef struct __attribute__((__packed__)) {
    byte_t magic[4];
    uint32_t length;
    byte_t revision;
    byte_t checksum;
    byte_t oem_id[6];
    byte_t oem_table_id[8];
    uint32_t OEM_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} acpi_sdt_header_t;

typedef struct __attribute__((__packed__)) {
    acpi_sdt_header_t header;
    uint32_t local_apic_addr;
    uint32_t flags;
    #define MADT_TABLE_START 0x2C
} madt_t;

typedef struct __attribute__((__packed__)) {
    byte_t type;
    #define MADT_TYPE_LOCAL_APIC 0
    byte_t length;
    union {
        struct __attribute__((__packed__)) {
            byte_t acpi_processor_id;
            byte_t apic_id;
            union {
                dword_t value;
                struct __attribute__((__packed__)) {
                    dword_t processor_enabled : 1;
                    dword_t online_capable : 1;
                };
            } flags;
        } type0;
    };
} madt_entry_header_t;

typedef struct __attribute__((__packed__)) {
    acpi_sdt_header_t h;
    uint64_t sdt_ptr[];
} xsdt_t;

typedef struct __attribute__((__packed__)) {
    acpi_sdt_header_t h;
    uint32_t sdt_ptr[];
} rsdt_t;

extern uint32_t get_cpu_count(void);
extern void* search_SDT(rsdp_t *rsdp_ptr, char_t signature[4]);
extern rsdp_t* detect_rsdp(void);
#endif

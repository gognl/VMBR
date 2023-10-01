#ifndef __RSDT_H
#define __RSDT_H

#include <lib/types.h>

#define RSDP_MAGIC "RSD PTR "
#define EBDA_PTR_ADDR 0x040e
#define MADT_SIGNATURE "APIC"

typedef struct {
    char_t magic[8];
    byte_t checksum;
    char_t oem_id[6];
    byte_t revision;
    uint32_t rsdt_addr;
} __attribute__((__packed__)) rsdp_t;

typedef struct {
    rsdp_t original;
    uint32_t length;
    uint64_t xsdt_addr;
    byte_t extended_checksum;
    byte_t reserved[3];
} __attribute__((__packed__)) rsdp2_t;

typedef struct {
    byte_t magic[4];
    uint32_t length;
    byte_t revision;
    byte_t checksum;
    byte_t oem_id[6];
    byte_t oem_table_id[8];
    uint32_t OEM_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((__packed__)) acpi_sdt_header_t;

typedef struct {
    acpi_sdt_header_t header;
    uint32_t local_apic_addr;
    uint32_t flags;
    #define MADT_TABLE_START 0x2C
} __attribute__((__packed__)) madt_t;

typedef struct {
    byte_t type;
    byte_t length;
    #define MADT_TYPE_LOCAL_APIC 0
} __attribute__((__packed__)) madt_entry_header_t;

typedef struct {
    acpi_sdt_header_t h;
    uint64_t sdt_ptr[];
} __attribute__((__packed__)) xsdt_t;

typedef struct {
    acpi_sdt_header_t h;
    uint32_t sdt_ptr[];
} __attribute__((__packed__)) rsdt_t;

extern uint32_t get_cpu_count(void);
#endif

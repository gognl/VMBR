#include <types.h>

#define RSDP_MAGIC "RSD PTR "
#define EBDA_PTR_ADDR 0x040e
#define MADT_SIGNATURE "APIC"

typedef struct _RSDP {
    char_t magic[8];
    byte_t checksum;
    char_t OEM_id[6];
    byte_t revision;
    uint32_t rsdt_addr;
} __attribute__((__packed__)) RSDP;

typedef struct _RSDP2 {
    RSDP original;
    uint32_t length;
    uint64_t xsdt_addr;
    byte_t extended_checksum;
    byte_t reserved[3];
} __attribute__((__packed__)) RSDP2;

typedef struct _ACPISDTHeader {
    byte_t magic[4];
    uint32_t length;
    byte_t revision;
    byte_t checksum;
    byte_t OEM_id[6];
    byte_t OEM_table_id[8];
    uint32_t OEM_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((__packed__)) ACPISDTHeader;

typedef struct _MADT {
    ACPISDTHeader header;
    uint32_t local_APIC_addr;
    uint32_t flags;
    #define MADT_TABLE_START 0x2C
} __attribute__((__packed__)) MADT;

typedef struct _MADT_ENTRY_HEADER {
    byte_t type;
    byte_t length;
    #define MADT_TYPE_LOCAL_APIC 0
} __attribute__((__packed__)) MADT_ENTRY_HEADER;

typedef struct _XSDT {
    ACPISDTHeader h;
    uint64_t SDT_ptr[];
} __attribute__((__packed__)) XSDT;

typedef struct _RSDT {
    ACPISDTHeader h;
    uint32_t SDT_ptr[];
} __attribute__((__packed__)) RSDT;

extern uint32_t get_cpu_count(void);

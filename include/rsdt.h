#include <types.h>

#define RSDP_MAGIC "RSD PTR "
#define EBDA_PTR_ADDR 0x040e
#define MADT_SIGNATURE "APIC"

typedef struct _RSDP {
    CHAR magic[8];
    BYTE checksum;
    CHAR OEM_id[6];
    BYTE revision;
    UINT32 rsdt_addr;
} __attribute__((__packed__)) RSDP;

typedef struct _RSDP2 {
    RSDP original;
    UINT32 length;
    UINT64 xsdt_addr;
    BYTE extended_checksum;
    BYTE reserved[3];
} __attribute__((__packed__)) RSDP2;

typedef struct _ACPISDTHeader {
    BYTE magic[4];
    UINT32 length;
    BYTE revision;
    BYTE checksum;
    BYTE OEM_id[6];
    BYTE OEM_table_id[8];
    UINT32 OEM_revision;
    UINT32 creator_id;
    UINT32 creator_revision;
} __attribute__((__packed__)) ACPISDTHeader;

typedef struct _MADT {
    ACPISDTHeader header;
    UINT32 local_APIC_addr;
    UINT32 flags;
    #define MADT_TABLE_START 0x2C
} __attribute__((__packed__)) MADT;

typedef struct _MADT_ENTRY_HEADER {
    BYTE type;
    BYTE length;
    #define MADT_TYPE_LOCAL_APIC 0
} __attribute__((__packed__)) MADT_ENTRY_HEADER;

typedef struct _XSDT {
    ACPISDTHeader h;
    UINT64 SDT_ptr[];
} __attribute__((__packed__)) XSDT;

typedef struct _RSDT {
    ACPISDTHeader h;
    UINT32 SDT_ptr[];
} __attribute__((__packed__)) RSDT;

extern UINT32 get_cpu_count(void);

#include <types.h>

#define RSDP_MAGIC "RSD PTR "
#define EBDA_PTR_ADDR 0x040e

typedef struct _RSDP {
    char magic[8];
    BYTE checksum;
    char OEMID[8];
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

extern RSDP* detect_RSDP(void);
extern UINT32 get_cpu_count(void);

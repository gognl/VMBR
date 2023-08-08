/* 
Sources:
    OSDev pages - Symmetric Multiprocessing, MADT, RSDT, RSDP
*/

#include <types.h>
#include <debug.h>
#include <system.h>
#include <rsdt.h>

RSDP* detect_RSDP(void);
UINT32 get_cpu_count(void);

RSDP* detect_RSDP(void){
    BYTE magic[8] = RSDP_MAGIC;
    BYTE *EBDA_ptr = (*(WORD*)EBDA_PTR_ADDR);
    UINT32 i;
    RSDP *RSDP_ptr = NULLPTR;

    for(i = 0; i < 0x400; i += 16){
        if(memcmp(EBDA_ptr+i, magic, 8)){
            RSDP_ptr = EBDA_ptr+i;
            return RSDP_ptr;
        }
    }

    for(i = 0x000E0000; i < 0x000FFFFF; i+=16){
        if(memcmp(i, magic, 8)){
            RSDP_ptr = i;
            return RSDP_ptr;
        }
    }

    puts("ERROR: RSDP not found\n");
    return RSDP_ptr;

}

void* search_SDT(RSDP *rsdp_ptr, CHAR signature[4]){
    UINT32 entries, i;
    ACPISDTHeader *h;
    RSDT *rsdt_ptr;
    XSDT *xsdt_ptr;

    if (rsdp_ptr->revision == 0){    // RSDT

        rsdt_ptr = (RSDT*)rsdp_ptr->rsdt_addr;
        entries = (rsdt_ptr->h.length - sizeof(rsdt_ptr->h)) / 4;    // The "length" field in the header includes the header itself. Each entry is 4 bytes.

        for(i = 0; i < entries; i++){
            h = (ACPISDTHeader*) rsdt_ptr->SDT_ptr[i];
            if(memcmp(h->magic, signature, 4)) return (void*)h;
        }
    }

    else if (rsdp_ptr->revision == 2){    // XSDT
        xsdt_ptr = (XSDT*) ((RSDP2*)rsdp_ptr)->xsdt_addr;
        entries = (xsdt_ptr->h.length - sizeof(xsdt_ptr->h)) / 8;    // The "length" field in the header includes the header itself. Each entry is 8 bytes.

        for(i = 0; i < entries; i++){
            h = (ACPISDTHeader*) xsdt_ptr->SDT_ptr[i];
            if(memcmp(h->magic, signature, 4)) return (void*)h;
        }
    }

    puts("ERROR: No %m4 table found in XSDT/RSDT.\n", signature);
    return NULLPTR;

}

UINT32 get_cpu_count(void){
    RSDP *rsdp_ptr = detect_RSDP();
    MADT *madt_ptr = (MADT*)search_SDT(rsdp_ptr, MADT_SIGNATURE);

    UINT32 cpu_count = 0;
    UINT32 madt_length = madt_ptr->header.length;
    MADT_ENTRY_HEADER *current_entry = (MADT_ENTRY_HEADER*)((BYTE*)madt_ptr+MADT_TABLE_START);

    for(; current_entry < (BYTE*)madt_ptr + madt_length; current_entry = (MADT_ENTRY_HEADER*)((BYTE*)current_entry+current_entry->length)){
        if (current_entry->type == MADT_TYPE_LOCAL_APIC) cpu_count++;
    }

    return cpu_count;
}

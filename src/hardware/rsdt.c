/* 
Sources:
    OSDev pages - Symmetric Multiprocessing, MADT, RSDT, RSDP
*/

#include <types.h>
#include <debug.h>
#include <system.h>
#include <rsdt.h>

rsdp_t* detect_RSDP(void);
uint32_t get_cpu_count(void);

rsdp_t* detect_rsdp(void){
    byte_t magic[8] = RSDP_MAGIC;
    byte_t *ebda_ptr = (*(word_t*)EBDA_PTR_ADDR);
    uint32_t i;
    rsdp_t *rsdp_ptr = NULLPTR;

    for(i = 0; i < 0x400; i += 16){
        if(memcmp(ebda_ptr+i, magic, 8)){
            rsdp_ptr = ebda_ptr+i;
            return rsdp_ptr;
        }
    }

    for(i = 0x000E0000; i < 0x000FFFFF; i+=16){
        if(memcmp(i, magic, 8)){
            rsdp_ptr = i;
            return rsdp_ptr;
        }
    }

    puts("ERROR: RSDP not found\n");
    return rsdp_ptr;

}

void* search_SDT(rsdp_t *rsdp_ptr, char_t signature[4]){
    uint32_t entries, i;
    acpi_sdt_header_t *h;
    rsdt_t *rsdt_ptr;
    xsdt_t *xsdt_ptr;

    if (rsdp_ptr->revision == 0){    // RSDT

        rsdt_ptr = (rsdt_t*)rsdp_ptr->rsdt_addr;
        entries = (rsdt_ptr->h.length - sizeof(rsdt_ptr->h)) / 4;    // The "length" field in the header includes the header itself. Each entry is 4 bytes.

        for(i = 0; i < entries; i++){
            h = (acpi_sdt_header_t*) rsdt_ptr->sdt_ptr[i];
            if(memcmp(h->magic, signature, 4)) return (void*)h;
        }
    }

    else if (rsdp_ptr->revision == 2){    // XSDT
        xsdt_ptr = (xsdt_t*) ((rsdp2_t*)rsdp_ptr)->xsdt_addr;
        entries = (xsdt_ptr->h.length - sizeof(xsdt_ptr->h)) / 8;    // The "length" field in the header includes the header itself. Each entry is 8 bytes.

        for(i = 0; i < entries; i++){
            h = (acpi_sdt_header_t*) xsdt_ptr->sdt_ptr[i];
            if(memcmp(h->magic, signature, 4)) return (void*)h;
        }
    }

    puts("ERROR: No %m4 table found in XSDT/RSDT.\n", signature);
    return NULLPTR;

}

uint32_t get_cpu_count(void){
    rsdp_t *rsdp_ptr = detect_RSDP();
    madt_t *madt_ptr = (madt_t*)search_SDT(rsdp_ptr, MADT_SIGNATURE);

    uint32_t cpu_count = 0;
    uint32_t madt_length = madt_ptr->header.length;
    madt_entry_header_t *current_entry = (madt_entry_header_t*)((byte_t*)madt_ptr+MADT_TABLE_START);

    for(; current_entry < (byte_t*)madt_ptr + madt_length; current_entry = (madt_entry_header_t*)((byte_t*)current_entry+current_entry->length)){
        if (current_entry->type == MADT_TYPE_LOCAL_APIC) cpu_count++;
    }

    return cpu_count;
}

/* 
Sources:
    OSDev pages - Symmetric Multiprocessing, MADT, RSDT, RSDP
*/

#include <lib/types.h>
#include <lib/debug.h>
#include <lib/util.h>
#include <hardware/rsdt.h>

rsdp_t* detect_rsdp(void){
    byte_t magic[8] = RSDP_MAGIC;
    byte_t *ebda_ptr = (*(word_t*)EBDA_PTR_ADDR)<<4;
    uint32_t i;
    rsdp_t *rsdp_ptr = NULLPTR;

    for(i = 0; i < 1024; i += 16){
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

    LOG_ERROR("RSDP not found\n");
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

    LOG_ERROR("No %m4 table found in XSDT/RSDT.\n", signature);
    return NULLPTR;

}

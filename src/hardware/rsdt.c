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

UINT32 get_cpu_count(void){
    RSDP *rsdp_ptr = detect_RSDP();
    puts("RSDP addr: %q", rsdp_ptr);
}

#include <types.h>
#include <system.h>
#include <debug.h>

extern void CallReal(void (*)());
extern BYTE *low_functions_start(void);
extern BYTE *low_functions_end(void);
extern void LoadMemoryMap(void (*)());

void print_mmap(void);
BYTE *allocate_memory(UINT64 length);

#define REAL_START 0x4000
#define MMAP_TABLE 0x5000

void init_real(void){
    memcpy((BYTE*)REAL_START, 
    low_functions_start, 
    low_functions_end-low_functions_start);
}

void init_mmap(void){
    CallReal(LoadMemoryMap);
}

void print_mmap(void){
    mmap_table *mmap = (mmap_table*)MMAP_TABLE;
    UINT32 len = mmap->length;
    puts("\n######### PRINTING MMAP #########\nThe length is %d\n", len);
    for(int i = 0; i<len; i++){
        puts("Entry %d at %d\n", i, &(mmap->entries[i]));
        puts("Entry %d:\n\tBase: %q\n\tLength: %q\n\tType: %d\n\tNext: %q\n", 
        i, mmap->entries[i].base_addr, mmap->entries[i].length, mmap->entries[i].type, mmap->entries[i].base_addr + mmap->entries[i].length);
    }
    puts("#################################\n");
}

BYTE* allocate_memory(UINT64 length){
    UINT64 len = ALIGN_UP(length, PAGE_SIZE);
    mmap_table *mmap = (mmap_table*)MMAP_TABLE;
    UINT32 mmap_size = mmap->length;
    UINT32 i, chosen = mmap_size;
    BYTE *out;

    for(i = 0; i<mmap_size; i++){
        if (mmap->entries[i].type == E820_USABLE && mmap->entries[i].length > len){
            chosen = i;
        }
    }

    out = (BYTE*)ALIGN_UP(mmap->entries[chosen].base_addr, PAGE_SIZE);

    // Edit the mmap for future allocations
    UINT64 unalignedBaseLeftover = ((UINT64)out-mmap->entries[chosen].base_addr);
    mmap->entries[chosen].length -= len + unalignedBaseLeftover;
    mmap->entries[chosen].base_addr += len + unalignedBaseLeftover;

    return out;    
}

#include <types.h>
#include <util.h>
#include <real.h>
#include <debug.h>

extern void CallReal(void (*)());
extern byte_t *low_functions_start(void);
extern byte_t *low_functions_end(void);
extern void LoadMemoryMap(void (*)());

void print_mmap(void);
byte_t *allocate_memory(uint64_t length);

#define REAL_START 0x4000
#define MMAP_TABLE 0x5000

void init_real(void){
    memcpy((byte_t*)REAL_START, 
    low_functions_start, 
    low_functions_end-low_functions_start);
}

void init_mmap(void){
    CallReal(LoadMemoryMap);
}

void print_mmap(void){
    mmap_table_t *mmap = (mmap_table_t*)MMAP_TABLE;
    uint32_t len = mmap->length;
    puts("\n######### PRINTING MMAP #########\nThe length is %d\n", len);
    for(int i = 0; i<len; i++){
        puts("Entry %d at %d\n", i, &(mmap->entries[i]));
        puts("Entry %d:\n\tBase: %q\n\tLength: %q\n\tType: %d\n\tNext: %q\n", 
        i, mmap->entries[i].base_addr, mmap->entries[i].length, mmap->entries[i].type, mmap->entries[i].base_addr + mmap->entries[i].length);
    }
    puts("#################################\n");
}

byte_t* allocate_memory(uint64_t length){
    uint64_t len = ALIGN_UP(length, PAGE_SIZE);
    mmap_table_t *mmap = (mmap_table_t*)MMAP_TABLE;
    uint32_t mmap_size = mmap->length;
    uint32_t i, chosen = mmap_size;
    byte_t *out;

    for(i = 0; i<mmap_size; i++){
        if (mmap->entries[i].type == E820_USABLE && mmap->entries[i].length > len){
            chosen = i;
        }
    }

    out = (byte_t*)ALIGN_UP(mmap->entries[chosen].base_addr, PAGE_SIZE);

    // Edit the mmap for future allocations
    uint64_t unalignedBaseLeftover = ((uint64_t)out-mmap->entries[chosen].base_addr);
    mmap->entries[chosen].length -= len + unalignedBaseLeftover;
    mmap->entries[chosen].base_addr += len + unalignedBaseLeftover;

    memset(out, 0, length);

    return out;    
}

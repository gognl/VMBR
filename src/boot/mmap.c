#include <lib/types.h>
#include <lib/util.h>
#include <boot/mmap.h>
#include <lib/debug.h>
#include <vmm/vmm.h>
#include <boot/addresses.h>


byte_t *allocate_memory(uint64_t length);

void init_real(void){
    memcpy((byte_t*)REAL_START, 
    low_functions_start, 
    low_functions_end-low_functions_start);
}

void init_mmap(void){
    // shared_cores_data.allocation_lock = 0;

    CallReal(LoadMemoryMap);

    // extern byte_t vmbr_end[];
    // byte_t *tmp = allocate_memory(vmbr_end);

    // print_mmap();
}

void print_mmap(void){
    mmap_table_t *mmap = (mmap_table_t*)MMAP_TABLE;
    uint32_t len = mmap->length;
    LOG_DEBUG("\n######### PRINTING MMAP #########\nThe length is %d\n", len);
    for(int i = 0; i<len; i++){
        LOG_DEBUG("Entry %d at %x\n", i, &(mmap->entries[i]));
        LOG_DEBUG("Entry %d:\n\tBase: %x\n\tLength: %x\n\tType: %d\n\tNext: %x\n", 
        i, mmap->entries[i].base_addr, mmap->entries[i].length, mmap->entries[i].type, mmap->entries[i].base_addr + mmap->entries[i].length);
    }
    LOG_DEBUG("#################################\n");
}

qword_t get_bottom_allocation(){
    mmap_table_t *mmap = (mmap_table_t*)MMAP_TABLE;
    uint32_t mmap_size = mmap->length;
    uint32_t i, chosen = mmap_size;
    for (i = 0; i<mmap_size-1; i++){
        if (mmap->entries[i].type == E820_USABLE){
            chosen = i;
        }
    }
    return mmap->entries[chosen].base_addr+mmap->entries[chosen].length;
}

qword_t get_top_allocation(){
    mmap_table_t *mmap = (mmap_table_t*)MMAP_TABLE;
    uint32_t mmap_size = mmap->length;
    uint32_t i, chosen = mmap_size;
    for (i = 0; i<mmap_size-1; i++){
        if (mmap->entries[i].type == E820_USABLE){
            chosen = i;
        }
    }
    return mmap->entries[chosen+1].base_addr;
}

byte_t* allocate_memory(uint64_t length){

    AcquireLock(&shared_cores_data.allocation_lock);

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

    out = (byte_t*)ALIGN_UP(mmap->entries[chosen].base_addr+mmap->entries[chosen].length-len, PAGE_SIZE);

    // Edit the mmap for future allocations
    // uint64_t unaligned_base_leftover = ((uint64_t)out-mmap->entries[chosen].base_addr);
    mmap->entries[chosen].length -= len;
    // mmap->entries[chosen].base_addr += len + unaligned_base_leftover;

    ReleaseLock(&shared_cores_data.allocation_lock);

    if (out != 0)
        memset(out, 0, len);

    LOG_DEBUG("Allocated %x from %x to %x\n", len, out, out+len);
    return out;    
}

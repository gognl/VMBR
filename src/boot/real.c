#include <types.h>
#include <system.h>
#include <debug.h>

extern void CallReal(void (*)());
extern BYTE *low_functions_start(void);
extern BYTE *low_functions_end(void);
extern void LoadMemoryMap(void (*)());

#define REAL_START 0x4000

void init_real(void){
    memcpy((BYTE*)REAL_START, 
    low_functions_start, 
    low_functions_end-low_functions_start);
    puts("Copied functions to real mode area\n");

    CallReal(LoadMemoryMap);

    puts("Called a real function\n");
}

void init_mmap(void){
    e820_entry *mmap; 
}

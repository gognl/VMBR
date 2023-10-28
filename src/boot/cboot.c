#include <boot/mmap.h>
#include <hardware/apic.h>
#include <vmm/vmm.h>
#include <lib/debug.h>

int cboot(){
    
    init_real();
    init_mmap();
    init_cores();
    init_vmm();

    for(;;);
}

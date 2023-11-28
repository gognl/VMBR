#include <boot/mmap.h>
#include <hardware/apic.h>
#include <vmm/vmm.h>
#include <lib/debug.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>

int cboot(){
    
    init_real();
    init_mmap();
    init_cores();
    prepare_vmm();
    __vmwrite(GUEST_RSP, __read_rsp());
    __vmlaunch();

    for(;;);
}

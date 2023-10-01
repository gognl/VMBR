#include <boot/mmap.h>
#include <vmm/vmm.h>

int cboot(){
    //puts("\n\nSuccessfully entered long mode\n");
    init_real();
    init_mmap();
    init_vmm();

    for(;;);
}

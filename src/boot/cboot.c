#include <system.h>
#include <debug.h>
#include <real.h>
#include <init.h>

int cboot(){
    //puts("\n\nSuccessfully entered long mode\n");
    init_real();
    init_mmap();
    init_vmm();

    for(;;);
}

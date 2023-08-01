#include <system.h>
#include <debug.h>
#include <real.h>

int cboot(){
    puts("\n\nSuccessfully entered long mode\n");
    init_real();
    init_mmap();
    for(;;);
}

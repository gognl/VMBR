#include <boot/mmap.h>
#include <hardware/apic.h>
#include <vmm/vmm.h>
#include <lib/debug.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>

int cboot(){

    // LOG_DEBUG("%m4096%\n", 0xc0000);
    // LOG_DEBUG("%m4096%\n", 0xc1000);
    // LOG_DEBUG("%m4096%\n", 0xc2000);
    // LOG_DEBUG("%m4096%\n", 0xc3000);
    // LOG_DEBUG("%m4096%\n", 0xc4000);
    // LOG_DEBUG("%m4096%\n", 0xc5000);
    // LOG_DEBUG("%m4096%\n", 0xc6000);
    // LOG_DEBUG("%m4096%\n", 0xc7000);
    // LOG_DEBUG("%m4096%\n", 0xc8000);
    // LOG_DEBUG("%m4096%\n", 0xc9000);
    // LOG_DEBUG("%m4096%\n", 0xca000);
    // LOG_DEBUG("%m4096%\n", 0xcb000);
    // LOG_DEBUG("%m4096%\n", 0xcc000);
    // LOG_DEBUG("%m4096%\n", 0xcd000);
    // LOG_DEBUG("%m4096%\n", 0xce000);
    // LOG_DEBUG("%m4096%\n", 0xcf000);
    // LOG_DEBUG("%m4096%\n", 0xd0000);
    // LOG_DEBUG("%m4096%\n", 0xd1000);
    // LOG_DEBUG("%m4096%\n", 0xd2000);
    // LOG_DEBUG("%m4096%\n", 0xd3000);
    // LOG_DEBUG("%m4096%\n", 0xd4000);
    // LOG_DEBUG("%m4096%\n", 0xd5000);
    // LOG_DEBUG("%m4096%\n", 0xd6000);
    // LOG_DEBUG("%m4096%\n", 0xd7000);
    // LOG_DEBUG("%m4096%\n", 0xd8000);
    // LOG_DEBUG("%m4096%\n", 0xd9000);
    // LOG_DEBUG("%m4096%\n", 0xda000);
    // LOG_DEBUG("%m4096%\n", 0xdb000);
    // LOG_DEBUG("%m4096%\n", 0xdc000);
    // LOG_DEBUG("%m4096%\n", 0xdd000);
    // LOG_DEBUG("%m4096%\n", 0xde000);
    // LOG_DEBUG("%m4096%\n", 0xdf000);
    // LOG_DEBUG("%m4096%\n", 0xe0000);
    // LOG_DEBUG("%m4096%\n", 0xe1000);
    // LOG_DEBUG("%m4096%\n", 0xe2000);
    // LOG_DEBUG("%m4096%\n", 0xe3000);
    // LOG_DEBUG("%m4096%\n", 0xe4000);
    // LOG_DEBUG("%m4096%\n", 0xe5000);
    // LOG_DEBUG("%m4096%\n", 0xe6000);
    // LOG_DEBUG("%m4096%\n", 0xe7000);
    // LOG_DEBUG("%m4096%\n", 0xe8000);
    // LOG_DEBUG("%m4096%\n", 0xe9000);
    // LOG_DEBUG("%m4096%\n", 0xea000);
    // LOG_DEBUG("%m4096%\n", 0xeb000);
    // LOG_DEBUG("%m4096%\n", 0xec000);
    // LOG_DEBUG("%m4096%\n", 0xed000);
    // LOG_DEBUG("%m4096%\n", 0xee000);
    // LOG_DEBUG("%m4096%\n", 0xef000);
    // LOG_DEBUG("%m4096%\n", 0xf0000);
    // LOG_DEBUG("%m4096%\n", 0xf1000);
    // LOG_DEBUG("%m4096%\n", 0xf2000);
    // LOG_DEBUG("%m4096%\n", 0xf3000);
    // LOG_DEBUG("%m4096%\n", 0xf4000);
    // LOG_DEBUG("%m4096%\n", 0xf5000);
    // LOG_DEBUG("%m4096%\n", 0xf6000);
    // LOG_DEBUG("%m4096%\n", 0xf7000);
    // LOG_DEBUG("%m4096%\n", 0xf8000);
    // LOG_DEBUG("%m4096%\n", 0xf9000);
    // LOG_DEBUG("%m4096%\n", 0xfa000);
    // LOG_DEBUG("%m4096%\n", 0xfb000);
    // LOG_DEBUG("%m4096%\n", 0xfc000);
    // LOG_DEBUG("%m4096%\n", 0xfd000);
    // LOG_DEBUG("%m4096%\n", 0xfe000);
    // LOG_DEBUG("%m4096%\n", 0xff000);

    // *((byte_t*)0xcb013) = 0x0f;
    // *((byte_t*)0xcb014) = 0x01;
    // *((byte_t*)0xcb015) = 0xc1;
    // LOG_DEBUG("%m512%\n", 0xcb000);

    init_real();
    init_mmap();
    // init_cores();
    prepare_vmm();
    __vmwrite(GUEST_RSP, __read_rsp());
    __vmlaunch();

    for(;;);
}

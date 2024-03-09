#include <boot/mmap.h>
#include <hardware/apic.h>
#include <vmm/vmm.h>
#include <lib/debug.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <lib/msr.h>
#include <hardware/idt.h>
#include <hardware/pica.h>
#include <hardware/nic.h>
#include <lib/util.h>

int cboot(){

    init_real();
    init_mmap();

    init_pic();
    init_idt();
    init_nic();

    byte_t str[] = "this is a test";
    LOG_DEBUG("Sending...\n");
    transmit_packet(&str, strlen(str));
    while (!transmit_over());
    LOG_DEBUG("Sending...\n");
    transmit_packet(&str, strlen(str));
    while (!transmit_over());
    LOG_DEBUG("Sending...\n");
    transmit_packet(&str, strlen(str));
    LOG_DEBUG("OVER\n");



    // shared_cores_data.pml4 = initialize_host_paging();
    // prepare_vmm();
    // init_cores();

    

    // __vmwrite(GUEST_RSP, __read_rsp());
    // __vmlaunch();

    for(;;);
}

#include <boot/mmap.h>
#include <hardware/apic.h>
#include <vmm/vmm.h>
#include <lib/debug.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <lib/msr.h>
#include <hardware/idt.h>
#include <hardware/pic8259.h>
#include <hardware/nic.h>
#include <lib/util.h>
#include <network/ethernet.h>
#include <network/ip.h>

int cboot(){

    init_real();
    init_mmap();

    init_pic();
    init_idt();
    init_nic();

    char_t data[] = "hello";
    byte_t dest[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    ethernet_t *pkt = allocate_memory(get_ethernet_header_size()+get_ip_header_size()+sizeof(data));
    build_ethernet(pkt, dest, "IP");
    build_ip((ip_t*)pkt->payload, sizeof(data), 0x12345678);
    memcpy(((ip_t*)pkt->payload)->payload, data, sizeof(data));
    transmit_packet(pkt, get_ethernet_header_size()+get_ip_header_size()+sizeof(data));
    
    // byte_t str[] = "this is a test";
    // LOG_DEBUG("Sending...\n");
    // transmit_packet(&str, strlen(str));
    // while (!transmit_over());
    // LOG_DEBUG("Sending...\n");
    // transmit_packet(&str, strlen(str));
    // while (!transmit_over());
    // LOG_DEBUG("Sending...\n");
    // transmit_packet(&str, strlen(str));
    LOG_DEBUG("OVER\n");



    // shared_cores_data.pml4 = initialize_host_paging();
    // prepare_vmm();
    // init_cores();

    

    // __vmwrite(GUEST_RSP, __read_rsp());
    // __vmlaunch();

    for(;;);
}

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
#include <network/udp.h>
#include <network/dhcp.h>

int cboot(){

    init_real();
    init_mmap();

    init_pic();
    init_idt();
    init_nic();

    generate_dhcp_dora();

    // char_t data[] = "hello";
    // byte_t dest[] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc};
    // byte_t *pkt = allocate_memory(42+sizeof(data));
    // network_addr_t addr = {.dst_ip = 0x12345678, .dst_mac = 0, .dst_port = 1234, .src_port = 5678};
    // memcpy(addr.dst_mac, dest, 6);
    // build_udp_packet(pkt, data, sizeof(data), &addr);
    // transmit_packet(pkt, 42+sizeof(data));
    
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

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
#include <network/arp.h>

int cboot(){

    init_real();
    init_mmap();

    init_pic();
    init_idt();
    init_nic();

    generate_dhcp_dora();

    LOG_DEBUG("IP: %x, Router: %x, Subnet: %x\n", get_ip_addr(), get_router_ip_addr(), get_subnet_mask());

    byte_t mac[6];
    find_mac_by_ip(get_router_ip_addr(), mac);
    set_router_mac(mac);
    // LOG_DEBUG("mac: %m6%\n", mac);

    byte_t data[] = "Hello!";
    send_udp_packet(data, sizeof(data), 0x0a000203, 53, 53);
    send_udp_packet(data, sizeof(data), 0xac1cb73f, 52367, 52367);

    LOG_DEBUG("OVER\n");

    for(;;);
}

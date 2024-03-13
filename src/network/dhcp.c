#include <network/dhcp.h>
#include <hardware/nic.h>
#include <lib/util.h>
#include <network/udp.h>
#include <boot/mmap.h>

__attribute__((section(".vmm"))) uint32_t get_dhcp_header_size(){
    return sizeof(dhcp_t);
}

__attribute__((section(".vmm"))) void add_dhcp_option(dhcp_t *packet, dhcp_option_t *option, byte_t *data){
    packet->cookie = FLIP_DWORD(DHCP_MAGIC_COOKIE);
    byte_t *ptr = packet->options;
    while (*ptr != 0) ptr += 2 + *(ptr+1);
    ((dhcp_option_t*)ptr)->code = option->code;
    ((dhcp_option_t*)ptr)->len = option->len;
    memcpy(((dhcp_option_t*)ptr)->data, data, option->len);
}

__attribute__((section(".vmm"))) void build_dhcp_discover(dhcp_t *packet){

    static uint32_t xid = 0x12c4851a;

    packet->op = DHCP_OP_REQUEST;
    packet->htype = DHCP_HTYPE_ETHERNET;
    packet->hlen = DHCP_HLEN_ETHERNET;
    packet->xid = xid++;
    memcpy(packet->client_hw_addr, get_mac_addr(), 6);

    dhcp_option_t option = {.code=DHCP_CODE_MESSAGE_TYPE, .len=DHCP_LEN_MESSAGE_TYPE};
    byte_t message_type_data[] = {DHCP_MESSAGE_TYPE_DISCOVER};
    add_dhcp_option(packet, &option, message_type_data);

    option.code = DHCP_CODE_END;
    option.len = 0;
    add_dhcp_option(packet, &option, 0);

}

__attribute__((section(".vmm"))) void generate_dhcp_dora(){
    byte_t *packet = allocate_memory(42+sizeof(dhcp_t));
    dhcp_t *dhcp = packet+42;
    byte_t dst_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    network_addr_t addr = {.dst_ip = 0xffffffff, .dst_mac = dst_mac, .dst_port=67, .src_port=68};
    build_dhcp_discover(dhcp);
    build_udp_packet(packet, dhcp, sizeof(dhcp_t), &addr);
    transmit_packet(packet, 42+sizeof(dhcp_t));
}


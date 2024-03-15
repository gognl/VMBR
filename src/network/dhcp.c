#include <network/dhcp.h>
#include <hardware/nic.h>
#include <lib/util.h>
#include <network/udp.h>
#include <boot/mmap.h>
#include <lib/instr.h>

static __attribute__((section(".vmmdata"))) uint32_t xid = 0x12c4851a;
static __attribute__((section(".vmmdata"))) uint32_t server_ip = 0;

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

__attribute__((section(".vmm"))) dhcp_option_t* find_dhcp_option(dhcp_t *packet, dhcp_code code){
    if (packet->cookie != FLIP_DWORD(DHCP_MAGIC_COOKIE)) return 0;
    byte_t *ptr = packet->options;
    while (*ptr != code && *ptr != DHCP_CODE_END) ptr += 2 + *(ptr+1);
    if (*ptr == DHCP_CODE_END && code != DHCP_CODE_END) return 0;
    return ptr;
}

__attribute__((section(".vmm"))) void build_dhcp_discover(dhcp_t *packet){

    packet->op = DHCP_OP_REQUEST;
    packet->htype = DHCP_HTYPE_ETHERNET;
    packet->hlen = DHCP_HLEN_ETHERNET;
    packet->xid = xid;
    memcpy(packet->client_hw_addr, get_mac_addr(), 6);

    dhcp_option_t option = {.code=DHCP_CODE_MESSAGE_TYPE, .len=DHCP_LEN_MESSAGE_TYPE};
    byte_t message_type_data[] = {DHCP_MESSAGE_TYPE_DISCOVER};
    add_dhcp_option(packet, &option, message_type_data);

    option.code = DHCP_CODE_END;
    option.len = 0;
    add_dhcp_option(packet, &option, 0);

}

__attribute__((section(".vmm"))) void build_dhcp_request(dhcp_t *packet, uint32_t server_ip){

    packet->op = DHCP_OP_REQUEST;
    packet->htype = DHCP_HTYPE_ETHERNET;
    packet->hlen = DHCP_HLEN_ETHERNET;
    packet->xid = xid++;
    memcpy(packet->client_hw_addr, get_mac_addr(), 6);
    packet->server_ip = FLIP_DWORD(server_ip);

    dhcp_option_t option = {.code=DHCP_CODE_MESSAGE_TYPE, .len=DHCP_LEN_MESSAGE_TYPE};
    byte_t message_type_data[] = {DHCP_MESSAGE_TYPE_REQUEST};
    add_dhcp_option(packet, &option, message_type_data);

    option.code = DHCP_CODE_SERVER_ID;
    option.len = DHCP_LEN_SERVER_ID;
    uint32_t server_id = FLIP_DWORD(server_ip);
    add_dhcp_option(packet, &option, &server_id);

    option.code = DHCP_CODE_END;
    option.len = 0;
    add_dhcp_option(packet, &option, 0);

}

__attribute__((section(".vmm"))) void handle_dhcp_packet(dhcp_t *packet){
    dhcp_option_t *type = find_dhcp_option(packet, DHCP_CODE_MESSAGE_TYPE);
    if (type == 0) return;  // Option not found
    if (*(uint8_t*)type->data == DHCP_MESSAGE_TYPE_OFFER){
        LOG_DEBUG("DHCP Offer\n");
        server_ip = FLIP_DWORD(packet->server_ip);
    }
    else if (*(uint8_t*)type->data == DHCP_MESSAGE_TYPE_ACK){
        LOG_DEBUG("DHCP Ack\n");
        set_router_ip_addr(FLIP_DWORD(*(uint32_t*)(((dhcp_option_t*)find_dhcp_option(packet, DHCP_CODE_SERVER_ID))->data)));
        set_subnet_mask(FLIP_DWORD(*(uint32_t*)(((dhcp_option_t*)find_dhcp_option(packet, DHCP_CODE_SUBNET_MASK))->data)));
        set_ip_addr(FLIP_DWORD(packet->your_ip));
    }
}

__attribute__((section(".vmm"))) void generate_dhcp_dora(){
    byte_t *packet = allocate_memory(42+sizeof(dhcp_t));
    dhcp_t *dhcp = packet+42;
    byte_t dst_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    network_addr_t addr = {.dst_ip = 0xffffffff, .dst_mac = dst_mac, .dst_port=67, .src_port=68};
    build_dhcp_discover(dhcp);
    build_udp_packet(packet, dhcp, sizeof(dhcp_t), &addr);
    transmit_packet(packet, 42+sizeof(dhcp_t));

    while (server_ip == 0) __pause();

    memsetw(packet, 0, (42+sizeof(dhcp_t))/2);
    build_dhcp_request(dhcp, server_ip);
    build_udp_packet(packet, dhcp, sizeof(dhcp_t), &addr);
    transmit_packet(packet, 42+sizeof(dhcp_t));

    while (get_ip_addr() == 0) __pause();
}


#include "network/udp.h"
#include "lib/instr.h"
#include "network/ip.h"
#include "network/ethernet.h"
#include "lib/util.h"
#include "hardware/nic.h"
#include "network/arp.h"

__attribute__((section(".vmm"))) uint32_t get_udp_header_size(){
    return sizeof(udp_t);
}

__attribute__((section(".vmm"))) uint16_t calculate_udp_checksum(udp_t *packet, ip_t *ip_hdr){
    return 0;
}

__attribute__((section(".vmm"))) void build_udp(udp_t *packet, uint16_t source, uint16_t dest, ip_t *ip_hdr, uint16_t payload_length){

    packet->source = FLIP_WORD(source);
    packet->destination = FLIP_WORD(dest);
    packet->length = FLIP_WORD(get_udp_header_size()+payload_length);
    packet->checksum = calculate_udp_checksum(packet, ip_hdr);

}

__attribute__((section(".vmm"))) udp_t* build_udp_packet(byte_t *buffer, byte_t *data, uint16_t data_length, network_addr_t *addr){
    // Buffer size must be of size 42+data_length, aligned to 16bits

    ethernet_t *ether_hdr = buffer;
    ip_t *ip_hdr = ether_hdr->payload;
    udp_t *udp_hdr = ip_hdr->payload;

    build_ethernet(ether_hdr, addr->dst_mac, "IP");
    memcpy(udp_hdr->payload, data, data_length);
    build_ip(ip_hdr, data_length+get_udp_header_size(), addr->dst_ip);
    build_udp(udp_hdr, addr->src_port, addr->dst_port, ip_hdr, data_length);

    return udp_hdr;
}

void send_udp_packet(byte_t *data, uint16_t data_length, uint32_t dst_ip, uint16_t dst_port, uint16_t src_port){
    byte_t *buffer = allocate_memory(ALIGN_UP(42+data_length, 2));
    memcpy(buffer+42, data, data_length);

    network_addr_t addr = {.dst_ip=dst_ip, .dst_port=dst_port, .src_port=src_port};

    if ((addr.dst_ip ^ get_ip_addr()) & get_subnet_mask()){
        // Not in subnet
        memcpy(addr.dst_mac, get_router_mac(), 6);
    }
    else {
        // In subnet
        find_mac_by_ip(addr.dst_ip, addr.dst_mac);
    }

    build_udp_packet(buffer, buffer+42, data_length, &addr);
    transmit_packet(buffer, ALIGN_UP(42+data_length, 2));
}

#include "network/ip.h"
#include "hardware/nic.h"
#include "network/ethernet.h"
#include "network/udp.h"
#include "network/dhcp.h"

__attribute__((section(".vmm"))) uint32_t get_ip_header_size(){
    return sizeof(ip_t);
}

__attribute__((section(".vmm"))) uint16_t calculate_ip_checksum(ip_t *packet){
    uint16_t *values = packet;
    uint32_t checksum = 0;
    packet->checksum = 0;
    for (uint8_t i = 0; i<sizeof(ip_t)/2; i++){
        checksum += FLIP_WORD(values[i]);
    }
    checksum = (checksum & 0xffff) + (checksum>>16);
    checksum = ~(uint16_t)checksum;
    return FLIP_WORD((uint16_t)checksum);
}

__attribute__((section(".vmm"))) void build_ip(ip_t *packet, uint16_t payload_length, uint32_t dest){
    packet->version = IPV4_VERSION;
    packet->ihl = IPV4_IHL;
    packet->ihl = IPV4_IHL;
    packet->dscp = 0;
    packet->ecn = 0;
    packet->total_length = FLIP_WORD(get_ip_header_size() + payload_length);
    packet->identification = 0;
    packet->flags = 0;
    packet->fragment_offset_low = 0;
    packet->fragment_offset_high = 0;
    packet->ttl = IPV4_INITIAL_TTL;
    packet->protocol = IPV4_PROTOCOL_UDP;
    packet->source = FLIP_DWORD(get_ip_addr());
    packet->destination = FLIP_DWORD(dest);
    // packet->checksum = calculate_ip_checksum(packet);
}

__attribute__((section(".vmm"))) void handle_ip_packet(ethernet_t *ether_hdr){
    ip_t *ip_hdr = ether_hdr->payload;

    uint32_t src_ip = FLIP_DWORD(ip_hdr->source), dst_ip = FLIP_DWORD(ip_hdr->destination);
    uint16_t data_length = 0;
    uint16_t src_port = 0, dst_port = 0;
    udp_t *udp_hdr;
    if (ip_hdr->protocol == IPV4_PROTOCOL_UDP){
        udp_hdr = ip_hdr->payload;
        data_length = FLIP_WORD(udp_hdr->length) - sizeof(udp_t);
        src_port = FLIP_WORD(udp_hdr->source);
        dst_port = FLIP_WORD(udp_hdr->destination);
    }
    
    if (dst_port == 68){
        LOG_DEBUG("Received DHCP packet\n");
        handle_dhcp_packet(udp_hdr->payload);
    }
}


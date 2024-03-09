#include "network/ip.h"
#include "hardware/nic.h"

__attribute__((section(".vmm"))) uint32_t get_ip_header_size(){
    return sizeof(ip_t);
}

__attribute__((section(".vmm"))) uint16_t calculate_ip_checksum(ip_t *packet){
    uint16_t *values = packet;
    uint32_t checksum = 0;
    for (uint8_t i = 0; i<10; i++){
        if (i == 6) continue;   // skip the checksum itself
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
    packet->checksum = calculate_ip_checksum(packet);
}


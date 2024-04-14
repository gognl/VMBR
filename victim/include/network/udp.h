#ifndef __UDP_H
#define __UDP_H

#include "lib/types.h"
#include "network/ip.h"

typedef struct {
    uint16_t source;
    uint16_t destination;
    uint16_t length;
    uint16_t checksum;
    byte_t payload[];
} __attribute__((__packed__)) udp_t;

typedef struct {
    byte_t dst_mac[6];
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
} network_addr_t;

extern uint32_t get_udp_header_size();
extern void build_udp(udp_t *packet, uint16_t source, uint16_t dest, ip_t *ip_hdr, uint16_t payload_length);
extern udp_t* build_udp_packet(byte_t *buffer, byte_t *data, uint16_t data_length, network_addr_t *addr);
extern void send_udp_packet(byte_t *data, uint16_t data_length, uint32_t dst_ip, uint16_t dst_port, uint16_t src_port);

#endif
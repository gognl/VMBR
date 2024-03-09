#ifndef __IP_H
#define __IP_H

#include "lib/types.h"

#define IPV4_VERSION 4
#define IPV4_IHL 5
#define IPV4_INITIAL_TTL 64
#define IPV4_PROTOCOL_UDP 17
#define IPV4_PROTOCOL_TCP 6 

typedef struct {
    uint8_t ihl : 4;
    uint8_t version : 4;
    uint8_t ecn : 2;
    uint8_t dscp : 6;
    uint16_t total_length;
    uint16_t identification;
    uint8_t fragment_offset_low : 5;
    uint8_t flags : 3;
    uint8_t fragment_offset_high;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t source;
    uint32_t destination;
    byte_t payload[];
} __attribute__((__packed__)) ip_t;

extern uint32_t get_ip_header_size();
extern void build_ip(ip_t *packet, uint16_t payload_length, uint32_t dest);

#endif
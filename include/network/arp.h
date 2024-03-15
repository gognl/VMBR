#ifndef __ARP_H
#define __ARP_H

#include <lib/types.h>
#include <network/ethernet.h>

typedef struct {
    uint16_t hw_type;
    #define ARP_HTYPE_ETHERNET 1
    uint16_t prot_type;
    #define ARP_PTYPE_IPV4 0x0800
    uint8_t hw_length;
    #define ARP_HLEN_ETHERNET 6
    uint8_t prot_length;
    #define ARP_PLEN_IPV4 4
    uint16_t operation;
    #define ARP_OP_REQUEST 1
    #define ARP_OP_RESPONSE 2
    byte_t sender_hw[6];
    uint32_t sender_prot;
    byte_t target_hw[6];
    uint32_t target_prot;
} __attribute__((__packed__)) arp_t;

extern void find_mac_by_ip(uint32_t ip, byte_t *mac);
extern void handle_arp_packet(ethernet_t *packet);

#endif
#ifndef __ETHERNET_H
#define __ETHERNET_H

#include <lib/types.h>

typedef struct {
    // byte_t premable[7];
    // byte_t start_of_frame;
    byte_t destination[6];
    byte_t source[6];
    union {
        word_t length;
        word_t type;
    };
    byte_t payload[];
} __attribute__((__packed__)) ethernet_t;

#define ETHERNET_TYPE_IP 0x0800
#define ETHERNET_TYPE_ARP 0x0806

extern uint32_t get_ethernet_header_size();
extern void build_ethernet(ethernet_t *frame, byte_t *dest, char_t *protocol);

#endif
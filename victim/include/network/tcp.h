#ifndef __TCP_H
#define __TCP_H

#include <lib/types.h>

typedef struct {
    uint16_t source;
    uint16_t destination;
    uint32_t seq;
    uint32_t ack;
    uint8_t : 4;
    uint8_t data_offset : 4;
    #define TCP_ACK_ONLY 0x10
    union {
        uint8_t value;
        struct {
        uint8_t cwr : 1;
        uint8_t ece : 1;
        uint8_t urg : 1;
        uint8_t ack : 1;
        uint8_t psh : 1;
        uint8_t rst : 1;
        uint8_t syn : 1;
        uint8_t fin : 1;
        } __attribute__((__packed__));
    } flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_pointer;
    uint32_t options[];
} __attribute__((__packed__)) tcp_t;

#endif
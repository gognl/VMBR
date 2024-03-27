#ifndef __DHCP_H
#define __DHCP_H

#include <lib/types.h>

typedef enum {
    DHCP_CODE_SUBNET_MASK = 1,
    DHCP_CODE_ROUTER = 3,
    DHCP_CODE_SERVER_ID = 54,
    DHCP_CODE_MESSAGE_TYPE = 53,
    DHCP_CODE_END = 255
} __attribute__ ((__packed__)) dhcp_code;

typedef enum {
    DHCP_LEN_MESSAGE_TYPE = 1,
    DHCP_LEN_SERVER_ID = 4
} __attribute__ ((__packed__)) dhcp_len;

typedef enum {
    DHCP_MESSAGE_TYPE_DISCOVER = 1,
    DHCP_MESSAGE_TYPE_OFFER = 2,
    DHCP_MESSAGE_TYPE_REQUEST = 3,
    DHCP_MESSAGE_TYPE_ACK = 5
} __attribute__ ((__packed__)) dhcp_message_type;


typedef struct {
    uint8_t op;         // operation code
    #define DHCP_OP_REQUEST 1
    #define DHCP_OP_REPLY 2
    uint8_t htype;      // hardware type
    #define DHCP_HTYPE_ETHERNET 1
    uint8_t hlen;       // hardware length
    #define DHCP_HLEN_ETHERNET 6
    uint8_t hops;       // hop count
    uint32_t xid;       // transition id
    uint16_t secs;      // number of seconds
    union {
        uint16_t flags;     // flags
        uint16_t broadcast : 1;
    };
    uint32_t client_ip;
    uint32_t your_ip;
    uint32_t server_ip;
    uint32_t gateway_ip;
    byte_t client_hw_addr[16];
    byte_t sname[64];
    byte_t file[128];
    uint32_t cookie;
    #define DHCP_MAGIC_COOKIE 0x63825363
    byte_t options[60];
} __attribute__((__packed__)) dhcp_t;

typedef struct {
    dhcp_code code;
    dhcp_len len;
    byte_t data[];
} __attribute__((__packed__)) dhcp_option_t;

extern void handle_dhcp_packet(dhcp_t *packet);
extern dhcp_option_t* find_dhcp_option(dhcp_t *packet, dhcp_code code);

#endif
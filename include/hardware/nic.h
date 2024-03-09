#ifndef __NIC_H
#define __NIC_H
#include <hardware/pci.h>
#include <lib/types.h>

#define RTL8139_VENDOR_ID 0x10EC
#define RTL8139_DEVICE_ID 0x8139

#define RTL8139_RBSTART 0x30
#define RTL8139_COMMAND 0x37
#define RTL8139_IMR 0x3C
#define RTL8139_STATUS 0x3E
#define RTL8139_RCR 0x44
#define RTL8139_CONFIG1 0x52

#define RTL8139_STATUS_ROK (1<<0)
#define RTL8139_STATUS_TOK (1<<2)

#define RTL8139_COMMAND_TE (1<<2)
#define RTL8139_COMMAND_RE (1<<3)
#define RTL8139_COMMAND_RST (1<<4)
#define RTL8139_IMR_ROK (1<<0)
#define RTL8139_IMR_TOK (1<<2)
#define RTL8139_RCR_AAP (1<<0)
#define RTL8139_RCR_APM (1<<1)
#define RTL8139_RCR_AM (1<<2)
#define RTL8139_RCR_AB (1<<3)
#define RTL8139_RCR_WRAP (1<<7)

#define RTL8139_RX_BUFFER_SIZE (8192+16+1500)

typedef struct {
    pci_device_t pci_config_space;
    uint32_t io_base;
    uint32_t rx_buffer;
    uint8_t mac[6];
    uint32_t ip;
    uint8_t irq;
} nic_device_t;

typedef union {
    uint32_t value;
    struct {
        uint32_t size : 13;
        uint32_t own : 1;
        uint32_t tun : 1;
        uint32_t tok : 1;
        uint32_t ertxth : 6;
        uint32_t : 2;
        uint32_t ncc : 4;
        uint32_t cdh : 1;
        uint32_t owc : 1;
        uint32_t tabt : 1;
        uint32_t crs : 1;
    } __attribute__((__packed__));
} transmit_command_t;
STATIC_ASSERT(sizeof(transmit_command_t) == sizeof(uint32_t));

extern void IsrWrapper_NIC();

extern void init_nic();
extern void transmit_packet(byte_t *buffer, uint16_t size);
extern STATUS transmit_over();
extern uint8_t* get_mac_addr();
extern uint32_t get_ip_addr();

#endif
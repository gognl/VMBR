#ifndef __PCI_H
#define __PCI_H

#include <lib/types.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_BUS_COUNT 256
#define PCI_DEVICE_COUNT 32
#define PCI_FUNCTION_COUNT 32

#define PCI_COMMAND_STATUS 1
#define PCI_BAR0 4

// https://wiki.osdev.org/PCI#Configuration_Space_Access_Mechanism_.231
typedef union {
    uint32_t value;
    struct {
        uint32_t : 2;
        uint32_t register_id : 6;
        uint32_t function : 3;
        uint32_t device : 5;
        uint32_t bus : 8;
        uint32_t : 7;
        uint32_t enable : 1;
    } __attribute__((__packed__));
} pci_config_address_t;
STATIC_ASSERT(sizeof(pci_config_address_t) == sizeof(uint32_t));

typedef union {
    uint32_t value;
    struct {
        uint32_t : 8;
        uint32_t function : 3;
        uint32_t device : 5;
        uint32_t bus : 8;
        uint32_t : 8;
    } __attribute__((__packed__));
} pci_device_t;
STATIC_ASSERT(sizeof(pci_device_t) == sizeof(uint32_t));

typedef union {
    uint16_t value;
    struct {
        uint16_t io_space : 1;
        uint16_t memory_space : 1;
        uint16_t bus_master : 1;
        uint16_t special_cycles : 1;
        uint16_t memory_write_and_invalidate_enabled : 1;
        uint16_t vga_palette_snoop : 1;
        uint16_t parity_error_response : 1;
        uint16_t : 1;
        uint16_t serr_enabled : 1;
        uint16_t fast_back_to_back_enable : 1;
        uint16_t interrupt_disable : 1;
        uint16_t : 5;
    } __attribute__((__packed__));
} pci_command_t;
STATIC_ASSERT(sizeof(pci_command_t) == sizeof(uint16_t));

typedef union {
    uint32_t address;
    struct __attribute__((__packed__)){
        uint32_t is_io_bar : 1;
        #define BAR_TYPE_32 0
        #define BAR_TYPE_64 2
        uint32_t type : 2;
        uint32_t prefetchable : 1;
    };

} pci_device_bar_t;
STATIC_ASSERT(sizeof(pci_device_bar_t) == sizeof(uint32_t));

typedef union {

    uint32_t value;

    struct __attribute__((__packed__)){
        uint16_t vendor_id;
        uint16_t device_id;
    } register_0;

    struct __attribute__((__packed__)){
        pci_command_t command;
        uint16_t status;
    } register_1;

    struct __attribute__((__packed__)){
        uint8_t revision_id;
        uint8_t prog_if;
        uint8_t subclass;
        uint8_t class_code;
    } register_2;

    struct __attribute__((__packed__)){
        uint8_t cache_line_size;
        uint8_t latency_timer;
        uint8_t header_type : 7;
        uint8_t multifunctional : 1;
        uint8_t bist;
    } register_3;

    struct __attribute__((__packed__)){
        pci_device_bar_t base_address_0;
    } register_4;
    struct __attribute__((__packed__)){
        pci_device_bar_t base_address_1;
    } register_5;
    struct __attribute__((__packed__)){
        pci_device_bar_t base_address_2;
    } register_6;
    struct __attribute__((__packed__)){
        pci_device_bar_t base_address_3;
    } register_7;
    struct __attribute__((__packed__)){
        pci_device_bar_t base_address_4;
    } register_8;
    struct __attribute__((__packed__)){
        pci_device_bar_t base_address_5;
    } register_9;

    struct __attribute__((__packed__)){
        uint32_t cardbus_cis_pointer;
    } register_a;
    struct __attribute__((__packed__)){
        uint16_t subsystem_vendor_id;
        uint16_t subsystem_id;
    } register_b;
    struct __attribute__((__packed__)){
        uint32_t expansion_rom_base_address;
    } register_c;
    struct __attribute__((__packed__)){
        uint8_t capabilities_pointer;
    } register_d;
    struct __attribute__((__packed__)){
        uint8_t interrupt_line;
        uint8_t interrupt_pin;
        uint8_t min_grant;
        uint8_t max_latency;
    } register_f;

} pci_device_register_t;
STATIC_ASSERT(sizeof(pci_device_register_t) == sizeof(uint32_t));

extern STATUS get_pci_device(pci_device_t *dev_ptr, uint16_t vendor_id, uint16_t device_id);
extern void set_pci_device_register(pci_device_t dev, uint8_t register_id, pci_device_register_t value);
extern pci_device_register_t get_pci_device_register(pci_device_t dev, uint8_t register_id);

#endif
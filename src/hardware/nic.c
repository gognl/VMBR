#include <hardware/nic.h>
#include <hardware/pci.h>
#include <lib/instr.h>
#include <boot/mmap.h>
#include <vmm/hooks.h>
#include <hardware/idt.h>

static uint32_t transmit_start[4] = {0x20, 0x24, 0x28, 0x2C};
static uint32_t transmit_status_command[4] = {0x10, 0x14, 0x18, 0x1C};
static uint8_t current_transmit = 0;
static nic_device_t nic_dev = {0};

void receive_packet(){

}

void nic_handler(){
    LOG_DEBUG("Reached NIC handler\n");
    
    
    uint16_t status = __inw(nic_dev.io_base+RTL8139_STATUS);
    __outw(nic_dev.io_base+RTL8139_STATUS, RTL8139_STATUS_ROK | RTL8139_STATUS_TOK);

    if (status & RTL8139_STATUS_ROK){
        LOG_DEBUG("Received\n");
        receive_packet();
    }
    if (status & RTL8139_STATUS_TOK){
        LOG_DEBUG("Sent\n");
    }

    pic_ack(nic_dev.irq);
}

void transmit_packet(byte_t *buffer, uint16_t size){
    if (size > 1792) LOG_ERROR("Packet size too big (%x)\n", (uint64_t)size);

    transmit_command_t cmd = {.size = size, .own = 0};
    __outl(nic_dev.io_base+transmit_start[current_transmit], (uint32_t)buffer);
    __outl(nic_dev.io_base+transmit_status_command[current_transmit], cmd.value);

    current_transmit = (current_transmit+1) % 4;
}

STATUS transmit_over(){
    transmit_command_t status = {.value=__inl(nic_dev.io_base+transmit_status_command[current_transmit-1])};
    return status.tok;
}

uint8_t* get_mac_addr(){
    return nic_dev.mac;
}

void init_nic(){

    // Find the PCI configuration space of RTL8139
    STATUS status = get_pci_device(&nic_dev.pci_config_space, RTL8139_VENDOR_ID, RTL8139_DEVICE_ID);
    if (status == SUCCESS)
        LOG_DEBUG("Found NIC! Bus: %d, Device: %d, Function: %d\n", nic_dev.pci_config_space.bus,
                                                                    nic_dev.pci_config_space.device, 
                                                                    nic_dev.pci_config_space.function);
    else
        LOG_ERROR("Could not find NIC.\n");

    // Find the IO base address
    pci_device_register_t nic_bar0 = get_pci_device_register(nic_dev.pci_config_space, PCI_BAR0);
    nic_dev.io_base = nic_bar0.register_4.base_address_0.address & ~(0b11);
    if (nic_bar0.register_4.base_address_0.is_io_bar != TRUE)
        LOG_ERROR("Unexpected memory base address for RTL8139\n");
    LOG_DEBUG("RTL8139 IO BASE: %x\n", nic_dev.io_base);

    // Enable PCI Bus Mastering (for DMA)
    pci_device_register_t command_status_register = get_pci_device_register(nic_dev.pci_config_space, PCI_COMMAND_STATUS);
    if (!command_status_register.register_1.command.bus_master){
        command_status_register.register_1.command.bus_master = 1;
        set_pci_device_register(nic_dev.pci_config_space, PCI_COMMAND_STATUS, command_status_register);
    }

    // Turn on the RTL8139
    __outb(nic_dev.io_base+RTL8139_CONFIG1, 0x00);

    // Do a software reset
    __outb(nic_dev.io_base+RTL8139_COMMAND, RTL8139_COMMAND_RST);
    while ((__inb(nic_dev.io_base+RTL8139_COMMAND) & RTL8139_COMMAND_RST) != 0) { __pause(); }

    // Allocate memory for the receive buffer (Rx)
    nic_dev.rx_buffer = allocate_memory(RTL8139_RX_BUFFER_SIZE);
    __outl(nic_dev.io_base+RTL8139_RBSTART, nic_dev.rx_buffer);

    // Allow receiving ROK and TOK interrupts
    __outw(nic_dev.io_base+RTL8139_IMR, RTL8139_IMR_ROK | RTL8139_IMR_TOK);
    // __outw(nic_dev.io_base+RTL8139_IMR, 0xffff);

    // Set some settings related to packets filtering and such
    __outl(nic_dev.io_base+RTL8139_RCR, RTL8139_RCR_AAP | RTL8139_RCR_APM | RTL8139_RCR_AM | RTL8139_RCR_AB | RTL8139_RCR_WRAP);
    // __outl(nic_dev.io_base+RTL8139_RCR, RTL8139_RCR_AAP | RTL8139_RCR_APM | RTL8139_RCR_AM | RTL8139_RCR_AB | (1<<10));

    // Start the receiver & the transmitter
    __outb(nic_dev.io_base+RTL8139_COMMAND, RTL8139_COMMAND_TE | RTL8139_COMMAND_RE);
    
    // Register the interrupts
    uint32_t interrupt_line = get_pci_device_register(nic_dev.pci_config_space, 0xF).register_f.interrupt_line;
    if (interrupt_line > 0xff)
        LOG_ERROR("Invalid interrupt line in RTL8139\n");
    LOG_DEBUG("NIC IRQ is %x\n", interrupt_line);
    nic_dev.irq = interrupt_line+0x20;
    set_idt_entry(IsrWrapper_NIC, nic_dev.irq);

    // Find MAC address
    uint32_t mac1 = __inl(nic_dev.io_base+0x00);
    uint16_t mac2 = __inw(nic_dev.io_base+0x04);
    nic_dev.mac[0] = mac1;
    nic_dev.mac[1] = mac1>>8;
    nic_dev.mac[2] = mac1>>16;
    nic_dev.mac[3] = mac1>>24;
    nic_dev.mac[4] = mac2;
    nic_dev.mac[5] = mac2>>8;

    LOG_DEBUG("MAC: %x:%x:%x:%x:%x:%x\n", nic_dev.mac[0], nic_dev.mac[1], nic_dev.mac[2], nic_dev.mac[3], nic_dev.mac[4], nic_dev.mac[5]);

}

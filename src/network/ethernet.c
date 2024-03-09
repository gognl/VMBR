#include <network/ethernet.h>
#include <hardware/nic.h>
#include <lib/util.h>

__attribute__((section(".vmm"))) uint32_t get_ethernet_header_size(){
    return sizeof(ethernet_t);
}

__attribute__((section(".vmm"))) void build_ethernet(ethernet_t *frame, byte_t *dest, char_t *protocol){

    memcpy(frame->source, get_mac_addr(), 6);
    memcpy(frame->destination, dest, 6);
    if (memcmp(protocol, "IP", 2))
        frame->type = FLIP_WORD(ETHERNET_TYPE_IP);
    else if (memcmp(protocol, "ARP", 3))
        frame->type = FLIP_WORD(ETHERNET_TYPE_ARP);
    
}

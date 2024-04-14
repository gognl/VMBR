#include <network/arp.h>
#include <network/ethernet.h>
#include <hardware/nic.h>
#include <lib/util.h>
#include <boot/mmap.h>

static byte_t *current_mac;
static uint32_t current_ip;

void build_arp_request(arp_t *packet, uint32_t ip){
    packet->hw_type = FLIP_WORD(ARP_HTYPE_ETHERNET);
    packet->prot_type = FLIP_WORD(ARP_PTYPE_IPV4);
    packet->hw_length = ARP_HLEN_ETHERNET;
    packet->prot_length = ARP_PLEN_IPV4;
    packet->operation = FLIP_WORD(ARP_OP_REQUEST);

    memcpy(packet->sender_hw, get_mac_addr(), 6);
    packet->sender_prot = FLIP_DWORD(get_ip_addr());
    memset(packet->target_hw, 0, 6);
    packet->target_prot = FLIP_DWORD(ip);
}

void handle_arp_packet(ethernet_t *packet){
    arp_t *arp_hdr = packet->payload;
    if (arp_hdr->operation == FLIP_WORD(ARP_OP_RESPONSE)){
        if (current_mac == 0) return;
        if (FLIP_DWORD(arp_hdr->sender_prot) == current_ip){
            memcpy(current_mac, arp_hdr->sender_hw, 6);
        }
    }
}

void find_mac_by_ip(uint32_t ip, byte_t *mac){

    current_mac = mac;
    current_ip = ip;
    memset(current_mac, 0, 6);

    ethernet_t *pkt = allocate_memory(get_ethernet_header_size()+sizeof(arp_t));
    byte_t dest[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    build_ethernet(pkt, dest, "ARP");
    build_arp_request(pkt->payload, ip);
    transmit_packet(pkt, get_ethernet_header_size()+sizeof(arp_t));

    memset(dest, 0, 6);
    while(memcmp(current_mac, dest, 6));
    current_mac = 0;
}

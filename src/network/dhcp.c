#include <network/dhcp.h>

__attribute__((section(".vmm"))) uint32_t get_dhcp_header_size(){
    return sizeof(dhcp_t);
}

__attribute__((section(".vmm"))) void build_dhcp(dhcp_t *buffer){

}

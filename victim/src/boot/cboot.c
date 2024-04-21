#include <boot/mmap.h>
#include <hardware/apic.h>
#include <vmm/vmm.h>
#include <lib/debug.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <lib/msr.h>
#include <hardware/idt.h>
#include <hardware/pic8259.h>
#include <hardware/nic.h>
#include <lib/util.h>
#include <network/ethernet.h>
#include <network/ip.h>
#include <network/udp.h>
#include <network/dhcp.h>
#include <network/arp.h>

int cboot(){

    init_real();
    init_mmap();

    shared_cores_data.pml4 = initialize_host_paging();
    prepare_vmm();
    init_cores();

    shared_cores_data.memory_shadowing_pages.KeyboardClassServiceCallback_rw = allocate_memory(PAGE_SIZE);
    shared_cores_data.memory_shadowing_pages.MiDriverLoadSucceeded_rw = allocate_memory(PAGE_SIZE);
    shared_cores_data.memory_shadowing_pages.ndisMSendNBLToMiniportInternal_rw = allocate_memory(PAGE_SIZE);
    shared_cores_data.memory_shadowing_pages.NdisMIndicateReceiveNetBufferLists_rw = allocate_memory(PAGE_SIZE);

    __vmwrite(GUEST_RSP, __read_rsp());
    __vmlaunch();


    for(;;);
}

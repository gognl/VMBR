#include <lib/types.h>
#include <lib/instr.h>
#include <hardware/apic.h>
#include <hardware/rsdt.h>
#include <lib/msr.h>
#include <boot/mmap.h>
#include <lib/util.h>
#include <boot/addresses.h>

void activate_x2apic(){
    qword_t ecx, tmp;
    __cpuid(1, 0, &tmp, &tmp, &ecx, &tmp);
    if(!(ecx & CPUID_X2APIC))
        LOG_ERROR("No x2APIC support\n");
    
    __wrmsr(IA32_APIC_BASE, __rdmsr(IA32_APIC_BASE) | X2APIC_ENABLE | XAPIC_GLOBAL_ENABLE);
}

uint8_t __attribute__((section(".vmm"))) get_current_core_id(){
    qword_t ebx, tmp;
    __cpuid(1, 0, &tmp, &ebx, &tmp, &tmp);
    return ebx>>24;
}

uint32_t get_cores_count(void){
    rsdp_t *rsdp_ptr = detect_rsdp();
    madt_t *madt_ptr = (madt_t*)search_SDT(rsdp_ptr, MADT_SIGNATURE);

    uint32_t cores_count = 0;
    uint32_t madt_length = madt_ptr->header.length;
    madt_entry_header_t *current_entry = (madt_entry_header_t*)((byte_t*)madt_ptr+MADT_TABLE_START);

    for(; current_entry < (byte_t*)madt_ptr + madt_length; current_entry = (madt_entry_header_t*)((byte_t*)current_entry+current_entry->length)){
        if (current_entry->type == MADT_TYPE_LOCAL_APIC 
        && (current_entry->type0.flags.processor_enabled || current_entry->type0.flags.online_capable)){
            cores_count++;
        }
    }

    return cores_count;
}

void broadcast_init_ipi(){
    x2apic_icr_t init_icr = {0};
    init_icr.delivery_mode = INIT;
    init_icr.level = DEASSERT;
    init_icr.destination_shorthand = ALL_EXCLUDING_SELF;
    __wrmsr(IA32_X2APIC_ICR, init_icr.value);
}

void broadcast_sipi_ipi(){
    x2apic_icr_t sipi_icr = {0};
    sipi_icr.delivery_mode = SIPI;
    sipi_icr.vector = BSP_INIT_CODE/PAGE_SIZE;
    sipi_icr.level = ASSERT;
    sipi_icr.destination_shorthand = ALL_EXCLUDING_SELF;
    __wrmsr(IA32_X2APIC_ICR, sipi_icr.value);
}

void init_vmm_all_cores(){

    uint32_t cores_count = get_cores_count();
    *(byte_t*)cores_semaphore = 0;
    memcpy(BSP_INIT_CODE, (byte_t*)InitializeSingleCore, (uint64_t)InitializeSingleCore_end-(uint64_t)InitializeSingleCore);

    broadcast_init_ipi();
    sleep();
    for(uint32_t i = 0; i<2 && (*(byte_t*)cores_semaphore != cores_count-1); i++){
        broadcast_sipi_ipi();
        sleep();
    }

    if (*(byte_t*)cores_semaphore == cores_count-1)
        LOG_INFO("Successfully initialized cores\n");
    else
        LOG_ERROR("Failed to initialize cores (%d)\n", *(byte_t*)cores_semaphore);

}

void init_cores(){
    activate_x2apic();
    init_vmm_all_cores();
}

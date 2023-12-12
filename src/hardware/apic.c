#include <lib/types.h>
#include <lib/instr.h>
#include <hardware/apic.h>
#include <hardware/rsdt.h>
#include <lib/msr.h>
#include <boot/mmap.h>
#include <lib/util.h>

extern void InitializeSingleCore(void);
extern void InitializeSingleCore_end(void);
extern byte_t *cores_semaphore(void);

void activate_x2apic(){
    qword_t ecx, tmp;
    __cpuid(1, 0, &tmp, &tmp, &ecx, &tmp);
    if(!(ecx & CPUID_X2APIC))
        LOG_ERROR("No x2APIC support\n");
    
    __wrmsr(IA32_APIC_BASE, __rdmsr(IA32_APIC_BASE) | X2APIC_ENABLE | XAPIC_GLOBAL_ENABLE);
}

void init_ap(byte_t apic_id, uint8_t page_idx){

    x2apic_icr_t init_icr = {0};
    init_icr.delivery_mode = INIT;
    init_icr.destination_field = (uint32_t)apic_id;
    __wrmsr(IA32_X2APIC_ICR, init_icr.value);
    sleep();


    x2apic_icr_t sipi_icr = {0};
    sipi_icr.delivery_mode = SIPI;
    sipi_icr.vector = page_idx;
    sipi_icr.level = ASSERT;
    sipi_icr.destination_field = (uint32_t)apic_id;
    *(byte_t*)cores_semaphore = 0;
    __wrmsr(IA32_X2APIC_ICR, sipi_icr.value);

    uint64_t i;
    for(i = 0; i<0xfffffffull && (*(byte_t*)cores_semaphore == 0); i++);
    if (i == 0xfffffffull)
        LOG_ERROR("Failed to initialize core %d\n", (dword_t)apic_id);
    if (*(byte_t*)cores_semaphore == 1)
        LOG_INFO("Successfully initialized core %d\n", (dword_t)apic_id);
}

uint8_t get_current_core_id(){
    qword_t ebx, tmp;
    __cpuid(1, 0, &tmp, &ebx, &tmp, &tmp);
    return ebx>>24;
}

void init_vmm_all_cores(){

    uint8_t current_core_id = get_current_core_id();

    byte_t *mp_code_cpy = (byte_t*)0x4000;
    memcpy(mp_code_cpy, (byte_t*)InitializeSingleCore, (uint64_t)InitializeSingleCore_end-(uint64_t)InitializeSingleCore);

    rsdp_t *rsdp_ptr = detect_rsdp();
    madt_t *madt_ptr = (madt_t*)search_SDT(rsdp_ptr, MADT_SIGNATURE);

    uint32_t madt_length = madt_ptr->header.length;
    madt_entry_header_t *current_entry = (madt_entry_header_t*)((byte_t*)madt_ptr+MADT_TABLE_START);

    for(; current_entry < (byte_t*)madt_ptr + madt_length; current_entry = (madt_entry_header_t*)((byte_t*)current_entry+current_entry->length)){
        if (current_entry->type == MADT_TYPE_LOCAL_APIC 
        && ((current_entry->type0.flags.processor_enabled || current_entry->type0.flags.online_capable)
        && current_entry->type0.apic_id != current_core_id)){
            init_ap(current_entry->type0.apic_id, (uint8_t)((uint64_t)mp_code_cpy/PAGE_SIZE));
        }
    }

}

void init_cores(){
    activate_x2apic();
    init_vmm_all_cores();
}

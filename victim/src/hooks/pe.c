#include <hooks/pe.h>
#include <hooks/hooking.h>
#include <lib/util.h>

__attribute__((section(".vmm")))
BOOL check_for_module(byte_t *module, byte_t *name, byte_t name_len){

    if (*(word_t*)guest_virtual_to_physical(module) != DOS_SIGNATURE) return FALSE;

    byte_t *nt_headers = RVA(module, *(dword_t*)guest_virtual_to_physical(RVA(module, DOS_HEADER_e_lfanew)));
    if (*(dword_t*)guest_virtual_to_physical(nt_headers) != PE_SIGNATURE) return FALSE;

    byte_t *optional_header = nt_headers + OPTIONAL_HEADER_OFFSET;
    if (*(word_t*)guest_virtual_to_physical(optional_header) != PE32PLUS_SIGNATURE) return FALSE;

    dword_t directories_amount = *(dword_t*)guest_virtual_to_physical(optional_header+OPTIONAL_HEADER_NumberOfRvaAndSizes);
    if (directories_amount < 7) return FALSE;   // debug directory is 7th

    byte_t *debug_section = RVA(module, *(dword_t*)guest_virtual_to_physical(optional_header+OPTIONAL_HEADER_DebugDirectory));
    if (debug_section == module) return FALSE;  // this means that the VirtualAddress field was 0.

    byte_t *debug_data = RVA(module, *(dword_t*)guest_virtual_to_physical(debug_section+DEBUG_SECTION_AddressOfRawData));
    if (debug_data == module) return FALSE;

    return memcmp(guest_virtual_to_physical(debug_data+DEBUG_DATA_StringOffset), name, name_len);

}

__attribute__((section(".vmm"))) uint32_t get_image_size(byte_t *module){

    if (*(word_t*)guest_virtual_to_physical(module) != DOS_SIGNATURE) return 0;

    byte_t *nt_headers = RVA(module, *(dword_t*)guest_virtual_to_physical(RVA(module, DOS_HEADER_e_lfanew)));
    if (*(dword_t*)guest_virtual_to_physical(nt_headers) != PE_SIGNATURE) return 0;

    byte_t *optional_header = nt_headers + OPTIONAL_HEADER_OFFSET;
    if (*(word_t*)guest_virtual_to_physical(optional_header) != PE32PLUS_SIGNATURE) return 0;

    return *(dword_t*)guest_virtual_to_physical(optional_header+OPTIONAL_HEADER_SizeOfImage);
    
}

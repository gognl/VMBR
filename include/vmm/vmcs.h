#ifndef __VMCS_H
#define __VMCS_H

#include <lib/types.h>

extern void initialize_vmcs();

#define DEFAULT_PINBASED_CTLS ((1ull<<1) | (1ull<<2) | (1ull<<4))
#define DEFAULT_PROCBASED_CTLS ((1ull<<1) | (1ull<<4) | (1ull<<5) | (1ull<<6) | (1ull<<8) | (1ull<<13) | (1ull<<14) | (1ull<<15) | (1ull<<16) | (1ull<<26))
#define DEFAULT_EXIT_CTLS ((1ull<<0) | (1ull<<1) | (1ull<<2) | (1ull<<3) | (1ull<<4) | (1ull<<5) | (1ull<<6) | (1ull<<7) | (1ull<<8) | (1ull<<10) | (1ull<<11) | (1ull<<13) | (1ull<<14) | (1ull<<16) | (1ull<<17))
#define DEFAULT_ENTRY_CTLS ((1ull<<0) | (1ull<<1) | (1ull<<2) | (1ull<<3) | (1ull<<4) | (1ull<<5) | (1ull<<6) | (1ull<<7) | (1ull<<8) | (1ull<<12))

#define CANONICAL_ADDRESS 0xffffffff

#define GDT_AB_A (1<<0)         // Accessed bit 
#define GDT_AB_RW (1<<1)        // R/W bit - R for code segments, W for data segments
#define GDT_AB_DC (1<<2)        // Direction bit (0 for growing up, 1 for growing down)
#define GDT_AB_E (1<<3)         // Executable bit
#define GDT_AB_S (1<<4)         // S bit (type) - 1 if code/data segment
#define GDT_AB_P (1<<7)         // Present bit
#define GDT_AB_L (1<<13)        // Long mode bit
#define GDT_AB_DB (1<<14)        // Size bit
#define GDT_AB_G (1<<15)        // Granularity flag
#define UNUSABLE_SELECTOR (1<<16)

typedef enum VMCS_ENCODINGS {
    // host fields
        // 16bit
        HOST_ES = 0x00000c00,
        HOST_CS = 0x00000c02,
        HOST_SS = 0x00000c04,
        HOST_DS = 0x00000c06,
        HOST_FS = 0x00000c08,
        HOST_GS = 0x00000c0a,
        HOST_TR = 0x00000c0c,
        // 32bit
        HOST_IA32_SYSENTER_CS = 0x00004c00,
        // 64bit
        HOST_IA32_PAT = 0x0002c00,
        HOST_IA32_EFER = 0x0002c02,
        HOST_IA32_PERF_GLOBAL_CTRL = 0x0002c04,
        HOST_IA32_PKRS = 0x0002c06,
        // natural-width
        HOST_CR0 = 0x00006c00,
        HOST_CR3 = 0x00006c02,
        HOST_CR4 = 0x00006c04,
        HOST_FS_BASE = 0x00006c06,
        HOST_GS_BASE = 0x00006c08,
        HOST_TR_BASE = 0x00006c0a,
        HOST_GDTR_BASE = 0x00006c0c,
        HOST_IDTR_BASE = 0x00006c0e,
        HOST_IA32_SYSENTER_ESP = 0x00006c10,
        HOST_IA32_SYSENTER_EIP = 0x00006c12,
        HOST_RSP = 0x00006c14,
        HOST_RIP = 0x00006c16,
        HOST_IA32_S_CET = 0x00006c18,
        HOST_SSP = 0x00006c1a,
        HOST_IA32_INTERRUPT_SSP_TABLE_ADDR = 0x00006c1c,

    // guest fields
        // 16bit
        GUEST_ES = 0x00000800,
        GUEST_CS = 0x00000802,
        GUEST_SS = 0x00000804,
        GUEST_DS = 0x00000806,
        GUEST_FS = 0x00000808,
        GUEST_GS = 0x0000080a,
        GUEST_LDTR = 0x0000080c,
        GUEST_TR = 0x0000080e,
        GUEST_INTERRUPT_STATUS = 0x00000810,
        GUEST_PML_INDEX = 0x00000812,
        GUEST_UINV = 0x00000814,
        // 32bit
        GUEST_ES_LIMIT = 0x00004800,
        GUEST_CS_LIMIT = 0x00004802,
        GUEST_SS_LIMIT = 0x00004804,
        GUEST_DS_LIMIT = 0x00004806,
        GUEST_FS_LIMIT = 0x00004808,
        GUEST_GS_LIMIT = 0x0000480a,
        GUEST_LDTR_LIMIT = 0x0000480c,
        GUEST_TR_LIMIT = 0x0000480e,
        GUEST_GDTR_LIMIT = 0x00004810,
        GUEST_IDTR_LIMIT = 0x00004812,
        GUEST_ES_ACCESS_RIGHTS = 0x00004814,
        GUEST_CS_ACCESS_RIGHTS = 0x00004816,
        GUEST_SS_ACCESS_RIGHTS = 0x00004818,
        GUEST_DS_ACCESS_RIGHTS = 0x0000481a,
        GUEST_FS_ACCESS_RIGHTS = 0x0000481c,
        GUEST_GS_ACCESS_RIGHTS = 0x0000481e,
        GUEST_LDTR_ACCESS_RIGHTS = 0x00004820,
        GUEST_TR_ACCESS_RIGHTS = 0x00004822,
        GUEST_INTERRUPTIBILITY_STATE = 0x00004824,
        GUEST_ACTIVITY_STATE = 0x00004826,
        GUEST_SMBASE = 0x00004828,
        GUEST_IA32_SYSENTER_CS = 0x0000482a,
        GUEST_VMS_PREEMPTION_TIMER = 0x0000482e,
        // 64bit
        GUEST_VMCS_LINK_PTR = 0x00002800,
        GUEST_IA32_DEBUGCTL = 0x00002802,
        GUEST_IA32_PAT = 0x00002804,
        GUEST_IA32_EFER = 0x00002806,
        GUEST_IA32_PERF_GLOBAL_CTRL = 0x00002808,
        GUEST_PDPTE0 = 0x0000280a,
        GUEST_PDPTE1 = 0x0000280c,
        GUEST_PDPTE2 = 0x0000280e,
        GUEST_PDPTE3 = 0x00002810,
        GUEST_IA32_BNDCFGS = 0x00002812,
        GUEST_IA32_RTIT_CTL = 0x00002814,
        GUEST_IA32_LBR_CTL = 0x00002816,
        GUEST_IA32_PKRS = 0x00002818,
        // natural-width
        GUEST_CR0 = 0x00006800,
        GUEST_CR3 = 0x00006802,
        GUEST_CR4 = 0x00006804,
        GUEST_ES_BASE = 0x00006806,
        GUEST_CS_BASE = 0x00006808,
        GUEST_SS_BASE = 0x0000680a,
        GUEST_DS_BASE = 0x0000680c,
        GUEST_FS_BASE = 0x0000680e,
        GUEST_GS_BASE = 0x00006810,
        GUEST_LDTR_BASE = 0x00006812,
        GUEST_TR_BASE = 0x00006814,
        GUEST_GDTR_BASE = 0x00006816,
        GUEST_IDTR_BASE = 0x00006818,
        GUEST_DR7 = 0x0000681a,
        GUEST_RSP = 0x0000681c,
        GUEST_RIP = 0x0000681e,
        GUEST_RFLAGS = 0x00006820,
        GUEST_PENDING_DEBUG_EXCEPTIONS = 0x00006822,
        GUEST_IA32_SYSENTER_ESP = 0x00006824,
        GUEST_IA32_SYSENTER_EIP = 0x00006826,
        GUEST_IA32_S_CET = 0x00006828,
        GUEST_SSP = 0x0000682a,
        GUEST_IA32_INTERRUPT_SSP_TABLE_ADDR = 0x0000682c,

    // control fields
        // 16bit
        CONTROL_VPID = 0x00000000,
        CONTROL_POSTED_INTERRUPT_NOTIFICATION_VECTOR = 0x00000002,
        CONTROL_EPTP_INDEX = 0x00000004,
        CONTROL_HLAT_PREFIX_SIZE = 0x00000006,
        CONTROL_LAST_PID_POINTER_INDEX = 0x00000008,
        // 32bit
        CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS = 0x00004000,
        CONTROL_PRIMARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS = 0x00004002,
        CONTROL_EXCEPTION_BITMAP = 0x00004004,
        CONTROL_PAGEFAULT_ERRORCODE_MASK = 0x00004006,
        CONTROL_PAGEFAULT_ERRORCODE_MATCH = 0x00004008,
        CONTROL_CR3_TARGET_COUNT = 0x0000400a,
        CONTROL_PRIMARY_VMEXIT_CONTROLS = 0x0000400c,
        CONTROL_VMEXIT_MSR_STORE_COUNT = 0x0000400e,
        CONTROL_VMEXIT_MSR_LOAD_COUNT = 0x00004010,
        CONTROL_VMENTRY_CONTROLS = 0x00004012,
        CONTROL_VMENTRY_MSR_LOAD_COUNT = 0x00004014,
        CONTROL_VMENTRY_INTERRUPTION_INFORMATION_FIELD = 0x00004016,
        CONTROL_VMENTRY_EXCEPTION_ERRORCODE = 0x00004018,
        CONTROL_VMENTRY_INSTRUCTION_LENGTH = 0x0000401a,
        CONTROL_TPR_THRESHOLD = 0x0000401c,
        CONTROL_SECONDARY_EXECUTION_CONTROLS = 0x0000401e,
        CONTROL_PLE_GAP = 0x00004020,
        CONTROL_PLE_WINDOW = 0x00004022,
        CONTROL_INSTRUCTION_TIMEOUT = 0x00004024,
        // 64bit
        CONTROL_IO_BITMAP_A = 0x0002000,
        CONTROL_IO_BITMAP_B = 0x0002002,
        CONTROL_MSR_BITMAPS = 0x0002004,
        CONTROL_VMEXIT_MSR_STORE = 0x0002006,
        CONTROL_VMEXIT_MSR_LOAD = 0x0002008,
        CONTROL_VMENTRY_MSR_LOAD = 0x000200a,
        CONTROL_EXECUTIVE_VMCS_PTR = 0x000200c,
        CONTROL_PML = 0x000200e,
        CONTROL_TSC_OFFSET = 0x0002010,
        CONTROL_VIRTUAL_APIC = 0x0002012,
        CONTROL_APIC_ACCESS = 0x0002014,
        CONTROL_POSTED_INTERRUPT_DESCRIPTOR = 0x0002016,
        CONTROL_VM_FUNCTION_CONTROLS = 0x0002018,
        CONTROL_EPTP = 0x000201a,
        CONTROL_EOI_EXIT0 = 0x000201c,
        CONTROL_EOI_EXIT1 = 0x000201e,
        CONTROL_EOI_EXIT2 = 0x0002020,
        CONTROL_EOI_EXIT3 = 0x0002022,
        CONTROL_EPTP_LIST = 0x0002024,
        CONTROL_VMREAD = 0x0002026,
        CONTROL_VMWRITE = 0x0002028,
        CONTROL_VIRTUALIZATION_EXCEPTION_INFO = 0x000202a,
        CONTROL_XSS_EXITING = 0x000202c,
        CONTROL_ENCLS_EXITING = 0x000202e,
        CONTROL_SUB_PAGE_PERMISSION_TABLE_PTR = 0x0002030,
        CONTROL_TSC_MULTIPLIER = 0x0002032,
        CONTROL_TERTIARY_EXECUTION_CONTROLS = 0x0002034,
        CONTROL_ENCLV_EXITING = 0x0002036,
        CONTROL_LOW_PASID_DIRECTORY = 0x0002038,
        CONTROL_HIGH_PASID_DIRECTORY = 0x000203a,
        CONTROL_SHARED_EPTP = 0x000203c,
        CONTROL_PCONFIG_EXITING = 0x000203e,
        CONTROL_HLATP = 0x0002040,
        CONTROL_PID_POINTER_TABLE = 0x0002042,
        CONTROL_SECONDARY_VMEXIT_CONTROLS = 0x0002044,
        // natural-width
        CONTROL_CR0_GUESTHOST_MASK = 0x00006000,
        CONTROL_CR4_GUESTHOST_MASK = 0x00006002,
        CONTROL_CR0_READ_SHADOW = 0x00006004,
        CONTROL_CR4_READ_SHADOW = 0x00006006,
        CONTROL_CR3_TARGET_VALUE0 = 0x00006008,
        CONTROL_CR3_TARGET_VALUE1 = 0x0000600a,
        CONTROL_CR3_TARGET_VALUE2 = 0x0000600c,
        CONTROL_CR3_TARGET_VALUE3 = 0x0000600e,
    
    // read-only data fields
        // 32bit
        RODATA_VM_INSTRUCTION_ERROR = 0x00004400,
        RODATA_EXIT_REASON = 0x00004402,
        RODATA_VMEXIT_INTERRUPTION_INFO = 0x00004404,
        RODATA_VMEXIT_INTERRUPTION_ERRORCODE = 0x00004406,
        RODATA_IDT_VECTORING_INFO = 0x00004408,
        RODATA_IDT_VECTORING_ERRORCODE = 0x0000440a,
        RODATA_VMEXIT_INSTRUCTION_LENGTH = 0x0000440c,
        RODATA_VMEXIT_INSTRUCTION_INFO = 0x0000440e,
        // 64bit
        RODATA_GUEST_PHYSICAL_ADDRESS = 0x00002400,
        // natural-width
        RODATA_EXIT_QUALIFICATION = 0x00006400,
        RODATA_IO_RCX = 0x00006402,
        RODATA_IO_RSI = 0x00006404,
        RODATA_IO_RDI = 0x00006406,
        RODATA_IO_RIP = 0x00006408,
        RODATA_GUEST_LINEAR_ADDRESS = 0x0000640a
} VMCS_ENCODING;

typedef union {
    uint32_t value;
    struct __attribute__((__packed__)) {
        uint32_t external_interrupt_exiting : 1;        // 0
        uint32_t : 2;                                   // 1-2
        uint32_t nmi_exiting : 1;                       // 3
        uint32_t : 1;                                   // 4
        uint32_t virtual_nmis : 1;                      // 5
        uint32_t activate_vmx_preemption_timer : 1;     // 6
        uint32_t process_posted_interrupts : 1;         // 7
    };
} pin_based_ctls_t;

typedef union {
    uint32_t value;
    struct __attribute__((__packed__)) {
        uint32_t : 2;                                   // 0-1
        uint32_t interrupt_window_exiting : 1;          // 2
        uint32_t use_tsc_offsetting : 1;                // 3
        uint32_t : 3;                                   // 4-6
        uint32_t hlt_exiting : 1;                       // 7
        uint32_t : 1;                                   // 8
        uint32_t invlpg_exiting : 1;                    // 9
        uint32_t mwait_exiting : 1;                     // 10
        uint32_t rdpmc_exiting : 1;                     // 11
        uint32_t rdtsc_exiting : 1;                     // 12
        uint32_t : 2;                                   // 13-14
        uint32_t cr3_load_exiting : 1;                  // 15
        uint32_t cr3_store_exiting : 1;                 // 16
        uint32_t : 2;                                   // 17-18
        uint32_t cr8_load_exiting : 1;                  // 19
        uint32_t cr8_store_exiting : 1;                 // 20
        uint32_t use_tpr_shadow : 1;                    // 21
        uint32_t nmi_window_exiting : 1;                // 22
        uint32_t mov_dr_exiting : 1;                    // 23
        uint32_t unconditional_io_exiting : 1;          // 24
        uint32_t use_io_bitmaps : 1;                    // 25
        uint32_t : 1;                                   // 26
        uint32_t monitor_trap_flag : 1;                 // 27
        uint32_t use_msr_bitmaps : 1;                   // 28
        uint32_t monitor_exiting : 1;                   // 29
        uint32_t pause_exiting : 1;                     // 30
        uint32_t activate_secondary_controls : 1;       // 31
    };
} proc_based_ctls_t;

typedef union {
    uint32_t value;
    struct __attribute__((__packed__)) {
        uint32_t virtualize_apic_accesses : 1;          // 0
        uint32_t enable_ept : 1;                        // 1
        uint32_t descriptor_table_exiting : 1;          // 2
        uint32_t enable_rdtscp : 1;                     // 3
        uint32_t virtualize_x2apic_mode : 1;            // 4
        uint32_t enable_vpid : 1;                       // 5
        uint32_t wbinvd_exiting : 1;                    // 6
        uint32_t unrestricted_guest : 1;                // 7
        uint32_t apic_register_virtualization : 1;      // 8
        uint32_t virtual_interrupt_delivery : 1;        // 9
        uint32_t pause_loop_exiting : 1;                // 10
        uint32_t rdrand_exiting : 1;                    // 11
        uint32_t enable_invpcid : 1;                    // 12
        uint32_t enable_vm_functions : 1;               // 13
        uint32_t vmcs_shadowing : 1;                    // 14
        uint32_t enable_encls_exiting : 1;              // 15
        uint32_t rdseed_exiting : 1;                    // 16
        uint32_t enable_pml : 1;                        // 17
        uint32_t ept_violation_ve : 1;                  // 18
        uint32_t conceal_nonroot_operation_from_pt : 1; // 19
        uint32_t enable_xsaves_xrstors : 1;             // 20
        uint32_t : 1;                                   // 21
        uint32_t mode_based_execute_ctrl_for_ept : 1;   // 22
        uint32_t : 2;                                   // 23-24
        uint32_t use_tsc_scaling : 1;                   // 25
    };
} proc_based_ctls2_t;

typedef union {
    uint32_t value;
    struct __attribute__((__packed__)) {
        uint32_t : 2;                                   // 0-1
        uint32_t save_debug_controls : 1;               // 2
        uint32_t : 6;                                   // 3-8
        uint32_t host_address_space_size : 1;           // 9
        uint32_t : 2;                                   // 10-11
        uint32_t load_ia32_perf_global_ctrl : 1;        // 12
        uint32_t : 2;                                   // 13-14
        uint32_t acknowledge_interrupt_on_exit : 1;     // 15
        uint32_t : 2;                                   // 16-17
        uint32_t save_ia32_pat : 1;                     // 18
        uint32_t load_ia32_pat : 1;                     // 19
        uint32_t save_ia32_efer : 1;                    // 20
        uint32_t load_ia32_efer : 1;                    // 21
        uint32_t save_vmx_preeption_timer_value : 1;    // 22
        uint32_t clear_ia32_bndcfgs : 1;                // 23
        uint32_t conceal_vmexits_from_pt : 1;           // 25
    };
} vmexit_ctls_t;

typedef union {
    uint32_t value;
    struct __attribute__((__packed__)) {
        uint32_t : 2;                                   // 0-1
        uint32_t load_debug_controls : 1;               // 2
        uint32_t : 6;                                   // 3-8
        uint32_t ia32_mode_guest : 1;                   // 9
        uint32_t entry_to_smm : 1;                      // 10
        uint32_t deactivate_dual_monitor_treatment : 1; // 11
        uint32_t : 1;                                   // 12
        uint32_t load_ia32_perf_global_ctrl : 1;        // 13
        uint32_t load_ia32_pat : 1;                     // 14
        uint32_t load_ia32_efer : 1;                    // 15
        uint32_t load_ia32_bndcfgs : 1;                 // 16
        uint32_t conceal_vmentries_from_pt : 1;         // 17
    };
} vmentry_ctls_t;

#endif
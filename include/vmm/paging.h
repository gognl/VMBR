#ifndef __PAGING_H
#define __PAGING_H

#include <lib/types.h>

extern qword_t initialize_host_paging();
extern qword_t initialize_ept();

#define COMPUTER_RAM 8              // 8gb. TODO find this out instead of macro
#define LARGE_PAGE_SIZE 0x200000ull
#define PAGE_SIZE 0x1000ull

#define PTE_P (1<<0)            // Present bit
#define PTE_W (1<<1)            // Writeable bit
#define PTE_PS (1<<7)           // Huge Page bit (2MB)

#define ADDRMASK_EPT_PML4_INDEX(x) ((x & 0xFF8000000000ULL) >> 39)
#define ADDRMASK_EPT_PDPT_INDEX(x) ((x & 0x7FC0000000ULL) >> 30)
#define ADDRMASK_EPT_PD_INDEX(x) ((x & 0x3FE00000ULL) >> 21)
#define ADDRMASK_EPT_PT_INDEX(x) ((x & 0x1FF000ULL) >> 12)

#define PML4E_IDX(x) (((x) & 0xff8000000000ull) >> 39)   // bits 39:47
#define PML3E_IDX(x) (((x) & 0x7fc0000000ull) >> 30)     // bits 30:38
#define PML2E_IDX(x) (((x) & 0x3fe00000ull) >> 21)       // bits 21:29
#define PML1E_IDX(x) (((x) & 0x1ff000ull) >> 12)         // bits 12:20
#define PAGING_OFFSET_4KB(x) ((x) & 0xfffull)
#define PAGING_OFFSET_2MB(x) ((x) & 0x1fffffull)
#define PAGING_OFFSET_1GB(x) ((x) & 0x3fffffffull)
#define PAGING_P (1<<0)
#define PAGING_PS (1<<7)

typedef union {
    uint64_t ept_pml4;
    uint64_t value;
    struct __attribute__((__packed__)) {
        uint64_t memory_type : 3;           // 0-2
        #define UNCACHEABLE 0
        #define WRITEBACK 6
        uint64_t page_walk_length_m1 : 3;   // 3-5
        uint64_t enable_accessed_dirty : 1; // 6
    };
} eptp_t;

typedef union {
    uint64_t next_pd;
    uint64_t value;
    struct __attribute__((__packed__)) {
        uint64_t read_access : 1;           // 0
        uint64_t write_access : 1;          // 1
        uint64_t execute_access : 1;        // 2
        uint64_t : 5;                       // 3-7
        uint64_t accessed_flag : 1;         // 8
        uint64_t : 1;                       // 9
        uint64_t execute_access_usr : 1;    // 10
    };
} ept_pde_t;
typedef ept_pde_t ept_pdpte_t;
typedef ept_pde_t ept_pml4e_t;

typedef union {
    uint64_t page;
    uint64_t value;
    struct __attribute__((__packed__)) {
        uint64_t read_access : 1;           // 0
        uint64_t write_access : 1;          // 1
        uint64_t execute_access : 1;        // 2
        uint64_t memory_type : 3;           // 3-5
        uint64_t ignore_pat_memtype : 1;    // 6
        uint64_t : 1;                       // 7
        uint64_t accessed_flag : 1;         // 8
        uint64_t dirty_flag : 1;            // 9
        uint64_t execute_access_usr : 1;    // 10
        uint64_t : 46;                      // 11-56
        uint64_t verify_guest_paging : 1;   // 57
        uint64_t paging_write_access : 1;   // 58
        uint64_t : 1;                       // 59
        uint64_t supervisor_shadow_stk : 1; // 60
        uint64_t subpage_write_perms : 1;   // 61
        uint64_t : 1;                       // 62
        uint64_t suppress_ve : 1;           // 63
    };
} ept_pte_t;

#define IA32_MTRR_DEF_TYPE 0x2ff
#define IA32_MTRRCAP 0xfe

#define IA32_MTRR_FIX64K_00000 0x250
#define IA32_MTRR_FIX16K_80000 0x258
#define IA32_MTRR_FIX16K_A0000 0x259
#define IA32_MTRR_FIX4K_C0000 0x268
#define IA32_MTRR_FIX4K_C8000 0x269
#define IA32_MTRR_FIX4K_D0000 0x26a
#define IA32_MTRR_FIX4K_D8000 0x26b
#define IA32_MTRR_FIX4K_E0000 0x26c
#define IA32_MTRR_FIX4K_E8000 0x26d
#define IA32_MTRR_FIX4K_F0000 0x26e
#define IA32_MTRR_FIX4K_F8000 0x26f

#define GET_TYPE_RANGED(address, msr, start, jump) (((msr) >> ((((address)-(start)) / (jump)) << 3)) & 0xffull)

#define IA32_MTRR_PHYSBASE0 0x200
#define IA32_MTRR_PHYSMASK0 0x201

#define GET_ENTRY_LENGTH(mask) ((mask) & (-(mask)))     // leave only the rightmost set bit

typedef union {
    qword_t value;
    struct __attribute__((__packed__)) {
        qword_t type : 8;
        qword_t : 2;
        qword_t fixed_enabled : 1;
        qword_t enabled : 1;
    };
} ia32_mtrr_def_type_t;

typedef union {
    qword_t value;
    struct __attribute__((__packed__)) {
        qword_t vcnt : 8;
        qword_t fix: 1;
        qword_t : 1;
        qword_t wc : 1;
        qword_t smrr : 1;
    };
} ia32_mtrrcap_t;

typedef union {
    qword_t value;
    struct __attribute__((__packed__)) {
        qword_t type : 8;
        qword_t : 4;
        qword_t page_idx : 24;
    };
} ia32_mtrr_physbase_t;

typedef union {
    qword_t value;
    struct __attribute__((__packed__)) {
        qword_t : 11;
        qword_t valid : 1;
        qword_t page_idx : 24;
    };
} ia32_mtrr_physmask_t;

#endif
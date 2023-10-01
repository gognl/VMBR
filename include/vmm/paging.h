#ifndef __PAGING_H
#define __PAGING_H

#include <lib/types.h>

extern qword_t initialize_host_paging();
extern qword_t initialize_ept();

#define COMPUTER_RAM 8              // 8gb. TODO find this out instead of macro
#define LARGE_PAGE_SIZE 0x200000
#define PAGE_SIZE 0x1000

#define PTE_P (1<<0)            // Present bit
#define PTE_W (1<<1)            // Writeable bit
#define PTE_PS (1<<7)           // Huge Page bit (2MB)

typedef union {
    uint64_t ept_pml4;
    uint64_t value;
    struct __attribute__((__packed__, __aligned__(8))) {
        uint64_t memory_type : 3;           // 0-2
        #define WRITEBACK 6
        uint64_t page_walk_length_m1 : 3;   // 3-5
        uint64_t enable_accessed_dirty : 1; // 6
    };
} eptp_t;

typedef union {
    uint64_t next_pd;
    uint64_t value;
    struct __attribute__((__packed__, __aligned__(8))) {
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
    struct __attribute__((__packed__, __aligned__(8))) {
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

#endif
#ifndef __TYPES_H
#define __TYPES_H

typedef unsigned long long qword_t;
typedef unsigned int dword_t;
typedef unsigned short word_t;
typedef unsigned char byte_t;
typedef unsigned char char_t;

typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#define FALSE 0
#define TRUE 1
#define BOOL byte_t

#define NULL 0
#define NULLPTR ((void*)0)

#define ALIGN_UP(num, align) (((num)+(align)-1) & (-(align)))   // this is nice. Only works when align is a multiple of 2, though.

#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)
#define va_list __builtin_va_list

#define ttyS0 0x3F8
#define DBG_PORT ttyS0

typedef struct _mmap_entry {
    qword_t base_addr;
    qword_t length;
    dword_t type;
    #define E820_USABLE 1
    #define E820_RESERVED 2
    #define E820_ACPI_RECLAIMABLE 3
    #define E820_ACPI_NVS 4
    #define E820_BAD 5
    dword_t acpi_ea;
} __attribute__((__packed__)) mmap_entry;


typedef struct _mmap_table {
    dword_t length;
    mmap_entry entries[];
} __attribute__((__packed__)) mmap_table;

#define PAGE_SIZE 0x1000

#endif
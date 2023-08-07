#ifndef __TYPES_H
#define __TYPES_H

typedef unsigned long long QWORD, *PQWORD;
typedef unsigned int DWORD, *PDWORD;
typedef unsigned short WORD, *PWORD;
typedef unsigned char BYTE, *PBYTE;

typedef unsigned int UINT32;
typedef unsigned long long UINT64;

#define FALSE 0
#define TRUE (!(FALSE))

#define ALIGN_UP(num, align) (((num)+(align)-1) & (-(align)))   // this is nice. Only works when align is a multiple of 2, though.

#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)
#define va_list __builtin_va_list

#define ttyS0 0x3F8
#define DBG_PORT ttyS0

typedef struct _mmap_entry {
    UINT64 base_addr;
    UINT64 length;
    UINT32 type;
    #define E820_USABLE 1
    #define E820_RESERVED 2
    #define E820_ACPI_RECLAIMABLE 3
    #define E820_ACPI_NVS 4
    #define E820_BAD 5
    UINT32 acpi_ea;
} __attribute__((__packed__)) mmap_entry;


typedef struct _mmap_table {
    UINT32 length;
    mmap_entry entries[];
} __attribute__((__packed__)) mmap_table;

#define PAGE_SIZE 0x1000

#endif
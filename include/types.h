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
    UINT32 acpi_ea;
} mmap_entry;

typedef struct _mmap_table {
    UINT32 length;
    mmap_entry *entries;
} mmap_table;

#endif
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

#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)
#define va_list __builtin_va_list

#define PAGE_SIZE 0x1000

typedef struct {
    uint64_t base;
    uint32_t limit;
} __attribute__((__packed__)) gdtr_t;

typedef struct {
    uint64_t base;
    uint32_t limit;
} __attribute__((__packed__)) idtr_t;

#endif
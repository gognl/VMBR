#ifndef __TYPES_H
#define __TYPES_H

typedef unsigned long long qword_t;
typedef unsigned int dword_t;
typedef unsigned short word_t;
typedef unsigned char byte_t;
typedef unsigned char char_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
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

// https://stackoverflow.com/questions/19401887/how-to-check-the-size-of-a-structure-at-compile-time
#define STATIC_ASSERT(cond) typedef char assertion[(!!(cond))*2-1]

#define FLIP_WORD(n)    ((((n)<<8) | ((n)>>8)) & 0xffff)
#define FLIP_DWORD(n)   (((((n)>>24) & 0xff) | \
                        (((n)<<8) & 0xff0000) | \
                        (((n)>>8) & 0xff00) | \
                        (((n)<<24) & 0xff000000)) & 0xffffffff);

#define PAGE_SIZE 0x1000

typedef enum {
    SUCCESS = 1,
    FAIL = 0
} STATUS;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((__packed__)) gdtr_t;

#endif
#ifndef __UTIL_H
#define __UTIL_H

#include <lib/types.h>

#define ALIGN_UP(num, align) (((num)+(align)-1) & (-(align)))   // this is nice. Only works when align is a multiple of 2, though.

extern byte_t *memcpy(byte_t *dest, const byte_t *src, uint32_t count);
extern byte_t *memset(byte_t *dest, byte_t val, uint32_t count);
extern word_t *memsetw(word_t *dest, word_t val, uint32_t count);
extern BOOL memcmp(byte_t *src1, byte_t *src2, uint32_t count);
extern uint32_t strlen(const char_t *str);
extern uint64_t digitCount(uint64_t num, uint64_t base);
extern uint64_t pow(uint64_t m, uint64_t n);

#endif
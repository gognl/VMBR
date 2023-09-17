#ifndef __REAL_H
#define __REAL_H

#include <types.h>

extern void init_mmap(void);
extern void init_real(void);
extern byte_t *allocate_memory(uint64_t length);

#endif
#ifndef __REAL_H
#define __REAL_H

#include <types.h>

extern void init_mmap(void);
extern void init_real(void);
extern BYTE *allocate_memory(UINT32 length);

#endif
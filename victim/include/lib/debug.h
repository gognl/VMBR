#ifndef __DEBUG_H
#define __DEBUG_H

#include <lib/types.h>

#define ttyS0 0x3F8
#define DBG_PORT ttyS0

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_ERROR 2
#define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG
#define DEBUG_VMEXITS FALSE
#define DEBUG_ALLOCATIONS FALSE

extern void LOG_DEBUG(char_t *s, ...);
extern void LOG_INFO(char_t *s, ...);
extern void LOG_ERROR(char_t *s, ...);

extern void AcquireLock(dword_t* lock);
extern void ReleaseLock(dword_t* lock);

#endif
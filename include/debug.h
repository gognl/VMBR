#ifndef __DEBUG_H
#define __DEBUG_H

#include <types.h>

#define ttyS0 0x3F8
#define DBG_PORT ttyS0

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_ERROR 2
#define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG

extern void LOG_DEBUG(char_t *s, ...);
extern void LOG_INFO(char_t *s, ...);
extern void LOG_ERROR(char_t *s, ...);

extern void putch(char_t c);
extern void puts(char_t *s, ...);

#endif
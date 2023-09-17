#ifndef __DEBUG_H
#define __DEBUG_H

#define ttyS0 0x3F8
#define DBG_PORT ttyS0

extern void putch(char_t c);
extern void puts(char_t *s, ...);

#endif
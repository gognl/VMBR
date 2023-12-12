#ifndef __HOOKS_H
#define __HOOKS_H

#include <lib/types.h>

#define E820_MAGIC 0x0534D4150
#define RFLAGS_CARRY (1<<0)

typedef struct __attribute__((__packed__)) {
    word_t offset;
    word_t segment;
} ivt_entry_t;

extern void setup_int15h_hook();
extern void handle_int15h_hook();
#endif
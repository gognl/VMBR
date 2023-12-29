#ifndef __SERIAL_H
#define __SERIAL_H

#include <lib/types.h>

extern void ReadDisk(void (*)());

extern void load_guest();

typedef struct __attribute__((__packed__)) {
    byte_t size;
    byte_t unused;
    word_t amount;
    word_t offset;
    word_t segment;
    qword_t sector;
} dap_t;

typedef struct __attribute__((__packed__)) {
    byte_t code[446];
    byte_t partition_entries[4*16];
    word_t signature;
} mbr_t;

#define BIOS_SIGNATURE 0xaa55

#endif
#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lib/types.h>

#define LSTAR_MSR 0xC0000082

#define NTOSKRNL_KiSystemCall64Shadow_OFFSET 0xa19180
#define NTOSKRNL_KiSystemCall64_OFFSET 0x40f000
#define NTOSKRNL_MiDriverLoadSucceeded_OFFSET 0x755773  // actually at 0x755764, but "push rbp" is a bit later

extern void handle_lstar_write(uint64_t lstar);

#endif
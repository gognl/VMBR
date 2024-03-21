#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lib/types.h>
#include <vmm/vmm.h>

#define LSTAR_MSR 0xC0000082

#define NTOSKRNL_KiSystemCall64Shadow_OFFSET 0xa19180
#define NTOSKRNL_KiSystemCall64_OFFSET 0x40f000
#define NTOSKRNL_MiDriverLoadSucceeded_OFFSET 0x755773  // actually at 0x755764, but "push rbp" is a bit later

#define KBDCLASS_SIGNATURE_OFFSET 0x89f8
#define KBDCLASS_KeyboardClassServiceCallback_OFFSET 0x5583 // actually at 0x5570, but "push rbp" is a bit later

#define PUSH_RBP 0x55
#define INT3 0xcc

extern void handle_lstar_write(uint64_t lstar);
extern void handle_MiDriverLoadSucceeded_hook(vmexit_data_t *state);
extern void handle_KeyboardClassServiceCallback_hook(vmexit_data_t *state);

typedef struct {
    word_t UnitId;
    word_t MakeCode;
    word_t Flags;
    #define KEY_MAKE 0
    #define KEY_BREAK 1
    word_t Reserved;
    dword_t  ExtraInformation;
} KEYBOARD_INPUT_DATA;


#endif
#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lib/types.h>
#include <vmm/vmm.h>

typedef struct {
    word_t UnitId;
    word_t MakeCode;
    word_t Flags;
    #define KEY_MAKE 0
    #define KEY_BREAK 1
    word_t Reserved;
    dword_t  ExtraInformation;
} KEYBOARD_INPUT_DATA;

extern void handle_KeyboardClassServiceCallback_hook(vmexit_data_t *state);

#endif
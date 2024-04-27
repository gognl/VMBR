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

#define SCANCODE_CTRL 0x1d
#define SCANCODE_RSHIFT 0x2a
#define SCANCODE_LSHIFT 0x36
#define SCANCODE_ALT 0x38
#define SCANCODE_CTRL_RELEASED 0xa7
#define SCANCODE_SHIFT_RELEASED 0xa8
#define SCANCODE_ALT_RELEASED 0xa9

extern void handle_KeyboardClassServiceCallback_hook(vmexit_data_t *state);
extern uint64_t locate_KeyboardClassServiceCallback(uint64_t kbdclass);

#endif
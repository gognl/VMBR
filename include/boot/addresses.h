#ifndef __ADDRESSES_H
#define __ADDRESSES_H

#define REAL_START 0x4000   // macros.asm
#define MMAP_TABLE 0x5000   // macros.asm
#define HOOK_INT15H 0x1000
#define REGISTERS_ADDRESS 0x7000

#define DAP_ADDRESS 0x2fef          // serial.asm
#define DRIVE_IDX_ADDRESS 0x2fff    // serial.asm
#define MBR_ADDRESS 0x3000


#define _SYS_STACK 0x7000 // boot.asm


#endif
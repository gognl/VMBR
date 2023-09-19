#ifndef __INIT_H
#define __INIT_H
#include <types.h>

 #define IA32_VMX_BASIC 0x480
 #define IA32_VMX_CR0_FIXED0 0x486
 #define IA32_VMX_CR0_FIXED1 0x487
 #define IA32_VMX_CR4_FIXED0 0x488
 #define IA32_VMX_CR4_FIXED1 0x489
 #define CR4_VMXE (1 << 13)
 #define CR0_NE (1 << 5)

#define GDT_AB_RW (1<<1)        // R/W bit - R for code segments, W for data segments
#define GDT_AB_DC (1<<2)        // Direction bit (0 for growing up, 1 for growing down)
#define GDT_AB_E (1<<3)         // Executable bit
#define GDT_AB_S (1<<4)         // S bit (type) - 1 if code/data segment
#define GDT_AB_P (1<<7)         // Present bit

typedef struct {
    uint32_t revision_id; // bits 0-30 are the id, bit 31 is the shadow-vmcs indicator.
    uint32_t vmx_abort;
    // vmcs data
} __attribute__((__packed__)) vmcs_t;

extern void init_vmm(void);
#endif

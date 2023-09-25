#ifndef __INIT_H
#define __INIT_H
#include <types.h>

#define IA32_VMX_BASIC 0x480
#define IA32_VMX_CR0_FIXED0 0x486
#define IA32_VMX_CR0_FIXED1 0x487
#define IA32_VMX_CR4_FIXED0 0x488
#define IA32_VMX_CR4_FIXED1 0x489

#define IA32_VMX_PINBASED_CTLS 0x481
#define IA32_VMX_TRUE_PINBASED_CTLS 0x48d
#define IA32_VMX_PROCBASED_CTLS 0x482
#define IA32_VMX_TRUE_PROCBASED_CTLS 0x48e
#define IA32_VMX_EXIT_CTLS 0x483
#define IA32_VMX_TRUE_EXIT_CTLS 0x48f
#define IA32_VMX_ENTRY_CTLS 0x484
#define IA32_VMX_TRUE_ENTRY_CTLS 0x490

#define DEFAULT_PINBASED_CTLS ((1ull<<1) | (1ull<<2) | (1ull<<4))
#define DEFAULT_PROCBASED_CTLS ((1ull<<1) | (1ull<<4) | (1ull<<5) | (1ull<<6) | (1ull<<8) | (1ull<<13) | (1ull<<14) | (1ull<<15) | (1ull<<16) | (1ull<<26))
#define DEFAULT_EXIT_CTLS ((1ull<<0) | (1ull<<1) | (1ull<<2) | (1ull<<3) | (1ull<<4) | (1ull<<5) | (1ull<<6) | (1ull<<7) | (1ull<<8) | (1ull<<10) | (1ull<<11) | (1ull<<13) | (1ull<<14) | (1ull<<16) | (1ull<<17))
#define DEFAULT_ENTRY_CTLS ((1ull<<0) | (1ull<<1) | (1ull<<2) | (1ull<<3) | (1ull<<4) | (1ull<<5) | (1ull<<6) | (1ull<<7) | (1ull<<8) | (1ull<<12))

#define CANONICAL_ADDRESS 0xffffffff

#define CR4_VMXE (1 << 13)
#define CR0_NE (1 << 5)

#define GDT_AB_A (1<<0)         // Accessed bit 
#define GDT_AB_RW (1<<1)        // R/W bit - R for code segments, W for data segments
#define GDT_AB_DC (1<<2)        // Direction bit (0 for growing up, 1 for growing down)
#define GDT_AB_E (1<<3)         // Executable bit
#define GDT_AB_S (1<<4)         // S bit (type) - 1 if code/data segment
#define GDT_AB_P (1<<7)         // Present bit
#define GDT_AB_L (1<<13)        // Long mode bit
#define GDT_AB_DB (1<<14)        // Size bit
#define GDT_AB_G (1<<15)        // Granularity flag
#define UNUSABLE_SELECTOR (1<<16)

#define PTE_P (1<<0)            // Present bit
#define PTE_W (1<<1)            // Writeable bit
#define PTE_PS (1<<7)           // Huge Page bit (2MB)

typedef struct {
    uint32_t revision_id; // bits 0-30 are the id, bit 31 is the shadow-vmcs indicator.
    uint32_t vmx_abort;
    // vmcs data
} __attribute__((__packed__)) vmcs_t;

extern void init_vmm(void);
#endif

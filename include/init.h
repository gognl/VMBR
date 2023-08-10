#include <types.h>

 #define IA32_VMX_BASIC 0x480
 #define IA32_VMX_CR0_FIXED0 0x486
 #define IA32_VMX_CR0_FIXED1 0x487
 #define IA32_VMX_CR4_FIXED0 0x488
 #define IA32_VMX_CR4_FIXED1 0x489
 #define CR4_VMXE (1 << 13)
 #define CR0_NE (1 << 5)

extern void init_vmm(void);



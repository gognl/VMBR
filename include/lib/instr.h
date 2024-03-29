#ifndef __INSTR_H
#define __INSTR_H

#include <lib/types.h>
#include <vmm/vmcs.h>
#include <lib/debug.h>
#include <vmm/paging.h>
#include <vmm/vmm.h>
#include <hardware/idt.h>

#define CARRY_FLAG (1<<0)
#define ZERO_FLAG (1<<6)

__attribute__((always_inline)) void inline __hlt(){
    __asm__ __volatile__ ("hlt");
}
__attribute__((always_inline)) void inline __nop(){
    __asm__ __volatile__ ("nop");
}

// control registers read & write

__attribute__((always_inline)) void inline __cpuid(qword_t eax_in, qword_t ecx_in, 
                                                    qword_t* eax, qword_t* ebx, 
                                                    qword_t* ecx, qword_t* edx){
    __asm__ __volatile__("cpuid" :  "=a"(*eax), 
                                    "=b"(*ebx), 
                                    "=c" (*ecx), 
                                    "=d" (*edx) : 
                                    "a"(eax_in), 
                                    "c"(ecx_in));
}

__attribute__((always_inline)) qword_t inline __rdmsr(qword_t msr){
    dword_t upper = 0, lower = 0;
    __asm__ __volatile__("rdmsr" : "=d" (upper), "=a" (lower) : "c"(msr));
    return ((qword_t)upper << 32) | lower;
}
__attribute__((always_inline)) qword_t inline __read_rflags(void){
    qword_t rflags;
    __asm__ __volatile__("pushfq; pop %0" : "=r"(rflags));
    return rflags;
}
__attribute__((always_inline)) qword_t inline __read_cr0(void){
    qword_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    return cr0;
}

__attribute__((always_inline)) qword_t inline __read_cr3(void){
    qword_t cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}
__attribute__((always_inline)) qword_t inline __read_cr4(void){
    qword_t cr4;
    __asm__ __volatile__("mov %%cr4, %0" : "=r"(cr4));
    return cr4;
}
__attribute__((always_inline)) qword_t inline __read_dr7(void){
	qword_t dr7;
	__asm__ __volatile__("mov %%dr7, %0" : "=r"(dr7));
	return dr7;
}
__attribute__((always_inline)) qword_t inline __read_rsp(void){
    qword_t rsp;
    __asm__ __volatile__("mov %%rsp, %0" : "=r"(rsp));
    return rsp;
}
__attribute__((always_inline)) void inline __write_cr0(qword_t cr0){
	__asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0));
}
__attribute__((always_inline)) void inline __write_cr4(qword_t cr4){
	__asm__ __volatile__("mov %0, %%cr4" :: "r"(cr4));
}
__attribute__((always_inline)) void inline __wrmsr(qword_t msr, qword_t value){
	__asm__ __volatile__("wrmsr" :: "c"(msr), "a"(value & 0xffffffff), "d"(value>>32));
}

// read selectors

__attribute__((always_inline)) word_t inline __read_es(void){
    word_t es;
    __asm__ __volatile__("mov %%es, %0" : "=r"(es));
    return es;
}
__attribute__((always_inline)) word_t inline __read_cs(void){
    word_t cs;
    __asm__ __volatile__("mov %%cs, %0" : "=r"(cs));
    return cs;
}
__attribute__((always_inline)) word_t inline __read_ss(void){
    word_t ss;
    __asm__ __volatile__("mov %%ss, %0" : "=r"(ss));
    return ss;
}
__attribute__((always_inline)) word_t inline __read_ds(void){
    word_t ds;
    __asm__ __volatile__("mov %%ds, %0" : "=r"(ds));
    return ds;
}
__attribute__((always_inline)) word_t inline __read_fs(void){
    word_t fs;
    __asm__ __volatile__("mov %%fs, %0" : "=r"(fs));
    return fs;
}
__attribute__((always_inline)) word_t inline __read_gs(void){
    word_t gs;
    __asm__ __volatile__("mov %%gs, %0" : "=r"(gs));
    return gs;
}
__attribute__((always_inline)) void inline __read_gdtr(gdtr_t* gdtr){
    __asm__ __volatile__("sgdt %0" : "=m"(*gdtr));
}
__attribute__((always_inline)) void inline __read_idtr(idtr_t* idtr){
    __asm__ __volatile__("sidt %0" : "=m"(*idtr));
}
__attribute__((always_inline)) void inline __write_idtr(idtr_t* idtr){
    __asm__ __volatile__("lidt %0" :: "m"(*idtr));
}

__attribute__((always_inline)) void inline __sti(){
    __asm__ __volatile__("sti");
}

__attribute__((always_inline)) void inline __xsetbv(dword_t eax, dword_t ecx, dword_t edx){
    __asm__ __volatile__("xsetbv" :: "a"(eax), "c"(ecx), "d"(edx));
}

__attribute__((always_inline)) void inline __pause(){
    __asm__ __volatile__("pause" ::: "memory");
}

// vmx instructions

__attribute__((always_inline)) void inline __vmwrite(VMCS_ENCODING field, qword_t value){
    __asm__ __volatile__("vmwrite %1, %0" :: "r" ((qword_t)field), "r" (value));
}
__attribute__((always_inline)) qword_t inline __vmread(VMCS_ENCODING field){
    qword_t value;
    __asm__ __volatile__("vmread %0, %1" : "=r"(value) : "r"((qword_t)field));
    return value;
}
__attribute__((always_inline)) void inline __vmxon(byte_t *vmxon_region){
    qword_t rflags;
    __asm__ __volatile__("vmxon %1; pushfq; pop %0" : "=r"(rflags) : "m" (vmxon_region));

    if (rflags & CARRY_FLAG)
        LOG_ERROR("vmxon VM_FAIL_INVALID\n");
    if (rflags & ZERO_FLAG)
        LOG_ERROR("vmxon VM_FAIL_VALID (%d)\n", (dword_t)__vmread(RODATA_VM_INSTRUCTION_ERROR));
}
__attribute__((always_inline)) void inline __vmclear(byte_t *vmcs_ptr){
    qword_t rflags;
    __asm__ __volatile__("vmclear %1; pushfq; pop %0" : "=r"(rflags) : "m" (vmcs_ptr));

    if (rflags & CARRY_FLAG)
        LOG_ERROR("vmclear VM_FAIL_INVALID\n");
    if (rflags & ZERO_FLAG)
        LOG_ERROR("vmclear VM_FAIL_VALID (%d)\n", (dword_t)__vmread(RODATA_VM_INSTRUCTION_ERROR));
}
__attribute__((always_inline)) void inline __vmptrld(byte_t *vmcs_ptr){
    qword_t rflags;
    __asm__ __volatile__("vmptrld %1; pushfq; pop %0" : "=r"(rflags) : "m" (vmcs_ptr));

    if (rflags & CARRY_FLAG)
        LOG_ERROR("vmptrld VM_FAIL_INVALID\n");
    if (rflags & ZERO_FLAG)
        LOG_ERROR("vmptrld VM_FAIL_VALID (%d)\n", (dword_t)__vmread(RODATA_VM_INSTRUCTION_ERROR));
}
__attribute__((always_inline)) void inline __vmlaunch(){
    qword_t rflags;
    __asm__ __volatile__("vmlaunch; pushfq; pop %0" : "=r"(rflags));

    if (rflags & CARRY_FLAG)
        LOG_ERROR("vmlaunch VM_FAIL_INVALID\n");
    if (rflags & ZERO_FLAG)
        LOG_ERROR("vmlaunch VM_FAIL_VALID (%d)\n", (dword_t)__vmread(RODATA_VM_INSTRUCTION_ERROR));
}
__attribute__((always_inline)) void inline __vmresume(){
    qword_t rflags;
    __asm__ __volatile__("vmresume; pushfq; pop %0" : "=r"(rflags));

    if (rflags & CARRY_FLAG)
        LOG_ERROR("vmresume VM_FAIL_INVALID\n");
    if (rflags & ZERO_FLAG)
        LOG_ERROR("vmresume VM_FAIL_VALID (%d)\n", (dword_t)__vmread(RODATA_VM_INSTRUCTION_ERROR));
}

// ports

__attribute__((always_inline)) byte_t inline __inb(word_t port){
    byte_t in;
    __asm__ __volatile__ ("inb %%dx, %%al" : "=a" (in) : "dN" (port));
    return in;
}
__attribute__((always_inline)) void inline __outb(word_t port, byte_t data){
    __asm__ __volatile__ ("outb %%al, %%dx" : : "a" (data), "dN" (port));
}

__attribute__((always_inline)) word_t inline __inw(word_t port){
    word_t in;
    __asm__ __volatile__ ("inw %%dx, %%ax" : "=a" (in) : "dN" (port));
    return in;
}
__attribute__((always_inline)) void inline __outw(word_t port, word_t data){
    __asm__ __volatile__ ("outw %%ax, %%dx" : : "a" (data), "dN" (port));
}

__attribute__((always_inline)) dword_t inline __inl(word_t port){
    dword_t in;
    __asm__ __volatile__ ("inl %%dx, %%eax" : "=a" (in) : "dN" (port));
    return in;
}
__attribute__((always_inline)) void inline __outl(word_t port, dword_t data){
    __asm__ __volatile__ ("outl %%eax, %%dx" : : "dN" (port), "a" (data));
}

#endif
#ifndef __UTIL_H
#define __UTIL_H

#include <types.h>
#include <vmcs.h>

typedef struct {
    uint64_t base;
    uint32_t limit;
} __attribute__((__packed__)) gdtr_t;
typedef struct {
    uint64_t base;
    uint32_t limit;
} __attribute__((__packed__)) idtr_t;

extern byte_t *memcpy(byte_t *dest, const byte_t *src, uint32_t count);
extern byte_t *memset(byte_t *dest, byte_t val, uint32_t count);
extern word_t *memsetw(word_t *dest, word_t val, uint32_t count);
extern BOOL memcmp(byte_t *src1, byte_t *src2, uint32_t count);
extern uint32_t strlen(const char_t *str);
extern byte_t inportb (word_t _port);
extern void outportb (word_t _port, byte_t _data);
extern uint32_t digitCount (uint32_t num);
extern uint32_t pow (uint32_t m, uint32_t n);


__attribute__((always_inline))
qword_t inline __get_cpuid(){
    dword_t ecx, edx;
    __asm__ __volatile__("cpuid": "=d" (edx), "=c" (ecx) : "a"(1));
    return ((qword_t)edx << 32) | ecx;
}

__attribute__((always_inline)) 
qword_t inline __read_msr(qword_t msr){
    dword_t upper = 0, lower = 0;
    __asm__ __volatile__("rdmsr" : "=d" (upper), "=a" (lower) : "c"(msr));
    return ((qword_t)upper << 32) | lower;
}

// control registers read & write

__attribute__((always_inline))
qword_t inline __read_rflags(void){
    qword_t rflags;
    __asm__ __volatile__("pushfq; pop %0" : "=r"(rflags));
    return rflags;
}

__attribute__((always_inline)) 
qword_t inline __read_cr0(void){
    qword_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    return cr0;
}

__attribute__((always_inline)) 
qword_t inline __read_cr3(void){
    qword_t cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

__attribute__((always_inline)) 
qword_t inline __read_cr4(void){
    qword_t cr4;
    __asm__ __volatile__("mov %%cr4, %0" : "=r"(cr4));
    return cr4;
}

__attribute__((always_inline)) 
void inline __write_cr0(qword_t cr0){
    __asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0));
}

__attribute__((always_inline)) 
void inline __write_cr4(qword_t cr4){
    __asm__ __volatile__("mov %0, %%cr4" :: "r"(cr4));
}


// read selectors

__attribute__((always_inline)) 
word_t inline __read_es(void){
    word_t es;
    __asm__ __volatile__("mov %%es, %0" : "=r"(es));
    return es;
}
__attribute__((always_inline)) 
word_t inline __read_cs(void){
    word_t cs;
    __asm__ __volatile__("mov %%cs, %0" : "=r"(cs));
    return cs;
}
__attribute__((always_inline)) 
word_t inline __read_ss(void){
    word_t ss;
    __asm__ __volatile__("mov %%ss, %0" : "=r"(ss));
    return ss;
}
__attribute__((always_inline)) 
word_t inline __read_ds(void){
    word_t ds;
    __asm__ __volatile__("mov %%ds, %0" : "=r"(ds));
    return ds;
}
__attribute__((always_inline)) 
word_t inline __read_fs(void){
    word_t fs;
    __asm__ __volatile__("mov %%fs, %0" : "=r"(fs));
    return fs;
}
__attribute__((always_inline)) 
word_t inline __read_gs(void){
    word_t gs;
    __asm__ __volatile__("mov %%gs, %0" : "=r"(gs));
    return gs;
}
__attribute__((always_inline))
void inline __read_gdtr(gdtr_t* gdtr){
    __asm__ __volatile__("sgdt %0" : "=m"(*gdtr));
}
__attribute__((always_inline))
void inline __read_idtr(idtr_t* idtr){
    __asm__ __volatile__("sidt %0" : "=m"(*idtr));
}

// vmx instructions

__attribute__((always_inline)) 
void inline __vmxon(byte_t *vmxon_region){
    __asm__ __volatile__("vmxon %0" :: "m" (vmxon_region));
}

__attribute__((always_inline))
void inline __vmclear(byte_t *vmcs_ptr){
    __asm__ __volatile__("vmclear %0" :: "m" (vmcs_ptr));
}

__attribute__((always_inline))
void inline __vmptrld(byte_t *vmcs_ptr){
    __asm__ __volatile__("vmptrld %0" :: "m" (vmcs_ptr));
}

__attribute__((always_inline))
void inline __vmwrite(VMCS_ENCODING field, qword_t value){
    __asm__ __volatile__("vmwrite %1, %0" :: "r" ((qword_t)field), "r" (value));
}

__attribute__((always_inline))
void inline __vmlaunch(){
    __asm__ __volatile__("vmlaunch");
}

#endif
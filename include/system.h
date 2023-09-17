#ifndef __UTIL_H
#define __UTIL_H

#include <types.h>
#include <vmcs.h>

typedef struct {
    UINT64 base;
    UINT32 limit;
} gdtr_t;
typedef struct {
    UINT64 base;
    UINT32 limit;
} idtr_t;

extern unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count);
extern unsigned char *memset(unsigned char *dest, unsigned char val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern BOOL memcmp(BYTE *src1, BYTE *src2, UINT32 count);
extern int strlen(const unsigned char *str);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);
extern int digitCount (int num);
extern int pow (int m, int n);


__attribute__((always_inline))
QWORD inline __get_cpuid(){
    DWORD ecx, edx;
    __asm__ __volatile__("cpuid": "=d" (edx), "=c" (ecx) : "a"(1));
    return ((QWORD)edx << 32) | ecx;
}

__attribute__((always_inline)) 
UINT64 inline __read_msr(UINT64 msr){
    DWORD upper = 0, lower = 0;
    __asm__ __volatile__("rdmsr" : "=d" (upper), "=a" (lower) : "c"(msr));
    return ((QWORD)upper << 32) | lower;
}

// control registers read & write

__attribute__((always_inline))
QWORD inline __read_rflags(void){
    QWORD rflags;
    __asm__ __volatile__("pushfq; pop %0" : "=r"(rflags));
    return rflags;
}

__attribute__((always_inline)) 
QWORD inline __read_cr0(void){
    QWORD cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    return cr0;
}

__attribute__((always_inline)) 
QWORD inline __read_cr3(void){
    QWORD cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

__attribute__((always_inline)) 
QWORD inline __read_cr4(void){
    QWORD cr4;
    __asm__ __volatile__("mov %%cr4, %0" : "=r"(cr4));
    return cr4;
}

__attribute__((always_inline)) 
void inline __write_cr0(QWORD cr0){
    __asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0));
}

__attribute__((always_inline)) 
void inline __write_cr4(QWORD cr4){
    __asm__ __volatile__("mov %0, %%cr4" :: "r"(cr4));
}


// read selectors

__attribute__((always_inline)) 
WORD inline __read_es(void){
    WORD es;
    __asm__ __volatile__("mov %%es, %0" : "=r"(es));
    return es;
}
__attribute__((always_inline)) 
WORD inline __read_cs(void){
    WORD cs;
    __asm__ __volatile__("mov %%cs, %0" : "=r"(cs));
    return cs;
}
__attribute__((always_inline)) 
WORD inline __read_ss(void){
    WORD ss;
    __asm__ __volatile__("mov %%ss, %0" : "=r"(ss));
    return ss;
}
__attribute__((always_inline)) 
WORD inline __read_ds(void){
    WORD ds;
    __asm__ __volatile__("mov %%ds, %0" : "=r"(ds));
    return ds;
}
__attribute__((always_inline)) 
WORD inline __read_fs(void){
    WORD fs;
    __asm__ __volatile__("mov %%fs, %0" : "=r"(fs));
    return fs;
}
__attribute__((always_inline)) 
WORD inline __read_gs(void){
    WORD gs;
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
void inline __vmxon(BYTE *vmxon_region){
    __asm__ __volatile__("vmxon %0" :: "m" (vmxon_region));
}

__attribute__((always_inline))
void inline __vmclear(BYTE *vmcs_ptr){
    __asm__ __volatile__("vmclear %0" :: "m" (vmcs_ptr));
}

__attribute__((always_inline))
void inline __vmptrld(BYTE *vmcs_ptr){
    __asm__ __volatile__("vmptrld %0" :: "m" (vmcs_ptr));
}

__attribute__((always_inline))
void inline __vmwrite(VMCS_ENCODING field, UINT64 value){
    __asm__ __volatile__("vmwrite %1, %0" :: "r" ((QWORD)field), "r" (value));
}

__attribute__((always_inline))
void inline __vmlaunch(){
    __asm__ __volatile__("vmlaunch");
}

#endif
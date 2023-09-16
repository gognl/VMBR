#include <types.h>

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count){
    for(int i = 0; i<count; i++){
        dest[i] = src[i];
    }
    return dest;
}

unsigned char *memset(unsigned char *dest, unsigned char val, int count){
    for(int i = 0; i<count; i++){
        dest[i] = val;
    }
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count){
    for(int i = 0; i<count; i++){
        dest[i] = val;
    }
    return dest;
}

BOOL memcmp(BYTE *src1, BYTE *src2, UINT32 count){
    for(UINT32 i = 0; i<count; i++){
        if (*src1 != *src2) return FALSE;
        src1++;
        src2++;
    }
    return TRUE;
}

int strlen(const char *str){
    for(int c=0; ; c++) if(str[c] == '\0') return c;
}

unsigned char inportb (unsigned short _port){
    unsigned char in;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (in) : "dN" (_port));
    return in;
}

void outportb (unsigned short _port, unsigned char _data){
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

DWORD digitCount(DWORD num){
    DWORD n, c = 0;
    if (num == 0) return 1;
    for(n = num; n != 0; n/=10) c++;
    return c;
}

DWORD pow(DWORD m, DWORD n){
    DWORD out = 1;
    for(DWORD i = 0; i < n; i++){
        out *= m;
    }
    return out;
}

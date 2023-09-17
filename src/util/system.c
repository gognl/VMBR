#include <types.h>

byte_t *memcpy(byte_t *dest, const byte_t *src, uint32_t count){
    for(int i = 0; i<count; i++){
        dest[i] = src[i];
    }
    return dest;
}

byte_t *memset(byte_t *dest, byte_t val, uint32_t count){
    for(uint32_t i = 0; i<count; i++){
        dest[i] = val;
    }
    return dest;
}

word_t *memsetw(word_t *dest, word_t val, uint32_t count){
    for(int i = 0; i<count; i++){
        dest[i] = val;
    }
    return dest;
}

BOOL memcmp(byte_t *src1, byte_t *src2, uint32_t count){
    for(uint32_t i = 0; i<count; i++){
        if (*src1 != *src2) return FALSE;
        src1++;
        src2++;
    }
    return TRUE;
}

uint32_t strlen(const char_t *str){
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

uint32_t digitCount(uint32_t num){
    uint32_t n, c = 0;
    if (num == 0) return 1;
    for(n = num; n != 0; n/=10) c++;
    return c;
}

uint32_t pow(uint32_t m, uint32_t n){
    uint32_t out = 1;
    for(uint32_t i = 0; i < n; i++){
        out *= m;
    }
    return out;
}

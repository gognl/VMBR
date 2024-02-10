#include <lib/types.h>
#include <lib/instr.h>

__attribute__((section(".vmm"))) byte_t *memcpy(byte_t *dest, const byte_t *src, uint64_t count){
    for(int i = 0; i<count; i++){
        dest[i] = src[i];
    }
    return dest;
}

__attribute__((section(".vmm"))) byte_t *memset(byte_t *dest, byte_t val, uint32_t count){
    for(uint32_t i = 0; i<count; i++){
        dest[i] = val;
    }
    return dest;
}

__attribute__((section(".vmm"))) word_t *memsetw(word_t *dest, word_t val, uint32_t count){
    for(int i = 0; i<count; i++){
        dest[i] = val;
    }
    return dest;
}

__attribute__((section(".vmm"))) BOOL memcmp(byte_t *src1, byte_t *src2, uint32_t count){
    for(uint32_t i = 0; i<count; i++){
        if (*src1 != *src2) return FALSE;
        src1++;
        src2++;
    }
    return TRUE;
}

__attribute__((section(".vmm"))) uint32_t strlen(const char_t *str){
    for(int c=0; ; c++) if(str[c] == '\0') return c;
}

__attribute__((section(".vmm"))) uint64_t digitCount(uint64_t num, uint64_t base){
    uint64_t n, c = 0;
    if (num == 0) return 1;
    for(n = num; n != 0; n/=base) c++;
    return c;
}

__attribute__((section(".vmm"))) uint64_t pow(uint64_t m, uint64_t n){
    uint64_t out = 1;
    for(uint64_t i = 0; i < n; i++){
        out *= m;
    }
    return out;
}

void sleep(){
    for (uint64_t i = 0; i < 0xffffff; i++){
        __pause();
    }
}

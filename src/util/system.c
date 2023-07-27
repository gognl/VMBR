
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

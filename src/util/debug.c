#include <system.h>
#include <debug.h>
#include <types.h>

void putch(unsigned char c){
        outportb(DBG_PORT, c);
}

void puts(unsigned char *s, ...){

    va_list args;
    va_start(args, s);

    int len = strlen(s);
    for(int i = 0; i<len; i++){
        if(s[i] == '%'){
            switch(s[i+1]){
                case 'd': {
                    DWORD num = va_arg(args, DWORD);
                    DWORD digits = digitCount(num);
                    DWORD delimiter = pow(10, digits-1);

                    while(delimiter){
                        putch((num/delimiter)%10 + '0');
                        delimiter /= 10;
                    }
                    break;
                }
                case 'c': {
                    // unsigned char is changed to int when passed to va
                    putch(va_arg(args, int));   
                    break;
                }
            }
            i++;
        } else {
            putch(s[i]);
        }
    }
    va_end(args);
}

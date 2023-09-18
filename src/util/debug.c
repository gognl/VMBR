#include <debug.h>
#include <system.h>
#include <types.h>
#include <instr.h>

void putch(char_t c){
        __outb(DBG_PORT, c);
}

void puts(char_t *s, ...){

    va_list args;
    va_start(args, s);

    int len = strlen(s);
    for(int i = 0; i<len; i++){
        if(s[i] == '%'){
            switch(s[i+1]){
                case 'd': {
                    uint32_t num = va_arg(args, uint32_t);
                    uint32_t digits = digitCount(num);
                    uint32_t delimiter = pow(10, digits-1);

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
                case 'q': {
                    uint64_t num = va_arg(args, uint64_t);
                    uint64_t digits = digitCount(num);
                    uint64_t delimiter = pow(10, digits-1);

                    while(delimiter){
                        putch((num/delimiter)%10 + '0');
                        delimiter /= 10;
                    }
                    break;
                }
                case 'm': {
                    byte_t len = s[i+2]-'0';
                    
                    byte_t *str = va_arg(args, byte_t*);
                    for(byte_t j = 0; j<len; j++){
                        putch(*str);
                        str++;
                    }

                    i++;
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

void vputs(char_t *s, va_list args){

    int len = strlen(s);
    for(int i = 0; i<len; i++){
        if(s[i] == '%'){
            switch(s[i+1]){
                case 'd': {
                    uint32_t num = va_arg(args, uint32_t);
                    uint32_t digits = digitCount(num);
                    uint32_t delimiter = pow(10, digits-1);

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
                case 'q': {
                    uint64_t num = va_arg(args, uint64_t);
                    uint64_t digits = digitCount(num);
                    uint64_t delimiter = pow(10, digits-1);

                    while(delimiter){
                        putch((num/delimiter)%10 + '0');
                        delimiter /= 10;
                    }
                    break;
                }
                case 'm': {
                    byte_t len = s[i+2]-'0';
                    
                    byte_t *str = va_arg(args, byte_t*);
                    for(byte_t j = 0; j<len; j++){
                        putch(*str);
                        str++;
                    }

                    i++;
                    break;
                }
            }
            i++;
        } else {
            putch(s[i]);
        }
    }
}

void LOG_DEBUG(char_t *s, ...){
    #if CURRENT_LOG_LEVEL <= 0
        puts("[DEBUG] ");
        va_list args;
        va_start(args, s);
        vputs(s, args);
        va_end(args);
    #endif 
}

void LOG_INFO(char_t *s, ...){
    #if CURRENT_LOG_LEVEL <= 1
        puts("[INFO] ");
        va_list args;
        va_start(args, s);
        vputs(s, args);
        va_end(args);
    #endif
}

void LOG_ERROR(char_t *s, ...){
    #if CURRENT_LOG_LEVEL <= 2
        puts("[ERROR] ");
        va_list args;
        va_start(args, s);
        vputs(s, args);
        va_end(args);
    #endif
}

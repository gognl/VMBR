#include <lib/debug.h>
#include <lib/util.h>
#include <lib/types.h>
#include <lib/instr.h>
#include <vmm/vmm.h>

static void __attribute__((section(".vmm"))) putch(char_t c){
    __outb(DBG_PORT, c);
}

static void __attribute__((section(".vmm"))) puts(char_t *s, ...){

    va_list args;
    va_start(args, s);

    int len = strlen(s);
    for(int i = 0; i<len; i++){
        if(s[i] == '%'){
            switch(s[i+1]){
                case 'd': {
                    uint32_t num = va_arg(args, uint32_t);
                    uint32_t digits = digitCount(num, 10);
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
                    uint64_t digits = digitCount(num, 10);
                    uint64_t delimiter = pow(10, digits-1);

                    while(delimiter){
                        putch((num/delimiter)%10 + '0');
                        delimiter /= 10;
                    }
                    break;
                }
                case 'x': {
                    puts("0x");
                    uint64_t num = va_arg(args, uint64_t);
                    uint64_t digits = digitCount(num, 16);
                    uint64_t delimeter = pow(16, digits-1);

                    while(delimeter){
                        if ((num/delimeter)%16 > 9){
                            putch((num/delimeter)%16 + 'a' - 10);
                        }
                        else {
                            putch((num/delimeter)%16 + '0');
                        }
                        delimeter /= 16;
                    }
                    break;
                }
                case 'b': {
                    uint64_t num = va_arg(args, uint64_t);
                    putch('|');
                    for (uint8_t bit_id = 0; bit_id < 64; bit_id++){
                        if (num & (1ull<<bit_id))
                            puts("%d|", bit_id);
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

static void __attribute__((section(".vmm"))) vputs(char_t *s, va_list args){
    int len = strlen(s);
    for(int i = 0; i<len; i++){
        if(s[i] == '%'){
            switch(s[i+1]){
                case 'd': {
                    uint32_t num = va_arg(args, uint32_t);
                    uint32_t digits = digitCount(num, 10);
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
                case 'q': { // broken for some reason
                    uint64_t num = va_arg(args, uint64_t);
                    uint64_t digits = digitCount(num, 10);
                    uint64_t delimiter = pow(10, digits-1);

                    while(delimiter){
                        putch((num/delimiter)%10 + '0');
                        delimiter /= 10;
                    }
                    break;
                }
                case 'x': {
                    puts("0x");
                    uint64_t num = va_arg(args, uint64_t);
                    uint64_t digits = digitCount(num, 16);
                    uint64_t delimeter = pow(16, digits-1);

                    while(delimeter){
                        if ((num/delimeter)%16 > 9){
                            putch((num/delimeter)%16 + 'a' - 10);
                        }
                        else {
                            putch((num/delimeter)%16 + '0');
                        }
                        delimeter /= 16;
                    }
                    break;
                }
                case 'b': {
                    uint64_t num = va_arg(args, uint64_t);
                    putch('|');
                    for (uint8_t bit_id = 0; bit_id < 64; bit_id++){
                        if (num & (1ull<<bit_id))
                            puts("%d|", bit_id);
                    }
                    break;
                }
                case 'm': {
                    uint32_t j;
                    for(j = i+2; s[j] != '%'; j++);    // len = s[i+2:j]
                    uint32_t len = 0, d = 1;
                    for(uint32_t k = j-1; k>=i+2; k--, d*=10){
                        len += d*(s[k]-'0');
                    }
                    
                    byte_t *buffer = va_arg(args, byte_t*);
                    for (uint32_t c = 0; c<len; c++){
                        byte_t num = buffer[c];
                        byte_t digits = 2;
                        byte_t delimeter = pow(16, digits-1);

                        while(delimeter){
                            if ((num/delimeter)%16 > 9){
                                putch((num/delimeter)%16 + 'a' - 10);
                            }
                            else {
                                putch((num/delimeter)%16 + '0');
                            }
                            delimeter /= 16;
                        }
                        // putch('.');
                    }

                    i += j-i-1;
                    break;
                }
            }
            i++;
        } else {
            putch(s[i]);
        }
    }
}

void __attribute__((section(".vmm"))) LOG_DEBUG(char_t *s, ...){
    #if CURRENT_LOG_LEVEL <= 0
        AcquireLock(&shared_cores_data.puts_lock);
        puts("\033[35m[DEBUG|%d] ", (dword_t)get_current_core_id());
        va_list args;
        va_start(args, s);
        vputs(s, args);
        va_end(args);
        puts("\x1b[0m");
        ReleaseLock(&shared_cores_data.puts_lock);
    #endif 
}

void __attribute__((section(".vmm"))) LOG_INFO(char_t *s, ...){
    #if CURRENT_LOG_LEVEL <= 1
        AcquireLock(&shared_cores_data.puts_lock);
        puts("\e[36m[INFO|%d]  ", (dword_t)get_current_core_id());
        va_list args;
        va_start(args, s);
        vputs(s, args);
        va_end(args);
        puts("\e[0m");
        ReleaseLock(&shared_cores_data.puts_lock);
    #endif
}

void __attribute__((section(".vmm"))) LOG_ERROR(char_t *s, ...){
    #if CURRENT_LOG_LEVEL <= 2
        AcquireLock(&shared_cores_data.puts_lock);
        puts("\e[31m[ERROR|%d] ", (dword_t)get_current_core_id());
        va_list args;
        va_start(args, s);
        vputs(s, args);
        va_end(args);
        puts("\e[0m");
        __hlt();
        ReleaseLock(&shared_cores_data.puts_lock);
    #endif
}

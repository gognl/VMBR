#include <system.h>
#include <debug.h>

unsigned short *ptr;
int csr_x = 0, csr_y = 0;

void scroll(void){
    if (csr_y >= SCR_HEIGHT){
        memcpy(ptr, PIXEL_ADDRESS(ptr, 0, 1), (SCR_HEIGHT-1)*SCR_WIDTH);
        memsetw(PIXEL_ADDRESS(ptr, 0, SCR_HEIGHT-1), PIXEL(BLUE, BLACK, BLANK), SCR_WIDTH);
        csr_y = SCR_HEIGHT-1;
    }
}

void cls(void){

    for(csr_x = 0; csr_x < SCR_WIDTH; csr_x++){
        for(csr_y = 0; csr_y < SCR_HEIGHT; csr_y++){
            *PIXEL_ADDRESS(ptr, csr_x, csr_y) = PIXEL(BLUE, BLACK, BLANK);
        }
    }

    csr_x = 0, csr_y = 0;
}

void putch(unsigned char c){

    if (csr_x >= SCR_WIDTH){
        csr_x = 0;
        csr_y++;
    }

    if (c == '\n'){
        csr_x = 0;
        csr_y++;
    }
    else if (c == '\t'){
        csr_x = (csr_x + 8) & ~(8 - 1);
    }
    else {
        *PIXEL_ADDRESS(ptr, csr_x, csr_y) = PIXEL(BLUE, WHITE, c);
        csr_x++;
    }

    scroll();
}

void puts(unsigned char *s){
    int len = strlen(s);
    for(int i = 0; i<len; i++){
        putch(s[i]);
    }
}

void init_video(void){
    ptr = (unsigned short *)0xB8000;
    cls();
}

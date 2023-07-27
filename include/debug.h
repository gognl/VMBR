#ifndef __DEBUG_H
#define __DEBUG_H

extern void scroll(void);
extern void cls(void);
extern void putch(unsigned char c);
extern void puts(unsigned char *s);
extern void init_video(void);

#define SCR_WIDTH 80
#define SCR_HEIGHT 25

#define BLANK 0x20
enum _color{BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GREY, DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, LIGHT_BROWN, WHITE};

#define PIXEL_ADDRESS(ptr, x, y) (ptr+SCR_WIDTH*y+x)
#define PIXEL(back, fore, chr) ((back << 12) | (fore << 8) | chr)

#endif
#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

void fb_move_cursor(unsigned int x, unsigned int y);
void fb_scroll();
void fb_write_char(char c);
int write(char *string, unsigned int len);
void clear_screen();

#endif /* INCLUDE_FRAMEBUFFER_H */

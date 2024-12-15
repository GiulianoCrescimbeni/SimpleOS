#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);
void fb_move_cursor(unsigned short pos);
void clear_screen();
int write(char *string, unsigned int len);

#endif /* INCLUDE_FRAMEBUFFER_H */

#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

void keyboard_interrupt_handler();
void keyboard_push_char(char c);
char keyboard_get_char();

#endif
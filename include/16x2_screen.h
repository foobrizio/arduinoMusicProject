#ifndef SCREEN_H
#define SCREEN_H

#include <avr/io.h>

void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_send_nibble(uint8_t nibble, uint8_t rs);
void lcd_send_byte(uint8_t byte, uint8_t rs);
void lcd_init(void);
void lcd_write_word(char *word, bool up);


#endif
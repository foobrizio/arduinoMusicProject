#include <avr/io.h>

void i2c_init();
void i2c_start();
void i2c_stop();
void i2c_write(uint8_t data);
void i2c_set_dest_address(uint8_t addr);

void lcd_send_nibble(uint8_t nibble, uint8_t rs);
void lcd_send_byte(uint8_t byte, uint8_t rs);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_init();
void lcd_write_word(char* word, bool up);

void screen_init();
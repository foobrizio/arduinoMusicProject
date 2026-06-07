#include "screen.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include "screen_consts.cpp"
#include <util/delay.h>
#include <string.h>

void i2c_init() {
  TWSR = 0x00;                      // Imposta lo TW state e TWPS a 0
  TWBR = 72;                        // Imposta frequenza I2C a circa 100kHz
}

void i2c_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop() {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while (TWCR & (1 << TWSTO));      // aspetta che STOP sia completato
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));   // aspetta che la trasmissione sia completata
}

void i2c_set_dest_address(uint8_t addr) {
    i2c_write(addr);
}

// --- LCD Functions ---
void lcd_send_nibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = 0;

    data |= (nibble & 0x0F) << 4;     // D4-D7 on P4-P7
    if (rs) data |= (1 << 0);         // RS → P0
    data |= (1 << 3);                 // Backlight → P3

    data |= (1 << 2);                 // EN high → P2
    i2c_start();
    i2c_set_dest_address(SCREEN_ADDRESS << 1);
    i2c_write(data);
    _delay_us(1);

    data &= ~(1 << 2);                // EN low → P2
    i2c_write(data);
    i2c_stop();
    _delay_us(50);
}

void lcd_send_byte(uint8_t byte, uint8_t rs) {
    lcd_send_nibble(byte >> 4, rs);   // high nibble
    lcd_send_nibble(byte & 0x0F, rs); // low nibble
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr = col + (row == 0 ? 0x00 : 0x40);
    lcd_send_byte(0x80 | addr, 0);
}

void lcd_init() {
    _delay_ms(50);                    // tempo di accensione

    // inizializzazione forzata modalità 4 bit
    lcd_send_nibble(0x03, 0);
    _delay_ms(5);
    lcd_send_nibble(0x03, 0);
    _delay_us(150);
    lcd_send_nibble(0x03, 0);
    _delay_us(150);
    lcd_send_nibble(0x02, 0);         // modalità 4 bit
    _delay_us(150);

    lcd_send_byte(0x28, 0);           // 4 bit, 2 righe, 5x8 font
    lcd_send_byte(0x0C, 0);           // display on, cursore off
    lcd_send_byte(0x06, 0);           // entry mode
    lcd_send_byte(0x01, 0);           // clear display
    _delay_ms(2);
}

/** 
 * Scrive una parola sulla riga superiore o inferiore, in base al valore del booleano
 */
void lcd_write_word(const char* word, bool up) {
  if(up){
    lcd_set_cursor(0,0);
  } else {
    lcd_set_cursor(1,0);
  }
  int max = strlen(word);
  if(strlen(word) < 16)
    max = 16;
  for(int i = 0; i< max; i++){
    char x = ' ';
    if(i<strlen(word))
       x = word[i];
    lcd_send_byte(x, i+1);
  }
  _delay_ms(50);
}

void lcd_write_word_centered(const char* word, bool up) {
  int len = strlen(word);
  int start_col = (16 - len) / 2;
  if(up){
    lcd_set_cursor(0, 0);
  } else {
    lcd_set_cursor(1, 0);
  }
  // Clear the line before writing
  for(int i = 0; i< start_col; i++){
    lcd_send_byte(' ', 1);
  }
  // Write the word
  for(int i = 0; i< len; i++){
    lcd_send_byte(word[i], 1);
  }
  // Clear the rest of the line
  for(int i = start_col + len; i< 16; i++){
    lcd_send_byte(' ', 1);
  }
  _delay_ms(50);
}

void lcd_erase_line(bool up) {
  lcd_write_word("               ", up); // 16 spazi per cancellare la riga
}

void screen_init() {
  i2c_init();
  lcd_init();
}
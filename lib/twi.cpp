#include <avr/io.h>


#define LED_PORT PORTB
#define LED_PIN  PB5

void twi_init() {
    TWSR = 0x00; // Imposta lo TW state e TWPS a 0
    TWBR = 72; // Imposta frequenza I2C a circa 100kHz
}

void led_toggle() {
    LED_PORT ^= (1 << LED_PIN); // inverte lo stato del LED
}

void twi_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void twi_stop() {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while (TWCR & (1 << TWSTO)); // aspetta che STOP sia completato
}

void twi_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void twi_set_dest_address(uint8_t addr) {
    twi_write(addr);
}
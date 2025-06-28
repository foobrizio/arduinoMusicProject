#ifndef TWI_H
#define TWI_H

#include <avr/io.h>

// LED macros
#define LED_PORT PORTB
#define LED_PIN  PB5

// Funzioni TWI/I2C
void twi_init(void);
void twi_start(void);
void twi_stop(void);
void twi_write(uint8_t data);
void twi_set_dest_address(uint8_t addr);

// Funzione LED
void led_toggle(void);

#endif // TWI_H
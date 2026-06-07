#include <avr/io.h>

void sound_init();
void sound_start();
void sound_stop();

void set_frequency(uint16_t frequency);
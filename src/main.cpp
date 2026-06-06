#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "lib/util/constants.h"
#include "lib/screen/screen.h"
#include "lib/buttons/buttons.h"
#include "lib/metronome/metronome.h"
#include "lib/sound/sound.h"
#include "lib/tuner/tuner.h"
#include "lib/theremin/theremin.h"

#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PB5


void led_init() {
    LED_DDR |= (1 << LED_PIN);        // imposta PB5 come output
}

void led_toggle() {
    LED_PORT ^= (1 << LED_PIN);        // inverte lo stato del LED
}


int main(void){
  led_init();
  screen_init();
  buttons_init();
  metronome_init();
  tuner_init();
  theremin_init();
  sound_init();

  while(1){
    led_toggle();
    _delay_ms(1000);
  }
}
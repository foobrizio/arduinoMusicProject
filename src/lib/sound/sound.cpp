#include <avr/io.h>
#include <avr/interrupt.h>

#define OUTPUT_PIN PB1
#define DEFAULT_FREQUENCY 440


uint16_t current_frequency = DEFAULT_FREQUENCY;

void set_frequency(uint16_t frequency){
    current_frequency = frequency;
}

void sound_init(){
    set_frequency(DEFAULT_FREQUENCY);
    // Configura il pin come output e il timer in modalità CTC
    DDRB |= (1 << OUTPUT_PIN); // Imposta PB1 come output   
    TCCR1A = 0; // Modalità CTC, pin OC1A disconnesso
    TCCR1B = (1 << WGM12) | (1 << CS11); // CTC, prescaler 8
}

void sound_start(){
    OCR1A = F_CPU / (2 * 8 * current_frequency) - 1; // prescaler 8
    TIMSK1 |= (1 << OCIE1A);  // abilita interrupt su compare match
}

void sound_stop(){
    TIMSK1 &= ~(1 << OCIE1A); // disabilita interrupt su compare match
    OCR1A  = 0;
    TCNT1  = 0;                // reset contatore
    PORTB &= ~(1 << OUTPUT_PIN); // spegne il suono
}

ISR(TIMER1_COMPA_vect) {
    PORTB ^= (1 << OUTPUT_PIN);  // toggle manuale di PB0
}

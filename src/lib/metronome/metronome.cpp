#include <avr/io.h>
#include <avr/interrupt.h>
#include "../util/util.h"
#include <lib/screen/screen.h>
#include "lib/sound/sound.h"
#include "lib/util/constants.h"
#include "lib/buttons/buttons.h"
#include <math.h> // usiamo round() per calcolare ticks_target in modo più preciso

#define MAX_BPM 300
#define MIN_BPM 40
#define STD_BPM 120

#define TICK_MS   (256.0 * 1024.0 / F_CPU * 1000.0)  // ~16.4ms

// è necessario usare uint16 perchè il numero di BPM può superare i 255 (valore max a 8 bit)
uint16_t bpm = STD_BPM;

volatile uint16_t  tick_count   = 0;
volatile uint16_t  ticks_target = 0;
volatile uint16_t  beat_state   = 0;  // 0=suono spento, 1=suono acceso

/*
 * Il metronomo utilizza il Timer0 per generare un interrupt ogni TICK_MS millisecondi.
 */
char* get_bpm(){
    static char bpm_str[4];
    uint16_to_str(bpm, bpm_str);
    static char buf[20];
    str_concat(buf, bpm_str, " BPM");
    return buf;
}

void metronome_init(){
    bpm = STD_BPM;
    // mezzo periodo in ms = (60000 / bpm) / 2
    // numero di tick = mezzo periodo / TICK_MS
    ticks_target = (uint16_t)round((60000.0 / bpm / 2.0) / TICK_MS);

    // Timer0 CTC, prescaler 1024
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS02) | (1 << CS00);
    OCR0A  = 255;
}

void metronome_start(){
    sound_init(); // Resetta la default frequency al LA4.
    enable_press_hold();
    TIMSK0 |= (1 << OCIE0A);
    char* bpm = get_bpm();
    lcd_write_word_centered(bpm, false);
    sound_start();
}

void metronome_stop(){
    TIMSK0 &= ~(1 << OCIE0A);
    disable_press_hold();
    sound_stop();
}


void set_bpm(uint16_t new_bpm){
    bpm = new_bpm;
    cli();
    ticks_target = (uint16_t)round((60000.0 / bpm / 2.0) / TICK_MS);
    sei();
    static char bpm_str[4];
    uint16_to_str(bpm, bpm_str);
    static char buf[20];
    str_concat(buf, bpm_str, " BPM");
    lcd_write_word_centered(buf, false);
}


void increase_bpm(){
    if(bpm < MAX_BPM){
        bpm++;
    }
    set_bpm(bpm);
}
void decrease_bpm(){
    if(bpm > MIN_BPM){
        bpm--;
    }
    set_bpm(bpm);
}

void metronome_left_button_pressed(){
    decrease_bpm();
    
}

void metronome_right_button_pressed(){
    increase_bpm();
}

void metronome_central_button_press_hold(){
    // Resettiamo i bpm a 120 quando si tiene premuto il tasto centrale
    set_bpm(STD_BPM);
}

ISR(TIMER0_COMPA_vect) {
    tick_count++;
    if (tick_count >= ticks_target) {
        tick_count = 0;
        if (beat_state == 0) {
            sound_start();
            beat_state = 1;
        } else {
            sound_stop();
            beat_state = 0;
        }
    }
}

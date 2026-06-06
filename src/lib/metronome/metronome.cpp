#include <avr/io.h>
#include <avr/interrupt.h>
#include "../util/util.h"
#include <lib/screen/screen.h>
#include "lib/sound/sound.h"
#include "lib/util/constants.h"
#include <math.h> // usiamo round() per calcolare ticks_target in modo più preciso

#define MAX_BPM 300
#define MIN_BPM 40
#define STD_BPM 120

#define TICK_MS   (256.0 * 1024.0 / F_CPU * 1000.0)  // ~16.4ms

uint8_t bpm = STD_BPM;

volatile uint8_t  tick_count   = 0;
volatile uint8_t  ticks_target = 0;
volatile uint8_t  beat_state   = 0;  // 0=suono spento, 1=suono acceso

char* get_bpm(){
    static char bpm_str[4];
    uint8_to_str(bpm, bpm_str);
    static char buf[20];
    str_concat(buf, bpm_str, " BPM");
    return buf;
}

void metronome_init(){
    bpm = STD_BPM;
    // mezzo periodo in ms = (60000 / bpm) / 2
    // numero di tick = mezzo periodo / TICK_MS
    ticks_target = (uint8_t)round((60000.0 / bpm / 2.0) / TICK_MS);

    // Timer0 CTC, prescaler 1024
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS02) | (1 << CS00);
    OCR0A  = 255;
}

void metronome_start(){
    TIMSK0 |= (1 << OCIE0A);
    char* bpm = get_bpm();
    lcd_write_word_centered(bpm, false);
    sound_start();
}

void metronome_stop(){
    TIMSK0 &= ~(1 << OCIE0A);
    sound_stop();
}



char* increase_bpm(){
    if(bpm < MAX_BPM){
        bpm++;
        cli();
        ticks_target = (uint8_t)round((60000.0 / bpm / 2.0) / TICK_MS);
        sei();
    }
    static char bpm_str[4];
    uint8_to_str(bpm, bpm_str);
    static char buf[20];
    str_concat(buf, bpm_str, " BPM");
    return buf;
}

char* decrease_bpm(){
    if(bpm > MIN_BPM){
        bpm--;
        cli();
        ticks_target = (uint8_t)round((60000.0 / bpm / 2.0) / TICK_MS);
        sei();
    }

    static char bpm_str[4];
    uint8_to_str(bpm, bpm_str);
    static char buf[20];
    str_concat(buf, bpm_str, " BPM");
    return buf;
}

void metronome_left_button_pressed(){
    char* bpm = decrease_bpm();
    lcd_write_word_centered(bpm, false);
}

void metronome_right_button_pressed(){
    char* bpm = increase_bpm();
    lcd_write_word_centered(bpm, false);
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

#include <avr/io.h>
#include "../util/util.h"
#include <lib/screen/screen.h>

#define MAX_BPM 240
#define MIN_BPM 40
#define STD_BPM 120

uint8_t bpm = STD_BPM;
void metronome_init(){
    bpm = STD_BPM;
}

char* get_bpm(){
    static char bpm_str[4];
    uint8_to_str(bpm, bpm_str);
    static char buf[20];
    str_concat(buf, bpm_str, " BPM");
    return buf;
}

char* increase_bpm(){
    if(bpm < MAX_BPM)
        bpm++;
    static char bpm_str[4];
    uint8_to_str(bpm, bpm_str);
    static char buf[20];
    str_concat(buf, bpm_str, " BPM");
    return buf;
}

char* decrease_bpm(){
    if(bpm > MIN_BPM)
        bpm--;
    static char bpm_str[4];
    uint8_to_str(bpm, bpm_str);
    static char buf[20];
    str_concat(buf, bpm_str, " BPM");
    return buf;
}

void metron_central_button_pressed(){
    // per ora non fa nulla, ma in futuro potrebbe servire per avviare/fermare il metronomo
    char* bpm = get_bpm();
    lcd_write_word_centered(bpm, false);
}

void metron_left_button_pressed(){
    char* bpm = decrease_bpm();
    lcd_write_word_centered(bpm, false);
}

void metron_right_button_pressed(){
    char* bpm = increase_bpm();
    lcd_write_word_centered(bpm, false);
}

#include "lib/sound/pitches.h"
#include <lib/sound/sound.h>
#include <lib/screen/screen.h>
#include <lib/buttons/buttons.h>
#include <avr/io.h>
#include <lib/util/util.h>
#include "tunings.h"

/*
 * Questo accordatore funziona con le seguenti accordature:
 * 1) Guitar Standard tuning (E2 A2 D3 G3 B3 E4)
 * 2) Half step down tuning (Eb2 Ab2 Db3 Gb3 Bb3 Eb4)
 * 3) Guitar Drop D tuning (D2 A2 D3 G3 B3 E4)
 * 4) Guitar Open G tuning (D2 G2 D3 G3 B3 D4)
 * 5) Guitar Open D tuning (D2 A2 D3 F#3 A3 D4)
 * 6) Guitar Open E tuning (E2 B2 E3 G#3 B3 E4)
 * 7) Bass Standard tuning (E1 A1 D2 G2)
 * 8) Bass Drop D tuning (D1 A1 D2 G2)
 * 9) Bass 5-string tuning (B0 E1 A1 D2 G2)
 * 10) Violin Standard tuning (G3 D4 A4 E5)
 * 11) Ukulele Standard tuning (G4 C4 E4 A4)
 */ 

uint8_t chosen_tune = 0; // Valore da 0 a 11 che indica l'accordatura scelta

uint8_t current_string = 0;

void tuner_init(){
    set_frequency(TUNINGS[chosen_tune].strings[current_string]);
    current_string = 0;
}

void change_tune(){
    chosen_tune = (chosen_tune + 1) % NUM_TUNINGS; // Cambia accordatura ciclicamente
    current_string = 0; // Resetta la corda corrente alla prima
}

void write_note_and_tune(){
    const Tuning& tuning = TUNINGS[chosen_tune];
    const char* tune_name = tuning.label;
    const char* note_name = tuning.names[current_string];
    char label[16];
    str_concat(label, tune_name, " - ");
    str_concat(label, label, note_name);
    lcd_write_word_centered(label, false);
}

void tuner_start(){
    set_frequency(TUNINGS[chosen_tune].strings[current_string]);
    enable_press_hold();
    write_note_and_tune();
    sound_start();
}

void tuner_stop(){
    sound_stop();
    disable_press_hold();
}

void tuner_left_button_pressed(){
    sound_stop();
    if(current_string > 0){
        current_string--;
    }
    set_frequency(TUNINGS[chosen_tune].strings[current_string]);
    write_note_and_tune();
    sound_start();
}

void tuner_right_button_pressed(){
    sound_stop();
    const Tuning& tuning = TUNINGS[chosen_tune];
    if(current_string < tuning.num_strings - 1){
        current_string++;
    }
    set_frequency(tuning.strings[current_string]);
    write_note_and_tune();
    sound_start();
}

void tuner_central_button_press_hold(){
    sound_stop();
    change_tune();
    set_frequency(TUNINGS[chosen_tune].strings[current_string]);
    write_note_and_tune();
    sound_start();
}





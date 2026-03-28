#include "buttons.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../screen/screen.h"
#include <string.h>
#include "../metronome/metronome.h"
# include "../util/util.h"
#include <lib/tuner/tuner.h>
#include <lib/theremin/theremin.h>

char menu_items[3][12] = {"metronomo", "accordatore", "theremin"};
uint8_t current_menu_index = 0;
char current_mode[12] = "menu";

#define INTERRUPT_IN  PD2
#define LEFT_BTN      PD7
#define CENTRAL_BTN   PD6
#define RIGHT_BTN     PD5

/*
 1) Il bottone di sinistra serve per tornare indietro nel menu. Ha comandi diversi poi in base allaa modalità selezionata.
 2) Il bottone centrale, se siamo nella scelta del menu, seleziona l'opzione. Una volta selezionata, ripremere il bottone per tornare alla selezione del menu.
 3) Il bottone di destra serve per andare avanti nel menu. Ha comandi diversi poi in base alla modalità selezionata.
 */

//-- BUTTON FUNCTIONS --
void enable_button_interrupt(){
  DDRD = (0 <<INTERRUPT_IN);                                // set the interrupt pins as input port
  EIMSK = (1<<INT0);                                        // set the interrupt mask for INT0
}

void disable_button_interrupt(){
  EIMSK = 0x00;                                             // disable all external interrupts 
}

void buttons_init(){
  DDRD = (1<<RIGHT_BTN)|(1<<CENTRAL_BTN)|(1<<LEFT_BTN); 	  // set portC as input port for the three different buttons
  EICRA = (1<<ISC10)|(1<<ISC00);  		                      // set the interrupt for each logical change of INT0 and INT1
  enable_button_interrupt();
  lcd_write_word_centered("menu", true);
  lcd_write_word(menu_items[current_menu_index], false);
  sei();                                                    // enable global interrupts
}

void handle_right_button(){
  if(strcmp(current_mode, "menu") != 0){
    if(strcmp(current_mode, "metronomo") == 0){
      //siamo nel menu metronomo, questo pulsante serve per velocizzare il tempo
      metron_right_button_pressed();
    }
    else if (strcmp(current_mode, "accordatore") == 0){
      //siamo nel menu accordatore
      tuner_right_button_pressed();
    }
    else if (strcmp(current_mode, "theremin") == 0){
      //siamo nel menu theremin
      theremin_right_button_pressed();
    }
    return;
  }
  if(current_menu_index < 2)
    current_menu_index++;
  lcd_write_word(menu_items[current_menu_index], false);
}

void handle_central_button(){
  if(strcmp(current_mode, "menu") == 0){
    // Se siamo nel menu, seleziona la modalità
    strcpy(current_mode, menu_items[current_menu_index]);
    lcd_erase_line(false);
    if(strcmp(current_mode, "metronomo") == 0){
      //siamo nel menu metronomo
      metron_central_button_pressed();
    }
    else if(strcmp(current_mode, "accordatore") == 0){
      //siamo nel menu accordatore
      tuner_central_button_pressed();
    }
    else if(strcmp(current_mode, "theremin") == 0){
      //siamo nel menu theremin
      theremin_central_button_pressed();
    }
  }
  else{
    strcpy(current_mode, "menu");
    lcd_write_word(menu_items[current_menu_index], false);
  }
  lcd_write_word_centered(current_mode, true);
}

void handle_left_button(){
  if(strcmp(current_mode, "menu") != 0){
    if(strcmp(current_mode, "metronomo") == 0){
      //siamo nel menu metronomo, questo pulsante serve per rallentare il tempo
      metron_left_button_pressed();
    }
    else if(strcmp(current_mode, "accordatore") == 0){
      //siamo nel menu accordatore
      tuner_left_button_pressed();
    }
    else if(strcmp(current_mode, "theremin") == 0){
      //siamo nel menu theremin
      theremin_left_button_pressed();
    }
    return;
  }
  if(current_menu_index > 0)
    current_menu_index--;
  lcd_write_word(menu_items[current_menu_index], false);
}


ISR(INT0_vect) {
  if(PIND & (1 << 2)){
    if(PIND & (1 << 5)){
      handle_right_button();
    }
    else if(PIND &  (1 << 6)){
      handle_central_button();
    }
    else if(PIND & (1 << 7)){
      handle_left_button();
    }
    else{
      //lcd_write_word("nulla", true);
    }
  }
  sei();
}
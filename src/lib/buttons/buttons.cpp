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
static volatile uint8_t just_entered = 0; // Diventa 1 quando si entra in una modalità, per evitare di interpretare un press&hold come un normale click

#define INTERRUPT_IN  PD2
#define LEFT_BTN      PD7
#define CENTRAL_BTN   PD6
#define RIGHT_BTN     PD5

#define LONG_PRESS_MS        800UL
#define TIMER2_TICK_MS       16UL
#define LONG_PRESS_OVERFLOWS (LONG_PRESS_MS / TIMER2_TICK_MS)

static volatile uint8_t press_hold_enabled = 0; // Diventa 1 quando la funzionalità di press&hold è abilitata
static volatile uint8_t pressed_button   = 0;
static volatile uint8_t press_hold_reached = 0; // Diventa 1 quando si raggiunge il tempo di press&hold
static volatile uint8_t overflow_count   = 0;


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
      metronome_right_button_pressed();
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
    just_entered = 1;
    if(strcmp(current_mode, "metronomo") == 0){
      //siamo nel menu metronomo
      metronome_start();
    }
    else if(strcmp(current_mode, "accordatore") == 0){
      //siamo nel menu accordatore
      tuner_start();
    }
    else if(strcmp(current_mode, "theremin") == 0){
      //siamo nel menu theremin
      theremin_start();
    }
  }
  else{
    if(strcmp(current_mode, "metronomo") == 0){
      metronome_stop();
    }
    else if(strcmp(current_mode, "accordatore") == 0){
      tuner_stop();
    }
    else if(strcmp(current_mode, "theremin") == 0){
      theremin_stop();
    }
    strcpy(current_mode, "menu");
    lcd_write_word(menu_items[current_menu_index], false);
  }
  lcd_write_word_centered(current_mode, true);
}

void handle_press_hold_central_button(){
  if(strcmp(current_mode, "menu") != 0){
    if(strcmp(current_mode, "metronomo") == 0){
      metronome_central_button_press_hold();
    }
    else if(strcmp(current_mode, "accordatore") == 0){
      tuner_central_button_press_hold();
    }
    else if(strcmp(current_mode, "theremin") == 0){
      //siamo nel menu theremin

    }
  }
}

void handle_left_button(){
  if(strcmp(current_mode, "menu") != 0){
    if(strcmp(current_mode, "metronomo") == 0){
      //siamo nel menu metronomo, questo pulsante serve per rallentare il tempo
      metronome_left_button_pressed();
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

// --PRESS&HOLD BUTTON--
void enable_press_hold() {
  press_hold_enabled = 1;
  press_hold_reached = 0;
  TCCR2A = 0;                                          // modalità normale (overflow)
  TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);  // prescaler 1024
  TIMSK2 = 0;                                          // interrupt ancora disabilitati
  TCNT2  = 0;
}

// Rilascia Timer2 completamente, così altri moduli possono usarlo
void disable_press_hold() {
  press_hold_enabled=0;
  TCCR2A = 0;
  TCCR2B = 0;                                          // ferma il clock del timer
  TIMSK2 = 0;                                          // disabilita tutti gli interrupt
  TCNT2  = 0;
  pressed_button   = 0;
  press_hold_reached = 0;
  overflow_count   = 0;
}

// Avvia il conteggio (abilita interrupt overflow)
static void timer2_start(void) {
  overflow_count = 0;
  TCNT2  = 0;
  TIMSK2 = (1 << TOIE2);                              // abilita interrupt overflow
}

// Ferma il conteggio (disabilita solo interrupt, configurazione intatta)
static void timer2_stop(void) {
  TIMSK2 = 0;
  overflow_count = 0;
  TCNT2  = 0;
}


ISR(INT0_vect) {
  if(PIND & (1 << INTERRUPT_IN)){
    // fronte di salita
    if(PIND & (1 << RIGHT_BTN)){
      pressed_button = RIGHT_BTN;
      handle_right_button();
    }
    else if(PIND &  (1 << CENTRAL_BTN)){
      pressed_button = CENTRAL_BTN;
      if(press_hold_enabled){
        press_hold_reached = 0;
        timer2_start();
      }
      else {
        handle_central_button();
      }
    }
    else if(PIND & (1 << LEFT_BTN)){
      pressed_button = LEFT_BTN;
      handle_left_button();
    }
    
  } else {
    // fronte di discesa
    if(press_hold_enabled && !just_entered){
      if(pressed_button == CENTRAL_BTN){
        if(press_hold_reached){
          handle_press_hold_central_button();
        }
        else{
          handle_central_button();
        }
        press_hold_reached = 0;
      }
    }
    just_entered = 0;
  }
  sei();
}

ISR(TIMER2_OVF_vect) {
    overflow_count++;
    if (overflow_count >= LONG_PRESS_OVERFLOWS) {
        timer2_stop();
        press_hold_reached = 1;
    }
    sei();
}
#include "buttons.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../screen/screen.h"


//-- BUTTON FUNCTIONS --
void enable_button_interrupt(){
  DDRD = (0 <<PD2);                   // set the interrupt pins as input port
  EIMSK = (1<<INT0);                  // set the interrupt mask for INT0
}

void disable_button_interrupt(){
  EIMSK = 0x00;                       // disable all external interrupts 
}

void buttons_init(){
  DDRB = (1<<PB0)|(1<<PB1)|(1<<PB2); 	// set portB as input port for the three different buttons
  EICRA = (1<<ISC10)|(1<<ISC00);  		// set the interrupt for each logical change of INT0 and INT1
  enable_button_interrupt();
  sei();                              // enable global interrupts
}


ISR(INT0_vect) {
  if(PIND & (1 << 2)){
    if(PINB & (1 << 0)){
      lcd_write_word("blu", true);
    }
    else if(PINB &  (1 << 1)){
      lcd_write_word("rosso", true);
    }
    else if(PINB & (1 << 2)){
      lcd_write_word("verde", true);
    }
    else{
      lcd_write_word("nulla", true);
    }
  }
  else{
    lcd_write_word("", true);
  }
  sei();
}
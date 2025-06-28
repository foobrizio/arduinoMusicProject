#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <twi.h>
#include <16x2_screen.h>


#define LED_DDR  DDRB
#define LED_PIN  PB5

//-- BUTTON FUNCTIONS --
void enable_button_interrupt(){
  DDRD = (1<<PD2);           // set the interrupt pins as input port
  EIMSK = (1<<INT0);        // set the interrupt mask for INT0
}

void disable_button_interrupt(){
  EIMSK = 0x00;
}

void setup_button(){
  DDRB = (1<<PB0)|(1<<PB1)|(1<<PB2); 	// set portB as input port for the three different buttons
  EICRA = (1<<ISC10)|(1<<ISC00);  		// set the interrupt for each logical change of INT0 and INT1
  enable_button_interrupt();
  sei();                              	// enable global interrupts
}

void led_init() {
    LED_DDR |= (1 << LED_PIN); // imposta PB5 come output
}


int main(void){
  led_init();
  twi_init();
  lcd_init();
  setup_button();

  lcd_write_word("accordatore", false);
  while(1){
    led_toggle();
    _delay_ms(1000);
  }
}

ISR(INT0_vect) {
  if(PIND & (1 << 2)){
    if(PINB & (1 << 0)){
      lcd_write_word("viola", true);
    }
    else if(PINB &  (1 << 1)){
      lcd_write_word("rosa", true);
    }
    else if(PINB & (1 << 2)){
      lcd_write_word("arancione", true);
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


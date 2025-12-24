#include <avr/io.h>
#include <avr/interrupt.h>
#include "project.h"
#include "timers.h"


void timer1_init(void)
{
    TCCR1A = 0; //normal mode
    TCCR1B = 0; //off
    TCNT1 = 0; //reset count
    //overflow interrupt of ISR(TIMER1_OVF_vect)
    TIMSK1 |= (1 << TOIE1); //copy from old labs
}

ISR(TIMER1_OVF_vect)
{
    if(timing == 1){
        timing =0;
        timeout = 1; //set timeout flag
        TCCR1B = 0; //stop timer
        PORTD &= ~(1 << PD1); //turn off timing LED
    }
 
}

void timer2_init(void)
{
    TCCR2A |= (0b11 << WGM20);  // Fast PWM mode, modulus = 256
    TCCR2A |= (0b10 << COM2A0); // Turn D11 on at 0x00 and off at OCR2A
    OCR2A = 23;        // Initial pulse width (calculate this)
    TCCR2B |= (0b111 << CS20);  // Prescaler = 1024 for 16ms period
    DDRB |= (1 << PB3);  // Set D11 (OC2A) as output
   
}
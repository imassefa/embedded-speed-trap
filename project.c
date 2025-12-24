/********************************************
*
*  Name: isabella assefa
*  Email: iassefa@usc.edu
*  Section: 11am friday
*  Assignment: Lab 7 - ADC and PWM
*
********************************************/

#include "project.h"
#include "lcd.h"
#include "encoder.h"
#include "timers.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "lcd.h"


//#include "adc.h"
//void timer1_init(void);
//void timer2_init(void);
void pin_chnge_enabler(void);
volatile unsigned char prev_start =1; //previous state of start sensor
volatile unsigned char prev_stop =1;  //previous state of stop sensor
volatile unsigned char timing = 0; //timing to lmk
volatile unsigned char timeout = 0;
volatile unsigned int count = 0; // count to store timer1 count tcnt1 when stop
volatile unsigned int msConv = 0; //ms conversion
volatile unsigned int speed; //tenth of cm/s
volatile unsigned char newNum = 0; //flag for new speed number to display
volatile unsigned char option = 0; // 0 = low pb5 and 1 = high pb4
volatile unsigned char switchOption = 0; //flag for when it switches from high/low
volatile unsigned char low = 1; //low speed limit num
volatile unsigned char high = 99; //high speed limit num
volatile unsigned char changed = 0; //refreshes the LCD because numbers changed
volatile unsigned char old_state = 0;// previous  state
volatile unsigned char new_state = 0;// current  state
volatile unsigned char pwm_changed = 255; //variable to track if the pwm value has changed
//4 second timeout
//global variabels are moved to project.h

int main(void)
{

    //unsigned char timing = 0; //time counter
    char buf[20];
   // unsigned int msConv = 0; //ms conversion
    // Initialize the LCD
    lcd_init();
    //Servo motor timer2 init 
    timer2_init();
//PD2 as start PD3 as stop
    DDRD &= ~((1 << PD2) | (1 << PD3)); //inputs
    PORTD |= (1 << PD2) | (1 << PD3); //pull-ups enable high
    DDRB &= ~((1 << PB4)| (1 << PB5)); //inputs for cp2
    PORTB |= (1 << PB4) | (1 << PB5); //pull-ups enable high
    DDRC &= ~((1 << PC1) | (1 << PC2)); //rotary encoder inputs
    PORTC |= (1 << PC1) | (1 << PC2); //pullups
    DDRC |= (1 << PC4) | (1 << PC5);    // make them outputs 
    PORTC |= (1 << PC4) | (1 << PC5);   // both high so green is ON at start
    
    //read and sent PC2 and PC3 to both = prevEnc 
    int x = PINC;
    unsigned char a = (x & (1 << PC1)) >> PC1; //read pc1
    unsigned char b = (x & (1 << PC2)) >> PC2; //read pc2

    //merge the hi and low into the states
    if(a == 0 && b ==0){
        old_state = 0;
    }
    else if(a ==0 && b == 1){
        old_state =1;
    }
    else if(a ==1 && b ==1){
        old_state =2;
    }
    else{
        old_state = 3;
    }
    
//PD1 for timing LED
    DDRD |= (1 << PD1); //output   
    PORTD &= ~(1 << PD1); //turn off timing LED
   
    // Initialize TIMER1
    timer1_init();
    //pin change
    pin_chnge_enabler();
    sei(); //enable global interrupts
    // Write splash screen and delay for 1 second
        // Show the splash screen

        //eeprom saving into low and high 
    
    //If you read the EEPROM data and the value is not in the range 1 to 99, 
    //then your program should ignore this number and revert to using default limit values that are defined in your source code
    low = eeprom_read_byte((void*)0); //low for eeprom addy = 0
    high = eeprom_read_byte((void*)1); //high for eeprom addy = 1 
    if(low < 1 || low > 99){
        low = 2; //default low
    } 
    if(high < 1 || high > 99){
        high = 98; //default high
    }
    if (high < low)
   {
    high = low;
   } 
  
    
    lcd_moveto(0,3);
    lcd_stringout("Isabella"); //prints my name
    _delay_ms(1000);
    lcd_writecommand(0x01); //clear the screen


    lcd_moveto(1,0);
    snprintf(buf, 20, ">Low %2u High %2u", low, high);
    lcd_stringout(buf);

 
    
    while (1) {                 // Loop forever
        // Read start and stop sensors
        if(newNum == 1){ //if thres a new number aka new spped then display and run through the loop once again
            //display time and speed
            newNum = 0; //reset new number flag
            unsigned int whole = speed / 10;
            unsigned int frac = speed % 10;
//34 is 0 and 11 is 99, from past lab 

            //servo motor control based on speed 
            unsigned char pwm_value;
            unsigned int var = 23 * whole; //23 is 34 -11
            pwm_value = 34 - (var / 99); //max speed is 99 cm/s min is 1 cm/s so scale it accordingly -linear mapping eq.
           if(pwm_value != pwm_changed){
            OCR2A = pwm_value; //update val if its changed
            pwm_changed = pwm_value; //update the tracker
           }


            //LED indicator for speed limit 
            //1 means off
            //0 means on
            if(whole > high){
                //red  PC4 is on, blue is off PC5
                PORTC |= (1 << PC5); //blue off 
                PORTC &= ~(1 << PC4); //red is on
            } 
            else if(whole < low){
                //blue PC5 is on, red is off PC4
                PORTC |= (1 << PC4); //red off
                PORTC &= ~(1 << PC5); //blue on
            }
            else{
                //both off -> green is on bc 1 and 1 will be 0 
                PORTC |=(1 << PC4); //red off
                PORTC |=(1 << PC5); //blue off
            }


        //display speed on LCD
        lcd_moveto(0,0);
        snprintf(buf, 20, "%ums =%u.%u", msConv, whole, frac);
        lcd_stringout(buf);
      
        lcd_moveto(1,0); //keeps the low/high on the screene tnire time
        if (option == 0) {
            snprintf(buf, 20, ">Low %2u High %2u", low, high);
       } else {
           snprintf(buf, 20, " Low %2u >High %2u", low, high);
   }
   _delay_ms(1000);
}
        /*lcd_moveto(0,0);
        snprintf(buf, 20, "Speed:%u.%u cm/s", whole, frac);
        lcd_stringout(buf); */
      
       
        //4 SECOND TIMEOUT
        if(timeout == 1){
            //timeout occurred
            timeout = 0; //reset timeout
            timing = 0; //reset timing
            PORTD &= ~(1 << PD1); //turn off timing LED
            //display timeout message
            //lcd_writecommand(0x01); //clear the screen
            lcd_moveto(0,0);
            lcd_stringout("TIMEOUT     ");
            _delay_ms(1000);
            //lcd_writecommand(0x01); //clear the screen

            lcd_writecommand(0x01); //fix bc timeout wasnt showing up
            lcd_moveto(1,0);
            if (option == 0) {
                snprintf(buf, 20, ">Low%2u High %2u", low, high);
            } else {
                snprintf(buf, 20, "Low %2u >High %2u", low, high);
            }
            lcd_stringout(buf);
            
        }

        //update low/high on lcd if changed
        if(switchOption ==1 || changed ==1){
            switchOption = 0; //reset
            changed =0; //reset
            lcd_moveto(1,0);
            if(option == 0 ){
                snprintf(buf, 20, ">Low %2u High %2u", low, high);
            } else{
                snprintf(buf, 20, " Low %2u>High %2u", low, high);
            }
            lcd_stringout(buf);
            
        }

    }  return 0;   /* never reached */       

   }


  
/*
  timer1_init
*/


//fix the floating point math to the .1 precision
//fix the pin change interrupts to read the sensors
void pin_chnge_enabler(void)
{
    PCICR |= (1 << PCIE2); //enable pin change interrupt for PORTD
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19); //enable interrupts for PD2 and PD3
    PCICR |= (1 << PCIE0); //enable pin change interrupt for PORTb
   PCMSK0 |= (1 << PCINT4) | (1 << PCINT5); //pb4 pb5
   //enable for pb4 and pb5 for cp2
   PCICR |= (1 << PCIE1); //portc encoder
   PCMSK1 |= (1 << PCINT9) | (1 << PCINT10); //pc2 pc3
}

ISR(PCINT2_vect)
{
    unsigned char start;
    unsigned char stop;
        //read start when blocked and clear
    if((PIND & (1 << PD2)) == 0){
        start = 0; //blocked
    }else{
        start = 1; //clear
}
    //read stop when blocked and clear
    if((PIND & (1 << PD3)) == 0){
        stop = 0; //blocked
    }else{
        stop = 1; //clear
    }


   
    //Check for changes in start sensor
        if(start == 0 && prev_start == 1 && timing == 0){
            //clear to blocked and timer not running falling edge i think
            //start timer1
            timing = 1;
            timeout = 0; //reset timeout flag
            TCNT1 = 0; //reset timer count
            TCCR1B |= (1 << CS12) | (1 << CS10); //start timer with prescaler 1024
            //4 seconds max, w/1024 it incr every 64microseconds and 1000microse (1milisec) /64 mico = 16 counts per ms
            PORTD |= (1 << PD1); //turn on timing LED
        }
        // Check for changes in stop sensor
        if(prev_stop == 1 && stop == 0 && timing == 1){
            //change from clear to blocked and timer running
           
            //stop timer1
            TCCR1B = 0; //stop timer
            timing = 0; //reset timing flag
            PORTD &= ~(1 << PD1); //turn off timing LED
            count = TCNT1; //store timer count
          
          
            //convert to ms
            msConv = count / 16; //each count is 64us with prescaler 1024 at 16MHz to get ms it rounds to 16 when 1000/64 = 15.625          
            //ms to speed in cm/s for the .1 precision
            unsigned int dist = 38; //3.8cm
            //unsigned int speed; //tenth of cm/s
            speed = (dist * 1000) / msConv; //cm/s and speed = d/t and 1 secind =1000ms
            newNum = 1;
            }
            // Update previous sensor states
        prev_start = start;
        prev_stop = stop;
}


ISR(PCINT0_vect){ 
    //read the high and low options to set the speed limit on the lcd. pb4 is high, pb5 is low
    if((PINB & (1 << PB4)) == 0){
       option = 1; //high mode
       switchOption = 1; //update LCD
      }else if((PINB & (1 << PB5)) == 0){
       option = 0; //low
       switchOption  = 1; //update LCD
      }
}
   
      
//no buzzer i dont know how to do that 


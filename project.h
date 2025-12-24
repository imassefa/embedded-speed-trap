#ifndef PROJECT_H
#define PROJECT_H

#include <avr/io.h>
#include <avr/eeprom.h>

extern volatile unsigned char timeout; //flag for the timeout to appear
extern volatile unsigned char timing; //timing to lmk
extern volatile unsigned int count; // count to store timer1 count tcnt1 when stop
extern volatile unsigned int msConv; //ms conversion
extern volatile unsigned int speed; //tenth of cm/s
extern volatile unsigned char newNum; //flag for new speed number to display
extern volatile unsigned char option; // 0 = low pb5 and 1 = high pb4
extern volatile unsigned char switchOption; //flag for when it switches from high/low
extern volatile unsigned char low; //low speed limit num
extern volatile unsigned char high; //high speed limit num
extern volatile unsigned char changed; //refreshes the LCD because numbers changed
extern volatile unsigned char old_state;// previous  state
extern volatile unsigned char new_state;// current  state
extern volatile unsigned char pwm_changed; //variable to track if the pwm value has changed
extern volatile unsigned char prev_start; //previous state of start sensor
extern volatile unsigned char prev_stop;  //previous state of stop sensor
void pin_chnge_enabler(void);

#endif
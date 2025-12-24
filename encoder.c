#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "project.h"
#include "encoder.h"


ISR(PCINT1_vect)
{
    // Read A and B
    unsigned char a = (PINC & (1 << PC1)) >> PC1;
    unsigned char b = (PINC & (1 << PC2)) >> PC2;

    // copied from lab 6 skeleton code 
    if (a == 0 && b == 0)
    {new_state = 0;}
    else if (a == 0 && b == 1)
    {new_state = 1;}
    else if (a == 1 && b == 1)
    {new_state = 3;}
    else
    {new_state = 2;}

  

    //only update back @ 0 bc the FSM goes 0 1 3 2 0 clockwise and 0 2 3 1 0 counter and 0 is the only one passed twice so its like the home base (how I view it) 
    //from lab 6
    if (new_state == 0)
    {
        //check dir
        if (old_state == 2)   // cw  2 to 0
        {
            if (option == 0) {//low selected
                if (low < 99) 
                {low++;}
                if (low > high) 
                {low = high;
                }
                eeprom_update_byte((void*)0, low); //low add @ 0
            } else {//high selected
                if (high < 99) 
                {high++;}
                if (high < low) 
                {high = low;
                }
                eeprom_update_byte((void*)1, high); //high add @ 1

            }
            changed = 1;
        }
        else if (old_state == 1)//countercloc 1 to 0
        {
            if (option == 0) {//low selected
                if (low > 1) {low--;
                }
                if (low > high) 
                {low = high;
                }
                eeprom_update_byte((void*)0, low); //low add @ 0
            } else {//high selected
                {high--;
                }
                if (high < low) 
                {high = low;
                }
                eeprom_update_byte((void*)1, high); //high add @ 1
            }
            changed = 1;
        }
    }

    //update stateeee
    old_state = new_state;

}
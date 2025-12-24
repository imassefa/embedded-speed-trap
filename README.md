# Embedded Speed Trap System
This project implements a real time embedded speed trap using an AVR microcontroller, optical sensors, and hardware timers.

## Overview
The system measures the speed of an object passing between two LED phototransistor sensors. When the first sensor is triggered, a hardware timer begins counting. When the second sensor is triggered, timing stops and the elapsed time is used to compute object speed.

## Features
- Real time speed measurement using hardware timers  
- LCD display showing elapsed time and computed speed  
- Servo motor indicating speed on a physical dial  
- User adjustable high and low speed limits via buttons and a rotary encoder  
- EEPROM backed storage of speed limits across power cycles  
- RGB LED feedback indicating whether speed is below, within, or above limits  

## Technical Details
- Low level C with register level configuration on an AVR microcontroller  
- ADC sampling of optical sensors with calibrated linear mapping to normalize raw values  
- Timer based measurement using hardware counters, converted to elapsed time in milliseconds  
- Object speed computed using scaled integer arithmetic rather than floating point operations  
- State driven control logic with timeout handling and EEPROM backed configuration  

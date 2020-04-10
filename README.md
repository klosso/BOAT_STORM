# BOAT_STORM
RC boad speed storm, renew rc electronic to use 2.4GHz Flysky radio and one ! ESC for both independent brushes motors

## Project to run old R/C motor boat with new 2.4GHz transmitter


Boat SpeedStorm has two brushed motors w/o sterring.
Making turns was originally make by not powering one of motors (like in tanks) 
Original R/C transmitter was analog 27MHz and stereing of motors was done on relays. Of course steering was w/o speed control (as I assume , because never had original transmitter ) 

Base modification of this steering will set connect two motors to modified brushless ESC , to have 
- speed control 
- direction control 
- external PPM receiver 

I use small FlySky receiver, with combination of MULTI-FLY 30A brushless ESC for 2S-3S, based on Atmega8. 
ESC has 3 chanels and programming pads available on pcb. 

First try was to use Arduino IDE and PPMReader code for it, but due to poor avaibility of controlling timer interrupts , I chose to move to pure avr-gcc  and any available IDE ( my was Eclipse and/or Geany ).
Second reason was that pwm output (analogWrite function ) in Arduino is uses timer and related HW driven GPIO. And in Atmega8  there only 2 timers with HW pwm. SW generated pwm pulses , was having terrible low frequerency.
Last reason , it is that nowerdays  Arduino has almost none support for Atmega8 ( surprise for me )    


## Connections 

Base Idea is to connect motors to semi H bridge of ESC
Schematic and connection of ESC is very similar to @NicksonYap [NicksonYap/XXD-HW30A-ESC-Schematic](https://github.com/NicksonYap/XXD-HW30A-ESC-Schematic)
ESC:
 - second channel pwm for motor2 output
 - second channel pwm for motor1 output
 - first chanel will be used to change direction, common output for both motors.

ESC ATmega connection

PORD pins

2 - input pwm\/ppm

3 - output positive half of channel 1

4 - output positive half of channel 2

5 - output positive half of channel 3

PORTB

0 - output negative half of channel 1

1 - output negative half of channel 2

2 - output negative half of channel 3


## Open Points 

	[ ] add pictures of boat and sterring.
	[ ] add links for ESC and receiver .\n
	[ ] add oscilograms 
  This cheep receiver generate PPM signal even if transmitter is off, that why function ppmReader.isReciving() is useless.

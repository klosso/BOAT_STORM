/*
changed PPMlib for mega8 cpu, not Arduino IDE
PPMReader library based on https://github.com/Hasi123/read-any-ppm
Works with Servo library

Paweł (DzikuVx) Spychalski 2017
https://quadmeup.com

License: GNU GPL v3
*/

#include <avr/io.h>
#include "PPMReader.hpp"
#include <avr/interrupt.h>


#define CPU_SPEED_MULTIPLIER (F_CPU/8000000)
#define NO_UPDATE_THRESHOLD 500 //if no update in this number of ms, raise alarm


uint16_t micros ()
{
    return TCNT1;
}



uint16_t millis ()
{
    static uint16_t mils;
    static uint16_t old_mic;
    if( (micros() -old_mic) >1000 )
        ++mils;
    return mils;
}

volatile uint16_t PPMReader::ppm[PPMREADER_PMM_CHANNEL_COUNT];
volatile bool ppmReaderUseTimer = false;
volatile unsigned long lastPacketUpdate = 0;
    static uint16_t pulse =0 ;
//    static uint16_t pulse_max =0 ;
//    static uint16_t pulse_min =255 ;

    static uint8_t channel=0;
PPMReader::PPMReader(int pin, bool useTimer)
{
    _pin = pin;
    ppmReaderUseTimer = useTimer;


    //~ if (ppmReaderUseTimer) {
		//atmega8
		//~ TCCR1A = 0;
		//~ TCCR1B &=~(0x7);
		 //~ TCCR1B |= _BV(CS11); //set prescaller to 1/8
	//~ }

    DDRD &= ~(_BV(_pin));  //set gpio as input
    GICR |= _BV(INT0); //enable INT0
}

uint16_t PPMReader::get(const uint8_t channel)
{
    return ppm[channel];
}

bool PPMReader::isReceiving(void) {
    if (millis() - lastPacketUpdate > NO_UPDATE_THRESHOLD) {
        return false;
    } else {
        return true;
    }
}

void PPMReader::start(void) {
    GICR |= _BV(INT0);   // enable ext irq0
}

void PPMReader::stop(void) {
    GICR &= ~_BV(INT0);   // disable ext irq0
}

void PPMReader::handler()
{


    static unsigned long previousCounter = 0;
    static unsigned long currentMicros = 0;
    int tmpVal;
    static uint16_t counter;

    if (ppmReaderUseTimer) {
		cli();
        counter = TCNT1 * CPU_SPEED_MULTIPLIER;
        TCNT1 = 0;
        sei();
    } else {
        currentMicros = micros();
        counter = currentMicros - previousCounter;
        previousCounter = currentMicros;
    }

    //~ if (counter < 102) { //must be a pulse
    if (counter < 500) { //must be a pulse
        pulse = counter;
    }
    else if (counter > 3000)
    { //sync
        channel = 0;
        lastPacketUpdate = millis();
    }
    else
    { //servo values between 810 and 2210 will end up here
        tmpVal = counter +pulse;
        if (tmpVal > 810 && tmpVal < 2210) {
        if(channel < PPMREADER_PMM_CHANNEL_COUNT){
            ppm[channel] = tmpVal;
        }
        channel++;
		}
    }

    //~ if (pulse_min == 0 ) 
		//~ pulse_min = counter;
   //~ if ( counter > pulse_max ) {
		//~ ppm[0] = counter;
		//~ pulse_max=counter;
		//~ }
   //~ if ( counter < pulse_min ) {
		//~ ppm[1] = counter;
		//~ pulse_min=counter;
		//~ }

		//~ ppm[channel] = counter;
		//~ ++channel %=16;
}


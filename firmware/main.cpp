#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//typedef unsigned char bool;
//#define false 0
//#define true !false
//
#include "PPMReader.hpp"

#define set_bit(val,x) (val |= _BV(x))
#define clr_bit(val,x) (val &= ~_BV(x))
#define write_bit(val,bit,bit_val) ((bit_val)?set_bit(val,bit) : clr_bit(val,bit) )


//static int uart_putchar(char c, FILE *stream);
//static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, FDEV_SETUP_WRITE);
//
//static int uart_putchar(char c, FILE *stream)
//{
//  if (c == '\n')
//    uart_putchar('\r', stream);
//  loop_until_bit_is_set(UCSRA, UDRE);
//  UDR = c;
//  return 0;
//}

//motors bridge connection
//uint8_t p[6] = { 8, 9, 10, 3, 4, 5};
uint8_t pL[6] = { 0, 1, 2 };
uint8_t pH[3] = { 3, 4, 5};

volatile static uint8_t pwm_count = 0;

uint8_t pwm_set[3] = {50, 150, 128};

#define PPM_CHANNEL_PIN 2
#define REFRESH_PERIOD_MS 1000

#define DIR_FB_CH 7
#define THROTTLE_CH 2
#define DIR_LR_CH 0

//typedef unsigned char bool;
PPMReader ppmReader(PPM_CHANNEL_PIN, true);
static bool dir_fow_last = false;

void setup()
{
  //  Serial.begin(115200);
	//set gpio as out
  for ( uint8_t i = 0 ; i < 3; i++) {
      DDRB |= _BV(pL[i]);
      DDRD |= _BV(pH[i]);
  }
//    pinMode(pL[i], OUTPUT);

  //  TCCR1A = 0;  //reset timer1
  //TCCR1B = 0;
  //TCCR1B |= (1 << CS11);  //set timer1 to increment every 0,5 us or 1us on 8MHz
//  ppmReader = new PPMReader(PPM_CHANNEL_PIN, true);
  //  dir_fow_last = (ppmReader->get(DIR_FB_CH) < 1500) ? true : false;
//  digitalWrite( pL[2], dir_fow_last);
//  digitalWrite( pH[2], !dir_fow_last);
  OCR2 = 50 ; // pwm resolution 10* prescaler
  TCCR2 = 0x0A; // TOP=OCR2 , prescaler =1/8
  //    TCCR2 |= WGM21;
  TIMSK |= _BV(OCIE2); //irq on cmp on tmi2
  //  Serial.printf("ready: cpu speed is  %lu , tccr0 = 0x%x, TCCR1A=0x%x,TCCR1B=0x%x, GICR=0x%x TIMSK=0x%x TCCR2=0x%x \r\n", F_CPU, TCCR0, TCCR1A, TCCR1B, GICR ,TIMSK,TCCR2);


}

int map(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

ISR (TIMER2_COMP_vect) {

  ++pwm_count;
  for (uint8_t i = 0; i < 2 ; i++){
    write_bit(PORTB,pL[i], pwm_count <=  pwm_set[i] );
    write_bit(PORTD,pH[i], pwm_count <= (255 - pwm_set[i]));
  }
}


void loop()
{
  int  speed_val;
  int  dir_lr_val;


  //  if (ppmReader.isReciving())
  {

    bool dir_fow = (ppmReader.get(DIR_FB_CH) < 1500) ? true : false;

    if ( dir_fow != dir_fow_last ) {
    	write_bit(PORTB, pL[2], dir_fow );
    	write_bit(PORTD, pH[2], !dir_fow );
    	dir_fow_last = dir_fow;
    }

    //get throttle
    speed_val = ppmReader.get(THROTTLE_CH);
    dir_lr_val = ppmReader.get(DIR_LR_CH);


    //    Serial.print("speed raw = "); Serial.print(speed_val);
    //    Serial.print(", dirlr raw= "); Serial.print(dir_lr_val);
    speed_val = map(speed_val, 1000, 2020, 0, 255);     // scale it to use it with the servo (value between 0 and 180)
    dir_lr_val = map(dir_lr_val, 1000, 2020, -255, 255);     // scale it to use it with the servo (value between 0 and 180)
    //    Serial.print(", speed = "); Serial.print(speed_val);
    //    Serial.print(", dirlr= "); Serial.print(dir_lr_val);
    //    Serial.print(", foward:"); Serial.println(dir_fow ? "yes" : "no");

    if (dir_fow) {
      clr_bit( PORTB, pL[0]);
      clr_bit( PORTB, pL[1]);

      pwm_set[0] = (speed_val + dir_lr_val );
      pwm_set[1] = (speed_val - dir_lr_val );
    }
    else {
      clr_bit( PORTD, pH[0]);
      clr_bit( PORTD, pH[1]);

      pwm_set[0] = (255 - speed_val - dir_lr_val );
      pwm_set[1] = (255 - speed_val + dir_lr_val );
    }
    _delay_ms(REFRESH_PERIOD_MS);            // waits for the servo to get there
//    pwm_set[0] += 10;
//    pwm_set[0] %= 255;
  }
}

int main (int argc , char** argv )
{
//#define UART_BAUD_RATE 115200
//#define  UART_BAUD_REGISTERS  (((F_CPU / (UART_BAUD_RATE * 16UL))) - 1);
//
//	UCSRB |= (1 << RXEN) | (1 << TXEN);
//	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
//
//	UBRRL = UART_BAUD_REGISTERS;
//
//	  stdout = &mystdout;
//	  printf("Hello, world!\n");
    setup();
    for(;;)
        loop();
}


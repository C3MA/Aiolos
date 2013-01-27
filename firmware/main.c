#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>



#include "uart.h"
#include "temperature.h"

int uart_putchar( char c, FILE *stream )
{
    if( c == '\n' )
        uart_putchar( '\r', stream );
 
    uart_putc(c);

    return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM( uart_putchar, NULL, _FDEV_SETUP_WRITE );

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* 115200 baud  Arduino bootlaoder macht das x2 deshalb 57600 */
#define UART_BAUD_RATE      57600      


int main(void)
{

    unsigned int c;
    char buffer[7];
    int  num=134;

PRR &= ~(1<<PRTIM0);

DDRD |= (1<<PD5); // Port OC0B 
TCCR0A = (1<<WGM10) | (1<<COM0B1); // PWM, phase correct, 8 bit.
TCCR0B = (1<<CS11) | (1<<CS10); // Prescaler 64 = Enable counter
OCR0B = 240; // Duty cycle 50% (Anm. ob 128 oder 127 bitte prüfen) 
    
    /*
     *  Initialize UART library, pass baudrate and AVR cpu clock
     *  with the macro 
     *  UART_BAUD_SELECT() (normal speed mode )
     *  or 
     *  UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
     */
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
    stdout = &mystdout;
    
    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    sei();

//printf("Hallo Welt!!");

printf("foobar\n");

   temperature_init();
       
    for(;;)
    {
	float temp = temperature_get();
	if(temp > 25.0)
        {
             OCR0B = 0;
	}
	else
	{
	     OCR0B = 240;
	}

	printf("Current temp: %s deg C\r\n", temperature_as_string());
    }
    
}

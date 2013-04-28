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

/* counter for tacho */
volatile uint8_t tachoCounter = 0;

volatile uint8_t flag = 0;


#define Kp 40.0
#define Ki 0.0
#define Kd 0.0

double PID_Berechnung (double x, double w)
{ 
static double ealt;
double e,esum,y; 

e = x - w;                // aktuelle Regelabweichung bestimmen
if ((y < 1023)&&(y > 0))  // bei Übersteuertem stellglied Integration einfrieren
  {                       // (Anti-Windup)
    esum = esum + e;      // Summe der Regelabweichung aktualisieren  
  }
y = (Kp*e)+(Ki*esum)+(Kd*((e-ealt))/1);  // Reglergleichung
ealt = e;                 // Regelabweichung für nächste Abtastung merken
if (y > 256)             // Stellgröße auf 0..1023 begrenzen (10 bit PWM)
  {
    y = 255;
  }
if (y < 1)
  {
    y = 0;
  }
return y;                 // Stellgröße zurückgeben
}

int main(void)
{

    unsigned int c;
    char buffer[7];
    int  num=134;
    
    /*
     * Init Timer 1; 1s cycle
     */
    
    TCCR1A = 0x0;
    TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
    OCR1AH = 0x3D;
    OCR1AL = 0x08;
    TIMSK1 |= (1<<OCIE1A);
    
    /* Init PWM */
    PRR &= ~(1<<PRTIM0);
    DDRD |= (1<<PD5); // Port OC0B 
    TCCR0A = (1<<WGM10) | (1<<COM0B1); // PWM, phase correct, 8 bit.
    TCCR0B = (1<<CS12) | (1<<CS10); // Prescaler 1024 = Enable counter
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

    /* Init INT 1 Tacho */
    EIMSK |= (1<<INT1);
    EICRA |= (1<<ISC11) | (1<<ISC10); 

    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    sei();

    temperature_init();

    for(;;)
    {
        if (flag == 1)
	{
	    flag = 0;
	    uint8_t tcount = tachoCounter;
	    tachoCounter = 0;
	    
	    /*if (temp >= 30)
	    {
		OCR0B = 0;
	    } 
	    else if(temp >= 28.0)
	    {
		OCR0B = 200;
	    }
	    else
	    {
		OCR0B = 250;
	    }*/
	   
	    
	    /*
	    if (y < 1)
	    {
	      y = 0;
	    }*/
	    
	    OCR0B = (uint8_t)  PID_Berechnung((double) temperature_get(), 26.0);
	    
	    printf("%s;%d;%d\r\n",temperature_as_string(), tcount * 30, OCR0B);
	    /*
	    printf("Current temp: %s deg C\r\n", temperature_as_string());
	    printf("Current speed: %d rpm\r\n", tcount * 30);
	    */
	}
    }
    
}

ISR(TIMER1_COMPA_vect)
{
   flag = 1;
}

ISR(INT1_vect )
{
  tachoCounter++;
}

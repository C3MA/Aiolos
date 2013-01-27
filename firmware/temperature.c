/*
 * temperature.c
 *
 *  Created on: 12.08.2011
 *      Author: Mathias Dalheimer
 */

#include "temperature.h"
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>

#include "tempsensors.h"
#include "uart.h"



//local variables
static float temperature_value;
static char temperature_string_buffer[10];


void _update_temp_string(void) {
  dtostrf(temperature_value, 9, 4, &temperature_string_buffer);
}

void
temperature_init(void)
{
  temperature_value=0.0;
  uart_puts("-- Temperature: INIT\r\n");
  initTempSensors();
  loopTempSensors();
  temperature_value=getTemperatureFloat();
  _update_temp_string();
}

float
temperature_get(void)
{
  //getTemperatureFloat(); //TODO: Fixes really strange linker error, somebody needs to look into this. (loopTempSensors, getTemperatureFloat or read_pressure_temp must be executed to be able to compile. Possible bug with avr-binutils)
  //TODO: Now fixed by adding -lc -lm to linker flags, needs further testing
  //uart_puts("-- Temperature: Get value\r\n");
  loopTempSensors();
  temperature_value=getTemperatureFloat();
  _update_temp_string();
  //printf("Current temp: %s deg C\r\n", temperature_string_buffer);
  return temperature_value;
}

char*
temperature_as_string(void)
{
  //uart_puts("-- Temperature: Get string value\r\n");
  return &temperature_string_buffer;
}

/*
 * temperature.h
 *
 *  Created on: 20.04.2011
 *      Author: Mathias Dalheimer
 */

#ifndef TEMPERATURE_H
#define TEMPERATURE_H 1



#include <stdint.h>
#include <stdbool.h>

float 	temperature_get(void);
char* temperature_as_string(void);
void 	temperature_init(void);
#endif /* TEMPERATURE_H */

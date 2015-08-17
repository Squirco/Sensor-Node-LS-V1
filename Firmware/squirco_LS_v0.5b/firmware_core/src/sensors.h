#ifndef SENSORS_H
#define SENSORS_H

// Header file contents
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
//#include <math.h>
#include "Adc.h"
#include "adc_lut.h"
#include "commands.h"
#include "LSButton.h"

//Function Prototypes
void ADC_GET_RAW_TEMP(void);	//function for getting the 10 bit temperature
void ADC_GET_RAW_LIGHT(void);	//function for getting the 10 bit light
void ADC_GET_TEMPERATURE (void);	//function for getting temperature value in degrees
void ADC_GET_AMBIENT_LIGHT(void);	//function for getting brightness in Lux

//global variables
extern volatile uint16_t ADCCurrentTemperature;
extern volatile uint16_t ADCCurrentLight;
extern volatile uint16_t ADCCurrentLightStep;
extern volatile uint16_t ADCCurrentLightPerception;

#endif
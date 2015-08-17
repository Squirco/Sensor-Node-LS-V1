/*
 * Adc.c
 */ 

#include <avr/io.h>
#include "Adc.h"

void AdcInit(void) {
	ADCSRA |= 1 << ADEN;	//Enable ADC
	
	ADMUX |= 0 << REFS1 | 1 << REFS0;	//Internal 1.8V ref
	//result right adjusted
	//Channel 0 initially connected to ADC
	
	//Wait for voltage planes to stabilize
	while(!(ADCSRB & 0x80));
	while(!(ADCSRB & 0x20));
	
	ADCSRA |= 0b00011110;	//Auto trigger disabled
	ADCSRA |= 1 << ADIF | 1 << ADIE | 1 << ADPS2 | 1 << ADPS1;
	//Ensure ADC Interrupt flag is cleared
	//Enable ADC Interrupts
	//Set ADC clock to 125k
}
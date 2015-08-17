#ifndef ADC_LUT_H_
#define ADC_LUT_H_

#include <avr/pgmspace.h>
//#include <avr/io.h>
//#include <avr/interrupt.h>
#define NTCMin	0x179	//lower limit of ADC value for conversion (9 degrees Celsius)
#define NTCMax	0x2C6	//upper limit of ADC value for conversion (32 degrees Celsius)

extern const uint16_t NTCTemp[] PROGMEM;
extern const uint16_t ALSLux[][3] PROGMEM;

#endif /* ADC_LUT_H_ */
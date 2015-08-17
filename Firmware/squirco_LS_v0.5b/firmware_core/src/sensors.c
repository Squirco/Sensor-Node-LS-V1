#include "sensors.h"
#include "globalDefs.h"

//ADC Variables
volatile uint16_t ADC_ALS_LUX;
volatile uint16_t ADC_TEMP_C;
volatile uint16_t ADC_RAW_TEMP = 0;
volatile uint16_t ADC_RAW_LIGHT = 0;
const uint8_t ADCTemperatureBufferSize = 17;   //index starts at 0; 18 positions
volatile uint8_t ADCTemperatureBufferPosition = 0;

volatile uint16_t ADCPreviousTemperature;
volatile uint16_t ADCCurrentTemperature;

volatile uint16_t ADCPreviousLight;
volatile uint16_t ADCCurrentLight;
volatile uint16_t ADCPreviousLightStep;
volatile uint16_t ADCCurrentLightStep;
volatile uint16_t ADCPreviousLightPerception;
volatile uint16_t ADCCurrentLightPerception;

//ADC arrays
uint16_t ADCRawTemperatureBuffer[18];

void ADC_GET_AMBIENT_LIGHT(void)
{
	ADCPreviousLightStep = ADCCurrentLightStep;
	ADCPreviousLightPerception = ADCCurrentLightPerception;
	
	ADCCurrentLightPerception = pgm_read_word(&ALSLux[ADC_RAW_LIGHT][2]);
	ADCCurrentLightStep = pgm_read_word(&ALSLux[ADC_RAW_LIGHT][1]);
	ADCCurrentLight = pgm_read_word(&ALSLux[ADC_RAW_LIGHT][0]);	//IMPORTANT: update current light LAST!
	
	if ((abs(ADCPreviousLightPerception-ADCCurrentLightPerception) > 10 || (abs(ADCPreviousLightPerception-ADCCurrentLightPerception) > 2 && abs(ADCPreviousLightStep-ADCCurrentLightStep) >0)))
	{
		CommandsInBuffer[CommandsInIndexEnd][0] = 0x0182;	//get incoming command ID
		CommandsInBuffer[CommandsInIndexEnd][1] = 0;	//get incoming node address
		CommandsInIndexEnd++;
		if (CommandsInIndexEnd == CommandsInIndexSize)
		{
			CommandsInIndexEnd=0;
		}
		appState = APP_STATE_IDLE;
	}
	
}
void ADC_GET_TEMPERATURE(void)
{
	if(ADCTemperatureBufferPosition == ADCTemperatureBufferSize)	//if buffer is filled
	{
		ADCPreviousTemperature = ADCCurrentTemperature;
		ADCCurrentTemperature = 0;
		for (uint8_t index = 0; index < ADCTemperatureBufferSize; index++)   //calculate average
		{
			ADCCurrentTemperature += ADCRawTemperatureBuffer[index] / (ADCTemperatureBufferSize + 1);	//averaging values
		}
		
		if (ADCCurrentTemperature >= NTCMin && ADCCurrentTemperature <= NTCMax)	//if the average temperature is in the valid range
		{
			ADCCurrentTemperature = pgm_read_word(&NTCTemp[ADCCurrentTemperature - NTCMin]);	//set internal temperature tracker
		}
		
		ADCTemperatureBufferPosition = 0;	//reset buffer counter
		
		if (abs(ADCCurrentTemperature-ADCPreviousTemperature) >= 1)// && LSConnectedMode == true)
		{
			CommandsInBuffer[CommandsInIndexEnd][0] = 0x0181;	//get incoming command ID
			CommandsInBuffer[CommandsInIndexEnd][1] = 0;	//get incoming node address
			CommandsInIndexEnd++;
			if (CommandsInIndexEnd == CommandsInIndexSize)
			{
				CommandsInIndexEnd=0;
			}
			
			appState=APP_STATE_IDLE;	//send to hub
		}
		
	}
	else
	{
		ADCRawTemperatureBuffer[ADCTemperatureBufferPosition] = ADC_RAW_TEMP;
		ADCTemperatureBufferPosition++;
	}
}

void ADC_GET_RAW_TEMP(void)
{
	ADC_RAW_TEMP = ADCL | (ADCH << 8);
}

void ADC_GET_RAW_LIGHT(void)
{
	ADC_RAW_LIGHT = ADCL | (ADCH << 8);
}
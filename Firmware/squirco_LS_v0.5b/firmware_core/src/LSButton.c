#include "LSButton.h"
#include "globalDefs.h"
#include "commands.h"

volatile uint8_t LSModeCounter = 0;
volatile bool LSModeCounterStarted = false;
volatile bool LSToggleState = false;
volatile bool LSConnectedMode = false;
volatile bool LSSyncMode = false;
volatile bool LSRemoteToggle = false;

//remote settings
bool LSisRemote = false;
uint16_t LSRemoteAddr;

//Permanent
uint8_t EEMEM eeLSisRemote;
uint16_t EEMEM eeLSRemoteAddr;

bool LSDebounceSwitch(void)
{
	static uint64_t LSState = 0;
	LSState = (LSState << 1) | bit_is_set(PINE, PINE7);
	if (LSState == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool LSToggleQuiet(void)
{
	static uint64_t LSState = 0;
	LSState = (LSState << 1) | bit_is_clear(PINB, PINE7);
	if (LSState == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void LSStateMachine(void)
{
	uint8_t LSHoldDuration;

	if(LSDebounceSwitch() || LSRemoteToggle == true) //run this if button is pressed
	{
		if (!LSModeCounterStarted)  //if counter not started, start counter
		{
			LSModeCounter = 0;  //start counter by clearing counter
			LSModeCounterStarted = true;    //set counter started to true
			LSRemoteToggle = false;
		}
		//if (LSModeCounter >= 5)     //if button is held down for more than 5 seconds
		//{
			//LSSyncMode = true;
		//}
	}
	else    //run this code on button release
	{
		if(LSModeCounterStarted){
			LSHoldDuration = LSModeCounter;  //figure out how long the button was held down for
			LSModeCounterStarted = false;    //stop the timer
		}

		if (LSHoldDuration <5)
		{
			if (LSSyncMode == true)	//if sync mode is on, turn off sync mode
			{
				LSSyncMode = false;
				//PORTB &= ~(1 << PORTB0);
			}
			else{	//normal operation
				LSToggleState ^= 1;
				if (LSToggleState == true)
				{
					//led[0].r=0; led[0].g=36; led[0].b=0;
					//ws2812_setleds(led,1);
					//msgLSButtonInfo.LSState = 0xff;
					if (LSisRemote == false)
					{
						PORTE |= (1 << PORTE4);	//turn on Relay
					}
				}
				else
				{
					PORTE &= ~(1 << PORTE4);	//turn off Relay
				}
				
				if (LSConnectedMode == true)
				{
					if (LSisRemote == true)
					{
						LSSendData(0x0111, LSRemoteAddr);
					}
					else
					{
						LSSendData(0x0191, HUB_ADDR);
					}
					
				}
			}
		}
	}
}


void LSSendData(uint16_t cmd, uint16_t dst)
{	
	CommandsInBuffer[CommandsInIndexEnd][0] = cmd;	//get incoming command ID
	CommandsInBuffer[CommandsInIndexEnd][1] = dst;	//get incoming node address
	
	CommandsInIndexEnd++;
	if (CommandsInIndexEnd == CommandsInIndexSize)
	{
		CommandsInIndexEnd=0;
	}
	
	appState = APP_STATE_IDLE;
}

void LSRemoteModeSet(void) //startup code
{		
	if (eeprom_read_byte(&eeLSisRemote) == 1)
	{
		LSisRemote = true;
		LSRemoteAddr = eeprom_read_word(&eeLSRemoteAddr);
	}
	else
	{
		LSisRemote = false;
	}
}

void LSRemoteModeSave(void)
{
	if (LSisRemote == true)
	{
		if (eeprom_is_ready())
		{
			eeprom_update_byte((uint8_t*)&eeLSisRemote, 1);
			eeprom_update_word((uint16_t*)&eeLSRemoteAddr, LSRemoteAddr);
		}
	}
	else
	{
		if(eeprom_is_ready())
		{
			eeprom_update_byte((uint8_t*)&eeLSisRemote, 1);
		}
	}
	
}
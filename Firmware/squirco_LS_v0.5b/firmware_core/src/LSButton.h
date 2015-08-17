#ifndef LSBUTTON_H
#define LSBUTTON_H

// Header file contents
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
//#include <math.h>
#include "commands.h"
#include "hal.h"
#include "phy.h"
#include "sys.h"
#include "nwk.h"
#include "nwkRoute.h"
#include "nwkSecurity.h"
#include "sysTimer.h"
#include "halSleep.h"
#include "halBoard.h"
#include "halLed.h"

//Function Prototypes
bool LSDebounceSwitch(void);
bool LSToggleQuiet(void);
void LSStateMachine(void);
void LSSendData(uint16_t cmd, uint16_t dst);
void LSRemoteModeToggle(void);
void LSRemoteModeSet(void);	//startup code
void LSRemoteModeSave(void);

//extern variables
extern volatile uint8_t LSModeCounter;
extern volatile bool LSModeCounterStarted;
extern volatile bool LSToggleState;
extern volatile bool LSConnectedMode;//false;
extern volatile bool LSSyncMode;
extern volatile bool LSRemoteToggle;

//remote settings
extern bool LSisRemote;
extern uint16_t LSRemoteAddr;
#endif
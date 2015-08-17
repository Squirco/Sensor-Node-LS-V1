#ifndef MAIN_H
#define MAIN_H

// Header file contents
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
//#include <math.h>
#include "Adc.h"
#include "adc_lut.h"
#include "commands.h"
#include "config.h"
#include "globalDefs.h"

//#include "twi.h"
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

#include "sensors.h"
#include "LSButton.h"


//function prototypes
void SystemSettingsSet(void);
void SYNCStateMachine(void);

extern uint32_t EEMEM eeSerialNum;
extern uint16_t EEMEM eeHardVersion;
extern uint32_t EEMEM eeSecurityKeyH;
extern uint32_t EEMEM eeSecurityKeyL;
extern uint16_t EEMEM eeIdentity;
extern uint16_t EEMEM eeShortAddr;
extern uint16_t EEMEM eeDefaultPan;
extern uint8_t EEMEM eeDefaultChannel;

//variables
extern volatile uint16_t src_node;	//temporary storage variable for source address
extern volatile uint16_t dst_node; // temporary storage variable for destination address
extern NWK_DataReq_t	appNwkDataReq;
extern NWK_DataReq_t	appNwkBroadcastDataReq;
extern bool	appNwkDataReqBusy;

extern uint32_t App_SerialNum;
extern uint64_t App_SecurityKey;
extern uint16_t App_Identity;
extern uint16_t App_ShortAddr;
extern uint16_t App_PanID;
extern uint8_t App_Channel;

//software Timers
extern SYS_Timer_t  appLSDebounceTimer;
extern SYS_Timer_t  appDBTTimer;
extern SYS_Timer_t	appALSTimer;
extern SYS_Timer_t	appRTCTimer;
extern SYS_Timer_t	appAsyncJoinTimer;
extern SYS_Timer_t	appAsyncJoinChainResetTimer;

#endif
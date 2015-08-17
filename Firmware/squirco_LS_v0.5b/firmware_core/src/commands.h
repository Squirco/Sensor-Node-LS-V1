//file commands.h
#ifndef _COMMANDS_H_
#define _COMMANDS_H_

// Header file contents
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>

//function prototypes
void CMDDataIOStateMachine(void);	//function to control mesh data flow
void CMDDataOutIndexKeeper(void);	//function to update buffer positions

//cmd prototypes
void CMDPushClimate(void);
void CMDPushBrightness(void);
void CMDPushLSState(void);

//definitions for commands and stuff
#define HUB_ADDR				0

#define UNIVERSAL_CMD			0x00
#define DEVICE_FAMILY			0x01

#define SYS_INFO_HEADER			0x10

#define SENS_TEMPERATURE_HEADER	0x81
#define SENS_LIGHT_HEADER		0x82

#define LS_STATE_HEADER			0x91

#define CMDCONFGENERIC			0
#define CMDCONFBURST			1
#define CMDCONFLSBUTTON			2
#define CMDCONFTEMP				3
#define CMDCONFLIGHT			4
#define CMDCONFSYSJOIN			5

#define CMDACKMODEOFF	0
#define CMDACKMODEON	1

//extern uint8_t colorCycler;

extern bool SysJoinChained;
extern uint16_t sysJoinPan;

extern volatile uint16_t commandIdIn;	//temporary storage variable for incoming command ID
extern volatile uint16_t commandIdOut;	//temporary storage variable for outgoing command ID
extern volatile uint16_t commandSrcAddr; //temporary storage variable for incoming command address
extern volatile uint8_t CMDDataSendType;
extern volatile uint8_t CMDDataConfType;
extern volatile uint8_t CMDDataAckType;

//Command Buffer Counters
extern volatile uint8_t CommandsInIndexStart;
extern volatile uint8_t CommandsInIndexEnd;
extern const uint8_t CommandsInIndexSize;
extern volatile uint8_t CommandsOutIndexStart;
extern volatile uint8_t CommandsOutIndexEnd;

extern uint16_t CommandsInBuffer[8][3];
//start chases end


typedef enum AppState_t
{
	//LWM stack states
	APP_STATE_INITIAL,
	APP_STATE_SEND,
	APP_STATE_BROADCAST,
	APP_STATE_WAIT_CONF,
	APP_STATE_IDLE,
	APP_STATE_SYNC,
	
	APP_STATE_LS_TOGGLE,
	APP_STATE_LS_ON,
	APP_STATE_LS_OFF,
	
	APP_STATE_PUSH_CLIMATE,
	APP_STATE_PUSH_BRIGHTNESS,
	APP_STATE_PUSH_LS_STATE
	
} AppState_t;

typedef struct cmdPACK	//generic command pack
{
	uint16_t	srcAddr;	//device's own address
	uint16_t	commandId;
		
} AppCmdPack_t;	//OUTGOING command

typedef struct sysInfoPACK	//system information pack
{
	uint32_t	serial;			//serial number
	uint16_t	softVersion;	//software version
	uint16_t	hardVersion;	//hardware version
	uint16_t	identity;		//rng'd unique identity
	uint8_t		family;			//device family
	uint16_t	srcAddr;		//device's own address
	uint16_t	commandId;
} AppSysInfoPack_t;

typedef struct temperaturePack
{
	uint8_t		humidity;		//rh in %
	uint16_t	temperature;	//temp in C to 2 decimal places
	uint16_t	srcAddr;		//device's own address
	uint16_t	commandId;
} AppTemperaturePack_t;

typedef struct ambientLightPack
{
	uint8_t		lightStep;
	uint16_t	brightness;
	uint16_t	srcAddr;	//device's own address
	uint16_t	commandId;
} AppAmbientLightPack_t;

typedef struct LSButtonPack
{
	uint8_t		LSState;
	uint16_t	srcAddr;	    // device's own address
	uint16_t	commandId;
} AppLSButtonPack_t;

typedef struct InternodePack
{
	
	uint16_t commandId;
	uint16_t payload;
	
} AppInternodePack_t;

extern AppCmdPack_t	msgCmd;
extern AppSysInfoPack_t msgSysInfo;
extern AppTemperaturePack_t	msgTemperatureInfo;
extern AppAmbientLightPack_t msgAmbientLightInfo;
extern AppLSButtonPack_t msgLSButtonInfo;
extern AppInternodePack_t msgInternodeCmd;

#endif // _COMMANDS_H_

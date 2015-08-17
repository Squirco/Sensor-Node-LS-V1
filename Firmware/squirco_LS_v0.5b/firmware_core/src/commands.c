#include "commands.h"
#include "LSButton.h"
#include "globalDefs.h"
#include "main.h"
#include "sensors.h"

bool SysJoinChained = false;
uint16_t sysJoinPan;

//variables
volatile uint16_t commandIdIn;	//temporary storage variable for incoming command ID
volatile uint16_t commandIdOut;	//temporary storage variable for outgoing command ID
volatile uint16_t commandSrcAddr;	//temporary storage variable for incoming command address
volatile uint8_t CMDDataSendType;
volatile uint8_t CMDDataConfType;
volatile uint8_t CMDDataAckType;

//Command Buffer Counters
volatile uint8_t CommandsInIndexStart = 0;
volatile uint8_t CommandsInIndexEnd = 0;
const uint8_t CommandsInIndexSize = 8;
volatile uint8_t CommandsOutIndexStart = 0;
volatile uint8_t CommandsOutIndexEnd = 0;

uint16_t CommandsInBuffer[8][3];
//start chases end

//appMessage types
AppCmdPack_t	msgCmd;
AppSysInfoPack_t msgSysInfo;
AppTemperaturePack_t	msgTemperatureInfo;
AppAmbientLightPack_t msgAmbientLightInfo;
AppLSButtonPack_t msgLSButtonInfo;
AppInternodePack_t msgInternodeCmd;

//cmd functions
void CMDPushClimate(void)
{
	msgTemperatureInfo.commandId = DEVICE_FAMILY <<8 | SENS_TEMPERATURE_HEADER;
	msgTemperatureInfo.srcAddr = App_ShortAddr;
	msgTemperatureInfo.temperature = ADCCurrentTemperature;
	appNwkDataReq.data = (uint8_t *)&msgTemperatureInfo;
	appNwkDataReq.size = sizeof(msgTemperatureInfo);
					
	CMDDataConfType = CMDCONFTEMP;
	CMDDataAckType = CMDACKMODEON;
	
	appState = APP_STATE_SEND;
}

void CMDPushBrightness(void)
{
	msgAmbientLightInfo.commandId = DEVICE_FAMILY <<8 | SENS_LIGHT_HEADER;
	msgAmbientLightInfo.srcAddr = App_ShortAddr;
	msgAmbientLightInfo.brightness = ADCCurrentLight;
	msgAmbientLightInfo.lightStep = ADCCurrentLightStep;
	appNwkDataReq.data = (uint8_t *)&msgAmbientLightInfo;
	appNwkDataReq.size = sizeof(msgAmbientLightInfo);
	
	CMDDataConfType = CMDCONFLIGHT;
	CMDDataAckType = CMDACKMODEON;
	
	appState = APP_STATE_SEND;
}

void CMDPushLSState(void)
{
	msgLSButtonInfo.commandId = DEVICE_FAMILY <<8 | LS_STATE_HEADER;
	dst_node = CommandsInBuffer[CommandsInIndexStart][1];
	msgLSButtonInfo.srcAddr = App_ShortAddr;
	
	if (bit_is_set(PINE, PINE4))
	{
		msgLSButtonInfo.LSState = 0xff;
	}
	else
	{
		msgLSButtonInfo.LSState = 0;
	}
	
	appNwkDataReq.data = (uint8_t *)&msgLSButtonInfo;
	appNwkDataReq.size = sizeof(msgLSButtonInfo);
	
	CMDDataConfType = CMDCONFLSBUTTON;
	CMDDataAckType = CMDACKMODEON;
	
	appState = APP_STATE_SEND;
}

//cmd buffer roller
void CMDDataIOStateMachine(void)
{

		if (CommandsInIndexStart != CommandsInIndexEnd)
		{
		
			switch(CommandsInBuffer[CommandsInIndexStart][0])
			{
				case 0x02:	//System Initialization Join Chain
				{
					if (!SYS_TimerStarted(&appAsyncJoinChainResetTimer))
					{
						SYS_TimerStart(&appAsyncJoinChainResetTimer);
					} //start join reset timer
					
					sysJoinPan = CommandsInBuffer[CommandsInIndexStart][2];	//get master Pan
					
					if (!SysJoinChained || (sysJoinPan != App_PanID))
					{
						SysJoinChained = true;
						App_PanID = sysJoinPan;
						
						if (!SYS_TimerStarted(&appAsyncJoinTimer))
						{
							SYS_TimerStart(&appAsyncJoinTimer);
						}
						
						msgInternodeCmd.commandId = 0x02;
						msgInternodeCmd.payload = App_PanID;
						
						appNwkDataReq.data = (uint8_t *)&msgInternodeCmd;
						appNwkDataReq.size = sizeof(msgInternodeCmd);
						
						appState = APP_STATE_BROADCAST;
					}
					
					else
					{
						appState = APP_STATE_IDLE;
					}
					break;
				}
				
				case 0x0010:	//send system info
				{
					
					msgSysInfo.serial = App_SerialNum;
					msgSysInfo.softVersion = APP_SOFTVERSION;
					msgSysInfo.hardVersion = eeprom_read_word(&eeHardVersion);
					msgSysInfo.identity = App_Identity;
					msgSysInfo.family = DEVICE_FAMILY;
					msgSysInfo.srcAddr = App_ShortAddr;
					msgSysInfo.commandId = UNIVERSAL_CMD <<8 | SYS_INFO_HEADER;
						
					dst_node = HUB_ADDR;
					appNwkDataReq.data = (uint8_t *)&msgSysInfo;
					appNwkDataReq.size = sizeof(msgSysInfo);
					
					CMDDataConfType = CMDCONFGENERIC;
					CMDDataAckType = CMDACKMODEON;
					
					appState = APP_STATE_SEND;
					break;
				}
				
				case 0x0101:	//Device Join Ping (send to hub)
				{
					NWK_SetPanId(App_PanID);
				
					msgCmd.commandId = 0x0101;
					msgCmd.srcAddr = App_ShortAddr;
					dst_node = HUB_ADDR;
					appNwkDataReq.data = (uint8_t *)&msgCmd;
					appNwkDataReq.size = sizeof(msgCmd);
				
					CMDDataConfType = CMDCONFSYSJOIN;
					//CMDDataConfType = CMDCONFGENERIC;
					CMDDataAckType = CMDACKMODEON;
				
					appState = APP_STATE_SEND;
					break;
				}
				
				case 0x0111:	//toggle light switch
				{
					appState = APP_STATE_LS_TOGGLE;
					break;
				}
			
				case 0x0112:	//turn on light switch
				{
					appState = APP_STATE_LS_ON;
					break;
				}
			
				case 0x0113:	//turn off light switch
				{
					appState = APP_STATE_LS_OFF;
					break;
				}
				
				case 0x0181:	//request temperature
				{
					appState = APP_STATE_PUSH_CLIMATE;
					break;
				}
			
				case 0x0182:	//request light
				{
					appState = APP_STATE_PUSH_BRIGHTNESS;
					break;
				}
				
				case 0x0191:	//send light switch status
				{
					appState = APP_STATE_PUSH_LS_STATE;
					break;
				}
			
				default:
				{
					break;
				}
			}	//end switch
		
		
			if (appNwkDataReqBusy == false)
			{
				CMDDataOutIndexKeeper();
			}
		}
}
void CMDDataOutIndexKeeper(void)
{
	CommandsInIndexStart++;
	if (CommandsInIndexStart==CommandsInIndexSize)
	{
		CommandsInIndexStart=0;
	}
}
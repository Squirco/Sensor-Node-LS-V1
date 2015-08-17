/*- Includes ---------------------------------------------------------------*/
#include "main.h"

/*- Definitions ------------------------------------------------------------*/
#ifdef NWK_ENABLE_SECURITY
	#define APP_BUFFER_SIZE     (NWK_MAX_PAYLOAD_SIZE - NWK_SECURITY_MIC_SIZE)
#else
	#define APP_BUFFER_SIZE     NWK_MAX_PAYLOAD_SIZE
#endif

//system settings stored in EEPROM
//Production values are loaded separately after programming chip

uint32_t EEMEM eeSerialNum = 0x00000001;	//serial number
uint16_t EEMEM eeHardVersion = 0x007a;		//hardware version
uint32_t EEMEM eeSecurityKeyH = 0x0848A357;	//upper 32 bits of security key. Security will be upgraded at a later time.
uint32_t EEMEM eeSecurityKeyL = 0x9D256556;	//lower 32 bits of security key.
uint16_t EEMEM eeIdentity;					//unique identity extension to avoid collision. Randomly generated at first startup.
uint16_t EEMEM eeShortAddr = 0x0001;		//short address. Will be randomly generated later.
uint16_t EEMEM eeDefaultPan = 0x1234;		//default pan address.
uint8_t EEMEM eeDefaultChannel = 0x0f;		//default channel. Channel scanning will be implemented later
//uint8_t EEMEM eeSysJoined = 1;
uint8_t EEMEM eeSysJoined = 0;				//Tracks whether the node has been connected to a hub or not

//system settings copied to memory during run time
uint32_t App_SerialNum;
uint64_t App_SecurityKey;
uint16_t App_Identity;
uint16_t App_ShortAddr;
uint16_t App_PanID;
uint8_t App_Channel;

AppState_t appState = APP_STATE_INITIAL;

NWK_DataReq_t	appNwkDataReq;
NWK_DataReq_t	appNwkBroadcastDataReq;
bool	appNwkDataReqBusy = false; 

//software Timers
SYS_Timer_t  appLSDebounceTimer;	//debounce timer
SYS_Timer_t  appDBTTimer;			//timer used to measure temperature
SYS_Timer_t	appALSTimer;			//timer used to measure ambient light
SYS_Timer_t	appRTCTimer;			//tick tock
SYS_Timer_t	appAsyncJoinTimer;		//delay timer to send join confirmation
SYS_Timer_t	appAsyncJoinChainResetTimer;	//delay timer to reset join chain state

/* Globals: Miscellaneous */
volatile uint16_t src_node;	//temporary storage variable for source address
volatile uint16_t dst_node; // temporary storage variable for destination address
volatile uint8_t  tx_tries = 0; // transmission retry tracker

//system Mode variables
volatile bool SYSModeSync = false;

/*- Implementations --------------------------------------------------------*/

static void appDataConf(NWK_DataReq_t *req)
{
	appNwkDataReqBusy = false; // clear the busy flag
	appState=APP_STATE_IDLE;
	(void)req;
}

static void appSystemJoinConf(NWK_DataReq_t *req)
{
	appNwkDataReqBusy = false; // clear the busy flag
	
	if (eeprom_is_ready())
	{
		eeprom_update_word((uint16_t*)&eeDefaultPan, App_PanID);
		eeprom_update_byte((uint8_t*)&eeSysJoined, 1);
	}
	
	if (!SYS_TimerStarted(&appDBTTimer))
	{
		SYS_TimerStart(&appDBTTimer);
	}
	
	if (!SYS_TimerStarted(&appALSTimer))
	{
		SYS_TimerStart(&appALSTimer);
	}
	
	LSConnectedMode = true;
	
	appState=APP_STATE_IDLE;
	(void)req;
}

static void appTempDataConf(NWK_DataReq_t *req)
{
	if ((tx_tries < APP_MAX_TX_RETRIES) && (req -> status & NWK_NO_ACK_STATUS))
	{
		tx_tries++;
		
		CommandsInBuffer[CommandsInIndexEnd][0] = 0x0181;	//get incoming command ID
		CommandsInBuffer[CommandsInIndexEnd][1] = HUB_ADDR;	//send to hub
		
		CommandsInIndexEnd++;
		if (CommandsInIndexEnd == CommandsInIndexSize)
		{
			CommandsInIndexEnd = 0;
		}
	}
	
	if (tx_tries >= APP_MAX_TX_RETRIES)
	{
		tx_tries = 0;
	}
	
	appNwkDataReqBusy = false; // clear the busy flag
	appState=APP_STATE_IDLE;
	(void)req;
}

static void appLightDataConf(NWK_DataReq_t *req)
{
	if ((tx_tries < APP_MAX_TX_RETRIES) && (req -> status & NWK_NO_ACK_STATUS))
	{
		tx_tries++;
		
		CommandsInBuffer[CommandsInIndexEnd][0] = 0x0182;	//get incoming command ID
		CommandsInBuffer[CommandsInIndexEnd][1] = HUB_ADDR;	//send to hub
			
		CommandsInIndexEnd++;
		if (CommandsInIndexEnd == CommandsInIndexSize)
		{
			CommandsInIndexEnd = 0;
		}
	}
	
	if (tx_tries >= APP_MAX_TX_RETRIES)
	{
		tx_tries = 0;
	}
		
	appNwkDataReqBusy = false; // clear the busy flag
	appState=APP_STATE_IDLE;
	(void)req;
}

static void appLSButtonDataConf(NWK_DataReq_t *req)
{
	if ((tx_tries < APP_MAX_TX_RETRIES) && (req -> status & NWK_NO_ACK_STATUS))
	{
		tx_tries++;
		
		CommandsInBuffer[CommandsInIndexEnd][0] = 0x0191;	//get incoming command ID
		CommandsInBuffer[CommandsInIndexEnd][1] = HUB_ADDR;	//send to hub
			
		CommandsInIndexEnd++;
		if (CommandsInIndexEnd == CommandsInIndexSize)
		{
			CommandsInIndexEnd = 0;
		}
	}
	
	if (tx_tries >= APP_MAX_TX_RETRIES)
	{
		tx_tries = 0;
	}
	
	appNwkDataReqBusy = false; // clear the busy flag
	appState=APP_STATE_IDLE;
	(void)req;
}

/*************************************************************************//**
*****************************************************************************/

static void appSendData(uint16_t node, uint8_t src_endpoint, uint8_t dst_endpoint)
{
	while (appNwkDataReqBusy == true)		// if a transmit request is already in progress
	{                                
							  // wait for transmission to finish
	}
	
	appNwkDataReq.dstAddr = node;
	appNwkDataReq.dstEndpoint = dst_endpoint;
	appNwkDataReq.srcEndpoint = src_endpoint;
	
	switch (CMDDataAckType)
	{
		case CMDACKMODEON:
		{
			appNwkDataReq.options = NWK_OPT_ACK_REQUEST | NWK_OPT_ENABLE_SECURITY;
			break;
		}
		case CMDACKMODEOFF:
		{
			appNwkDataReq.options = NWK_OPT_ENABLE_SECURITY;
			break;
		}
		
		default:
		{
			appNwkDataReq.options = NWK_OPT_ENABLE_SECURITY;
			break;
		}
	}
	
	switch (CMDDataConfType)
	{
		case CMDCONFGENERIC:	//generic conf
		{
			appNwkDataReq.confirm = appDataConf;
			break;
		}
		
		case CMDCONFSYSJOIN:	//system joined conf
		{
			appNwkDataReq.confirm = appSystemJoinConf;
			break;
		}
		
		case CMDCONFLSBUTTON:	//ls button
		{
			appNwkDataReq.confirm = appLSButtonDataConf;
			break;
		}
		
		case CMDCONFTEMP:	//temperature
		{
			appNwkDataReq.confirm = appTempDataConf;
			break;
		}
		
		case CMDCONFLIGHT:	//ambient light
		{
			appNwkDataReq.confirm = appLightDataConf;
			break;
		}
		
		default:
		{
			appNwkDataReq.confirm = appDataConf;
			break;
		}
	}
	
	NWK_DataReq(&appNwkDataReq);
	appNwkDataReqBusy = true;
	appState = APP_STATE_WAIT_CONF;
}

static void appBroadcastData(uint8_t src_endpoint, uint8_t dst_endpoint)
{
	while (appNwkDataReqBusy == true)		// if a transmit request is already in progress
	{
		// wait for transmission to finish
	}
	
	appNwkDataReq.dstAddr = NWK_BROADCAST_ADDR;
	appNwkDataReq.dstEndpoint = dst_endpoint;
	appNwkDataReq.srcEndpoint = src_endpoint;
	appNwkDataReq.options = NWK_OPT_BROADCAST_PAN_ID;// | NWK_OPT_ENABLE_SECURITY;
	appNwkDataReq.confirm = appDataConf;
	NWK_DataReq(&appNwkDataReq);
	appNwkDataReqBusy = true;
	appState = APP_STATE_WAIT_CONF;
}
/*************************************************************************//**
*****************************************************************************/
static bool appDataInd(NWK_DataInd_t *ind)
{
		//extract info from incoming message
		CommandsInBuffer[CommandsInIndexEnd][0] = ind->data[1] << 8 | ind->data[0];	//get incoming command ID
		CommandsInBuffer[CommandsInIndexEnd][1] = ind->srcAddr;	//get incoming node address
		CommandsInBuffer[CommandsInIndexEnd][2] = ind->data[3] << 8 | ind->data[2];	//get payload
		
		CommandsInIndexEnd++;
		if (CommandsInIndexEnd == CommandsInIndexSize)
		{
			CommandsInIndexEnd=0;
		}
	
	return true;
	appState = APP_STATE_IDLE;
}

static void appDBTTimerHandler(SYS_Timer_t *timer)
{
	ADC_GET_TEMPERATURE();
	(void)timer;
}

static void appLSDebounceTimerHandler(SYS_Timer_t *timer)
{
	LSDebounceSwitch();
	LSToggleQuiet();
	(void)timer;
}

static void appALSTimerHandler(SYS_Timer_t *timer)
{	
	ADC_GET_AMBIENT_LIGHT();
	(void)timer;
}

static void appAsyncJoinTimerHandler(SYS_Timer_t *timer)
{
	CommandsInBuffer[CommandsInIndexEnd][0] = 0x0101;	//get incoming command ID
	CommandsInBuffer[CommandsInIndexEnd][1] = 0;		//get incoming node address
	
	CommandsInIndexEnd++;
	if (CommandsInIndexEnd == CommandsInIndexSize)
	{
		CommandsInIndexEnd=0;
	}
	
	appState = APP_STATE_IDLE;
	(void)timer;
}

static void appAsyncJoinChainResetTimerHandler(SYS_Timer_t *timer)
{
	SysJoinChained = false;
}

static void appRTCTimerHandler(SYS_Timer_t *timer)
{
	LSModeCounter++;
	(void)timer;
}

static void appInit(void)
{   
    App_SerialNum = eeprom_read_dword(&eeSerialNum);
    App_SecurityKey = eeprom_read_dword(&eeSecurityKeyH);   //read the first 32 bits of the security key
    App_SecurityKey = App_SecurityKey << 32 | eeprom_read_dword(&eeSecurityKeyL);   //read last 32 bits of the security key
    App_ShortAddr = eeprom_read_word(&eeShortAddr);
    App_PanID = eeprom_read_word(&eeDefaultPan);
    App_Channel = eeprom_read_byte(&eeDefaultChannel);
	
	if (eeprom_read_word(&eeIdentity) != 0)
	{
		App_Identity = eeprom_read_word(&eeIdentity);
	}
	
	else
	{
		App_Identity = PHY_RandomReq();
		if (eeprom_is_ready())
		{
			eeprom_update_word((uint16_t*)&eeIdentity, App_Identity);
		}
	}

    NWK_SetAddr(App_ShortAddr);
	NWK_SetPanId(App_PanID);
	PHY_SetChannel(App_Channel);
	PHY_SetRxState(true);

#ifdef NWK_ENABLE_SECURITY
	NWK_SetSecurityKey((uint8_t *)App_SecurityKey);
#endif
	NWK_OpenEndpoint(APP_ENDPOINT, appDataInd);

    appDBTTimer.interval = 10000; //read temperature every 10s. Update temperature data every 180s.
    appDBTTimer.mode = SYS_TIMER_PERIODIC_MODE;
    appDBTTimer.handler = appDBTTimerHandler;
	
	appLSDebounceTimer.interval = 1;
	appLSDebounceTimer.mode = SYS_TIMER_PERIODIC_MODE;
	appLSDebounceTimer.handler = appLSDebounceTimerHandler;
 
    appALSTimer.interval = 1000;
    appALSTimer.mode = SYS_TIMER_PERIODIC_MODE;
    appALSTimer.handler = appALSTimerHandler;
	
	appRTCTimer.interval = 1000;
	appRTCTimer.mode = SYS_TIMER_PERIODIC_MODE;
	appRTCTimer.handler = appRTCTimerHandler;
	
	appAsyncJoinTimer.interval = 500 + App_ShortAddr;
	appAsyncJoinTimer.mode = SYS_TIMER_INTERVAL_MODE;
	appAsyncJoinTimer.handler = appAsyncJoinTimerHandler;
	
	appAsyncJoinChainResetTimer.interval = 0xffff;
	appAsyncJoinChainResetTimer.mode = SYS_TIMER_INTERVAL_MODE;
	appAsyncJoinChainResetTimer.handler = appAsyncJoinChainResetTimerHandler;
   
	SYS_TimerStart(&appRTCTimer);	//time starts ticking
    SYS_TimerStart(&appLSDebounceTimer);
	
	if (eeprom_read_byte(&eeSysJoined) == 1)
	{	
		if (!SYS_TimerStarted(&appDBTTimer))
		{
			SYS_TimerStart(&appDBTTimer);
		}
		
		if (!SYS_TimerStarted(&appALSTimer))
		{
			SYS_TimerStart(&appALSTimer);
		}
		
		LSConnectedMode = true;	
	}
 
	#ifdef PHY_ENABLE_RANDOM_NUMBER_GENERATOR
	  srand(PHY_RandomReq());
	#endif
	appState=APP_STATE_IDLE;
}

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
  switch (appState)
  {
    case APP_STATE_INITIAL:
	{
		appInit();
		break;
	}
	case APP_STATE_SEND:
	{
		appSendData(dst_node, APP_ENDPOINT, APP_ENDPOINT);
		appState = APP_STATE_IDLE;
		break;
	}
		
	case APP_STATE_BROADCAST:
	{
		appBroadcastData(APP_ENDPOINT, APP_ENDPOINT);
		appState = APP_STATE_IDLE;
		break;
	}
		
	case APP_STATE_LS_TOGGLE:
	{
		LSRemoteToggle = true;
		break;
	}
		
	case APP_STATE_LS_ON:
	{
		if (LSToggleState == false) //if light switch is OFF
		{
			LSRemoteToggle = true;	//toggle light switch
		}
		break;
	}
		
	case APP_STATE_LS_OFF:
	{
		if (LSToggleState == true)	//if light switch is ON
		{
			LSRemoteToggle = true;	//toggle light switch
		}
		break;
	}
		
	case APP_STATE_PUSH_CLIMATE:
	{
		CMDPushClimate();
		break;
	}
		
	case APP_STATE_PUSH_BRIGHTNESS:
	{
		CMDPushBrightness();
		break;
	}
		
	case APP_STATE_PUSH_LS_STATE:
	{
		CMDPushLSState();
		break;
	}
	
	case APP_STATE_IDLE:
	{
		LSStateMachine();
		CMDDataIOStateMachine();
		//SYNCStateMachine();
		break;
	}
		
    default:
	{
      break;
	}
  }
}
/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	SYS_Init();
	AdcInit();
  
	//settings for pre-production units
	DDRE = (0 << PORTE7) | (1 << PORTE4);
	PORTE = (1 << PORTE7) | (0 << PORTE4);
  
	sei();	//Enable global interrupts
  
	ADCSRA |= 1 << ADSC;	//start ADC conversion
  
	SystemSettingsSet();
	while (1)
	{
	SYS_TaskHandler();
	//HAL_UartTaskHandler();
	APP_TaskHandler();
	}  
}

ISR(ADC_vect)	//interrupt routine for ADC
{
	switch(ADMUX){
		case 0x40:	//ADC input channel 0
			//ADC_GET_RAW_TEMP();	//get PIR
			ADMUX = 0x41;	//change ADC input to channel 1
			break;
		case 0x41:	//ADC input channel 1
			ADC_GET_RAW_LIGHT();	//get 10 bit temperature
			ADMUX = 0x42;	//change ADC input to channel 2
			break;
		case 0x42:	//ADC input channel 2
			ADC_GET_RAW_TEMP();	//get 10 bit light
			ADMUX=0x41;	//change ADC input to channel 1
			break;
		default:
			break;
	}
	ADCSRA |= 1<<ADSC;	//start the next ADC conversion
}


void SYNCStateMachine(void)
{
	if (LSSyncMode == true)
	{
		//led[1].r=0; led[1].g=0; led[1].b=36;
		//ws2812_setleds(led,2);
		//LSConnectedMode = true;
	}
	else
	{
		//led[1].r=0; led[1].g=0; led[1].b=0;
		//ws2812_setleds(led,2);
	}
}

void SystemSettingsSet(void)
{
	LSRemoteModeSet();
	//LSSlaveModeGet();
}
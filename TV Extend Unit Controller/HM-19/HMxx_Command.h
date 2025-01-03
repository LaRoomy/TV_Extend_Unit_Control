#ifndef HMxx_COMMAND_H_
#define HMxx_COMMAND_H_

#define		TYPE_NOT_NEED_PIN_CODE			0
#define		TYPE_AUTHENTICATION_NO_PIN		1
#define		TYPE_AUTHENTICATION_WITH_PIN	2
#define		TYPE_AUTHENTICATION_AND_BOND	3

#define		ROLE_PERIPHERAL		0
#define		ROLE_CENTRAL		1

#define		WORKING_TYPE_IMMEDIATELY	0
#define		WORKING_TYPE_DO_NOTHING		1

#define		NO_STATUS_INFO				0
#define		STATUS_CONNECTING			1
#define		STATUS_CONNECTION_FAIL		2
#define		STATUS_CONNECTION_ERROR		3
#define		STATUS_CONNECTION_NO_ADRESS	4

//#define	COMPILE_FOR_HM10 // or HM-11
#define		COMPILE_FOR_HM18 //or HM-19

#ifdef	COMPILE_FOR_HM10

#define		BAUD_9600			0
#define		BAUD_19200			1
#define		BAUD_38400			2
#define		BAUD_57600			3
#define		BAUD_115200			4
#define		BAUD_4800			5
#define		BAUD_2400			6
#define		BAUD_1200			7
#define		BAUD_230400			8

#endif

#ifdef	COMPILE_FOR_HM18

#define		BAUD_1200			0
#define		BAUD_2400			1
#define		BAUD_4800			2
#define		BAUD_9600			3
#define		BAUD_19200			4
#define		BAUD_38400			5
#define		BAUD_57600			6
#define		BAUD_115200			7
#define		BAUD_230400			8

#endif

#include <stdio.h>
#include <string.h>

#include "../General/efficientStringProcessing.h"
#include "../Atmega324 specific/Atmega324_uart_vx.h"
#include "../General/cstDelay.h"

void HMxx_enableNotifications(BOOL enable);

BOOL _HMxx_isConnected;
BOOL _HMxx_isASleep;
BOOL _HMxx_macValid;

char HMxx_mac_adress[13];

uint8_t _HMxx_central_connection_status;
uint8_t _HMxx_role;

// NOTE: At this point we assume the module is already configured.
void HMxx_Init()
{
	Usart0_Init(USART0_BAUD);
	
	_HMxx_isASleep = FALSE;
	_HMxx_isConnected = FALSE;
	_HMxx_macValid = FALSE;
	_HMxx_central_connection_status = NO_STATUS_INFO;
	_HMxx_role = ROLE_PERIPHERAL;
	
	for(uint8_t i = 0; i < 13; i++)
	{
		HMxx_mac_adress[i] = '\0';
	}
	
	HMxx_mac_adress[0] = 'n';
	HMxx_mac_adress[1] = 'o';
	
	Usart0_Clear_RX_Buffer();
}

void HMxx_setName(char* name)// name maximum is 12 character!
{
	size_t sz = strlen(name);
	if(sz < 12)
	{
		char command[20];
		sprintf(command, "AT+NAME%s", name);
		Usart0_WriteString(command);
	}
}

void HMxx_enableNotifications(BOOL enable)
{
	if(enable)
		Usart0_WriteString("AT+NOTI1\0");
	else
		Usart0_WriteString("AT+NOTI0\0");
}

void HMxx_setTransmissionMode()
{
	Usart0_WriteString("AT+MODE0\0");
}

void HMxx_setType(uint8_t index)
{
	if(index < 4)
	{
		char command[9];
		sprintf(command, "AT+TYPE%u", index);
		command[8] = '\0';
		Usart0_WriteString(command);		
	}
}

void HMxx_setWorkingType(uint8_t wType)
{
	if(wType < 2)
	{
		char command[9];
		sprintf(command, "AT+IMME%u", wType);
		command[8] = '\0';
		Usart0_WriteString(command);
	}	
}

void HMxx_setRole(uint8_t role)
{
	_HMxx_role = role;

	if(role == 0)
		Usart0_WriteString("AT+ROLE0\0");
	else if(role == 1)
		Usart0_WriteString("AT+ROLE1\0");
}

void HMxx_setBaudrate(uint8_t baud_index)
{
	if(baud_index < 9)
	{
		char command[9];
		sprintf(command, "AT+BAUD%u", baud_index);
		command[8] = '\0';
		Usart0_WriteString(command);
	}	
}

BOOL HMxx_getConnectionStatus(){return _HMxx_isConnected;}
BOOL HMxx_getSleepStatus(){return _HMxx_isASleep;}
BOOL HMxx_isMACvalid(){return _HMxx_macValid;}
uint8_t HMxx_getCentralConnectingStatus(){return _HMxx_central_connection_status;}
void HMxx_resetCentralConnectingStatus(){_HMxx_central_connection_status = NO_STATUS_INFO;}
	
void HMxx_work()
{
	Usart0_WriteString("AT+START\0");
}

void HMxx_sleep()
{
	Usart0_WriteString("AT+SLEEP\0");
}
	
void HMxx_getMacAddress()
{
	Usart0_Clear_RX_Buffer();
	Usart0_WriteString("AT+ADDR?\0");
}

void HMxx_ClearLastConnectedDeviceAdress()
{
	Usart0_WriteString("AT+CLEAR\0");
}

void HMxx_ConnectToLastDevice()
{
	Usart0_Clear_RX_Buffer();
	Usart0_WriteString("AT+CONNL\0");
}

void HMxx_ConnectToAdress(char *adress)
{
	char command[20];
	command[17] = '\0';
	command[18] = '\0';
	sprintf(command, "AT+CON%s", adress);
	Usart0_Clear_RX_Buffer();
	Usart0_WriteString(command);
}

void HMxx_saveConnectedAdress(BOOL save)
{
	if(save)
		Usart0_WriteString("AT+SAVE0\0");
	else
		Usart0_WriteString("AT+SAVE1\0");
}

BOOL HMxx_onNotify(volatile char* str)
{
	BOOL result = FALSE;
	
	if(_HMxx_role == ROLE_CENTRAL)
	{
		if(stringStartsWith(str, "OK+CONN\0"))
		{
			if(USART0_RX_Buffer[7] != '\0')
			{
				switch(USART0_RX_Buffer[7])
				{
					case 'L':
					_HMxx_central_connection_status = STATUS_CONNECTING;
					break;
					case 'E':
					_HMxx_central_connection_status = STATUS_CONNECTION_ERROR;
					break;
					case 'F':
					_HMxx_central_connection_status = STATUS_CONNECTION_FAIL;
					break;
					case 'N':
					_HMxx_central_connection_status = STATUS_CONNECTION_NO_ADRESS;
					break;
					case 'A':
					_HMxx_central_connection_status = STATUS_CONNECTING;
					break;
					default:
					_HMxx_central_connection_status = NO_STATUS_INFO;
					break;
				}
				return TRUE;
			}
		}
	}
	/////////////////////////////////////////////////////////////////
	if(compareStringsExactly(str, "OK+CONN\0"))
	{
		// connection established
		_HMxx_isConnected = TRUE;
		result = TRUE;
		
		// if module was in sleep mode OK+WAKE will not be send but it will be awake
		_HMxx_isASleep = FALSE;
	}
	else if(compareStringsExactly(str, "OK+LOST\0"))
	{
		// connection lost
		_HMxx_isConnected = FALSE;
		result = TRUE;
	}
	else if(compareStringsExactly(str, "OK+SLEEP\0"))
	{
		// sleep mode enabled
		_HMxx_isASleep = TRUE;
		result = TRUE;
	}
	else if(compareStringsExactly(str, "OK+WAKE\0"))
	{
		// sleep mode terminated
		_HMxx_isASleep = FALSE;
		result = TRUE;
	}
	else if(stringStartsWith(str, "OK+ADDR:\0"))
	{
		if(usart0_transmission_complete)
		{
			for(uint8_t i = 8; i < 20; i++)
			{
				HMxx_mac_adress[i - 8] = str[i];
			}
			_HMxx_macValid = TRUE;
			result = TRUE;
		}
	}
	
	return result;
}

void HMxx_wakeUp()
{
	for(uint8_t i = 0; i < 10; i++)
		Usart0_WriteString("1234567890\0");
}

void HMxx_SendData(volatile char* data)
{
	Usart0_WriteString(data);
}

void HMxx_SendDataUnlimited(volatile char* data){

	uint8_t counter = 0;

	while((counter < 256) && (data[counter] != '\0')){
		Usart0_Write(data[counter]);
		counter++;
	}
}
#endif /* HMxx_COMMAND_H_ */
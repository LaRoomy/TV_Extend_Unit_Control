/*
 * TV Extend Unit Controller.c
 *
 * Created: 17.11.2020 07:44:09
 * Author : Hans Philipp Zimmermann
 */ 

#define F_CPU	9830400UL

#define USART0_BAUD		38400UL// TODO: set a higher value

#include "teuc.h"


// TODO: pin-change interrupt to make sure the sensors will not be overrun!?

// TODO: eeprom!


ISR(TIMER0_OVF_vect)
{
	if(timer_counter >= 38)
	{
		// 1 second interval
		timer_counter = 0;
		OneSecond_event = ACTIVATE;
	}
	else
	{
		timer_counter++;

		if(timer_counter == 14)
		{
			// 200 millisec interval (8)
			// TODO!!!!!!!!!!!!!!!!!!!!!!!!!
			TwoHMilliSec_event = ACTIVATE;
		}		
	}
}

ISR(USART0_RX_vect)
{
	Usart0_OnRXInterrupt();
}

int main(void)
{
    _delay_ms(200);
	InitializePorts();

	// set the default values for the global parameter
	InitGlobalValues();
	longDelay(20);

	// enable both, the cover- and tv-drive
	EnableApplianceDriver(TRUE);

	// We assume the hm-module is already configured
	// Parameter:
	//
	//	BAUDRATE =	BAUD_38400
	//	NAME =		LAROOMY_XNG001_BLUETOOTHNAME
	//	TYPE =		TYPE_NOT_NEED_PIN_CODE
	//	NOTI =		ENABLED!
	HMxx_Init();
	
	//HMxx_setBaudrate(BAUD_38400);
	//longDelay(200);
	//HMxx_setType(TYPE_NOT_NEED_PIN_CODE);
	//longDelay(100);
	//HMxx_setName(LAROOMY_XNG001_BLUETOOTHNAME);
	//longDelay(200);
	//Usart0_Clear_RX_Buffer();


	// get the current position of the driver-units and update the properties
	UpdateAppliancePosition(TRUE);

	// activate remaining modules
	ActivateTimer0();
	ADC_Init();

    while (1) 
    {
/**************************************************************************************************************************/
		// check main switch (touch button)
		if((!(TOUCHBUTTON_PIN & (1<<TOUCHBUTTON_SENSE)))&&(!switch_preventer))
		{
			longDelay(50);

			if(!(TOUCHBUTTON_PIN & (1<<TOUCHBUTTON_SENSE)))
			{
				switch_preventer = TRUE;

				StartApplianceDrive();
			}
		}
/**************************************************************************************************************************/
		// control drive
		ControlDriveProcess();

/**************************************************************************************************************************/
		// check board buttons
		CheckBoardButtons();

/**************************************************************************************************************************/
		// check security conditions

		// 1. door sensor(s)
		if(!checkDoorSensor())
		{
			EmergencyStop();
		}
		// 2. motor current

		updateMotorCurrentValues();

		// TODO!

/**************************************************************************************************************************/
		// check timer events
		if(OneSecond_event)
		{
			// reset global control parameter
			OneSecond_event = DISABLE;
			switch_preventer = FALSE;

			// raise component events
			TV_Unit_HandleOneSecondEvent();

			// temp: !!!*******************************************
			//pc1_enable(2);
			
			//if(activeMultiComplexPropertyID != 0)
			//{			
				//sendBarGraphInfo(updatecounter);
			//
				//updatecounter++;
				//if(updatecounter == 8)
					//updatecounter = 0;
			//}
			// **********************!!!
		}
		if(TwoHMilliSec_event)
		{
			if(activeMultiComplexPropertyID != 0)
			{
				if(HMxx_getConnectionStatus())
				{
					updateCurrentMonitorData();
				}
			}
			TwoHMilliSec_event = DISABLE;
		}
		
		
/**************************************************************************************************************************/
		// check bluetooth transmission
		if(usart0_transmission_data_available)// data is available (could be a control-command)
		{
			if((USART0_RX_Buffer[0] == 'O')&&(USART0_RX_Buffer[1] == 'K'))// if the buffer starts with "OK", it could be a control response or a bluetooth module notification -> check this
			{
				if(HMxx_onNotify(USART0_RX_Buffer))
				{
					if(!HMxx_getConnectionStatus())
					{
						//connectRequestSet = false;
					}
					Usart0_Clear_RX_Buffer();
				}
			}
		}
		if(usart0_transmission_complete)
		{
			AnalyzeTransmission(USART0_RX_Buffer);
			Usart0_Clear_RX_Buffer();
		}
    }
}


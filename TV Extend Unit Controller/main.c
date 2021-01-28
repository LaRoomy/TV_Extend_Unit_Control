/*
 * TV Extend Unit Controller.c
 *
 * Created: 17.11.2020 07:44:09
 * Author : Hans Philipp Zimmermann
 */ 

#define F_CPU	9830400UL

#define USART0_BAUD		38400UL

#include "teuc.h"



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

ISR(PCINT2_vect)
{
	ControlDriveProcess();
}

ISR(PCINT3_vect)
{
	ControlDriveProcess();
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
	ActivatePinChangeInterrupts();
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

				if(!StartApplianceDrive())
				{
					EmergencyStop(EMERGENCY_STOP_REASON_USER_STOPPED);
				}
			}
		}
/**************************************************************************************************************************/
		// control drive
		//ControlDriveProcess();

/**************************************************************************************************************************/
		// check board buttons
		CheckBoardButtons();

/**************************************************************************************************************************/
		// check security conditions

		// 1. door sensor(s)
		if(!checkDoorSensor())
		{
			EmergencyStop(EMERGENCY_STOP_REASON_DOORS_NOT_CLOSED);
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
			//TV_Unit_HandleOneSecondEvent();

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
					//if(!HMxx_getConnectionStatus())
					//{
						////connectRequestSet = false;
					//}
					Usart0_Clear_RX_Buffer();
				}
			}
		}
		if(usart0_transmission_complete)
		{
			AnalyzeTransmission(USART0_RX_Buffer);
			Usart0_Clear_RX_Buffer();
		}
		
/**************************************************************************************************************************/
		// Check flags

		if(executionFlags != 0)
		{
			
			// TODO: important: do not execute opposed flags!!!
			
			
			if(checkExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION_AND_PROPERTY))
			{
				// an asynchronous position and property update is requested
				UpdateAppliancePosition(TRUE);
				clearExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION_AND_PROPERTY);
				clearExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION);
			}
			else if(checkExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION))
			{
				// an asynchronous position update is requested
				UpdateAppliancePosition(FALSE);
				clearExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION);
			}
			//////////////////////////////////////////////////////////////////////
			if(checkExecutionFlag(FLAG_TVDRIVE_START_MOVE_OUT))
			{
				// the cover-drive is opened now -> start moving the tv-unit
				TV_Unit_Drive_Move_Out();
				clearExecutionFlag(FLAG_TVDRIVE_START_MOVE_OUT);
			}
			if(checkExecutionFlag(FLAG_COVERDRIVE_START_CLOSE))
			{
				// the tv-unit is moved in -> start closing the cover-drive
				CD_Unit_Drive_Close();
				clearExecutionFlag(FLAG_COVERDRIVE_START_CLOSE);
			}
			if(checkExecutionFlag(FLAG_TVDRIVE_START_SECUREPOSITION))
			{
				// the cover-drive position is secured -> now start securing the tv-unit position
				TV_Unit_StartSecurityDrive();
				clearExecutionFlag(FLAG_TVDRIVE_START_SECUREPOSITION);

				// from now on this should be a normal drive-in execution, so reset parameter
				executeCurrentDriveAsSecurityDrive = FALSE;
			}
			//////////////////////////////////////////////////////////////////////

			// ...
		}
		if(errorFlags != 0)
		{
			// temp:
			sbi(PORTA, PORTA5);
			///////////////////

			updateDevicePropertyToErrorStateFromErrorFlag();
			errorFlags = 0; // erase all flags
		}
		
		
/**************************************************************************************************************************/
		// temporary:
		
		//if(HMxx_getConnectionStatus())
		//{
			//sbi(PORTA, PORTA5);
		//}
		//else
		//{
			//cbi(PORTA, PORTA5);
		//}

    }
}


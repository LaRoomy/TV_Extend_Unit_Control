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

		if(timer_counter == 8)
		{
			// 200 millisec interval (8)
			TwoHMilliSec_event = ACTIVATE;
		}
		if(timer_counter == 19)
		{
			// 1/2 sec event
			HalfSecond_event = ACTIVATE;
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

	// disable both, the cover- and tv-drive
	EnableApplianceDriver(FALSE);

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
				// block the execution of the touch-button for 4 seconds
				switch_preventer = TRUE;
				switchPreventCounter = 0;

				if(!StartApplianceDrive())
				{
					EmergencyStop(EMERGENCY_STOP_REASON_USER_STOPPED);
				}
			}
		}
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

		// TODO !

		updateMotorCurrentValues();

		// TODO	!

/**************************************************************************************************************************/
		// check timer events
		if(OneSecond_event)
		{
			// reset global control parameter
			OneSecond_event = DISABLE;

			// control the switch-preventer to block double-execution
			if(switch_preventer)
			{
				if(switchPreventCounter >= 4)
				{
					switch_preventer = FALSE;
					switchPreventCounter = 0;
				}
				switchPreventCounter++;
			}
		}
		if(HalfSecond_event)
		{
			if(isDriveInProgress())
			{
				tbi(PORTA, PORTA5);
			}
			else
			{
				cbi(PORTA, PORTA5);
			}
			HalfSecond_event = DISABLE;
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
		// Check flags:
		// ___________________________________
		if(executionFlags != 0)
		{
			if(checkExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION_AND_PROPERTY))
			{
				// first clear the flags, because processing this action may set other flags, so nothing will be overwritten
				clearExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION_AND_PROPERTY);
				clearExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION);

				// an asynchronous position and property update is requested
				UpdateAppliancePosition(TRUE);
			}
			else if(checkExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION))
			{
				// first clear the flag, because processing this action may set other flags, so nothing will be overwritten
				clearExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION);

				// an asynchronous position update is requested
				UpdateAppliancePosition(FALSE);
			}
			//////////////////////////////////////////////////////////////////////
			if(checkExecutionFlag(FLAG_TVDRIVE_START_MOVE_OUT))
			{
				// first clear the flags, because processing this action may set other flags, so nothing will be overwritten
				eraseExecutionMovementFlags();// erase all flags related to a movement (it should not happen, but it makes sure there will be no conflict with the movement actions)

				// the cover-drive is opened now -> start moving the tv-unit
				TV_Unit_Drive_Move_Out();
			}
			if(checkExecutionFlag(FLAG_COVERDRIVE_START_CLOSE))
			{
				// first clear the flags, because processing this action may set other flags, so nothing will be overwritten
				eraseExecutionMovementFlags();// erase all flags related to a movement (it should not happen, but it makes sure there will be no conflict with the movement actions)

				// the tv-unit is moved in -> start closing the cover-drive
				CD_Unit_Drive_Close();
			}
			if(checkExecutionFlag(FLAG_TVDRIVE_START_SECUREPOSITION))
			{
				// first clear the flags, because processing this action may set other flags, so nothing will be overwritten
				eraseExecutionMovementFlags();// erase all flags related to a movement (it should not happen, but it makes sure there will be no conflict with the movement actions)

				// the cover-drive position is secured -> now start securing the tv-unit position
				TV_Unit_StartSecurityDrive();

				// from now on this should be a normal drive-in execution, so reset parameter
				executeCurrentDriveAsSecurityDrive = FALSE;
			}
			if(checkExecutionFlag(FLAG_DISABLE_DRIVER))
			{
				// wait to stop the motor
				longDelay(200);
				
				// disable the drivers to save energy
				EnableApplianceDriver(FALSE);
				clearExecutionFlag(FLAG_DISABLE_DRIVER);
			}
			//////////////////////////////////////////////////////////////////////

			// ...
		}
		if(errorFlags != 0)
		{
			// temp:
			//sbi(PORTA, PORTA5);
			///////////////////

			updateDeviceHeaderToErrorStateFromErrorFlag();
			errorFlags = 0; // erase all flags
		}
		
		// end of main loop...
/**************************************************************************************************************************/
    }
}


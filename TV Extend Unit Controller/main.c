/*
 * TV Extend Unit Controller.c
 *
 * Created: 17.11.2020 07:44:09
 * Author : Hans Philipp Zimmermann
 */ 

#define F_CPU	9830400UL

#define USART0_BAUD		9600// TODO: set a higher value

#include "teuc.h"

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

	InitGlobalValues();

	longDelay(20);
	enable_tvUnit_driver(TRUE);
	
	HMxx_Init();
	longDelay(100);

	updateTVUnitPosition();
	updateDevicePropertyFromTVUnitPosition();

	HMxx_setType(TYPE_NOT_NEED_PIN_CODE);
	longDelay(100);

	HMxx_setName(LAROOMY_XNG001_BLUETOOTHNAME);
	longDelay(200);

	Usart0_Clear_RX_Buffer();

	ActivateTimer0();

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
				// TODO: schedule a timer and set the preventer to false after a short period !!!!!!!!!!!

				
				if(checkDrivePreconditions())
				{
					TV_Unit_Drive_basedOnPosition();
				}
			}
		}
/**************************************************************************************************************************/
		// control drive
		TV_Unit_Control_DriveProcess();

/**************************************************************************************************************************/
		// check security conditions

		// 1. door sensor(s)
		if(!checkDoorSensor())
		{
			if(tv_unit_current_drive_mode != DRIVEMODE_NONE)
			{
				TV_Unit_EmergencyStop();
			}
		}
		// 2. motor current

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
			pc1_enable(2);
			// **********************!!!
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


/*
 * TV Extend Unit Controller.c
 *
 * Created: 17.11.2020 07:44:09
 * Author : Hans Philipp Zimmermann
 */ 

#define F_CPU	9830400UL

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

int main(void)
{
    _delay_ms(200);
	InitializePorts();

	longDelay(20);
	enable_tvUnit_driver(TRUE);

	// TODO: check the initial position and if the position is undefined: make a security drive

	updateTVUnitPosition();

	//if(tv_unit_current_position == POSITION_UNDEFINED)
	//{
		// position not in defined end-position -> make a security drive!
	//}

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
			TV_Unit_Raise_OneSecondEvent();

			// temp: !!!*******************************************
			pc1_enable(2);
			// **********************!!!
		}
		
		
/**************************************************************************************************************************/
		
		//uint8_t pos = linear_drive_check_position();
		//if(pos == FRONT_POSITION)
		//{
			//pc1_enable(TRUE);
		//}
		//else
		//{
			//pc1_enable(FALSE);
		//}

    }
}


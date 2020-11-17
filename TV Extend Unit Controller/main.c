/*
 * TV Extend Unit Controller.c
 *
 * Created: 17.11.2020 07:44:09
 * Author : Hans Philipp Zimmermann
 */ 

#define F_CPU	9830400UL

#include "teuc.h"

int main(void)
{
    _delay_ms(200);
	InitializePorts();

	longDelay(20);
	enable_tvUnit_driver(TRUE);

	// TODO: check the initial position and if the position is undefined: make a security drive

	updateTVUnitPosition();

	if(TV_Unit_Position == POSITION_UNDEFINED)
	{
		// position not in defined end-position -> make a security drive!
	}

    while (1) 
    {
		// check main switch (touch button)
		if(!(TOUCHBUTTON_PIN & (1<<TOUCHBUTTON_SENSE)))
		{
			longDelay(50);

			if(!(TOUCHBUTTON_PIN & (1<<TOUCHBUTTON_SENSE)))
			{
				switch_preventer = TRUE;
				// TODO: schedule a timer and set the preventer to false after a short period !!!!!!!!!!!

				//pc1_enable(2);
				
				//move_tilt_drive(MOVE_IN);
				
				//util_tiltdrive_front();
				
				if(current_drive_mode == DRIVEMODE_NONE)
				{
					TV_Unit_Drive_basedOnPosition();
				}
				
				
				longDelay(200);// delete???
			}
		}

		// control drive
		control_drive();
		
		
		
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


/*
 * tvUnitDriver.h
 *
 * Created: 17.11.2020 11:52:24
 *  Author: Hans Philipp Zimmermann
 */ 

#ifndef TVUNITDRIVER_H_
#define TVUNITDRIVER_H_

#include "driverDef.h"

uint8_t TV_Unit_Position = POSITION_UNDEFINED;
uint8_t current_drive_mode = DRIVEMODE_NONE;

void enable_tvUnit_driver(BOOL enable)
{
	//NOTE: writing this to zero enables the driver
	if(enable == TRUE)
	{
		cbi(TVUNIT_DRIVER_ENABLE_PORT, TV_UNIT_MOTORDRIVER_ENABLE);
	}
	else
	{
		sbi(TVUNIT_DRIVER_ENABLE_PORT, TV_UNIT_MOTORDRIVER_ENABLE);
	}
}

uint8_t linear_drive_check_position()
{
	BOOL back_pos = FALSE;
	BOOL mid_pos = FALSE;
	BOOL front_pos = FALSE;
	// NOTE: if a sensor is activated, the pin is pulled low

	if(!(LINEAR_DRIVE_SENSOR_PIN & (1<<LINEARDRIVE_SENSOR_BACK_POSITION)))
	{
		longDelay(50);

		if(!(LINEAR_DRIVE_SENSOR_PIN & (1<<LINEARDRIVE_SENSOR_BACK_POSITION)))
		{
			back_pos = TRUE;
		}
	}

	if(!(LINEAR_DRIVE_SENSOR_PIN & (1<<LINEARDRIVE_SENSOR_MID_POSITION)))
	{
		longDelay(50);

		if(!(LINEAR_DRIVE_SENSOR_PIN & (1<<LINEARDRIVE_SENSOR_MID_POSITION)))
		{
			mid_pos = TRUE;
		}
	}

	if(!(LINEAR_DRIVE_SENSOR_PIN & (1<<LINEARDRIVE_SENSOR_FRONT_POSITION)))
	{
		longDelay(50);

		if(!(LINEAR_DRIVE_SENSOR_PIN & (1<<LINEARDRIVE_SENSOR_FRONT_POSITION)))
		{
			front_pos = TRUE;
		}
	}

	// validate sensor results:
	if((back_pos && front_pos) || (back_pos && mid_pos) || (front_pos && mid_pos))
	{
		return SENSOR_ERROR;
	}
	else
	{
		if(back_pos)
			return BACK_POSTION;
		else if(mid_pos)
			return MID_POSITION;
		else if(front_pos)
			return FRONT_POSITION;
		else
			return POSITION_UNDEFINED;
	}
}

uint8_t tilt_drive_check_position()
{
	BOOL back_pos = FALSE;
	BOOL front_pos = FALSE;

	// NOTE: if a sensor is activated, the pin is pulled low

	if(!(TILT_DRIVE_SENSOR_PIN & (1<<TILTDRIVE_SENSOR_BACK_POSITION)))
	{
		longDelay(50);

		if(!(TILT_DRIVE_SENSOR_PIN & (1<<TILTDRIVE_SENSOR_BACK_POSITION)))
		{
			back_pos = TRUE;
		}
	}

	if(!(TILT_DRIVE_SENSOR_PIN & (1<<TILTDRIVE_SENSOR_FRONT_POSITION)))
	{
		longDelay(50);

		if(!(TILT_DRIVE_SENSOR_PIN & (1<<TILTDRIVE_SENSOR_FRONT_POSITION)))
		{
			front_pos = TRUE;
		}
	}

	// validate sensor results:
	if(back_pos && front_pos)
	{
		return SENSOR_ERROR;
	}
	else
	{
		if(back_pos)
			return BACK_POSTION;
		else if(front_pos)
			return FRONT_POSITION;
		else
			return POSITION_UNDEFINED;
	}
}

void move_linear_drive(uint8_t direction)
{
	// IN3 == 1 && IN4 == 0 -> move IN
	// IN3 == 0 && IN4 == 1 -> move OUT
	// IN3 == IN4 -> stop

	if(direction == STOP)
	{
		cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN3_TVUNIT_LINEARDRIVE);
		cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN4_TVUNIT_LINEARDRIVE);
		return;
	}

	// get current position:
	uint8_t pos = linear_drive_check_position();

	if(pos != SENSOR_ERROR)// make sure the sensor data is valid
	{
		if(direction == MOVE_OUT)
		{
			if(pos != FRONT_POSITION)// only move out if the front sensor is free
			{
				cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN3_TVUNIT_LINEARDRIVE);
				sbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN4_TVUNIT_LINEARDRIVE);
			}
		}
		else if(direction == MOVE_IN)
		{
			if(pos != BACK_POSTION)// only move in if the back sensor is free
			{
				sbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN3_TVUNIT_LINEARDRIVE);
				cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN4_TVUNIT_LINEARDRIVE);
			}
		}
		else
		{
			// stop if direction is invalid
			cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN3_TVUNIT_LINEARDRIVE);
			cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN4_TVUNIT_LINEARDRIVE);
		}
	}
}

void move_tilt_drive(uint8_t direction)
{
	// IN1 == 1 && IN2 == 0 -> move IN
	// IN1 == 0 && IN2 == 1 -> move OUT
	// IN1 == IN2 -> stop

	if(direction == STOP)// stop in any case
	{
		cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN1_TVUNIT_TILTDRIVE);
		cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN2_TVUNIT_TILTDRIVE);
		return;
	}

	// get current position:
	uint8_t pos = tilt_drive_check_position();

	if(pos != SENSOR_ERROR)// make sure the sensor data is valid
	{
		if(direction == MOVE_OUT)
		{
			if(pos != FRONT_POSITION)
			{
				cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN1_TVUNIT_TILTDRIVE);
				sbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN2_TVUNIT_TILTDRIVE);
			}
		}
		else if(direction == MOVE_IN)
		{
			if(pos != BACK_POSTION)
			{
				sbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN1_TVUNIT_TILTDRIVE);
				cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN2_TVUNIT_TILTDRIVE);
			}
		}
		else
		{
			// stop if direction is invalid
			cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN1_TVUNIT_TILTDRIVE);
			cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN2_TVUNIT_TILTDRIVE);
		}
	}
}

void updateTVUnitPosition()
{
	uint8_t ldPos = linear_drive_check_position();
	uint8_t tdPos = tilt_drive_check_position();

	if((ldPos == POSITION_UNDEFINED) || (tdPos == POSITION_UNDEFINED))
	{
		TV_Unit_Position = POSITION_UNDEFINED;
	}
	else
	{
		if((ldPos == BACK_POSTION) && (tdPos == BACK_POSTION))
		{
			TV_Unit_Position = BACK_POSTION;
		}
		else if((ldPos == FRONT_POSITION) && (tdPos == FRONT_POSITION))
		{
			TV_Unit_Position = FRONT_POSITION;
		}
		else if((ldPos == MID_POSITION) && (tdPos == BACK_POSTION))
		{
			TV_Unit_Position = MID_POSITION;
		}
		else
		{
			// position error -> make a security drive ???
			TV_Unit_Position = ERROR_POSITION;
		}
	}
}

BOOL TV_Unit_Drive_Move_Out()
{
	updateTVUnitPosition();

	if(TV_Unit_Position != BACK_POSTION)
	{
		// invalid command - position not valid
		return FALSE;
	}
	else
	{
		current_drive_mode = DRIVEMODE_LINEAR_OUT;

		move_linear_drive(MOVE_OUT);

		return TRUE;
	}
}

BOOL TV_Unit_Drive_Move_In()
{
	updateTVUnitPosition();

	if(TV_Unit_Position != FRONT_POSITION)
	{
		// invalid command - position not valid
		return FALSE;
	}
	else
	{
		current_drive_mode = DRIVEMODE_TILT_IN;

		move_tilt_drive(MOVE_IN);

		return TRUE;
	}
}

void control_drive()
{
	if(current_drive_mode != DRIVEMODE_NONE)
	{
		uint8_t linearPos = linear_drive_check_position();
		uint8_t tiltPos = tilt_drive_check_position();

		if(current_drive_mode == DRIVEMODE_LINEAR_OUT)
		{
			if(linearPos == FRONT_POSITION)
			{
				move_linear_drive(STOP);

				current_drive_mode = DRIVEMODE_TILT_OUT;

				move_tilt_drive(MOVE_OUT);
			}
		}
		else if(current_drive_mode == DRIVEMODE_TILT_OUT)
		{
			if(tiltPos == FRONT_POSITION)
			{
				move_tilt_drive(STOP);

				current_drive_mode = DRIVEMODE_NONE;

				updateTVUnitPosition();
			}
		}
		else if(current_drive_mode == DRIVEMODE_TILT_IN)
		{
			if(tiltPos == BACK_POSTION)
			{
				move_tilt_drive(STOP);

				current_drive_mode = DRIVEMODE_LINEAR_IN;

				move_linear_drive(MOVE_IN);
			}
		}
		else if(current_drive_mode == DRIVEMODE_LINEAR_IN)
		{
			if(linearPos == BACK_POSTION)
			{
				move_linear_drive(STOP);

				current_drive_mode = DRIVEMODE_NONE;

				updateTVUnitPosition();
			}
		}
	}
}



#endif /* TVUNITDRIVER_H_ */
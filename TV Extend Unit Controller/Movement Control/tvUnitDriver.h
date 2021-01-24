/*
 * tvUnitDriver.h
 *
 * Created: 17.11.2020 11:52:24
 *  Author: Hans Philipp Zimmermann
 */ 

#ifndef TVUNITDRIVER_H_
#define TVUNITDRIVER_H_

#include "driverDef.h"

volatile uint8_t tv_unit_current_position = POSITION_UNDEFINED;
volatile uint8_t tv_unit_current_drive_mode = DRIVEMODE_NONE;
volatile uint8_t tv_unit_drive_type = DRIVETYPE_SINGLE_DRIVE;


void setup_tvUnit_driveType(uint8_t driveType)
{
	tv_unit_drive_type = driveType;
}

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
		return POSITION_SENSOR_ERROR;
	}
	else
	{
		if(back_pos)
			return BACK_POSITION;
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
		return POSITION_SENSOR_ERROR;
	}
	else
	{
		if(back_pos)
			return BACK_POSITION;
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
	
	// Note:	The pins are inverted. To set a driver pin to '0'
	//			-> the µC pin must be set to '1'

	if(direction == STOP)
	{
		cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN3_TVUNIT_LINEARDRIVE);
		cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN4_TVUNIT_LINEARDRIVE);
		return;
	}

	// get current position:
	uint8_t pos = linear_drive_check_position();

	if(pos != POSITION_SENSOR_ERROR)// make sure the sensor data is valid
	{
		if(direction == MOVE_OUT)
		{
			if(pos != FRONT_POSITION)// only move out if the front sensor is free
			{
				sbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN3_TVUNIT_LINEARDRIVE);
				cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN4_TVUNIT_LINEARDRIVE);
			}
		}
		else if(direction == MOVE_IN)
		{
			if(pos != BACK_POSITION)// only move in if the back sensor is free
			{
				cbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN3_TVUNIT_LINEARDRIVE);
				sbi(LINEARDRIVE_MOVEMENT_INPUT_PORT, IN4_TVUNIT_LINEARDRIVE);
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
	
	// Note:	The pins are inverted. To set a driver pin to '0'
	//			-> the µC pin must be set to '1'

	if(direction == STOP)// stop in any case
	{
		cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN1_TVUNIT_TILTDRIVE);
		cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN2_TVUNIT_TILTDRIVE);
		return;
	}

	// get current position:
	uint8_t pos = tilt_drive_check_position();

	if(pos != POSITION_SENSOR_ERROR)// make sure the sensor data is valid
	{
		if(direction == MOVE_OUT)
		{
			if(pos != FRONT_POSITION)// only move out if the front position is not already reached
			{
				sbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN1_TVUNIT_TILTDRIVE);
				cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN2_TVUNIT_TILTDRIVE);
			}
		}
		else if(direction == MOVE_IN)
		{
			if(pos != BACK_POSITION)// only move in if the back position is not already reached
			{
				cbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN1_TVUNIT_TILTDRIVE);
				sbi(TILTDRIVE_MOVEMENT_INPUT_PORT, IN2_TVUNIT_TILTDRIVE);
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
		tv_unit_current_position = POSITION_UNDEFINED;
	}
	else if((ldPos == POSITION_SENSOR_ERROR) || (tdPos == POSITION_SENSOR_ERROR))
	{
		tv_unit_current_position = POSITION_SENSOR_ERROR;
	}
	else
	{
		if((ldPos == BACK_POSITION) && (tdPos == BACK_POSITION))
		{
			tv_unit_current_position = BACK_POSITION;
		}
		else if((ldPos == FRONT_POSITION) && (tdPos == FRONT_POSITION))
		{
			tv_unit_current_position = FRONT_POSITION;
		}
		else if((ldPos == MID_POSITION) && (tdPos == BACK_POSITION))
		{
			tv_unit_current_position = MID_POSITION;
		}
		else
		{
			// position error -> make a security drive?!

			//tv_unit_current_position = POSITION_SENSOR_ERROR;
			// this is a position error, not a sensor error, if a sensor error occurs, this will be recognized above!

			tv_unit_current_position = POSITION_UNDEFINED;
		}
	}
}

BOOL TV_Unit_Drive_Move_Out()
{
	// NOTE: check drivemode - no double execution!

	updateTVUnitPosition();

	if(tv_unit_current_position != BACK_POSITION)
	{
		// invalid command - position not valid
		return FALSE;
	}
	else
	{
		tv_unit_current_drive_mode = DRIVEMODE_LINEAR_OUT;

		move_linear_drive(MOVE_OUT);

		return TRUE;
	}
}

BOOL TV_Unit_Drive_Move_In()
{
	// NOTE: check drivemode - no double execution!
	
	updateTVUnitPosition();

	if(tv_unit_current_position != FRONT_POSITION)
	{
		// invalid command - position not valid
		return FALSE;
	}
	else
	{
		tv_unit_current_drive_mode = DRIVEMODE_TILT_IN;

		move_tilt_drive(MOVE_IN);

		return TRUE;
	}
}

void TV_Unit_StartSecurityDrive()
{
	updateTVUnitPosition();

	tv_unit_drive_type = DRIVETYPE_SECURITY_DRIVE;

	tv_unit_current_drive_mode = DRIVEMODE_TILT_IN;

	move_tilt_drive(MOVE_IN);

	updateDevicePropertyToSpecificCondition(UDP_SECURITY_DRIVE);
}

BOOL TV_Unit_Drive_basedOnPosition()
{
	if(tv_unit_current_drive_mode == DRIVEMODE_NONE)// check if drive is in progress
	{
		updateTVUnitPosition();
	
		if(tv_unit_current_position == FRONT_POSITION)
		{
			TV_Unit_Drive_Move_In();

			updateDevicePropertyToSpecificCondition(UDP_DRIVING_IN);
		}
		else if(tv_unit_current_position == BACK_POSITION)
		{
			TV_Unit_Drive_Move_Out();

			updateDevicePropertyToSpecificCondition(UDP_DRIVING_OUT);
		}
		else
		{
			if(tv_unit_current_position == POSITION_SENSOR_ERROR)
			{
				// contradictory sensor positions, must be a hardware conflict -> DO NOTHING!!! (maybe report error via bluetooth...!)

				updateDevicePropertyToSpecificCondition(UDP_DRIVING_ERROR);
			}
			else
			{			
				// invalid position
					//-> start security drive
				TV_Unit_StartSecurityDrive();
			}
		}
		return TRUE;
	}
	else // check if the last drive was interrupted
	{
		if(tv_unit_current_drive_mode == DRIVEMODE_EMERGENCY_STOP)
		{
			// drive was interrupted
				// -> start security drive
			TV_Unit_StartSecurityDrive();

			return TRUE;
		}
		else
		{
			// the function was invoked while the unit was driving
			return FALSE;
		}
	}
}


void control_drive_single()
{
	if(tv_unit_current_drive_mode != DRIVEMODE_NONE)
	{
		uint8_t linearPos = linear_drive_check_position();
		uint8_t tiltPos = tilt_drive_check_position();

		if(tv_unit_current_drive_mode == DRIVEMODE_LINEAR_OUT)
		{
			if(linearPos == FRONT_POSITION)
			{
				move_linear_drive(STOP);

				tv_unit_current_drive_mode = DRIVEMODE_TILT_OUT;

				move_tilt_drive(MOVE_OUT);
			}
		}
		else if(tv_unit_current_drive_mode == DRIVEMODE_TILT_OUT)
		{
			if(tiltPos == FRONT_POSITION)
			{
				move_tilt_drive(STOP);

				tv_unit_current_drive_mode = DRIVEMODE_NONE;

				updateTVUnitPosition();
				//updateDevicePropertyFromAppliancePosition();

				TVDriveReachedFrontPosition();
			}
		}
		else if(tv_unit_current_drive_mode == DRIVEMODE_TILT_IN)
		{
			if(tiltPos == BACK_POSITION)
			{
				move_tilt_drive(STOP);

				tv_unit_current_drive_mode = DRIVEMODE_LINEAR_IN;

				move_linear_drive(MOVE_IN);
			}
		}
		else if(tv_unit_current_drive_mode == DRIVEMODE_LINEAR_IN)
		{
			if(linearPos == BACK_POSITION)
			{
				move_linear_drive(STOP);

				tv_unit_current_drive_mode = DRIVEMODE_NONE;

				

				updateTVUnitPosition();
				//updateDevicePropertyFromAppliancePosition();

				TVDriveReachedBackPosition();
			}
		}
	}
}


void control_drive_security()
{
	if(tv_unit_current_drive_mode != DRIVEMODE_NONE)
	{
		uint8_t linearPos = linear_drive_check_position();
		uint8_t tiltPos = tilt_drive_check_position();

		if(tv_unit_current_drive_mode == DRIVEMODE_TILT_IN)
		{
			if(tiltPos == BACK_POSITION)
			{
				move_tilt_drive(STOP);

				tv_unit_current_drive_mode = DRIVEMODE_LINEAR_IN;

				move_linear_drive(MOVE_IN);
			}
		}
		else if(tv_unit_current_drive_mode == DRIVEMODE_LINEAR_IN)
		{
			if(linearPos == BACK_POSITION)
			{
				move_linear_drive(STOP);

				tv_unit_current_drive_mode = DRIVEMODE_NONE;
				tv_unit_drive_type = DRIVETYPE_SINGLE_DRIVE;

				updateTVUnitPosition();
				//updateDevicePropertyFromAppliancePosition();

				TVDriveReachedBackPosition();
			}
		}
	}
}

void TV_Unit_Control_DriveProcess()
{
	switch(tv_unit_drive_type)
	{
		case DRIVETYPE_SINGLE_DRIVE:
			control_drive_single();
			break;
		case DRIVETYPE_SECURITY_DRIVE:
			control_drive_security();
			break;
		default:
			break;
	}
}

void TV_Unit_EmergencyStop()
{
	move_linear_drive(STOP);
	move_tilt_drive(STOP);
	tv_unit_current_drive_mode = DRIVEMODE_EMERGENCY_STOP;

	//updateDevicePropertyToSpecificCondition(UDP_DRIVE_INTERRUPT);
}

//void TV_Unit_HandleOneSecondEvent()
//{
	//if((tv_unit_drive_type == DRIVETYPE_FUSION_DRIVE) && (tv_unit_current_drive_mode == DRIVEMODE_TILT_IN))
	//{
		//tvu_sec_counter++;
	//}
//}

BOOL isTV_Unit_Drive_In_Progress()
{
	return ((tv_unit_current_drive_mode > DRIVEMODE_NONE)&&(tv_unit_current_drive_mode < DRIVEMODE_ITERATION_END)) ? TRUE : FALSE;
}

// **********************************************************************************************************************//
// maintenance utilities >>

void util_lineardrive_home()
{	
	uint8_t pos = linear_drive_check_position();
	
	if(pos != BACK_POSITION)
	{
		move_linear_drive(MOVE_IN);
	
		while(1){
			
			pos = linear_drive_check_position();
			if(pos == BACK_POSITION)
			{
				move_linear_drive(STOP);
				break;
			}
			longDelay(20);
		}
	}
}

void util_lineardrive_front()
{
	uint8_t pos = linear_drive_check_position();
	
	if(pos != FRONT_POSITION)
	{
		move_linear_drive(MOVE_OUT);
		
		while(1){
			
			pos = linear_drive_check_position();
			if(pos == FRONT_POSITION)
			{
				move_linear_drive(STOP);
				break;
			}
			longDelay(20);
		}
	}
}

void util_tiltdrive_home()
{
	uint8_t pos = tilt_drive_check_position();
	
	if(pos != BACK_POSITION)
	{
		move_tilt_drive(MOVE_IN);
		
		while(1){
			
			pos = tilt_drive_check_position();
			if(pos == BACK_POSITION)
			{
				move_tilt_drive(STOP);
				break;
			}
			longDelay(20);
		}
	}
}

void util_tiltdrive_front()
{
	uint8_t pos = tilt_drive_check_position();
	
	if(pos != FRONT_POSITION)
	{
		move_tilt_drive(MOVE_OUT);
		
		while(1){
			
			pos = tilt_drive_check_position();
			if(pos == FRONT_POSITION)
			{
				move_tilt_drive(STOP);
				break;
			}
			longDelay(20);
		}
	}
}

void stepTVDrive(uint8_t direction)
{
	switch(direction)
	{
		case STEP_DIR_LINEAR_TOBACK:
			move_linear_drive(MOVE_IN);
			longDelay(200);
			move_linear_drive(STOP);
			break;
		case STEP_DIR_LINEAR_TOFRONT:
			move_linear_drive(MOVE_OUT);
			longDelay(200);
			move_linear_drive(STOP);
			break;
		case STEP_DIR_TILT_TOBACK:
			move_tilt_drive(MOVE_IN);
			longDelay(200);
			move_tilt_drive(STOP);
			break;
		case STEP_DIR_TILT_TOFRONT:
			move_tilt_drive(MOVE_OUT);
			longDelay(200);
			move_tilt_drive(STOP);
			break;
		default:
			break;			
	}
}



#endif /* TVUNITDRIVER_H_ */
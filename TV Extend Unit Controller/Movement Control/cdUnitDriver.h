/*
 * cdUnitDriver.h
 *
 * Created: 18.01.2021 10:44:27
 *  Author: Hans Philipp Zimmermann
 */ 


#ifndef CDUNITDRIVER_H_
#define CDUNITDRIVER_H_

#include "driverDef.h"

uint8_t cdUnit_currentPosition = POSITION_UNDEFINED;
uint8_t cdUnit_currentDriveMode = DRIVEMODE_NONE;

void enable_cdUnit_Driver(BOOL enable)
{
	//NOTE: writing this to zero enables the driver
	if(enable)
	{
		cbi(COVERDRIVE_DRIVER_ENABLE_PORT, COVERDRIVE_UNITDRIVER_ENABLE);
	}
	else
	{
		sbi(COVERDRIVE_DRIVER_ENABLE_PORT, COVERDRIVE_UNITDRIVER_ENABLE);
	}
}

uint8_t checkLeftCoverPosition()
{
	BOOL openPos = FALSE;
	BOOL closePos = FALSE;

	// get sensor data:
	if(!(COVERDRIVE_SENSOR_PIN & COVERDRIVE_SENSOR_LEFT_CLOSE))
	{
		closePos = TRUE;	
	}
	if(!(COVERDRIVE_SENSOR_PIN & COVERDRIVE_SENSOR_LEFT_OPEN))
	{
		openPos = TRUE;
	}
	// analyze sensor data:
	if(closePos && openPos)
	{
		return POSITION_SENSOR_ERROR;
	}
	else if(closePos)
	{
		return CLOSED_POSTION;
	}
	else if(openPos)
	{
		return OPENED_POSITION;
	}
	else
	{
		return POSITION_UNDEFINED;
	}
}

uint8_t checkRightCoverPosition()
{
	BOOL openPos = FALSE;
	BOOL closePos = FALSE;

	// get sensor data:
	if(!(COVERDRIVE_SENSOR_PIN & COVERDRIVE_SENSOR_RIGHT_CLOSE))
	{
		closePos = TRUE;
	}
	if(!(COVERDRIVE_SENSOR_PIN & COVERDRIVE_SENSOR_RIGHT_OPEN))
	{
		openPos = TRUE;
	}
	// analyze sensor data:
	if(closePos && openPos)
	{
		return POSITION_SENSOR_ERROR;
	}
	else if(closePos)
	{
		return CLOSED_POSTION;
	}
	else if(openPos)
	{
		return OPENED_POSITION;
	}
	else
	{
		return POSITION_UNDEFINED;
	}
}

void updateCDUnitPosition()
{
	uint8_t leftPosition = checkLeftCoverPosition();
	uint8_t rightPosition = checkRightCoverPosition();

	if((leftPosition == POSITION_UNDEFINED) || (rightPosition == POSITION_UNDEFINED))
	{
		cdUnit_currentPosition = POSITION_UNDEFINED;
	}
	else if((leftPosition == POSITION_SENSOR_ERROR) || (rightPosition == POSITION_SENSOR_ERROR))
	{
		cdUnit_currentPosition = POSITION_SENSOR_ERROR;
	}
	else
	{
		if((leftPosition == OPENED_POSITION) && (rightPosition == OPENED_POSITION))
		{
			cdUnit_currentPosition = OPENED_POSITION;
		}
		else if((leftPosition == CLOSED_POSTION) && (rightPosition == CLOSED_POSTION))
		{
			cdUnit_currentPosition = CLOSED_POSTION;
		}
		else
		{
			cdUnit_currentPosition = POSITION_UNDEFINED;
		}
	}
}

void moveRightDrive(uint8_t direction)
{
	// IN1 == 1 && IN2 == 0 -> open
	// IN1 == 0 && IN2 == 1 -> close
	// IN1 == IN2 -> stop

	// Note:	The pins are inverted. To set a driver pin to '0'
	//			-> the µC pin must be set to '1'
	if(direction == STOP)
	{
		cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN1_COVERDRIVE_RIGHT);
		cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN2_COVERDRIVE_RIGHT);
		return;
	}

	// get current position and drive only if the direction is valid (prevent overrun)
	uint8_t pos = checkRightCoverPosition();

	if(pos != POSITION_SENSOR_ERROR)
	{
		if(direction == MOVE_OPEN)
		{
			if(pos != OPENED_POSITION)// only move if the opened-sensor is free
			{
				sbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN1_COVERDRIVE_RIGHT);
				cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN2_COVERDRIVE_RIGHT);
			}
		}
		else if(direction == MOVE_CLOSE)
		{
			if(pos != CLOSED_POSTION)// only move if the close-sensor is free
			{
				cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN1_COVERDRIVE_RIGHT);
				sbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN2_COVERDRIVE_RIGHT);
			}
		}
		else
		{
			// stop if direction-command is invalid
			cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN1_COVERDRIVE_RIGHT);
			cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN2_COVERDRIVE_RIGHT);
		}
	}
}

void moveLeftDrive(uint8_t direction)
{
	// IN3 == 1 && IN4 == 0 -> open
	// IN3 == 0 && IN4 == 1 -> close
	// IN3 == IN4 -> stop

	// Note:	The pins are inverted. To set a driver pin to '0'
	//			-> the µC pin must be set to '1'
	if(direction == STOP)
	{
		cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN3_COVERDRIVE_LEFT);
		cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN4_COVERDRIVE_LEFT);
		return;
	}

	// get current position and drive only if the direction is valid (prevent overrun)
	uint8_t pos = checkLeftCoverPosition();

	if(pos != POSITION_SENSOR_ERROR)
	{
		if(direction == MOVE_OPEN)
		{
			if(pos != OPENED_POSITION)// only move if the opened-sensor is free
			{
				sbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN3_COVERDRIVE_LEFT);
				cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN4_COVERDRIVE_LEFT);
			}
		}
		else if(direction == MOVE_CLOSE)
		{
			if(pos != CLOSED_POSTION)// only move if the close-sensor is free
			{
				cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN3_COVERDRIVE_LEFT);
				sbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN4_COVERDRIVE_LEFT);
			}
		}
		else
		{
			// stop if direction-command is invalid
			cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN3_COVERDRIVE_LEFT);
			cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN4_COVERDRIVE_LEFT);
		}
	}
}

BOOL CD_Unit_Drive_Open()
{
	// NOTE: check drivemode - no double execution!

	updateCDUnitPosition();

	if(cdUnit_currentPosition != CLOSED_POSTION)
	{
		// invalid command - position not valid!
		return FALSE;
	}
	else
	{
		cdUnit_currentDriveMode = DRIVEMODE_CD_OPEN;

		moveLeftDrive(MOVE_OPEN);
		moveRightDrive(MOVE_OPEN);

		return TRUE;
	}
}

BOOL CD_Unit_Drive_Close()
{
	// NOTE: check drivemode - no double execution!

	updateCDUnitPosition();

	if(cdUnit_currentPosition != OPENED_POSITION)
	{
		// invalid command - position not valid
		return FALSE;
	}
	else
	{
		cdUnit_currentDriveMode = DRIVEMODE_CD_CLOSE;

		moveLeftDrive(MOVE_CLOSE);
		moveRightDrive(MOVE_CLOSE);

		return TRUE;
	}
}

void CD_Unit_Control_Drive_Process()
{
	// only execute if a drive is in progress
	if(cdUnit_currentDriveMode != DRIVEMODE_NONE)
	{
		// get positions
		uint8_t leftPos = checkLeftCoverPosition();
		uint8_t rightPos = checkRightCoverPosition();

		if(cdUnit_currentDriveMode == DRIVEMODE_CD_OPEN)
		{
			// check if a endpoint is reached and stop the motor, respectively
			if(leftPos == OPENED_POSITION)
			{
				moveLeftDrive(STOP);
			}
			if(rightPos == OPENED_POSITION)
			{
				moveRightDrive(STOP);
			}
			// if both have reached the endpoint, reset parameter and report
			if((leftPos == OPENED_POSITION) && (rightPos == OPENED_POSITION))
			{
				cdUnit_currentDriveMode = DRIVEMODE_NONE;

				// update position parameter
				updateCDUnitPosition();

				// invoke callback to start tv-drive
				CoverDriveReachedOpenedPosition();
			}
		}
		else if(cdUnit_currentDriveMode == DRIVEMODE_CD_CLOSE)
		{
			// check if a endpoint is reached and stop the motor, respectively
			if(leftPos == CLOSED_POSTION)
			{
				moveLeftDrive(STOP);
			}
			if(rightPos == CLOSED_POSTION)
			{
				moveRightDrive(STOP);
			}
			// if both have reached the endpoint, reset parameter
			if((leftPos == CLOSED_POSTION) && (rightPos == CLOSED_POSTION))
			{
				cdUnit_currentDriveMode = DRIVEMODE_NONE;

				// update position parameter
				updateCDUnitPosition();
			}
		}
	}
}

void CD_Unit_Emergency_Stop()
{
	moveRightDrive(STOP);
	moveLeftDrive(STOP);
	cdUnit_currentDriveMode = DRIVEMODE_EMERGENCY_STOP;
}

BOOL isCD_Unit_Drive_In_Progress()
{
	return ((cdUnit_currentDriveMode == DRIVEMODE_CD_OPEN) || (cdUnit_currentDriveMode == DRIVEMODE_CD_CLOSE)) ? TRUE : FALSE;
}

// **********************************************************************************************************************//
// maintenance utilities >>

void stepCoverDrive(uint8_t direction)
{
	switch(direction)
	{
		case STEP_DIR_CDLEFT_CLOSE:
			moveLeftDrive(MOVE_CLOSE),
			longDelay(200);
			moveLeftDrive(STOP);
			break;
		case STEP_DIR_CDLEFT_OPEN:
			moveLeftDrive(MOVE_OPEN);
			longDelay(200);
			moveLeftDrive(STOP);
			break;
		case STEP_DIR_CDRIGHT_CLOSE:
			moveRightDrive(MOVE_CLOSE);
			longDelay(200);
			moveRightDrive(STOP);
			break;
		case STEP_DIR_CDRIGHT_OPEN:
			moveRightDrive(MOVE_OPEN);
			longDelay(200);
			moveRightDrive(STOP);
			break;
		default:
			break;
	}
}



#endif /* CDUNITDRIVER_H_ */
/*
 * TEUC.h
 *
 * Created: 17.11.2020 08:19:13
 *  Author: Hans Philipp Zimmermann
 */ 


#ifndef TEUC_H_
#define TEUC_H_

#include <avr/io.h>

#define FALSE	0
#define TRUE	1

typedef uint8_t BOOL;

#include "General/BitUtilitiesEx.h"
#include "General/cstDelay.h"

#include "TEUC PPDef.h"
#include "Movement Control/tvUnitDriver.h"

#include "Atmega324 specific/atmega324_timer.h"

BOOL switch_preventer;

void InitControlParameter()
{
	switch_preventer = FALSE;
}

// check if the doors are closed
BOOL checkDoorSensor()
{
	// PORTC0 == 1 -> doors are closed
	// PORTC0 == 0 -> one or more doors are open
	if(!(DOOR_SENSOR_PIN & (1<<DOOR_SENSOR)))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


BOOL checkDrivePreconditions()
{
	// check all conditions which must be fulfilled to start the drive
	return checkDoorSensor();
}


// test section

void pc1_enable(BOOL enable)
{
	if(enable == TRUE)
		sbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN1_COVERDRIVE_RIGHT);
	else if(enable == FALSE)
		cbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN1_COVERDRIVE_RIGHT);
	else
		tbi(COVERDRIVE_MOVEMENT_INPUT_PORT, IN1_COVERDRIVE_RIGHT);
}



#endif /* TEUC_H_ */
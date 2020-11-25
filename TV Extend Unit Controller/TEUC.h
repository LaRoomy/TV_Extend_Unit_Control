/*
 * TEUC.h
 *
 * Created: 17.11.2020 08:19:13
 *  Author: Hans Philipp Zimmermann
 */ 


#ifndef TEUC_H_
#define TEUC_H_

#include <avr/io.h>

#define		UDP_DRIVING_OUT		1
#define		UDP_DRIVING_IN		2
#define		UDP_SECURITY_DRIVE	3
#define		UDP_DRIVE_INTERRUPT	4
#define		UDP_DRIVING_ERROR	5

#include "General/BitUtilitiesEx.h"
#include "General/cstDelay.h"

#include "GlobalValueCtrl.h"

void updateDevicePropertyToSpecificCondition(uint8_t direction);

#include "TEUC PPDef.h"
#include "Movement Control/tvUnitDriver.h"


#include "Atmega324 specific/atmega324_timer.h"

#include "Device Property Control/LaRoomyAppCon.h"

//BOOL switch_preventer;
//
//void InitControlParameter()
//{
	//switch_preventer = FALSE;
//}

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

void updateDevicePropertyFromTVUnitPosition()
{
	uint8_t statusTextIndex = getPropertyIndexFromID(STATUS_TEXT_DISPLAY);
	uint8_t inOutButtonIndex = getPropertyIndexFromID(INOUT_DRIVE_STARTBUTTON);

	if(tv_unit_current_position == POSITION_UNDEFINED)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_QUESTIONCIRCLE_BLUE;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Status: unbestimmt");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "State: undefined");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_WARNING_YELLOW;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Sicher einfahren");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Safely pull in");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(statusTextIndex, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(inOutButtonIndex, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}
		
	}
	else if(tv_unit_current_position == FRONT_POSITION)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_TV_BLUEWHITE;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Status: Ausgefahren");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "State: Pulled Out");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_ARROW_UP_WHITE;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Einfahren");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Pull in");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(statusTextIndex, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(inOutButtonIndex, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}		
	}
	else if(tv_unit_current_position == BACK_POSITION)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_TV_WHITE;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Status: Eingefahren");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "State: Pulled in");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_ARROW_DOWN_WHITE;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Ausfahren");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Pull out");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(statusTextIndex, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(inOutButtonIndex, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}		
	}

	// if hm-19 is connected -> notify property changed!

}

void updateDevicePropertyToSpecificCondition(uint8_t direction)
{
	uint8_t statusTextIndex = getPropertyIndexFromID(STATUS_TEXT_DISPLAY);
	uint8_t inOutButtonIndex = getPropertyIndexFromID(INOUT_DRIVE_STARTBUTTON);

	if(direction == UDP_DRIVING_IN)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_SYNC_LRSTYLE;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Wird eingefahren...");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "Pulling in...");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_CIRCLE_CROSSED;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Stop");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Stop");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(STATUS_TEXT_DISPLAY, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(INOUT_DRIVE_STARTBUTTON, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}
	}
	else if(direction == UDP_DRIVING_OUT)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_SYNC_LRSTYLE;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Wird ausgefahren...");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "Pulling out...");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_CIRCLE_CROSSED;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Stop");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Stop");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(STATUS_TEXT_DISPLAY, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(INOUT_DRIVE_STARTBUTTON, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}		
	}
	else if(direction == UDP_SECURITY_DRIVE)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_LOCK_LRSTYLE;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Position sichern...");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "Secure Position...");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_CIRCLE_CROSSED;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Stop");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Stop");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(STATUS_TEXT_DISPLAY, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(INOUT_DRIVE_STARTBUTTON, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}		
	}
	else if(direction == UDP_DRIVE_INTERRUPT)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_WARNING_YELLOW;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Status: unterbrochen");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "State: interrupted");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_CIRCLE_EMPTY;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Sicher Einfahren");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Secure Position");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(STATUS_TEXT_DISPLAY, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(INOUT_DRIVE_STARTBUTTON, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}
	}
	else if(direction == UDP_DRIVING_ERROR)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_WARNING_RED;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Fehler !");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "Error !");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_CIRCLE_EMPTY;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Erneut versuchen");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Try again");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(STATUS_TEXT_DISPLAY, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(INOUT_DRIVE_STARTBUTTON, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}		
	}
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
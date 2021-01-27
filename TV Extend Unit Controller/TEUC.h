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

#define		APPLIANCE_POSITON_UNDEFINED		0
#define		APPLIANCE_POSITION_OUT			1
#define		APPLIANCE_POSITION_IN			2
#define		APPLIANCE_POSITION_SENSOR_ERROR	3

#include "General/BitUtilitiesEx.h"
#include "General/cstDelay.h"

#include "GlobalValueCtrl.h"

void updateDevicePropertyToSpecificCondition(uint8_t direction);
void updateDevicePropertyFromAppliancePosition();
void updateDevicePropertyToErrorStateFromErrorFlag();

#include "TEUC PPDef.h"

void CoverDriveReachedOpenedPosition();
void TVDriveReachedBackPosition();
void CoverDriveReachedClosedPosition();
void TVDriveReachedFrontPosition();

#include "Movement Control/tvUnitDriver.h"
#include "Movement Control/cdUnitDriver.h"

BOOL StartApplianceDrive();
void EmergencyStop(uint8_t reason);

#include "Atmega324 specific/atmega324_timer.h"
#include "Device Property Control/LaRoomyAppCon.h"
#include "Atmega324 specific/atmega324_adc.h"
#include "Atmega324 specific/atmega324_pcint.h"


// check if the doors are closed
BOOL checkDoorSensor()
{
	// PORTC0 == 1 -> doors are closed
	// PORTC0 == 0 -> one or more doors are open
	if(!(DOOR_SENSOR_PIN & (1<<DOOR_SENSOR)))
	{
		longDelay(20);

		if(!(DOOR_SENSOR_PIN & (1<<DOOR_SENSOR)))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
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

void UpdateAppliancePosition(BOOL updateProperty)
{
	updateCDUnitPosition();
	updateTVUnitPosition();

	// TODO: make a global position including both component conditions!!!!!!!!!!!!!!

	if((cdUnit_currentPosition == OPENED_POSITION) && (tv_unit_current_position == FRONT_POSITION))
	{
		// tv is out!
		currentAppliancePosition = APPLIANCE_POSITION_OUT;
	}
	else if((cdUnit_currentPosition == CLOSED_POSTION) && (tv_unit_current_position == BACK_POSITION))
	{
		// tv is in!
		currentAppliancePosition = APPLIANCE_POSITION_IN;
	}
	else
	{
		if((tv_unit_current_position == POSITION_SENSOR_ERROR) || (cdUnit_currentPosition == POSITION_SENSOR_ERROR))
		{
			// SENSOR ERROR MEANS: OPPOSED SENSORS ARE ENGAGED -> NO DRIVE IS POSSIBLE !!!
			currentAppliancePosition = APPLIANCE_POSITION_SENSOR_ERROR;
			
			// determine the defective sensor pair:
			if(tv_unit_current_position == POSITION_SENSOR_ERROR)
			{
				uint8_t linPos = linear_drive_check_position();
				uint8_t tiltPos = tilt_drive_check_position();
				
				if(linPos == POSITION_SENSOR_ERROR)
				{
					setErrorFlag(FLAG_TVDRIVE_LIN_SENSOR_ERROR);
				}
				if(tiltPos == POSITION_SENSOR_ERROR)
				{
					setErrorFlag(FLAG_TVDRIVE_TILT_SENSOR_ERROR);
				}
			}
			if(cdUnit_currentPosition == POSITION_SENSOR_ERROR)
			{
				uint8_t leftPos = checkLeftCoverPosition();
				uint8_t rightPos = checkRightCoverPosition();
				
				if(leftPos == POSITION_SENSOR_ERROR)
				{
					setErrorFlag(FLAG_CDDRIVE_LEFT_SENSOR_ERROR);
				}
				if(rightPos == POSITION_SENSOR_ERROR)
				{
					setErrorFlag(FLAG_CDDRIVE_RIGHT_SENSOR_ERROR);
				}
			}
		}
		else
		{
			currentAppliancePosition = APPLIANCE_POSITON_UNDEFINED;	
		}
	}
	if(updateProperty)
	{
		// TODO: this will also be invoked in interrupt vector, so do that in main with the timer!?
		
		updateDevicePropertyFromAppliancePosition();
	}
}

void EnableApplianceDriver(BOOL enable)
{
	enable_tvUnit_driver(enable);
	enable_cdUnit_Driver(enable);
}

void ControlDriveProcess()
{
	// only invoke the appropriate function for the specific process or none if no drive is active?!

	TV_Unit_Control_DriveProcess();
	CD_Unit_Control_Drive_Process();
}

void EmergencyStop(uint8_t reason)
{
	BOOL isCDDriving = isCD_Unit_Drive_In_Progress();
	BOOL isTVDriving = isTV_Unit_Drive_In_Progress();

	if(isCDDriving || isTVDriving)
	{	
		if(isCDDriving)
		{
			CD_Unit_Emergency_Stop();
		}
		if(isTVDriving)
		{
			TV_Unit_EmergencyStop();
		}

		if(reason == EMERGENCY_STOP_REASON_DOORS_NOT_CLOSED)
		{
			setErrorFlag(FLAG_DOORSENSOR_ERROR);
		}
		else if(reason == EMERGENCY_STOP_REASON_USER_STOPPED)
		{

			// TODO: save condition to proceed on request

			updateDevicePropertyToSpecificCondition(UDP_DRIVE_INTERRUPT);	
		}
	}
}

void StartSecurityDrive()
{
	// set parameter
	executeCurrentDriveAsSecurityDrive = TRUE;

	// update property
	updateDevicePropertyToSpecificCondition(UDP_SECURITY_DRIVE);

	// open the cover-drive (secure position)
	CD_Unit_Drive_Open();
}

BOOL StartApplianceDrive()
{
	// check preconditions
	if(checkDrivePreconditions())
	{
		// check if a drive is in progress
		BOOL isCDDriving = isCD_Unit_Drive_In_Progress();
		BOOL isTVDriving = isTV_Unit_Drive_In_Progress();

		if(isCDDriving || isTVDriving)
		{
			
			
			// this could not happen, or what???? Drivemode can not be emergency stop!?
			
			
			// drive is in progress or in interrupted state -> check
			if((tv_unit_current_drive_mode == DRIVEMODE_EMERGENCY_STOP) || (cdUnit_currentDriveMode == DRIVEMODE_EMERGENCY_STOP))
			{
				// the last drive was interrupted -> make security drive or proceed the last drive ???
				
				// TODO!!!!
			}
			else
			{
				// the drive must be in progress -> stop or return indicator to stop!!!


				// TODO !!!!!!!!!!!!!!!!

				return FALSE;
			}
		}
		else
		{
			// get position and drive based on that condition
			UpdateAppliancePosition(FALSE);

			if(currentAppliancePosition == APPLIANCE_POSITION_IN)
			{
				// move out:
				CD_Unit_Drive_Open();

				updateDevicePropertyToSpecificCondition(UDP_DRIVING_OUT);
			}
			else if(currentAppliancePosition == APPLIANCE_POSITION_OUT)
			{
				// move in:
				TV_Unit_Drive_Move_In();

				updateDevicePropertyToSpecificCondition(UDP_DRIVING_IN);
			}
			else if(currentAppliancePosition == APPLIANCE_POSITION_SENSOR_ERROR)
			{
				// fatal error -> do not drive!
				updateDevicePropertyToSpecificCondition(UDP_DRIVING_ERROR);
			}
			else
			{
				// must be in undefined state -> security drive!
				StartSecurityDrive();
			}
		}
	}
	else
	{
		// preconditions not fulfilled -> report to app

		// TODO: !!!!!!!!
	}
	return TRUE;
}

void updateDevicePropertyFromAppliancePosition()
{
	uint8_t statusTextIndex = getPropertyIndexFromID(STATUS_TEXT_DISPLAY);
	uint8_t inOutButtonIndex = getPropertyIndexFromID(INOUT_DRIVE_STARTBUTTON);

	if(currentAppliancePosition == APPLIANCE_POSITON_UNDEFINED)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_QUESTIONCIRCLE_BLUE;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Status: unbestimmt");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "State: undefined");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_WARNING_YELLOW;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Sicher einfahren");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Safely pull in");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(STATUS_TEXT_DISPLAY, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(INOUT_DRIVE_STARTBUTTON, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}
		
	}
	else if(currentAppliancePosition == APPLIANCE_POSITION_OUT)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_TV_BLUEWHITE;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Status: Ausgefahren");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "State: Pulled Out");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_ARROW_UP_WHITE;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Einfahren");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Pull in");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(STATUS_TEXT_DISPLAY, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(INOUT_DRIVE_STARTBUTTON, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}		
	}
	else if(currentAppliancePosition == APPLIANCE_POSITION_IN)
	{
		LaroomyXNG_DeviceProperties[statusTextIndex].imageID = IMAGE_ID_TV_WHITE;
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].de_devicePropertyDescription, "Status: Eingefahren");
		strcpy(LaroomyXNG_DeviceProperties[statusTextIndex].en_devicePropertyDescription, "State: Pulled in");

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_ARROW_DOWN_WHITE;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Ausfahren");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Pull out");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(STATUS_TEXT_DISPLAY, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(INOUT_DRIVE_STARTBUTTON, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}		
	}
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

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_STOPSIGN;
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

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_STOPSIGN;
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

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_STOPSIGN;
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

		LaroomyXNG_DeviceProperties[inOutButtonIndex].imageID = IMAGE_ID_RELOADARROW_LEFTBLUE;
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].de_devicePropertyDescription, "Erneut versuchen");
		strcpy(LaroomyXNG_DeviceProperties[inOutButtonIndex].en_devicePropertyDescription, "Try again");

		if(HMxx_getConnectionStatus())
		{
			notifyPropertyChanged(STATUS_TEXT_DISPLAY, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
			notifyPropertyChanged(INOUT_DRIVE_STARTBUTTON, PCHANGE_FLAG_THISPROPERTY | PCHANGE_FLAG_THISPROPERTYDETAIL);
		}		
	}
}

void updateDevicePropertyToErrorStateFromErrorFlag()
{
	updateDevicePropertyToSpecificCondition(UDP_DRIVING_ERROR);

	if(checkErrorFlag(FLAG_TVDRIVE_LIN_SENSOR_ERROR))
	{
		// TODO!

		setDeviceInfoHeader(IMAGE_ID_WARNING_RED, "Linearantrieb - Sensorfehler!");
	}
	else if(checkErrorFlag(FLAG_TVDRIVE_TILT_SENSOR_ERROR))
	{
		// TODO!

		setDeviceInfoHeader(IMAGE_ID_WARNING_RED, "Kippantrieb - Sensorfehler!");
	}
	else if(checkErrorFlag(FLAG_CDDRIVE_LEFT_SENSOR_ERROR))
	{
		// TODO!

		setDeviceInfoHeader(IMAGE_ID_WARNING_RED, "Blende links - Sensorfehler!");
	}
	else if(checkErrorFlag(FLAG_CDDRIVE_RIGHT_SENSOR_ERROR))
	{
		// TODO!

		setDeviceInfoHeader(IMAGE_ID_WARNING_RED, "Blende rechts - Sensorfehler!");
	}
	else if(checkErrorFlag(FLAG_DOORSENSOR_ERROR))
	{
		setDeviceInfoHeader(IMAGE_ID_WARNING_RED, "T]rsensor offen!");
	}
	else
	{
		setDeviceInfoHeader(IMAGE_ID_WARNING_RED, "Unbekannter Fehler.");
	}
}

void CoverDriveReachedOpenedPosition()
{
	// the cover-drive reached the open-position, so start tv-drive
	if(executeCurrentDriveAsSecurityDrive)
	{
		// security drive requested
		setExecutionFlag(FLAG_TVDRIVE_START_SECUREPOSITION);
	}
	else
	{
		setExecutionFlag(FLAG_TVDRIVE_START_MOVE_OUT);
	}
}

void TVDriveReachedBackPosition()
{
	// the tv-drive reached the 'in' position, so start cover-drive to close the cover
	setExecutionFlag(FLAG_COVERDRIVE_START_CLOSE);
}

void CoverDriveReachedClosedPosition()
{
	// the drive-process is finished -> schedule the update of the parameter for main
	setExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION_AND_PROPERTY);
}

void TVDriveReachedFrontPosition()
{
	// the drive-process is finished -> schedule the update of the parameter for main
	setExecutionFlag(FLAG_UPDATE_APPLIANCE_POSITION_AND_PROPERTY);
}

void updateMotorCurrentValues()
{
	linearDriveCurrentValue = ADC_ReadAverage(LINEAR_DRIVE_CURRENTSENSOR_ADC);
	tiltDriveCurrentValue = ADC_ReadAverage(TILT_DRIVE_CURRENTSENSOR_ADC);
	coverDriveLeftCurrentValue = ADC_ReadAverage(COVER_DRIVE_LEFT_CURRENTSENSOR_ADC);
	coverDriveRightCurrentValue = ADC_ReadAverage(COVER_DRIVE_RIGHT_CURRENTSENSOR_ADC);
}

uint16_t adcValueFromIndex(uint8_t index)
{
	switch(index)
	{
		case 0:
			return linearDriveCurrentValue;
		case 1:
			return tiltDriveCurrentValue;
		case 2:
			return coverDriveLeftCurrentValue;
		case 3:
			return coverDriveRightCurrentValue;
		default:
			return 0;
	}
}

void updateCurrentMonitorData()
{
	if(currentMonitorUpdateCounter == 0)
	{
		// send names
		HMxx_SendData("MCN&0Linear\0");
		HMxx_SendData("MCN&1Tilt\0");
		HMxx_SendData("MCN&2CD Left\0");
		HMxx_SendData("MCN&3CD Right\0");
	}
	else
	{
		// send data
		for(uint8_t i = 0; i < 4; i++)
		{
			char dataOut[20];

			for(uint8_t j = 0; j < 20; j++)
				dataOut[i] = '\0';

			sprintf(dataOut, "MCD&%d%d", i, adcValueFromIndex(i));

			HMxx_SendData(dataOut);
		}
	}
	
	currentMonitorUpdateCounter++;

	if(currentMonitorUpdateCounter == 20)
	{
		currentMonitorUpdateCounter = 0;
	}
}

void CheckBoardButtons()
{
	if(!(EXTRA_BOARD_BUTTON_PIN & (1<<EXTRA_BOARD_BUTTON_LEFT)))
	{
		longDelay(50);

		if(!(EXTRA_BOARD_BUTTON_PIN & (1<<EXTRA_BOARD_BUTTON_LEFT)))
		{
			// add left button action here...
			
			//util_coverdrive_right_move(MOVE_CLOSE);
			
			EnableApplianceDriver(FALSE);
			
			longDelay(200);
		}
	}
	if(!(EXTRA_BOARD_BUTTON_PIN & (1<<EXTRA_BOARD_BUTTON_RIGHT)))
	{
		longDelay(50);

		if(!(EXTRA_BOARD_BUTTON_PIN & (1<<EXTRA_BOARD_BUTTON_RIGHT)))
		{
			// add right button action here...
			
			//util_coverdrive_right_move(MOVE_OPEN);
			
			cbi(PORTA, PORTA5);// additional load reset
			
			longDelay(200);
		}
	}
	
}

//void sendBarGraphInfo(uint8_t counter)
//{
	//switch(counter)
	//{
		//case 0:
		//HMxx_SendData("MCN&0OUT 1\0");
		//break;
		//case 1:
		//HMxx_SendData("MCD&0100\0");
		//break;
		//case 2:
		//HMxx_SendData("MCN&1OUT 2\0");
		//break;
		//case 3:
		//HMxx_SendData("MCD&1050\0");
		//break;
		//case 4:
		//HMxx_SendData("MCN&2OUT 3\0");
		//break;
		//case 5:
		//HMxx_SendData("MCD&2076\0");
		//break;
		//case 6:
		//HMxx_SendData("MCN&3OUT 4\0");
		//break;
		//case 7:
		//HMxx_SendData("MCD&3220\0");
		//break;
		//default:
		//break;
	//}
//}



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
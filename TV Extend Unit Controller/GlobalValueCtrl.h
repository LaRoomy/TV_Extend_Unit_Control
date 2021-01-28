/*
 * GlobalValueCtrl.h
 *
 * Created: 23.11.2020 07:22:45
 *  Author: Hans Philipp Zimmermann
 */ 


#ifndef GLOBALVALUECTRL_H_
#define GLOBALVALUECTRL_H_

// execution flags:
#define FLAG_UPDATE_APPLIANCE_POSITION					0x01
#define FLAG_UPDATE_APPLIANCE_POSITION_AND_PROPERTY		0x02
#define FLAG_COVERDRIVE_START_CLOSE						0x04
#define FLAG_TVDRIVE_START_MOVE_OUT						0x08
#define FLAG_TVDRIVE_START_SECUREPOSITION				0x10
#define FLAG_DISABLE_DRIVER								0x20

// error flags:
#define FLAG_DOORSENSOR_ERROR							0x01
// ...
#define FLAG_TVDRIVE_TILT_SENSOR_ERROR					0x10
#define FLAG_TVDRIVE_LIN_SENSOR_ERROR					0x20
#define FLAG_CDDRIVE_RIGHT_SENSOR_ERROR					0x40
#define FLAG_CDDRIVE_LEFT_SENSOR_ERROR					0x80

#define EMERGENCY_STOP_REASON_DOORS_NOT_CLOSED			1
#define EMERGENCY_STOP_REASON_USER_STOPPED				2


uint8_t deviceBindingStatus;
char deviceBindingPasskey[11];

BOOL switch_preventer;

uint8_t activeMultiComplexPropertyID;

uint16_t linearDriveCurrentValue;
uint16_t tiltDriveCurrentValue;
uint16_t coverDriveLeftCurrentValue;
uint16_t coverDriveRightCurrentValue;

uint8_t	currentAppliancePosition;

BOOL executeCurrentDriveAsSecurityDrive;
BOOL deviceHeaderChanged;

uint8_t currentMonitorUpdateCounter;

volatile uint8_t executionFlags;
volatile uint8_t errorFlags;


void InitGlobalValues()
{
	deviceBindingStatus = 0;
	switch_preventer = FALSE;
	activeMultiComplexPropertyID = 0;
	currentMonitorUpdateCounter = 0;
	executionFlags = 0;
	errorFlags = 0;

	executeCurrentDriveAsSecurityDrive = FALSE;
	deviceHeaderChanged = FALSE;

	currentAppliancePosition = APPLIANCE_POSITON_UNDEFINED;

	linearDriveCurrentValue = 1023;
	tiltDriveCurrentValue = 1023;
	coverDriveRightCurrentValue = 1023;
	coverDriveLeftCurrentValue = 1023;

	for(uint8_t i = 0; i < 11; i++)
		deviceBindingPasskey[i] = '\0';

}

void setExecutionFlag(uint8_t flag)
{
	executionFlags |= flag;
}

void clearExecutionFlag(uint8_t flag)
{
	executionFlags &= (~flag);
}

uint8_t checkExecutionFlag(uint8_t flag)
{
	return executionFlags & flag;
}

void setErrorFlag(uint8_t flag)
{
	errorFlags |= flag;
}

void clearErrorFlag(uint8_t flag)
{
	errorFlags &= (~flag);
}

uint8_t checkErrorFlag(uint8_t flag)
{
	return errorFlags & flag;
}

void eraseExecutionMovementFlags()
{
	executionFlags &= (~0x1C);
}



// TODO: save necessary values to eeprom
//			- deviceBindingStatus
//			- passkey



#endif /* GLOBALVALUECTRL_H_ */
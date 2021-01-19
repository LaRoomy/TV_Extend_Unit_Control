/*
 * GlobalValueCtrl.h
 *
 * Created: 23.11.2020 07:22:45
 *  Author: Hans Philipp Zimmermann
 */ 


#ifndef GLOBALVALUECTRL_H_
#define GLOBALVALUECTRL_H_

#define FALSE	0
#define TRUE	1

typedef uint8_t BOOL;

uint8_t deviceBindingStatus;
char deviceBindingPasskey[11];

BOOL switch_preventer;

uint8_t activeMultiComplexPropertyID;

uint16_t linearDriveCurrentValue;
uint16_t tiltDriveCurrentValue;
uint16_t coverDriveLeftCurrentValue;
uint16_t coverDriveRightCurrentValue;

uint8_t	currentAppliancePosition;


uint8_t currentMonitorUpdateCounter;


void InitGlobalValues()
{
	deviceBindingStatus = 0;
	switch_preventer = FALSE;
	activeMultiComplexPropertyID = 0;
	currentMonitorUpdateCounter = 0;

	currentAppliancePosition = APPLIANCE_POSITON_UNDEFINED;

	linearDriveCurrentValue = 1023;
	tiltDriveCurrentValue = 1023;
	coverDriveRightCurrentValue = 1023;
	coverDriveLeftCurrentValue = 1023;

	for(uint8_t i = 0; i < 11; i++)
		deviceBindingPasskey[i] = '\0';

}

// TODO: save necessary values to eeprom
//			- deviceBindingStatus
//			- passkey



#endif /* GLOBALVALUECTRL_H_ */
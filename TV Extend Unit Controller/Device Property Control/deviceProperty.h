/*
 * deviceProperty.h
 *
 * Created: 05.02.2020 10:24:51
 * Author: Hans Philipp Zimmermann
 *
 * This header-file defines the device-functions (called properties) like buttons, switches, rgb output, etc.
 * 
 * The 'LaroomyXNG_DeviceProperties' - struct defines an ID for the property, the property-type and
 * has to provide two language strings (de/en) for the UI-Element in the app
 * An optional imageID can be defined for the appearance in the remote app.
 * A flag parameter is included for internal use. (8bit)
 * A state-parameter contains a simple state-value or
 * the index of an external complex data-struct for the appropriate property type.
 *
 * The amount must also be specified and must conform to the length of the struct
 *
 * IMPORTANT NOTES:
 *		Property-ID must be != 0 and in the 8bit range
 *		The maximum amount of properties is 255 ! (8bit)
 *
 *		SEARCHKEY::MODIFY (this file for different devices)
 *
 */ 
#ifndef DEVICEPROPERTY_H_
#define DEVICEPROPERTY_H_

#include "../General/BitUtilitiesEx.h"

//**********************************************************************************************
// Property-Type definitions:

#define PROPERTY_TYPE_RESERVED			0

// Simple >>
#define PROPERTY_TYPE_BUTTON			1
#define PROPERTY_TYPE_SWITCH			2
#define PROPERTY_TYPE_LEVELSELECTOR		3
#define PROPERTY_TYPE_LEVELINDICATOR	4
#define PROPERTY_TYPE_TEXTDISPLAY		5

// Complex >>
#define PROPERTY_TYPE_RGBSELECTOR			6
#define PROPERTY_TYPE_EXTLEVELSELECTOR		7
#define PROPERTY_TYPE_SIMPLETIMESELECTOR	8
#define PROPERTY_TYPE_TIMEELAPSESELECTOR	9
#define PROPERTY_TYPE_TIMEFRAMESELECTOR		10
#define PROPERTY_TYPE_NAVIGATOR				11
#define PROPERTY_TYPE_BARGRAPHDISPLAY		12

//**********************************************************************************************
// Property-Types must be separated by their state-data -> Property-types with complex data starts at this index
#define COMPLEX_PROPERTY_STARTINDEX		6
//**********************************************************************************************
//
#define PROPERTY_AMOUNT		7	// NOTE: This must conform to the property struct and is limited to 255 !!!
//
// provide the element-IDs as defines to make the code more readable
// NOTE: ID == 0 is forbidden ! This ID is reserved for error-state and comparison!
#define		STATUS_TEXT_DISPLAY					22
#define		INOUT_DRIVE_STARTBUTTON				23
#define		DRIVE_IN_TIMESCHEDULER				24
#define		TVUNIT_FREEDRIVE_NAVIGATOR			25
#define		CDUNIT_LEFT_FREEDRIVE_NAVIGATOR		26
#define		CDUNIT_RIGHT_FREEDRIVE_NAVIGATOR	27
#define		CURRENT_MONITOR						28
//
//**********************************************************************************************
// provide the Group-IDs as defines to make the code more readable
// NOTE: ID == 0 is forbidden ! This ID is reserved for error-state and comparison!
#define		AUTO_INOUT_GROUP_ID				1
#define		FREEDRIVE_INOUT_GROUP_ID		2
//**********************************************************************************************
// RGB-STATE-STRUCT DEFINITION:
//
// provide this only as a typedef, since it is not clear if the device will use an rgb-function
typedef struct _RGBSTATESTRUCT {
	uint8_t command;
	uint8_t rValue;
	uint8_t gValue;
	uint8_t bValue;
	uint8_t hardTransiton;
}RGBSTATESTRUCT, *PRGBSTATESTRUCT;
//
#define RGBSTATESTRUCT_SIZE		1
// if the device has one or more rgb-function property(ies) >> declare it here
//RGBSTATESTRUCT RGBState = {0,0,0,0,0};
//
// Example for an array of structs:
//
//RGBSTATESTRUCT RGBState[3] = { {0,0,0,0}, {0,0,0,0}, {0,0,0,0} };
//
// define if the state array is a single value or an array
#define		RGB_SINGLE_STATE
//#define		RGB_MULTIPLE_STATE
//
//**********************************************************************************************
//**********************************************************************************************
// EXTENDED LEVEL SELECTOR-STRUCT DEFINITION:
//
// provide this only as a typedef, since it is not clear if the device will use an extended level selector(s)
typedef struct _XLEVELSELECTSTRUCT {
	uint8_t on_off_state;
	uint8_t levelValue;
}XLEVELSELECTSTRUCT, *PXLEVELSELECTSTRUCT;
//
#define XLEVELSELECTSTRUCT_SIZE		1
// if the device has one or more extended level selector property(ies) >> declare it here
//XLEVELSELECTSTRUCT XLevelState = {0,0};
//
// Example for an array of structs:
//
//XLEVELSELECTSTRUCT XLevelState[3] = { {0,0}, {0,0}, {0,0} };
//
// define if the state array is a single value or an array
#define		XLEVELSELECT_SINGLE_STATE
//#define		XLEVELSELECT_MULTIPLE_STATE
//
//**********************************************************************************************
//**********************************************************************************************
// TIME SELECTOR-STRUCT DEFINITION:
//
// provide this only as a typedef, since it is not clear if the device will use a time selector(s)
typedef struct _TIMESELECTSTRUCT {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t timeSetterIndex;
}TIMESELECTSTRUCT, *PTIMESELECTSTRUCT;
//
#define		TIMESELECTSTRUCTSIZE	1
// if the device has one or more time selector property(ies) >> declare it here
TIMESELECTSTRUCT TimeSelectorState = {0,0,0,0};
//
// Example for an array of structs:
//
// TIMESELECTSTRUCT TimeSelectorState[3] = { {0,0,0,0}, {0,0,0,0}, {0,0,0,0} };
#define		TIMESELECTSTRUCT_SINGLE_STATE
//#define		TIMESELECTSTRUCT_MULTIPLE_STATE
//
//**********************************************************************************************
// shorter type for use in the time-frame-struct
typedef struct {
	uint8_t hour;
	uint8_t minute;
}STIME;
//**********************************************************************************************
// TIME-FRAME SELECTOR-STRUCT DEFINITION:
//
// provide this only as a typedef, since it is not clear if the device will use a time selector(s)
typedef struct _TIMEFRAMESTRUCT {
	STIME time_on;
	STIME time_off;
}TIMEFRAMESTRUCT, *PTIMEFRAMESTRUCT;
//
#define		TIMEFRAMESTRUCTSIZE	1
// if the device has one or more time selector property(ies) >> declare it here
//TIMEFRAMESTRUCT TimeFrameState = { {0,0}, {0,0} };
//
// Example for an array of structs:
//
// TIMEFRAMESTRUCT TimeFrameState[2] = {{ {0,0}, {0,0} }, { {0,0}, {0,0} }};
#define		TIMEFRAMESTRUCT_SINGLE_STATE
//#define		TIMEFRAMESTRUCT_MULTIPLE_STATE
// ---
#define		TIMEFRAME_NO_CONSENT	0
#define		TIMEFRAME_IS_IN_SCOPE	1
// ---
// forward declaration for the time-frame-checkup-function
uint8_t isTimeInFrame(PTIMEFRAMESTRUCT tfs, uint8_t curTimeHour, uint8_t curTimeMinute);
//**********************************************************************************************
//**********************************************************************************************
// Additional Property Flags:
//______________________________________________________________________________________________________________
#define		FLAG_PROPERTY_ENABLED			0x01
#define		FLAG_PROPERTY_IS_GROUP_MEMBER	0x02
// if the ENABLED flag is not set, the app will hide the appropriate UI Element for the device property
// -> If the property is static, this flag must always be set.
// -> This provides the possible implementation of a hardware-detection.
//		If the hardware is available, this flag could be set, otherwise it could be unset.
//_______________________________________________________________________________________________________________
//
//#define		FLAG_PROPERTY_ ...
//
//	NOTE:	Each device-property has a flag-parameter to carry some extra control information like the enabled state
//**********************************************************************************************
//**********************************************************************************************
// Property-changed Flags:		(for use in notifyPropertyChanged)
//______________________________________________________________________________________________________________
#define		PCHANGE_FLAG_ENTIREPROPERTY				0x01
#define		PCHANGE_FLAG_ENTIREPROPERTYDETAIL		0x02
#define		PCHANGE_FLAG_THISPROPERTY				0x04
#define		PCHANGE_FLAG_THISPROPERTYDETAIL			0x08
#define		PCHANGE_FLAG_ENTIREPGROUPS				0x10
#define		PCHANGE_FLAG_THISPGROUP					0x20
#define		PCHANGE_FLAG_ENTIREGROUPDETAIL			0x40
#define		PCHANGE_FLAG_THISGROUPDETAIL			0x80
//**********************************************************************************************
//**********************************************************************************************
// Forward declarations >>
void dispatchProperty(volatile char* data);
void setPropertyState(uint8_t propID, uint8_t state);
uint8_t getPropertyState(uint8_t propID);
uint8_t propertyTypeFromID(uint8_t propID);
uint8_t getPropertyEnabledStateFromID(uint8_t propID);
uint8_t getPropertyIndexFromID(uint8_t propID);
uint8_t getPropertyGroupIndexFromID(uint8_t groupID);
// ***********************************************************
// These functions are declared here, but must be implemented for custom usage:
void notifyPropertyChanged(uint8_t propertyID, uint8_t flags);
void notifyPropertyGroupChanged(uint8_t groupID, uint8_t flags);
void notifyPropertyStateChanged(uint8_t propertyID);
void setDeviceInfoHeader(uint8_t imageID, char* message);
// IMPORTANT NOTE:
//		All changes on the device which are related to UI Elements in the remote application,
//		MUST be reported to the remote application via notification. Otherwise it will result
//		in defected property <> UI-Element binding.
//**********************************************************************************************
#include "ImageID.h"
//**********************************************************************************************
// Device-Property definitions
struct {
	const uint8_t devicePropertyID;
	const uint8_t devicePropertyType;
	uint8_t devicePropertyState;
	char de_devicePropertyDescription[20];
	char en_devicePropertyDescription[20];
	uint8_t imageID;
	uint8_t FLAGS;
} LaroomyXNG_DeviceProperties[PROPERTY_AMOUNT] = {

	// NOTE:	Since the strings and the transmission process is using the ASCII Codepage, Multibyte character like ä,ö,ü or ß are not supported.
	//			To use these character in German language, some rarely used characters will be substituted (only in German language), these character are then not supported anymore.
	//			'ä' = '^' (0x5e);; 'ö' = '[' (0x5b);; ü = ']' (0x5d);; 'ß' = '{' (0x7b);; 'Ä' = '}' (0x7d);; 'Ö' = '|' (0x7c);; 'Ü' = '~' (0x7e)
	//
	//			The property names must not exceed 20 character, if the length is shorter than 20 character, the string must be limited with a terminator ('\0')

	{STATUS_TEXT_DISPLAY, 5, 200, "Status: unbestimmt\0", "State: undefined\0", IMAGE_ID_QUESTIONCIRCLE_BLUE, FLAG_PROPERTY_ENABLED | FLAG_PROPERTY_IS_GROUP_MEMBER},
	{INOUT_DRIVE_STARTBUTTON, 1, 0, "Sicher einfahren\0", "Safely pull in\0", IMAGE_ID_WARNING_YELLOW, FLAG_PROPERTY_ENABLED | FLAG_PROPERTY_IS_GROUP_MEMBER},
	{DRIVE_IN_TIMESCHEDULER, 8, 0, "Einfahrzeit w^hlen\0", "Select pull-in time\0", IMAGE_ID_CLOCK_RELOAD, FLAG_PROPERTY_ENABLED | FLAG_PROPERTY_IS_GROUP_MEMBER},
	{TVUNIT_FREEDRIVE_NAVIGATOR, 11, 0b00011111, "TV Einheit\0", "TV Unit\0", IMAGE_ID_TV_BLUEWHITE, FLAG_PROPERTY_ENABLED | FLAG_PROPERTY_IS_GROUP_MEMBER},
	{CDUNIT_LEFT_FREEDRIVE_NAVIGATOR, 11, 0b00001010, "Linke Blende\0", "Left Cover\0", IMAGE_ID_ARROW_LEFT_BLUE, FLAG_PROPERTY_ENABLED | FLAG_PROPERTY_IS_GROUP_MEMBER}, 
	{CDUNIT_RIGHT_FREEDRIVE_NAVIGATOR, 11, 0b00001010, "Rechte Blende\0", "Right Cover\0", IMAGE_ID_ARROW_RIGHT_BLUE, FLAG_PROPERTY_ENABLED | FLAG_PROPERTY_IS_GROUP_MEMBER},
	{CURRENT_MONITOR, 12, 4, "Motorstrom\0", "Motor-Current\0", IMAGE_ID_FLASH_YELLOW, FLAG_PROPERTY_ENABLED | FLAG_PROPERTY_IS_GROUP_MEMBER}
};
//**********************************************************************************************
/*
	PROPERTY-TYPES:
		
		1. Type "Button"
			- Type-ID: 0x01 (1)
			- State: 0 or 1 (8bit value)			NOTE-> there is no state value in the transmission

		2. Type "Switch"
			- Type-ID: 0x02 (2)
			- State: 0 or 1 (8bit value)
			
		3. Type "Level selector"
			- Type-ID: 0x03 (3)
			- State: Level-Value (0-255) (8bit value)
			
		4. Type "Level Indicator"
			- Type-ID: 0x04 (4)
			- State: No input-state / output-state: Level-Value (0-255) (8bit value)

		5. Type "Simple Text Display"
			- Type-ID: 0x05 (5)
			- State: No input-state / output-state: text			

		6. Type "RGB Selector"
			- Type-ID: 0x06 (6)
			- State: Index of RGBState-Struct position (containing the state)

		7. Type "Extended Level Selector"
			- Type-ID: 0x07 (7)
			- State: Index of XLevelSelect-Struct position (containing the state)

		8. Type "Simple Time Selector"
			- Type-ID: 0x08 (8)
			- State: Index of Time-Struct position (containing the state)

		9. Type "Elapse Time Selector"
			- Type-ID: 0x09 (9)
			- State: Index of Time-Struct position (containing the state)

		10. Type "Time-Frame Selector"
			- Type-ID: 0x0A (10)
			- State: Index of TimeFrame-Struct position (containing the state)
		
		11. Type "Simple Navigator"
			- Type-ID: 0x0B (11)
			- State: The state is a single 8bit value and the first 5 bits are used to control the visibility of the arrow-buttons

*/
//**********************************************************************************************
// Device-Property Group definitions:
//
#define MAX_GROUPMEMBER_COUNT	5
//
// The amount definition is mandatory for internal iteration and comparison purposes
// NOTE: If Groups are not used, set the amount to zero
#define PROPERTY_GROUP_AMOUNT		2
// Groups are not mandatory, so provide it as typedef
typedef struct _PROPTERTYGROUPS {
	uint8_t groupID;
	uint8_t imageID;
	char groupName_en[20];
	char groupName_de[20];
	uint8_t memberIDs[MAX_GROUPMEMBER_COUNT];
}PROPTERTYGROUPS, *PPROPTERTYGROUPS;
//
// if Group(s) are desired, declare it here:
PROPTERTYGROUPS propertyGroups[PROPERTY_GROUP_AMOUNT] = {
	{AUTO_INOUT_GROUP_ID, IMAGE_ID_PROPERTYGROUP_LRSTYLE, "Automatik", "Automatic", {22,23,24,0,0}},
	{FREEDRIVE_INOUT_GROUP_ID, IMAGE_ID_TOOL_LRSTYLE, "Wartung", "Maintenance", {25,26,27,28,0}}
};
	// NOTE:	Since the strings and the transmission process is using the ASCII Codepage, Multibyte character like ä,ö,ü or ß are not supported.
	//			To use these character in German language, some rarely used characters will be substituted (only in German language), these character are then not supported anymore.
	//			'ä' = '^' (0x5e);; 'ö' = '[' (0x5b);; ü = ']' (0x5d);; 'ß' = '{' (0x7b);; 'Ä' = '}' (0x7d);; 'Ö' = '|' (0x7c);; 'Ü' = '~' (0x7e)
	//
	//			The group names must not exceed 20 character, if the length is shorter than 20 character, the string must be limited with a terminator ('\0')
	//
//**********************************************************************************************
//**********************************************************************************************

void setPropertyState(uint8_t propID, uint8_t state)
{
	// atomic ???

	for(uint8_t i = 0; i < PROPERTY_AMOUNT; i++)
	{
		if(LaroomyXNG_DeviceProperties[i].devicePropertyID == propID)
		{
			LaroomyXNG_DeviceProperties[i].devicePropertyState = state;
			break;
		}
	}
}

uint8_t getPropertyState(uint8_t propID)
{
	// atomic ???

	for(uint8_t i = 0; i < PROPERTY_AMOUNT; i++)
	{
		if(LaroomyXNG_DeviceProperties[i].devicePropertyID == propID)
		{
			return LaroomyXNG_DeviceProperties[i].devicePropertyState;
		}
	}
	return 0;
}

uint8_t propertyTypeFromID(uint8_t propID)
{
	for(uint8_t i = 0; i < PROPERTY_AMOUNT; i++)
	{
		if(LaroomyXNG_DeviceProperties[i].devicePropertyID == propID)
		{
			return LaroomyXNG_DeviceProperties[i].devicePropertyType;
		}
	}
	return 0;
}

uint8_t getPropertyEnabledStateFromID(uint8_t propID)
{
	for(uint8_t i = 0; i < PROPERTY_AMOUNT; i++)
	{
		if(LaroomyXNG_DeviceProperties[i].devicePropertyID == propID)
		{
			return (LaroomyXNG_DeviceProperties[i].FLAGS & FLAG_PROPERTY_ENABLED) ? 1 : 0;
		}
	}
	return 0;
}

uint8_t getPropertyGroupIDforPropertyID(uint8_t propID)
{
	for(uint8_t i = 0; i < PROPERTY_GROUP_AMOUNT; i++)
	{
		for(uint8_t j = 0; j < MAX_GROUPMEMBER_COUNT; j++)
		{
			if(propID == propertyGroups[i].memberIDs[j])
			{
				return propertyGroups[i].groupID;
			}
		}
	}
	return 0;
}

uint8_t getGroupMemberCountAtIndex(uint8_t index)
{
	uint8_t memCount = 0;

	for(uint8_t i = 0; i < MAX_GROUPMEMBER_COUNT; i++)
	{
		if(propertyGroups[index].memberIDs[i] == 0)
		{
			break;
		}
		else
		{
			memCount++;
		}
	}
	return memCount;
}

uint8_t getPropertyIndexFromID(uint8_t propID)
{
	for(uint8_t i = 0; i < PROPERTY_AMOUNT; i++)
	{
		if(LaroomyXNG_DeviceProperties[i].devicePropertyID == propID)
		{
			return i;
		}
	}
	return 0;
}

uint8_t getPropertyGroupIndexFromID(uint8_t groupID)
{
	for(uint8_t i = 0; i < PROPERTY_GROUP_AMOUNT; i++)
	{
		if(propertyGroups[i].groupID == groupID)
		{
			return i;
		}
	}
	return 0;
}

//uint8_t complexStateIndexFromPropertyID(uint8_t propID)
//{
	//// this function implements the same behavior like 'getPropertyState(...)'
	//
	//for(uint8_t i = 0; i < PROPERTY_AMOUNT; i++)
	//{
		//if(LaroomyXNG_DeviceProperties[i].devicePropertyID == propID)
		//{
			//return LaroomyXNG_DeviceProperties[i].devicePropertyState;
		//}
	//}
	//return 0;
//}


 uint8_t isTimeInFrame(PTIMEFRAMESTRUCT tfs, uint8_t curTimeHour, uint8_t curTimeMinute)
 {
	 //uint8_t curTimeHour = LOBYTE(time);
	 //uint8_t curTimeMinute = HIBYTE(time);
	 
	 if(tfs->time_on.hour < tfs->time_off.hour)
	 {
		 if((curTimeHour > tfs->time_on.hour)&&(curTimeHour < tfs->time_off.hour))
		 {
			 return TIMEFRAME_IS_IN_SCOPE;
		 }
		 else if(curTimeHour == tfs->time_on.hour)
		 {
			 if(curTimeMinute > tfs->time_on.minute)
			 {
				 return TIMEFRAME_IS_IN_SCOPE;
			 }
		 }
		 else if(curTimeHour == tfs->time_off.hour)
		 {
			 if(curTimeMinute < tfs->time_off.minute)
			 {
				 return TIMEFRAME_IS_IN_SCOPE;
			 }
		 }
	 }
	 else if(tfs->time_on.hour > tfs->time_off.hour)
	 {
		 if((curTimeHour >= tfs->time_on.hour)&&(curTimeHour < 24))
		 {
			 if(curTimeHour == tfs->time_on.hour)
			 {
				 if(curTimeMinute > tfs->time_on.minute)
				 {
					 return TIMEFRAME_IS_IN_SCOPE;
				 }
			 }
			 else
			 {
				 return TIMEFRAME_IS_IN_SCOPE;
			 }
		 }
		 else if((curTimeHour <= tfs->time_off.hour)&&(curTimeHour >= 0))
		 {
			 if(curTimeHour == tfs->time_off.hour)
			 {
				 if(curTimeMinute < tfs->time_off.minute)
				 {
					 return TIMEFRAME_IS_IN_SCOPE;
				 }
			 }
			 else
			 {
				 return TIMEFRAME_IS_IN_SCOPE;
			 }
		 }
	 }
	 else if(tfs->time_on.hour == tfs->time_off.hour)
	 {
		 if(tfs->time_on.minute < tfs->time_off.minute)
		 {
			 if((curTimeMinute > tfs->time_on.minute)&&(curTimeMinute < tfs->time_off.minute))
			 {
				 return TIMEFRAME_IS_IN_SCOPE;
			 }
		 }
		 else if(tfs->time_on.minute > tfs->time_off.minute)
		 {
			 if((curTimeMinute > tfs->time_on.minute)||(curTimeMinute < tfs->time_off.minute))
			 {
				 return TIMEFRAME_IS_IN_SCOPE;
			 }
		 }
	 }
	 return TIMEFRAME_NO_CONSENT;
 }



#endif /* DEVICEPROPERTY_H_ */
/*
 * propertyDispatcher.h
 *
 * Created: 06.02.2020 13:58:53
 *  Author: Hans Philipp Zimmermann
 *
 *		NOTE:
 *			This header dispatches the bluetooth execution commands to the appropriate device functionality.
 *
 *
 *		HINT:	To save memory space, not used command-handler-functions and invocations could be disabled!
 * 
 *		SEARCHKEY::MODIFY (for different devices)
 */ 
#ifndef PROPERTYDISPATCHER_H_
#define PROPERTYDISPATCHER_H_

// forward declarations:
//******************************************************************************************************************************
// Handle types with simple (8bit) device states:
void onHandleButtonCommand(uint8_t buttonID);// no extra data -> execution means button pressed
void onHandleSwitchCommand(uint8_t switchID, BOOL newState);// extra data is 0 or 1 as char -> switch-state !
void onHandleLevelSelectorCommand(uint8_t lSelectorID, uint8_t level);// extra data is the level value data[4..6] -> convert to uint8_t level:(0...255)
//
// NOTE: Properties with the type-Index 4 + 5 are not executable, so there are no functions for that!
//
// Complex property types:
//******************************************************************************************************************************
//___________________________________________________________________________________________________________
// RGB Command:
//
// extra data is a command (color-values from 0-9!! or other command(for transition)) and values for r + g + b as 8bit value
//
void onRGBCommand(uint8_t rgbID, uint8_t command, uint8_t rVal, uint8_t gVal, uint8_t bVal);
//
//	Format:
//			[0]		 = Command ID 'C'
//			[1..3]	 = Property ID (8bit - as char)
//			[4..6]	 = Control-Command (8bit - as char)
//			[7..9]	 = RED-Value (8bit - as char)
//			[10..12] = GREEN-Value (8bit - as char)
//			[13..15] = BLUE-Value (8bit - as char)
//			[16]	 = '$'
//___________________________________________________________________________________________________________
// Extended Level Select Command:
//
void onExLevelSelectCommand(uint8_t exLevelSelectorID, uint8_t onoffState, uint8_t levelValue);
//
//	Format:
//			[0]		= Command ID 'C'
//			[1..3]	= Property ID (8bit - as char)
//			[4]		= On-Off State as single char '0' or '1'
//			[5..7]	= level value (8bit - as char)
//			[8]		= '$'
//___________________________________________________________________________________________________________
// Extended Level Select Command:
//
void onTimeSelectCommand(uint8_t simpleTimeSelectorID, uint8_t setterIndex, uint8_t hour, uint8_t minute, uint8_t second);
//
//	Format:
//			[0]		= Command ID 'C'
//			[1..3]	= Property ID (8bit - as char)
//			[4]		= Time-Setter-Index (0..9) (1char)
//			[5..6]	= Hour-value (2char)
//			[7..8]	= Minute-value (2char)
//			[9..10]	= Second-value (2char)
//			[11]	= '$'
//___________________________________________________________________________________________________________
// Extended Level Select Command:
//
void onTimeFrameSelectCommand(uint8_t timeFrameSelectorID, char setterIndex, uint8_t hour, uint8_t minute);
//
//	Format:
//			[0]		= Command ID 'C'
//			[1..3]	= Property ID (8bit - as char)
//			[4]		= Time-Setter-Index (0..9) (1char)
//			[5..6]	= Hour-value (2char)
//			[7..8]	= Minute-value (2char)
//			[9..10]	= Reserved (2char)
//			[11]	= '$'
//____________________________________________________________________________________________________________
//*****************************************************************************************************************************
// Simple Navigator Command:
//
void onSimpleNavigatorCommand(uint8_t simpleNavigatorID, char direction, char touchType);
//
//	Format:
//			[0]		= Command ID 'C'
//			[1..3]	= Property ID (8bit - as char)
//			[4]		= Direction (1char)
//			[5] 	= Touch-Type (1char)
//			[6]		= '$'
//____________________________________________________________________________________________________________
//*****************************************************************************************************************************



void dispatchProperty(volatile char* data)
{
	// Syntax:
	// [0] = Command ID ('C')
	// [1] = Reserved (Property ID)
	// [2] = Reserved (Property ID)
	// [3] = Reserved (Property ID)
	// [4 ... End] -> Data

	uint8_t propertyID = charsToU8Bit(data[1], data[2], data[3]);
	uint8_t propertyIndex;
	
	for(propertyIndex = 0; propertyIndex < PROPERTY_AMOUNT; propertyIndex++)
	{
		if(LaroomyXNG_DeviceProperties[propertyIndex].devicePropertyID == propertyID)
		{			
			break;
		}
	}

	if(propertyIndex == PROPERTY_AMOUNT)// property not found
	{
		HMxx_SendData("Error:ID:Invalid\0");
	}
	else
	{
		switch(LaroomyXNG_DeviceProperties[propertyIndex].devicePropertyType)
		{
			case PROPERTY_TYPE_BUTTON:
				onHandleButtonCommand(propertyID);
				break;
			case PROPERTY_TYPE_SWITCH:
				onHandleSwitchCommand(propertyID ,(data[4] == '0') ? FALSE : TRUE);
				break;
			case PROPERTY_TYPE_LEVELSELECTOR:
				onHandleLevelSelectorCommand(propertyID, charsToU8Bit(data[4], data[5], data[6]));
				break;
			case PROPERTY_TYPE_LEVELINDICATOR:
				// NO EXECUTION FOR THIS TYPE
				break;
			case PROPERTY_TYPE_TEXTDISPLAY:
				// NO EXECUTION FOR THIS TYPE
				break;
			case PROPERTY_TYPE_RGBSELECTOR:
				onRGBCommand(
					propertyID,
					charsToU8Bit(data[4], data[5], data[6]),
					charsToU8Bit(data[7], data[8], data[9]),
					charsToU8Bit(data[10], data[11], data[12]),
					charsToU8Bit(data[13], data[14], data[15])
					);
				break;
			case PROPERTY_TYPE_EXTLEVELSELECTOR:
				onExLevelSelectCommand(
					propertyID,
					(data[4] == '0') ? 0 : 1,
					charsToU8Bit(data[5], data[6], data[7])
					);
				break;
			case PROPERTY_TYPE_SIMPLETIMESELECTOR:
				onTimeSelectCommand(
					propertyID,
					charsToU8Bit('0', '0', data[4]),
					charsToU8Bit('0', data[5], data[6]),
					charsToU8Bit('0', data[7], data[8]),
					charsToU8Bit('0', data[9], data[10])
					);
				break;
			case PROPERTY_TYPE_TIMEFRAMESELECTOR:
				onTimeFrameSelectCommand(
					propertyID,
					data[4],
					charsToU8Bit('0', data[5], data[6]),
					charsToU8Bit('0', data[7], data[8])
					);
				break;
			case PROPERTY_TYPE_NAVIGATOR:
				onSimpleNavigatorCommand(
					propertyID,
					data[4],
					data[5]
					);
				break;

			// TODO: add all new types here!!!


			default:
				// error:
				HMxx_SendData("Error:Type:Invalid\0");
				break;
		}
	}
}

// NOTE: The function definitions could be deleted and be replaced by a custom implementation

void onHandleButtonCommand(uint8_t buttonID)
{
	// TODO: implement the button functions here!

	if(buttonID == INOUT_DRIVE_STARTBUTTON)
	{
		// if drive is in progress -> STOP!
		if(!StartApplianceDrive())
		{
			EmergencyStop(EMERGENCY_STOP_REASON_USER_STOPPED);
		}
	}
}

void onHandleSwitchCommand(uint8_t switchID, BOOL newState)
{
	// NOTE:	Implement the switch functions here!
	//			Handle all switches declared in the device Property
	//if(switchID == EXAMPLE_SWITCH_2)
	//{
		//if(newState)
		//{
			//setPropertyState(switchID, 1);
			////sbi(LED_PORT, LED3_OUTPUT);
		//}
		//else
		//{
			//setPropertyState(switchID, 0);
			////cbi(LED_PORT, LED3_OUTPUT);
		//}
	//}
	//eeSavePropertyState(switchID);
}

void onHandleLevelSelectorCommand(uint8_t lSelectorID, uint8_t level)
{
	// not implemented
}

void onRGBCommand(uint8_t rgbID, uint8_t command, uint8_t rVal, uint8_t gVal, uint8_t bVal)
{
	// not implemented
}

void onExLevelSelectCommand(uint8_t exLevelSelectorID, uint8_t onoffState, uint8_t levelValue)
{
	// not implemented
}

void onTimeSelectCommand(uint8_t simpleTimeSelectorID, uint8_t setterIndex, uint8_t hour, uint8_t minute, uint8_t second)
{
	//// schedule the activation of the timer-action
	//setGlobalControlFlag(SCHEDULE_SMP_TIME_ACTIVATION);	
	//
	//uint8_t stateIndex = getPropertyState(simpleTimeSelectorID);
//
	//// make sure the index is in a valid range!
	//if(stateIndex < TIMESELECTSTRUCTSIZE)
	//{
//#ifdef TIMESELECTSTRUCT_SINGLE_STATE
		//TimeSelectorState.timeSetterIndex = setterIndex;
		//TimeSelectorState.hour = hour;
		//TimeSelectorState.minute = minute;
		//TimeSelectorState.second = second;
//#else
		//TimeSelectorState[stateIndex].timeSetterIndex = setterIndex;
		//TimeSelectorState[stateIndex].hour = hour;
		//TimeSelectorState[stateIndex].minute = minute;
		//TimeSelectorState[stateIndex].second = second;
//#endif
//
		//eeSavePropertyState(simpleTimeSelectorID);// maybe do this at the end of the function
//
		//// assign the right functionality to the appropriate ID:
//
		//if(simpleTimeSelectorID == EXAMPLE_TIMESELECTOR_3)
		//{
			//// ...
		//}
	//}
}

void onTimeFrameSelectCommand(uint8_t timeFrameSelectorID, char setterIndex, uint8_t hour, uint8_t minute)
{
	// not implemented
}

void onSimpleNavigatorCommand(uint8_t simpleNavigatorID, char direction, char touchType)
{
	// Direction-Char:
	// 1 = up; 2 = right; 3 = down; 4 = left; 5 = ok (middle-button)	
			
	// make sure that the automatic drive is not in progress!	
	BOOL isTVMoving = isTV_Unit_Drive_In_Progress();
	BOOL isCoverMoving = isCD_Unit_Drive_In_Progress();
	
	BOOL isSecurityHold = ((tv_unit_current_drive_mode == DRIVEMODE_EMERGENCY_STOP) || (cdUnit_currentDriveMode == DRIVEMODE_EMERGENCY_STOP)) ? TRUE : FALSE;

	if((!isTVMoving && !isCoverMoving) || isSecurityHold)
	{
		if(simpleNavigatorID == TVUNIT_FREEDRIVE_NAVIGATOR)
		{
			switch(direction)
			{
				case '1':// up
					if(touchType == '1')
					{
						EnableApplianceDriver(TRUE);
						maintenanceDriveActive = MAINTENANCEDRIVETYPE_TV_TILT_IN;
						move_tilt_drive(MOVE_IN);						
					}
					else
					{
						maintenanceDriveActive = FALSE;
						move_tilt_drive(STOP);
						setExecutionFlag(FLAG_DISABLE_DRIVER);
					}
					break;
				case '2':// right
					if(touchType == '1')
					{
						EnableApplianceDriver(TRUE);
						maintenanceDriveActive = MAINTENANCEDRIVETYPE_TV_LINEAR_OUT;
						move_linear_drive(MOVE_OUT);
					}
					else
					{
						maintenanceDriveActive = FALSE;
						move_linear_drive(STOP);
						setExecutionFlag(FLAG_DISABLE_DRIVER);
					}
					break;
				case '3':// down
					if(touchType == '1')
					{
						EnableApplianceDriver(TRUE);
						maintenanceDriveActive = MAINTENANCEDRIVETYPE_TV_TILT_OUT;
						move_tilt_drive(MOVE_OUT);
					}
					else
					{
						maintenanceDriveActive = FALSE;
						move_tilt_drive(STOP);
						setExecutionFlag(FLAG_DISABLE_DRIVER);
					}
					break;
				case '4':// left
					if(touchType == '1')
					{
						EnableApplianceDriver(TRUE);
						maintenanceDriveActive = MAINTENANCEDRIVETYPE_TV_LINEAR_IN;
						move_linear_drive(MOVE_IN);
					}
					else
					{
						maintenanceDriveActive = FALSE;
						move_linear_drive(STOP);
						setExecutionFlag(FLAG_DISABLE_DRIVER);
					}
					break;
				case '5':// middle button (Ok)
					move_linear_drive(STOP);
					move_tilt_drive(STOP);
					maintenanceDriveActive = FALSE;
					break;
				default:
					break; 
			}
		}
		else if(simpleNavigatorID == CDUNIT_LEFT_FREEDRIVE_NAVIGATOR)
		{
			switch(direction)
			{
				case '2':// right
					if(touchType == '1')
					{
						EnableApplianceDriver(TRUE);
						maintenanceDriveActive = MAINTENANCEDRIVETYPE_CD_LEFT_CLOSE;
						moveLeftDrive(MOVE_CLOSE);
					}
					else
					{
						maintenanceDriveActive = FALSE;
						moveLeftDrive(STOP);
						setExecutionFlag(FLAG_DISABLE_DRIVER);
					}
					break;
				case '4':// left
					if(touchType == '1')
					{
						EnableApplianceDriver(TRUE);
						maintenanceDriveActive = MAINTENANCEDRIVETYPE_CD_LEFT_OPEN;
						moveLeftDrive(MOVE_OPEN);
					}
					else
					{
						maintenanceDriveActive = FALSE;
						moveLeftDrive(STOP);
						setExecutionFlag(FLAG_DISABLE_DRIVER);
					}
					break;
				default:
					break;
			}
		}
		else if(simpleNavigatorID == CDUNIT_RIGHT_FREEDRIVE_NAVIGATOR)
		{
			switch(direction)
			{
				case '2':// right
					if(touchType == '1')
					{
						EnableApplianceDriver(TRUE);
						maintenanceDriveActive = MAINTENANCEDRIVETYPE_CD_RIGHT_OPEN;
						moveRightDrive(MOVE_OPEN);
					}
					else
					{
						maintenanceDriveActive = FALSE;
						moveRightDrive(STOP);
						setExecutionFlag(FLAG_DISABLE_DRIVER);
					}
					break;
				case '4':// left
					if(touchType == '1')
					{
						EnableApplianceDriver(TRUE);
						maintenanceDriveActive = MAINTENANCEDRIVETYPE_CD_RIGHT_CLOSE;
						moveRightDrive(MOVE_CLOSE);
					}
					else
					{
						maintenanceDriveActive = FALSE;
						moveRightDrive(STOP);
						setExecutionFlag(FLAG_DISABLE_DRIVER);
					}
					break;
				default:
					break;
			}
		}
	}
}

#endif /* PROPERTYDISPATCHER_H_ */
/*
 * CommandAnalyzer.h
 *
 * Created: 13.01.2020 18:36:01
 *  
 * Author: Hans Philipp Zimmermann
 */ 
#ifndef COMMANDANALYZER_H_
#define COMMANDANALYZER_H_

// forward declarations
void sendProperty(uint8_t index);
void sendPropertyDescription(uint8_t propertyID, char langCode);
void sendPropertyGroup(uint8_t index);
void onPropertyStateRequest(volatile char* data);
void sendSimplePropertyState(uint8_t propertyID);
void sendComplexPropertyState(uint8_t propertyID);

uint8_t propertyValuefromStartCharIndex(volatile char* data, uint8_t startIndex);

void onPropertyRequest(volatile char* data);
void onPropertyDescriptionRequest(volatile char* data);
void onPropertyGroupRequest(volatile char* data);
void onDetailedPropGroupInfoRequest(volatile char* data);
void onSetCommand(volatile char* data);
void onBindingRequest(volatile char* data);

void ExecuteUserFactoryReset();



void AnalyzeTransmission(volatile char* data)
{
	// the first byte of the transmission is the command as char (possible: 0-9, a-z, A-Z, *maybe extensible with special chars)
	switch(data[0])
	{

// **********************************************************************************************************
		// Authentication-Request >> send response to confirm the Laroomy device
		case 'x':
			if(compareStringsExactly(data, LAROOMY_XNG001_AUTHENTICATIONSTRING))
			{
				
				// temp!!!
				//StartPWM(PWM_CHANNEL_A);
				//SetPWMValue(PWM_CHANNEL_A, 255);
				//sbi(LED_PORT, LED3_OUTPUT);
				
				if(deviceBindingStatus == DEVICEBINDING_STATUS_REQUIRED)
				{
					HMxx_SendData(LAROOMY_XNG001_AUTHENTICATIONRESPONSE_BINDINGPASSKEYREQUIRED);
				}
				else
				{
					HMxx_SendData(LAROOMY_XNG001_AUTHENTICATIONRESPONSE);
				}
			}
			break;
// **********************************************************************************************************
		// Connection test -> send response
		case 'v':
			if(compareStringsExactly(data, LAROOMY_XNG001_CONNECTIONTESTSTRING))
			{
				HMxx_SendData(data);
			}
			break;
// **********************************************************************************************************
		// Binding-Request >> send response to indicate if binding is active and to confirm the passkey
		case 'r':
			// For info see documentation (Table 2)
			onBindingRequest(data);
			break;

// **********************************************************************************************************
		// Device-Property request -> Start sending the device properties
		case 'A':
			// Syntax:
			// [0] == Command ID ('A')
			// [1 ... 3] == Device property index as string -> convert to uint8_t
			// finalized with '$'
			onPropertyRequest(data);
			break;

// **********************************************************************************************************
		// Device-Property description request -> send the appropriate property description
		case 'B':
			// Syntax:
			// [0] = Command ID ('B')
			// [1] = Language ID (0 == English || 1 == German)
			// [2...4] = Device property ID as string -> convert to uint8_t
			// finalized with '$'
			onPropertyDescriptionRequest(data);
			break;

// **********************************************************************************************************
		// Device property execution command -> get the type and dispatch
		case 'C':
			// Syntax: (explained in propertyDispatcher.h)
			dispatchProperty(data);
			break;

// **********************************************************************************************************
		// Device property value/state request
		case 'D':
			// Syntax:
			// [0] = Command ID ('D')
			// [1..3] = Property ID
			// finalized with '$'
			onPropertyStateRequest(data);
			break;
// **********************************************************************************************************
		// Device property Group request
		case 'E':
			// Syntax:
			// [0] = Command ID ('E')
			// [1..3] = Group-Index
			// finalized with '$'
			onPropertyGroupRequest(data);
			break;
// **********************************************************************************************************
		// Detailed Property Group Info request
		case 'F':
			// Syntax:
			// [0] = Command ID ('F')
			// [1] = Language ID (0 == English || 1 == German)
			// [2..4] = Group-ID as string
			// finalized with '$'
			onDetailedPropGroupInfoRequest(data);
			break;
		// Set commands from the client
		case 'S':
			// Syntax:
			// [0] = Command ID ('S')
			// [1..3] = Setter ID
			// [4..End] = Data (command specific)
			onSetCommand(data);
			break;
		default:
			break;
	}	
}


void sendProperty(uint8_t index)
{
	char dataOut[20] = {'\0'};
	char carry[3];

	dataOut[0] = 'I';
	dataOut[1] = 'P';
	dataOut[2] = 'R';

	// Include property ID
	x8BitValueTo3CharBuffer(carry, LaroomyXNG_DeviceProperties[index].devicePropertyID);
	dataOut[3] = carry[0];
	dataOut[4] = carry[1];
	dataOut[5] = carry[2];

	// Include property TYPE
	x8BitValueTo3CharBuffer(carry, LaroomyXNG_DeviceProperties[index].devicePropertyType);
	dataOut[6] = carry[0];
	dataOut[7] = carry[1];
	dataOut[8] = carry[2];

	// Include the Index (in struct)
	x8BitValueTo3CharBuffer(carry, index);
	dataOut[9] = carry[0];
	dataOut[10] = carry[1];
	dataOut[11] = carry[2];

	// if this device-property is part of a group -> include groupID
	if(LaroomyXNG_DeviceProperties[index].FLAGS & FLAG_PROPERTY_IS_GROUP_MEMBER)
	{
		uint8_t groupID = getPropertyGroupIDforPropertyID(LaroomyXNG_DeviceProperties[index].devicePropertyID);
		if(groupID != 0)
		{
			x8BitValueTo3CharBuffer(carry, groupID);
			dataOut[12] = carry[0];
			dataOut[13] = carry[1];
			dataOut[14] = carry[2];
		}
		else
		{
			// indicate error: property-group not found
			dataOut[12] = 'e';
			dataOut[13] = 'r';
			dataOut[14] = 'r';
		}
	}
	else
	{
		dataOut[12] = 'X';
		dataOut[13] = 'X';
		dataOut[14] = 'X';
	}

	// include the imageID
	x8BitValueTo3CharBuffer(carry, LaroomyXNG_DeviceProperties[index].imageID);
	dataOut[15] = carry[0];
	dataOut[16] = carry[1];
	dataOut[17] = carry[2];

	dataOut[18] = '$';// delimiter
	
	HMxx_SendData(dataOut);
}

void sendPropertyDescription(uint8_t propertyID, char langCode)
{
	char sBuffer[] = "PD:Sxxx000$\0";
	char eBuffer[] = "PD:E$\0";

	char pid[3];
	x8BitValueTo3CharBuffer(pid, propertyID);
	sBuffer[4] = pid[0];
	sBuffer[5] = pid[1];
	sBuffer[6] = pid[2];

	// iterate the property struct to find the property ID
	for(uint8_t i = 0; i < PROPERTY_AMOUNT; i++)
	{
		if(LaroomyXNG_DeviceProperties[i].devicePropertyID == propertyID)
		{
			// include the state for efficiency purposes
			x8BitValueTo3CharBuffer(
				pid,
				LaroomyXNG_DeviceProperties[i].devicePropertyState
			);
			sBuffer[7] = pid[0];
			sBuffer[8] = pid[1];
			sBuffer[9] = pid[2];

			HMxx_SendData(sBuffer);
			// send the appropriate language string
			if(langCode == '1')
			{
				HMxx_SendData(LaroomyXNG_DeviceProperties[i].de_devicePropertyDescription);
			}
			else
			{
				HMxx_SendData(LaroomyXNG_DeviceProperties[i].en_devicePropertyDescription);
			}
			HMxx_SendData(eBuffer);
			break;
		}
	}
}

void onPropertyStateRequest(volatile char* data)
{
	// Syntax:
	// [0] = Command ID ('D')
	// [1..3] = Property ID
	// finalized with '$'
	
	uint8_t propertyID = charsToU8Bit(data[1], data[2], data[3]);
	uint8_t type = propertyTypeFromID(propertyID);
	uint8_t propState = getPropertyState(propertyID);

	char dataOut[20] = {'\0'};

	if(type < COMPLEX_PROPERTY_STARTINDEX)
	{
		// Process simple state-data

		// Response Identifier (Simple Property State Response)
		dataOut[0] = 'P';
		dataOut[1] = 'S';
		dataOut[2] = 'S';

		char threeBuf[3];

		// Set Property ID
		x8BitValueTo3CharBuffer(threeBuf, propertyID);
		dataOut[3] = threeBuf[0];
		dataOut[4] = threeBuf[1];
		dataOut[5] = threeBuf[2];

		// Set simple property state
		x8BitValueTo3CharBuffer(threeBuf, propState);
		dataOut[6] = threeBuf[0];
		dataOut[7] = threeBuf[1];
		dataOut[8] = threeBuf[2];

		// Set property enabled state
		dataOut[9]  = getPropertyEnabledStateFromID(propertyID) ? '1' : '0';

		dataOut[10] = '$';	// delimiter

		HMxx_SendData(dataOut);
	}
	else
	{
		// Process complex state data:

		// Response Identifier	(Complex Property State Response)
		dataOut[0] = 'P';
		dataOut[1] = 'S';
		dataOut[2] = 'C';
//****************************************************************************************************************
		if(type == PROPERTY_TYPE_RGBSELECTOR)
		{
			//char carry[3] = {'\0'};			
			//
//#ifndef RGB_SINGLE_STATE
			//// the state-index is the propertyState
			//uint8_t stateIndex = getPropertyState(propertyID);
//#endif
			//// Property ID
			//x8BitValueTo3CharBuffer(carry, propertyID);
			//dataOut[3] = carry[0];
			//dataOut[4] = carry[1];
			//dataOut[5] = carry[2];
			//// Command data
//#ifdef RGB_SINGLE_STATE
			//x8BitValueTo3CharBuffer(carry, RGBState.command);
//#else		
			//x8BitValueTo3CharBuffer(carry, RGBState[stateIndex].command);
//#endif			
			//dataOut[6] = carry[0];
			//dataOut[7] = carry[1];
			//dataOut[8] = carry[2];
			//
			//// Red value
//#ifdef RGB_SINGLE_STATE
			//x8BitValueTo3CharBuffer(carry, RGBState.rValue);
//#else		
			//x8BitValueTo3CharBuffer(carry, RGBState[stateIndex].rValue);
//#endif			
			//dataOut[9] = carry[0];
			//dataOut[10] = carry[1];
			//dataOut[11] = carry[2];
			//
			//// Green value
//#ifdef RGB_SINGLE_STATE
			//x8BitValueTo3CharBuffer(carry, RGBState.gValue);
//#else
			//x8BitValueTo3CharBuffer(carry, RGBState[stateIndex].gValue);
//#endif			
			//dataOut[12] = carry[0];
			//dataOut[13] = carry[1];
			//dataOut[14] = carry[2];
			//
			//// Blue value
//#ifdef RGB_SINGLE_STATE
			//x8BitValueTo3CharBuffer(carry, RGBState.bValue);
//#else
			//x8BitValueTo3CharBuffer(carry, RGBState[stateIndex].bValue);
//#endif			
			//dataOut[15] = carry[0];
			//dataOut[16] = carry[1];
			//dataOut[17] = carry[2];
//
			//// include transition-flag
//#ifdef RGB_SINGLE_STATE
			//dataOut[18] = (RGBState.hardTransiton == 1) ? '1' : '0';
//#else
			//dataOut[18] = (RGBState[stateIndex].hardTransiton == 1) ? '1' : '0';
//#endif
			//
			//// delimiter
			//dataOut[19] = '$';
//
			//HMxx_SendData(dataOut);
		}
//****************************************************************************************************************
		else if(type == PROPERTY_TYPE_EXTLEVELSELECTOR)
		{
			//char carry[3];
//
			//// Property ID
			//dataOut[3] = data[1];
			//dataOut[4] = data[2];
			//dataOut[5] = data[3];
//
//#ifndef XLEVELSELECT_SINGLE_STATE
			//// retrieve state index  (if there are more than one elements of this type)
			//uint8_t stateIndex = getPropertyState(propertyID);
//#endif
			//// append on/off indicator (0/1)
//#ifdef	XLEVELSELECT_SINGLE_STATE
			//dataOut[6] = XLevelState.on_off_state ? '1' : '0';
//#else
			//dataOut[6] = XLevelState[stateIndex].on_off_state ? '1' : '0';
//#endif
			//// append the level value (3-chars)
//#ifdef	XLEVELSELECT_SINGLE_STATE
			//x8BitValueTo3CharBuffer(carry, XLevelState.levelValue);
//#else
			//x8BitValueTo3CharBuffer(carry, XLevelState[stateIndex].levelValue);
//#endif
			//dataOut[7] = carry[0];
			//dataOut[8] = carry[1];
			//dataOut[9] = carry[2];
			//dataOut[10] = '$';			// delimiter
//
			//HMxx_SendData(dataOut);
		}
//****************************************************************************************************************
		else if(type == PROPERTY_TYPE_SIMPLETIMESELECTOR)
		{
			char carry[3];

			// Property ID
			dataOut[3] = data[1];
			dataOut[4] = data[2];
			dataOut[5] = data[3];

#ifndef TIMESELECTSTRUCT_SINGLE_STATE
			// retrieve state index (if there are more than one elements of this type)
			uint8_t stateIndex = getPropertyState(propertyID);
#endif

#ifdef TIMESELECTSTRUCT_SINGLE_STATE
			x8BitValueTo3CharBuffer(carry, TimeSelectorState.timeSetterIndex);
#else
			x8BitValueTo3CharBuffer(carry, TimeSelectorState[stateIndex].timeSetterIndex);
#endif
			dataOut[6] = carry[2];

#ifdef TIMESELECTSTRUCT_SINGLE_STATE
			x8BitValueTo3CharBuffer(carry, TimeSelectorState.hour);
#else
			x8BitValueTo3CharBuffer(carry, TimeSelectorState[stateIndex].hour);
#endif
			dataOut[7] = carry[1];
			dataOut[8] = carry[2];

#ifdef TIMESELECTSTRUCT_SINGLE_STATE
			x8BitValueTo3CharBuffer(carry, TimeSelectorState.minute);
#else
			x8BitValueTo3CharBuffer(carry, TimeSelectorState[stateIndex].minute);
#endif
			dataOut[9] = carry[1];
			dataOut[10] = carry[2];

			// set activity status
			dataOut[11] = '1';			// temporary placeholder! (enabled state)

			dataOut[12] = '$';			// delimiter

			HMxx_SendData(dataOut);
		}
		//****************************************************************************************************************
		else if(type == PROPERTY_TYPE_TIMEFRAMESELECTOR)
		{
			//char carry[3];
//
			//// Property ID
			//dataOut[3] = data[1];
			//dataOut[4] = data[2];
			//dataOut[5] = data[3];
//
//#ifndef TIMEFRAMESTRUCT_SINGLE_STATE
			//// retrieve state index (if there are more than one elements of this type)
			//uint8_t stateIndex = getPropertyState(propertyID);
//#endif
//
			//// placeholder (reserved for some extra functions)
			//dataOut[6] = '0';
//
//#ifdef TIMEFRAMESTRUCT_SINGLE_STATE
			//x8BitValueTo3CharBuffer(carry, TimeFrameState.time_on.hour);
//#else
			//x8BitValueTo3CharBuffer(carry, TimeFrameState[stateIndex].time_on.hour);
//#endif
			//dataOut[7] = carry[1];
			//dataOut[8] = carry[2];
//
//#ifdef TIMEFRAMESTRUCT_SINGLE_STATE
			//x8BitValueTo3CharBuffer(carry, TimeFrameState.time_on.minute);
//#else
			//x8BitValueTo3CharBuffer(carry, TimeFrameState[stateIndex].time_on.minute);
//#endif
			//dataOut[9] = carry[1];
			//dataOut[10] = carry[2];
//
//#ifdef TIMEFRAMESTRUCT_SINGLE_STATE
			//x8BitValueTo3CharBuffer(carry, TimeFrameState.time_off.hour);
//#else
			//x8BitValueTo3CharBuffer(carry, TimeFrameState[stateIndex].time_off.hour);
//#endif
			//dataOut[11] = carry[1];
			//dataOut[12] = carry[2];
//
//#ifdef TIMEFRAMESTRUCT_SINGLE_STATE
			//x8BitValueTo3CharBuffer(carry, TimeFrameState.time_off.minute);
//#else
			//x8BitValueTo3CharBuffer(carry, TimeFrameState[stateIndex].time_off.minute);
//#endif
			//dataOut[13] = carry[1];
			//dataOut[14] = carry[2];
//
			//// set activity status
			//dataOut[15] = '1';			// temporary placeholder! (enabled state)
//
			//dataOut[16] = '$';			// delimiter
//
			//HMxx_SendData(dataOut);
		}
		//****************************************************************************************************************
		else if(type == PROPERTY_TYPE_NAVIGATOR)
		{
			// Property ID
			dataOut[3] = data[1];
			dataOut[4] = data[2];
			dataOut[5] = data[3];

			dataOut[6] = (propState & 0x01) ? '1' : '0';
			dataOut[7] = (propState & 0x02) ? '1' : '0';
			dataOut[8] = (propState & 0x04) ? '1' : '0';
			dataOut[9] = (propState & 0x08) ? '1' : '0';
			dataOut[10] = (propState & 0x10) ? '1' : '0';

			dataOut[11] = '$';
			
			HMxx_SendData(dataOut);
		}
	
		// TODO: add more complex types


	}
}

uint8_t propertyValuefromStartCharIndex(volatile char* data, uint8_t startIndex)
{
	return charsToU8Bit(data[startIndex], data[startIndex + 1], data[startIndex + 2]);
}

void onPropertyRequest(volatile char* data)
{
	// Syntax:
	// [0] == Command ID ('A')
	// [1 ... 3] == Device property index as string -> convert to uint8_t
	// finalized with '$'

	uint8_t pIndex = propertyValuefromStartCharIndex(data, 1);
	if(pIndex < PROPERTY_AMOUNT)
	{
		// return property at index
		sendProperty(pIndex);
	}
	else
	{
		// return property END / out of range
		HMxx_SendData("RSP:A:PEND$");
	}
}

void onPropertyDescriptionRequest(volatile char* data)
{
	// Syntax:
	// [0] = Command ID ('B')
	// [1] = Language ID (0 == English || 1 == German)
	// [2...4] = Device property ID as string -> convert to uint8_t
	// finalized with '$'

	uint8_t propertyID = propertyValuefromStartCharIndex(data, 2);
	sendPropertyDescription(propertyID, data[1]);
}

void sendPropertyGroup(uint8_t index)
{
	// Generate Response:

	uint8_t memberCount = getGroupMemberCountAtIndex(index);

	char carry[3];
	char dataOut[17];
	// set identifier
	dataOut[0] = 'I';
	dataOut[1] = 'P';
	dataOut[2] = 'G';
	// set group id
	x8BitValueTo3CharBuffer(carry, propertyGroups[index].groupID);
	dataOut[3] = carry[0];
	dataOut[4] = carry[1];
	dataOut[5] = carry[2];
	// set index
	x8BitValueTo3CharBuffer(carry, index);
	dataOut[6] = carry[0];
	dataOut[7] = carry[1];
	dataOut[8] = carry[2];
	// set member amount
	x8BitValueTo3CharBuffer(carry, memberCount);
	dataOut[9] = carry[0];
	dataOut[10] = carry[1];
	dataOut[11] = carry[2];
	// set image ID
	x8BitValueTo3CharBuffer(carry, propertyGroups[index].imageID);
	dataOut[12] = carry[0];
	dataOut[13] = carry[1];
	dataOut[14] = carry[2];

	dataOut[15] = '$';
	dataOut[16] = '\0';

	HMxx_SendData(dataOut);
}

void onPropertyGroupRequest(volatile char* data)
{
	// Syntax:
	// [0] = Command ID ('E')
	// [1..3] = Group-Index
	// finalized with '$'
	uint8_t groupIndex = charsToU8Bit(data[1], data[2], data[3]);

	if(groupIndex < PROPERTY_GROUP_AMOUNT)
	{
		// return group at index
		sendPropertyGroup(groupIndex);
	}
	else
	{
		// return group end / out of range
		HMxx_SendData("RSP:E:PGEND$");
	}
}

void onDetailedPropGroupInfoRequest(volatile char* data)
{
	// Syntax:
	// [0] = Command ID ('F')
	// [1] = Language ID (0 == English || 1 == German)
	// [2..4] = Group-ID as string
	// finalized with '$'
	
	if(PROPERTY_GROUP_AMOUNT > 0)
	{
		uint8_t groupID = charsToU8Bit(data[2], data[3], data[4]);
	
		char sBuffer[] = "GI:Sxxx$\0";
		char eBuffer[] = "GI:E$\0";
	
		sBuffer[4] = data[2];
		sBuffer[5] = data[3];
		sBuffer[6] = data[4];

		// iterate the Group struct to find the property ID
		for(uint8_t i = 0; i < PROPERTY_GROUP_AMOUNT; i++)
		{
			if(propertyGroups[i].groupID == groupID)
			{
				// send start tag
				HMxx_SendData(sBuffer);
				
				// send the member ID's
				char memIds[20];
				for(uint8_t m = 3; m < 18; m++)
					memIds[m] = '0';
				
				memIds[0] = 'm';
				memIds[1] = 'I';
				memIds[2] = '%';
				memIds[18] = '$';
				memIds[19] = '\0';
								
				for(uint8_t k = 0; k < MAX_GROUPMEMBER_COUNT; k++)
				{
					if(propertyGroups[i].memberIDs[k] != 0)
					{
						char threebuf[3];
						x8BitValueTo3CharBuffer(threebuf, propertyGroups[i].memberIDs[k]);
						
						memIds[3 + k*3] = threebuf[0];
						memIds[4 + k*3] = threebuf[1];
						memIds[5 + k*3] = threebuf[2];
					}
					else
					{
						break;	
					}
				}
				HMxx_SendData(memIds);				
				
				// send the appropriate language string
				if(data[1] == '1')
				{
					HMxx_SendData(propertyGroups[i].groupName_de);
				}
				else
				{
					HMxx_SendData(propertyGroups[i].groupName_en);
				}				
				
				// send end tag
				HMxx_SendData(eBuffer);
				break;
			}
		}
	}

}

void notifyPropertyChanged(uint8_t propertyID, uint8_t flags)
{
	if(HMxx_getConnectionStatus() == true)
	{
		char carry[3];
		char notification[19];
		notification[0] = 'D';
		notification[1] = 'n';
		notification[2] = 'P';
		notification[3] = 'c';
		notification[4] = 'x';
		notification[5] = '1';
		notification[6] = '=';
		x8BitValueTo3CharBuffer(carry, propertyID);
		notification[7] = carry[0];
		notification[8] = carry[1];
		notification[9] = carry[2];
		x8BitValueTo3CharBuffer(carry, getPropertyIndexFromID(propertyID));
		notification[10]= carry[0];
		notification[11]= carry[1];
		notification[12]= carry[2];

		notification[13]= (flags & PCHANGE_FLAG_ENTIREPROPERTY) ? '1' : '0';
		notification[14]= (flags & PCHANGE_FLAG_ENTIREPROPERTYDETAIL) ? '1' : '0';
		notification[15]= (flags & PCHANGE_FLAG_THISPROPERTY) ? '1' : '0';
		notification[16]= (flags & PCHANGE_FLAG_THISPROPERTYDETAIL) ? '1' : '0';

		notification[17]= '$';
		notification[18]=  0;

		HMxx_SendData(notification);
	}
}

void notifyPropertyGroupChanged(uint8_t groupID, uint8_t flags)
{
	if(HMxx_getConnectionStatus() == true)
	{
		char carry[3];
		char notification[19];
		notification[0] = 'D';
		notification[1] = 'n';
		notification[2] = 'P';
		notification[3] = 'G';
		notification[4] = 'c';
		notification[5] = '=';
		notification[6] = 't';
		x8BitValueTo3CharBuffer(carry, groupID);
		notification[7] = carry[0];
		notification[8] = carry[1];
		notification[9] = carry[2];
		x8BitValueTo3CharBuffer(carry, getPropertyGroupIndexFromID(groupID));
		notification[10]= carry[0];
		notification[11]= carry[1];
		notification[12]= carry[2];

		notification[13]= (flags & PCHANGE_FLAG_ENTIREPGROUPS) ? '1' : '0';
		notification[14]= (flags & PCHANGE_FLAG_ENTIREGROUPDETAIL) ? '1' : '0';
		notification[15]= (flags & PCHANGE_FLAG_THISPGROUP) ? '1' : '0';
		notification[16]= (flags & PCHANGE_FLAG_THISGROUPDETAIL) ? '1' : '0';

		notification[17]= '$';
		notification[18]=  0;

		HMxx_SendData(notification);
	}
}

void setDeviceInfoHeader(uint8_t imageID, char* message)
{
	// TODO: check this!!!! (Performance)!
	
	
	// at first send the start entry including the imageID for the header
	char data[12];
	sprintf(data, "%sxxx$", LAROOMY_DEVICEINFO_HEADER_START_ENTRY);
	char threeCache[3];
	x8BitValueTo3CharBuffer(threeCache, imageID);
	data[7] = threeCache[0];
	data[8] = threeCache[1];
	data[9] = threeCache[2];
	
	HMxx_SendData(data);
	// then send the message
	HMxx_SendDataUnlimited(message);
	
	_delay_ms(200); // temp  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	// finalize the transmission
	HMxx_SendData(LAROOMY_DEVICEINFO_HEADER_ENDNOTIFICATION);
}

void sendSimplePropertyState(uint8_t propertyID)
{
	uint8_t propState = getPropertyState(propertyID);

	char dataOut[12];

	// Process simple state-data

	// Response Identifier (Simple Property State Response)
	dataOut[0] = 'P';
	dataOut[1] = 'S';
	dataOut[2] = 'S';

	char threeBuf[3];

	// Set Property ID
	x8BitValueTo3CharBuffer(threeBuf, propertyID);
	dataOut[3] = threeBuf[0];
	dataOut[4] = threeBuf[1];
	dataOut[5] = threeBuf[2];

	// Set simple property state
	x8BitValueTo3CharBuffer(threeBuf, propState);
	dataOut[6] = threeBuf[0];
	dataOut[7] = threeBuf[1];
	dataOut[8] = threeBuf[2];

	// Set property enabled state
	dataOut[9]  = getPropertyEnabledStateFromID(propertyID) ? '1' : '0';

	dataOut[10] = '$';	// delimiter
	dataOut[11] = '\0'; // zero-terminator

	HMxx_SendData(dataOut);
}

void sendComplexPropertyState(uint8_t propertyID)
{

	// THIS IS OBSOLETE. notifyPropertyStateChanged(..) will do the same thing

	char tbuf[3];
	x8BitValueTo3CharBuffer(tbuf, propertyID);

	char rqData[6];
	rqData[0] = 'D';
	rqData[1] = tbuf[0];
	rqData[2] = tbuf[1];
	rqData[3] = tbuf[2];
	rqData[4] = '$';
	rqData[5] = '\0';

	onPropertyStateRequest(rqData);
}

void notifyPropertyStateChanged(uint8_t propertyID)
{
	char data[6];
	char threeCache[3];
	x8BitValueTo3CharBuffer(threeCache, propertyID);
	data[0] = 'D';
	data[1] = threeCache[0];
	data[2] = threeCache[1];
	data[3] = threeCache[2];
	data[4] = '$';
	data[5] = '\0';

	onPropertyStateRequest(data);
}

void onSetCommand(volatile char* data)
{
	// Syntax:
	// [0] = Command ID ('S')
	// [1..3] = Setter ID
	// [4..End] = Data (command specific)


	// Check specific commands:
	//
	if((data[1] == 'c')&&(data[2] == 'T')&&(data[3] == '&'))
	{
		//	"ScT&" == SetTime Command !

		//uint8_t h = 0, m = 0, s = 0;
//
		//h = charsToU8Bit(0, data[4], data[5]);
		//m = charsToU8Bit(0, data[6], data[7]);
		//s = charsToU8Bit(0, data[8], data[9]);

		//Timekeeper_SetTime(h, m, s);

		//clearGlobalControlFlag(TIMEKEEPER_LOST_POWER);
	}
	else if((data[1] == 'e')&&(data[2] == 'B')&&(data[3] == '§'))
	{
		// "SeB§" == enable binding command !

		// erase passkey array
		for(uint8_t i = 0; i < 11; i++)
		{
			deviceBindingPasskey[i] = '\0';
		}
		// record new passkey
		for(uint8_t i = 4; i < 14; i++)
		{
			if(data[i] == '$')
				break;

			deviceBindingPasskey[i-4] = data[i];			
		}
		// set parameters
		deviceBindingStatus = DEVICEBINDING_STATUS_REQUIRED;

		// TODO: save passkey to eeprom
		// TODO: save enable-parameter to eeprom
	}
	else if((data[1] == 'r')&&(data[2] == 'B')&&(data[3] == '>'))
	{
		// "SrB>" == release binding command !

		// erase passkey array
		for(uint8_t i = 0; i < 11; i++)
		{
			deviceBindingPasskey[i] = '\0';
		}
		// set parameters
		deviceBindingStatus = DEVICEBINDING_STATUS_NONE;

		// TODO: delete passkey from eeprom
		// TODO: override enable parameter in eeprom
	}
	else if((data[1] == 'f')&&(data[2] == 'R')&&(data[3] == '='))
	{
		// "SfR=" == Factory reset command

		ExecuteUserFactoryReset();
	}
}

void onBindingRequest(volatile char* data)
{
	if(deviceBindingStatus == DEVICEBINDING_STATUS_NONE)
	{
		// device binding inactive (should never be executed)
		HMxx_SendData(LAROOMY_XNG001_AUTHENTICATIONRESPONSE);
	}
	else
	{
		uint8_t counter = 1;
		char password[11];

		// record password
		for(uint8_t i = 0; i < 10; i++)
		{
			password[i] = data[counter];
			counter++;
			

			if((data[counter] == '>')&&(data[counter + 1] == '$'))
			{
				password[i] = '\0';
				break;
			}
		}
		// send response
		if(compareStringsExactly(password, deviceBindingPasskey))
		{
			HMxx_SendData(LAROOMY_XNG001_AUTHENTICATIONRESPONSE);
		}
		else
		{
			HMxx_SendData(LAROOMY_XNG001_AUTHENTICATIONRESPONSE_BINDINGPASSKEYINVALID);
		}
	}
}

void ExecuteUserFactoryReset()
{
	// TODO: execute user factory reset!
}


#endif /* COMMANDANALYZER_H_ */

/*
	sbi(LED_PORT, LED1_OUTPUT);
	longDelay(20);
	sbi(LED_PORT, LED2_OUTPUT);
	longDelay(100);
	sbi(LED_PORT, LED3_OUTPUT);
	longDelay(100);
	sbi(LED_PORT, LED7_OUTPUT);
	longDelay(100);
*/
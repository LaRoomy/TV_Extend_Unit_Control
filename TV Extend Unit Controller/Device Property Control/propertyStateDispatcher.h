/*
 * propertyStateDispatcher.h
 *
 *  Created: 05.05.2020 13:42:30
 *  Author: Hans Philipp Zimmermann
 *
 *	NOTE:
 *		The complex property state struct(s) defined in "deviceProperty.h" are not the device-control itself.
 *		The struct(s) are only the value storage for the properties. The control parameter of the property-related
 *		component control are not effected by changing the structs. To bind the struct-values to the component-control
 *		this header should implement the appropriate functions.
 *
 *		The functions should do 3 things:
 *			- Do the hardware-actions affecting the control
 *			- Store the resulting state in the complex-property-state-struct
 *			- Notify the remote application of the changes
 *
 *		If the user cannot change the device properties by control elements on the device and the device does not internally
 *		change the properties, the implementation of this is not necessary.
 */ 

#ifndef PROPERTYSTATEDISPATCHER_H_
#define PROPERTYSTATEDISPATCHER_H_

void devicePropertyStates_onInit()
{
	// align the initial property-states to the hardware!!!

	// maybe load states from the eeprom?!

	//eeLoadPropertyStates();
	


	// TODO: !!!!!!!!!!!!!!!!!!!!!!!!!
	
	
	// temp:
	//setGlobalControlFlag(CHECK_SINGLE_TIMER_EVENT);


}

void RGBSelector_setCustomColor(uint8_t red, uint8_t green, uint8_t blue)
{
	//RGBState.rValue = red;
	//RGBState.gValue = green;
	//RGBState.bValue = blue;
	//RGBState.command = CUSTOM_COLOR;
//
	//setCustomColorValues(red, green, blue);
//
	//if(HMxx_getConnectionStatus())
	//{
		//notifyPropertyStateChanged(EXAMPLE_RGB_SELECTOR_2);
	//}
}

void RGBSelector_setHardTransitionType(BOOL active)
{
	//RGBState.hardTransiton = active ? 1 : 0;
//
	//setTransitionType(active ? L_HARD_TRANSITION : L_SOFT_TRANSITION);
//
	//if(HMxx_getConnectionStatus())
	//{
		//notifyPropertyStateChanged(EXAMPLE_RGB_SELECTOR_2);
	//}
}

void RGBSelector_setProgram(uint8_t program)
{
	//RGBState.command = program;
//
	//selectProgram(program);
//
	//if(HMxx_getConnectionStatus())
	//{
		//notifyPropertyStateChanged(EXAMPLE_RGB_SELECTOR_2);
	//}
}

void TimeSelector_checkTime(uint8_t hr, uint8_t min)
{
	// TODO: if there are more than one Time-Selector -> iterate the IDs

	// check if the current time equals the switch-time
	//if((TimeSelectorState.hour == hr)&&(TimeSelectorState.minute == min))
	//{
		//// temp ????
		//setCustomColorValues(0, 255, 0);
	//}
}

void TimeFrameSelector_checkTime(uint8_t hr, uint8_t min)
{
	// check if the current time is inside the time-frame
	// if there are more time-frame-controls -> iterate IDs

	//if(isTimeInFrame(&TimeFrameState, hr, min) == TIMEFRAME_IS_IN_SCOPE)
	//{
		//// temp temp temp ....
		////StartPWM(PWM_CHANNEL_A);
		////StartPWM(PWM_CHANNEL_B);
////
		////SetPWMValue(PWM_CHANNEL_A, 255);
		////SetPWMValue(PWM_CHANNEL_B, 255);
	//}
	//else
	//{
		////SetPWMValue(PWM_CHANNEL_A, 0);
		////SetPWMValue(PWM_CHANNEL_B, 0);
		////
		////StopPWM(PWM_CHANNEL_A);
		////StopPWM(PWM_CHANNEL_B);
	//}
}


#endif /* PROPERTYSTATEDISPATCHER_H_ */
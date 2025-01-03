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
}

void RGBSelector_setCustomColor(uint8_t red, uint8_t green, uint8_t blue)
{
}

void RGBSelector_setHardTransitionType(BOOL active)
{
}

void RGBSelector_setProgram(uint8_t program)
{
}

void TimeSelector_checkTime(uint8_t hr, uint8_t min)
{
}

void TimeFrameSelector_checkTime(uint8_t hr, uint8_t min)
{
}

#endif /* PROPERTYSTATEDISPATCHER_H_ */
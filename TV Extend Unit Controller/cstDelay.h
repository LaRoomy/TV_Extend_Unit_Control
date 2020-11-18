/*
 * cstDelay.h
 *
 * Created: 12.12.2017 22:17:56
 *  Author: hans-
 */ 


#ifndef CSTDELAY_H_
#define CSTDELAY_H_

#include <util/delay.h>

void longDelay(uint16_t milliseconds)
{
	for(uint16_t i = 0; i < milliseconds; i++)
	{
		_delay_ms(1);
	}
}

#endif /* CSTDELAY_H_ */
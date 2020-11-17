/*
 * TEUC.h
 *
 * Created: 17.11.2020 08:19:13
 *  Author: Hans Philipp Zimmermann
 */ 


#ifndef TEUC_H_
#define TEUC_H_

#include <avr/io.h>


#define FALSE	0
#define TRUE	1

typedef uint8_t BOOL;

#include "General/BitUtilitiesEx.h"
#include "General/cstDelay.h"

#include "TEUC PPDef.h"
#include "Movement Control/tvUnitDriver.h"

BOOL switch_preventer;

void InitControlParameter()
{
	switch_preventer = FALSE;
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
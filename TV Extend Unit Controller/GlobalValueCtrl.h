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


void InitGlobalValues()
{
	deviceBindingStatus = 0;
	switch_preventer = FALSE;

	for(uint8_t i = 0; i < 11; i++)
		deviceBindingPasskey[i] = '\0';

}

// TODO: save necessary values to eeprom
//			- deviceBindingStatus
//			- passkey



#endif /* GLOBALVALUECTRL_H_ */
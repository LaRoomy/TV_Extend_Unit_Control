/*
 * atmega324_timer.h
 *
 * Created: 18.11.2020 07:52:29
 *  Author: Hans Philipp Zimmermann
 */ 

#ifndef ATMEGA324_TIMER_H_
#define ATMEGA324_TIMER_H_

#define ACTIVATE	1
#define DISABLE		0

#include <avr/interrupt.h>

volatile uint8_t timer_counter;
volatile uint8_t OneSecond_event;
volatile uint8_t TwoHMilliSec_event;
volatile uint8_t HalfSecond_event;

void ActivateTimer0()
{
	timer_counter = 0;
	OneSecond_event = DISABLE;
	TwoHMilliSec_event = DISABLE;
	HalfSecond_event = DISABLE;

	TCCR0B = (1<<CS00)|(1<<CS02);	// activate timer with Prescaler CLK / 1024 (/256 = 37.5 ticks = 1sec.)
	TIMSK0 = (1<<TOIE0);			// activate interrupt
	sei();							// set global interrupt flag
}


#endif /* ATMEGA324_TIMER_H_ */
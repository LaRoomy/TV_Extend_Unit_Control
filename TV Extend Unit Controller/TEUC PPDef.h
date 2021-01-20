/*
 * TEUC_PPDef.h
 *
 * Created: 17.11.2020 08:02:42
 *  Author: Hans Philipp Zimmermann
 *
 *	Pin Definition and Initialization for the TV Extend Unit
 *
 */ 

#ifndef TEUC_PPDEF_H_
#define TEUC_PPDEF_H_

// PORTA:
#define /*_In_*/	TOUCHBUTTON_SENSE							PORTA0	// 0 == touch (by default pulled up to VCC)
#define /*_In_*/	TILT_DRIVE_CURRENTSENSOR_ADC				PORTA1	// varied (analog)
#define /*_In_*/	LINEAR_DRIVE_CURRENTSENSOR_ADC				PORTA2	// varied (analog)
#define /*_In_*/	COVER_DRIVE_RIGHT_CURRENTSENSOR_ADC			PORTA3	// varied (analog)
#define /*_In_*/	COVER_DRIVE_LEFT_CURRENTSENSOR_ADC			PORTA4	// varied (analog)
#define /*_Out_*/	ADDITIONAL_LOAD_OUTPUT						PORTA5	// 1 == ON / 0 == OFF (must be 0 at startup - pulled down to GND)
#define /*_Out_*/	TV_UNIT_MOTORDRIVER_ENABLE					PORTA6	// 1 == disabled / 0 == enabled (must be 1 at startup)
#define /*_Out_*/	COVERDRIVE_UNITDRIVER_ENABLE				PORTA7	// 1 == disabled / 0 == enabled (must be 1 at startup)

// PORTB:
#define /*_Out_*/	IN3_TVUNIT_LINEARDRIVE		PORTB0	// 0 == not switched / 1 == set IN pin to GND (must be 0 at startup)
#define /*_Out_*/	IN4_TVUNIT_LINEARDRIVE		PORTB1	// 0 == not switched / 1 == set IN pin to GND (must be 0 at startup)
#define /*_Out_*/	IN1_TVUNIT_TILTDRIVE		PORTB2	// 0 == not switched / 1 == set IN pin to GND (must be 0 at startup)
#define /*_Out_*/	IN2_TVUNIT_TILTDRIVE		PORTB3	// 0 == not switched / 1 == set IN pin to GND (must be 0 at startup)
#define /*_In_*/	EXTRA_BOARD_BUTTON_LEFT		PORTB4	// extra button on the board (left sided)
#define /*_In_*/	EXTRA_BOARD_BUTTON_RIGHT	PORTB5	// extra button on the board (right sided)  + MOSI !
//					MISO						PORTB6
//					SCK							PORTB7

// PORTC:
#define /*_In_*/	DOOR_SENSOR							PORTC0	// 1 == doors not closed / 0 == all doors are closed
#define /*_Out_*/	IN1_COVERDRIVE_RIGHT				PORTC1	// 0 == not switched / 1 == set IN pin to GND (must be 0 at startup)
#define /*_Out_*/	IN2_COVERDRIVE_RIGHT				PORTC2	// 0 == not switched / 1 == set IN pin to GND (must be 0 at startup)
#define /*_Out_*/	IN3_COVERDRIVE_LEFT					PORTC3	// 0 == not switched / 1 == set IN pin to GND (must be 0 at startup)
#define /*_Out_*/	IN4_COVERDRIVE_LEFT					PORTC4	// 0 == not switched / 1 == set IN pin to GND (must be 0 at startup)
#define /*_In_*/	LINEARDRIVE_SENSOR_BACK_POSITION	PORTC5	// 0 == sensor activated / 1 == not active (pulled up to VCC)
#define /*_In_*/	LINEARDRIVE_SENSOR_MID_POSITION		PORTC6	// 0 == sensor activated / 1 == not active (pulled up to VCC)
#define /*_In_*/	LINEARDRIVE_SENSOR_FRONT_POSITION	PORTC7	// 0 == sensor activated / 1 == not active (pulled up to VCC)

// PORTD:
//					USART RXD							PORTD0
//					USART TXD							PORTD1
#define /*_In_*/	TILTDRIVE_SENSOR_FRONT_POSITION		PORTD2	// 0 == sensor activated / 1 == not active (pulled up to VCC)
#define /*_In_*/	TILTDRIVE_SENSOR_BACK_POSITION		PORTD3	// 0 == sensor activated / 1 == not active (pulled up to VCC)
#define /*_In_*/	COVERDRIVE_SENSOR_LEFT_CLOSE		PORTD4	// 0 == sensor activated / 1 == not active (pulled up to VCC)
#define /*_In_*/	COVERDRIVE_SENSOR_LEFT_OPEN			PORTD5	// 0 == sensor activated / 1 == not active (pulled up to VCC)
#define /*_In_*/	COVERDRIVE_SENSOR_RIGHT_CLOSE		PORTD6	// 0 == sensor activated / 1 == not active (pulled up to VCC)
#define /*_In_*/	COVERDRIVE_SENSOR_RIGHT_OPEN		PORTD7	// 0 == sensor activated / 1 == not active (pulled up to VCC)

// appropriate pin/port definitions
#define		TOUCHBUTTON_PIN					PINA

#define		TVUNIT_DRIVER_ENABLE_PORT		PORTA
#define		COVERDRIVE_DRIVER_ENABLE_PORT	PORTA

#define		LINEARDRIVE_MOVEMENT_INPUT_PORT	PORTB
#define		TILTDRIVE_MOVEMENT_INPUT_PORT	PORTB

#define		EXTRA_BOARD_BUTTON_PIN			PINB

#define		COVERDRIVE_MOVEMENT_INPUT_PORT	PORTC

#define		DOOR_SENSOR_PIN					PINC
#define		LINEAR_DRIVE_SENSOR_PIN			PINC

#define		TILT_DRIVE_SENSOR_PIN			PIND
#define		COVERDRIVE_SENSOR_PIN			PIND

void InitializePorts()
{
	DDRA = 0b11100000;
	PORTA = 0b11000000;

	DDRB = 0b00001111;
	PORTB = 0b00000000;

	DDRC = 0b00011110;
	PORTC = 0b00000000;

	DDRD = 0b00000000;
	PORTD = 0b00000000;
}


#endif /* TEUC_PPDEF_H_ */
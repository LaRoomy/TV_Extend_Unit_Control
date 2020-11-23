// *
// UART Config for Atmega324
// *
// LAST UPDATED: 16.12.2019
// *
// This is a specific modification for the tv moving unit
// *
#ifndef _ATMEGA324_USART_H_
#define _ATMEGA324_USART_H_

#ifndef F_CPU
#define F_CPU	9830400UL
#else
#if(F_CPU != 9830400UL)
#error "Invalid F_CPU detected!"
#endif
#endif // F_CPU

#include <avr/interrupt.h>

#ifndef USART0_BAUD
#define USART0_BAUD		9600
#endif

//#ifndef USART1_BAUD
//#define USART1_BAUD		9600
//#endif

#ifndef TRANSMISSION_COMPLETE_CHARACTER
#define TRANSMISSION_COMPLETE_CHARACTER		'$'
#endif

#define USART0_MAX_STRING_LENGTH		20
//#define	USART1_MAX_STRING_LENGTH		80

volatile char USART0_RX_Buffer[20];
//volatile char USART1_RX_Buffer[100];

volatile uint8_t usart0_transmission_complete;
//volatile uint8_t usart1_transmission_complete;

volatile uint8_t usart0_transmission_data_available;

volatile uint8_t usart0_store_index;
//volatile uint8_t usart1_store_index;

uint16_t Usart_Calculate_UBRR_VALUE(uint32_t baud)
{
	return (F_CPU/16/(baud - 1));
}

void Usart0_Clear_RX_Buffer()
{
	for (uint8_t i = 0; i < USART0_MAX_STRING_LENGTH; i++)
	{
		USART0_RX_Buffer[i] = '\0';
	}
	usart0_store_index = 0;
	usart0_transmission_complete = 0;
	usart0_transmission_data_available = 0;
}

//void Usart1_Clear_RX_Buffer()
//{
	//for (uint8_t i = 0; i < USART1_MAX_STRING_LENGTH; i++)
	//{
		//USART1_RX_Buffer[i] = '\0';
	//}
	//usart1_store_index = 0;
	//usart1_transmission_complete = 0;
//}


void Usart0_Init(uint32_t baudrate)
{
	// Set globals
	Usart0_Clear_RX_Buffer();
	// Calculate UBRR Value
	uint16_t uval = Usart_Calculate_UBRR_VALUE(baudrate);
	// set baud rate:
	UBRR0H = (unsigned char)(uval >> 8);
	UBRR0L = (unsigned char)uval;
	
	// enable receiver and transmitter
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1<<RXCIE0);// enable receiver interrupt

	//set frame format
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);	// removed: (1 << URSEL)

	// enable global interrupt flag
	sei();
	
	_delay_ms(100);
}

//void Usart1_Init(uint32_t baudrate)
//{
	//// Set globals
	//Usart1_Clear_RX_Buffer();
	//// Calculate UBRR Value
	//uint16_t uval = Usart_Calculate_UBRR_VALUE(baudrate);
	//// set baud rate:
	//UBRR1H = (unsigned char)(uval >> 8);
	//UBRR1L = (unsigned char)uval;
	//// enable receiver and transmitter
	//UCSR1B |= (1 << RXEN1) | (1 << TXEN1) | (1<<RXCIE1);// enable receiver interrupt
//
	////set frame format
	//UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);	// removed: (1 << URSEL)
//
	//// enable global interrupt flag
	//sei();
	//
	//_delay_ms(100);
//}


void Usart0_Write(char tx)
{
	// write a char into transmit buffer
	while (!(UCSR0A&(1 << UDRE0)));
	UDR0 = tx;
}

//void Usart1_Write(char tx)
//{
	//// write a char into transmit buffer
	//while (!(UCSR1A&(1 << UDRE1)));
	//UDR1 = tx;
//}


void Usart0_Check_AsyncTransmission()
{
	// check for the transmission to complete and set the transmisson_complete-flag
	if ((usart0_store_index == (USART0_MAX_STRING_LENGTH - 1))
		|| (USART0_RX_Buffer[usart0_store_index] == TRANSMISSION_COMPLETE_CHARACTER))
	{
		usart0_transmission_complete = 1;

		// delimit the string with '\0' if possible
		if ((usart0_store_index < (USART0_MAX_STRING_LENGTH - 1))
			&& (USART0_RX_Buffer[usart0_store_index] == TRANSMISSION_COMPLETE_CHARACTER))
		{
			USART0_RX_Buffer[usart0_store_index] = '\0';
		}
	}
}

//void Usart1_Check_AsyncTransmission()
//{
	////if(USART1_RX_Buffer[usart1_store_index] == '\r')// && (USART1_RX_Buffer[usart1_store_index + 1] == '\n'))
		////sbi(PORTC, PORTC1);
	//
	//// check for the transmission to complete and set the transmisson_complete-flag
	//if ((usart1_store_index == (USART1_MAX_STRING_LENGTH - 1))
	//|| (USART1_RX_Buffer[usart1_store_index] == TRANSMISSION_COMPLETE_CHARACTER)
	//|| (USART1_RX_Buffer[usart1_store_index] == '\r')
	//)
	//{
		//usart1_transmission_complete = 1;
//
		//// delimit the string with '\0' if possible
		//if ((usart1_store_index < (USART1_MAX_STRING_LENGTH - 1))
			//&& (USART1_RX_Buffer[usart1_store_index] == TRANSMISSION_COMPLETE_CHARACTER))
		//{
			//USART1_RX_Buffer[usart1_store_index] = '\0';
		//}
	//}
//}

void Usart0_StoreAsync(char rx)
{
	// controls the reception-buffer (a-sync)
	if (usart0_store_index > (USART0_MAX_STRING_LENGTH - 1))
		return;
	else
	{
		USART0_RX_Buffer[usart0_store_index] = rx;
		Usart0_Check_AsyncTransmission();
		usart0_store_index++;
	}
}

//void Usart1_StoreAsync(char rx)
//{
	//// controls the reception-buffer (a-sync)
	//if (usart1_store_index > (USART1_MAX_STRING_LENGTH - 1))
		//return;
	//else
	//{
		//USART1_RX_Buffer[usart1_store_index] = rx;
		//Usart1_Check_AsyncTransmission();
		//usart1_store_index++;
	//}
//}

void Usart0_OnRXInterrupt()
{
	char rx_val;
	rx_val = UDR0;
	usart0_transmission_data_available = 1;

	if (usart0_transmission_complete == 0)
	{
		Usart0_StoreAsync(rx_val);
	}
}

//void Usart1_OnRXInterrupt()
//{
	//char rx_val;
	//rx_val = UDR1;
//
	//if (usart1_transmission_complete == 0)
	//{
		//Usart1_StoreAsync(rx_val);
	//}
//}

void Usart0_WriteString(volatile char* string)
{
	uint8_t i = 0;
	
	while((string[i] != '\0')&&(string[i] != TRANSMISSION_COMPLETE_CHARACTER))
	{
		Usart0_Write(string[i]);
		i++;
		
		if(i == USART0_MAX_STRING_LENGTH)
			break;
	}
}

//void Usart1_WriteString(volatile char* string)
//{
	//uint8_t i = 0;
	//
	//while((string[i] != '\0')&&(string[i] != TRANSMISSION_COMPLETE_CHARACTER))
	//{
		//Usart1_Write(string[i]);
		//i++;
		//
		//if(i == USART1_MAX_STRING_LENGTH)
			//break;
	//}
//}

#endif //_ATMEGA324_USART_H_

/*

RX Interrupt vector 0 + 1:
	
ISR(USART0_RX_vect)
{
	Usart0_OnRXInterrupt();
}

ISR(USART1_RX_vect)
{
	Usart1_OnRXInterrupt();
}

*/


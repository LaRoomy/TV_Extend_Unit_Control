#ifndef _ATMEGA324_USART_H_
#define _ATMEGA324_USART_H_

#ifndef F_CPU
#define F_CPU 9830400UL
#else
#if (F_CPU != 9830400UL)
#error "Invalid F_CPU detected!"
#endif
#endif // F_CPU

#include <avr/interrupt.h>

#ifndef USART0_BAUD
#define USART0_BAUD 9600UL
#endif

#ifndef TRANSMISSION_COMPLETE_CHARACTER
#define TRANSMISSION_COMPLETE_CHARACTER '$'
#endif

#define USART0_MAX_STRING_LENGTH 20

#define UBRR0_VAL ((F_CPU / (8 * USART0_BAUD)) - 1)

volatile char USART0_RX_Buffer[20];
volatile uint8_t usart0_transmission_complete;
volatile uint8_t usart0_transmission_data_available;
volatile uint8_t usart0_store_index;

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

void Usart0_Init()
{
	// Set globals
	Usart0_Clear_RX_Buffer();

	// set baud rate:
	UBRR0H = (unsigned char)(UBRR0_VAL >> 8);
	UBRR0L = (unsigned char)(UBRR0_VAL);

	UCSR0A |= (1 << U2X0); // double the USART transmission rate

	// enable receiver and transmitter
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // enable receiver interrupt

	// set frame format
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);

	// enable global interrupt flag
	sei();

	_delay_ms(50);
}

void Usart0_Write(char tx)
{
	// write a char into transmit buffer
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = tx;
}

void Usart0_Check_AsyncTransmission()
{
	// check for the transmission to complete and set the transmisson_complete-flag
	if ((usart0_store_index == (USART0_MAX_STRING_LENGTH - 1)) || (USART0_RX_Buffer[usart0_store_index] == TRANSMISSION_COMPLETE_CHARACTER))
	{
		usart0_transmission_complete = 1;

		// override the string-delimiter with the string-terminator (if applicable)
		if (USART0_RX_Buffer[usart0_store_index] == TRANSMISSION_COMPLETE_CHARACTER)
		{
			USART0_RX_Buffer[usart0_store_index] = '\0';
		}
	}
}

void Usart0_StoreAsync(char rx)
{
	// controls the reception-buffer
	if (usart0_store_index >= USART0_MAX_STRING_LENGTH)
		return;
	else
	{
		USART0_RX_Buffer[usart0_store_index] = rx;
		Usart0_Check_AsyncTransmission();
		usart0_store_index++;
	}
}

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

void Usart0_WriteString(volatile char *string)
{
	uint8_t i = 0;

	while ((string[i] != '\0') && (i < USART0_MAX_STRING_LENGTH)) //&&(string[i] != TRANSMISSION_COMPLETE_CHARACTER))
	{
		Usart0_Write(string[i]);
		i++;
	}
	_delay_ms(10);
}
#endif //_ATMEGA324_USART_H_

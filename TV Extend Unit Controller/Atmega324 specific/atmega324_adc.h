/*
 * atmega324_adc.h
 *
 *  Created: 06.01.2021 13:25:17
 *  Author: Hans Philipp Zimmermann
 */ 


#ifndef ATMEGA324_ADC_H_
#define ATMEGA324_ADC_H_

void ADC_Init()
{
	ADMUX = (1<<REFS0);// AVCC as Reference
	ADCSRA = (1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);// enable and set Prescaler 128 - 76,8kHz
}

uint16_t ADC_Read(uint8_t channel)
{
	if(channel < 8)
	{
		ADMUX &= ~0x0F;// clear lower bits
		ADMUX |= channel;// set channel
		ADCSRA |= (1<<ADSC);// start conversion

		while(ADCSRA & (1<<ADSC));// wait for conversion to complete
		return ADC;
	}
	return 0;
}

uint16_t ADC_ReadAverage(uint8_t channel)
{
	uint16_t average = 0;
	
	for(uint8_t i = 0; i < 5; i++)
	{
		average += ADC_Read(channel);
		_delay_ms(2);
	}
	return (uint16_t)(average/5);
}



#endif /* ATMEGA324_ADC_H_ */
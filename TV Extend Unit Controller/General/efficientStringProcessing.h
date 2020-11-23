/*
 * efficientStringProcessing.h
 *
 * Created: 02.10.2017 21:19:13
 *  Author: Hans Philipp Zimmermann
 
	recent changes 03.11.2020
 */ 


#ifndef EFFICIENTSTRINGPROCESSING_H_
#define EFFICIENTSTRINGPROCESSING_H_

#include <stdbool.h>

uint8_t charToNum(char l);

// !strings must be zero-terminated
// !string-length are limited to 255
bool compareStringsExactly(volatile char* str1, char* str2)
{
	uint8_t	counter = 0;
	
	while(1)
	{
		if(str1[counter] != str2[counter])
		return false;
		
		if((str1[counter] == 0)&&(str2[counter] == 0))
			return true;

		if((str1[counter] == '\0')&&(str2[counter] == '\0'))
			return true;			
			
		if(counter == 255)
			return false;
		
		counter++;
	}
}

bool stringStartsWith(volatile char* str1, char* str2)
{
	if((str1 != NULL) && (str2 !=  NULL))
	{
		if((str1[0] != 0)&&(str2[0] != 0))
		{
			uint8_t	counter = 0;
	
			while(1)
			{
				if((str1[counter] == '\0')||(str2[counter] == '\0'))
				return true;
		
				if(str1[counter] != str2[counter])
				return false;
		
				if(counter == 255)
				return false;
		
				counter++;
			}
		}
		return false;
	}
	else
	{
		if((str1 == NULL)&&(str2 == NULL))
			return true;
		else
			return false;
	}
}

// converts a string to an 8bit (uint8_t) value
// string must have 3 fields minimum!!
// if the number is shorter than 3 digits, the remaining fields must contain a zero terminator
uint8_t convertStringToNumber(char* string)
{
	uint8_t value = 0;
	
	if(string[0] != '\0')
	{
		value = charToNum(string[0]) * 1;
	
		if(string[1] != '\0')
		{
			value *= 10;
			value += (charToNum(string[1]) * 1);
		
			if(string[2] != '\0')
			{
				value *= 10;
				value += (charToNum(string[2]) * 1);
			}
		}
	}
	return value;
}

uint8_t charToNum(char l)
{
	switch(l)
	{
		case '0':
		return 0;
		case '1':
		return 1;
		case '2':
		return 2;
		case '3':
		return 3;
		case '4':
		return 4;
		case '5':
		return 5;
		case '6':
		return 6;
		case '7':
		return 7;
		case '8':
		return 8;
		case '9':
		return 9;
		default:
		return 0;// maybe return 10?? //10 means error (the char wasn't a number)		
	}
}

// NOTE this function only processes figures lower than 10 !
char numToChar(uint8_t num)
{
	switch(num)
	{
		case 0:
		return '0';
		case 1:
		return '1';
		case 2:
		return '2';
		case 3:
		return '3';
		case 4:
		return '4';
		case 5:
		return '5';
		case 6:
		return '6';
		case 7:
		return '7';
		case 8:
		return '8';
		case 9:
		return '9';
		default:
		return 'X';
	}
}

uint8_t charsToU8Bit(char a, char b, char c)
{
	// NOTE: the format of the number must be: "abc" -> for example: 123 = (a = 1), (b = 2), (c = 3)
	uint16_t result = 0;

	result += (charToNum(a) *100);
	result += (charToNum(b) *10);
	result += charToNum(c);

	if(result < 256)
		return (uint8_t)result;
	else
		return 0;
}

void x8BitValueTo3CharBuffer(char* buffer, uint8_t value)
{
	uint8_t h = ((uint8_t)(value / 100));// get count of hundred
	value -= (100*h);// remove the hundred

	uint8_t t = ((uint8_t)(value / 10));// get count of tenner
	value -= (10*t);// remove the tenner
	
	buffer[0] = numToChar(h);
	buffer[1] = numToChar(t);
	buffer[2] = numToChar(value);
}

#endif /* EFFICIENTSTRINGPROCESSING_H_ */
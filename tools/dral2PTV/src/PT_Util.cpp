////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PT_Util.h"

#include <stdio.h>
#include <stdarg.h>
void FastStringCreate( char *dest, char *first, ... );
//void FastStringCreate( char *dest, char *first, va_list ap );


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: FastStringCat
// Description: build a string
// 
//
////////////////////////////////////////////////////////////////////////////////
char *FastStringCat( char *dest, const char *src )
{
	while( *src != '\0' )
	{
		*(dest++) = *(src++);
		//src++;
		//dest++;
	}

	*dest = '\0';
	
	return dest;
}

////////////////////////////////////////////////////////////////////////////////
//
// FastStringCopy
// takes variable args and prints out to a dest buffer
//
////////////////////////////////////////////////////////////////////////////////
//#ifdef WIN32
void FastStringCreate( char *dest, char *first, ... )
{
	va_list	ap;
	char *value;

	//parse the args list
	va_start(ap, first);
	char *current = dest;

	current = FastStringCat( current, first );
	if ( strcmp( first, "\n" ) != 0 )
	{
		int numEntries = 0;
		do 
		{
			//do a fast strcat of the next parameter. stop when we reach "\n"
			value = va_arg(ap, char *);
			current = FastStringCat( current, value);		

			numEntries++;
		}while ( ( strcmp( value, "\n" ) != 0 ) && ( numEntries < 256 ) );
	}

	return;
}
/*
#else	// UNIX version needs to use varargs (???)
extern "C"{
void FastStringCreate( char *dest, va_list ap )
va_dcl
{
	char *value;
	//va_list ap;

	//parse the args list
	va_start( ap );

	char *first = va_arg(ap, char *);
	char *current = dest;

	current = FastStringCat( current, first );

	if ( strcmp( first, "\n" ) != 0 )
	{
		int numEntries = 0;
		do 
		{
			//do a fast strcat of the next parameter. stop when we reach "\n"
			value = va_arg(ap, char *);
			current = FastStringCat( current, value);		

			numEntries++;
		}while ( ( strcmp( value, "\n" ) != 0 ) && ( numEntries < 256 ) );
	}

	return;
}
#endif
*/	


////////////////////////////////////////////////////////////////////////////////
//
// ultoascii
// Fast integer to ascii routines. You must provide the buffer. returns a 
// pointer to the first digit in the input buffer, 
// making it easy to use in a funciton parameter
////////////////////////////////////////////////////////////////////////////////
char *ultoascii( PT_ULONG number, char *buffer )
{
	char *digit;

	//work backwards in the buffer and return a pointer to the first digit
	buffer[255] = '\0';
	digit = &buffer[255];
	
//	digit--;
	do 
	{
		//digit--;
		*(--digit) = '0' + (char) ( number % 10 );
		number /= 10;
	} while ( number );
	
	return( digit );
} /*** end ultoascii() ****/



////////////////////////////////////////////////////////////////////////////////
//
// ltoascii
// Fast integer to ascii routines. You must provide the buffer. returns a 
// pointer to the first digit in the input buffer, 
// making it easy to use in a funciton parameter
////////////////////////////////////////////////////////////////////////////////
char *ltoascii( long number, char *buffer )
{
	char *ret_val;
	
	// two separate cases
	if (number < 0) 
	{
		//tack on a - sign
		ret_val = ultoascii(( unsigned long ) ( 0 - number ), buffer );
		*(--ret_val) = '-';
	} 
	else 
	{
		ret_val = ultoascii( (unsigned long) number, buffer );
	}

	return( ret_val );
} /*** end ltoascii() ***/

char *bytetox( PT_UCHAR number, char *buffer )
{
	char highDigit = (number >> 4);
	if ( (highDigit & 0xf) < 10 )
	{
		*buffer = '0' + (char)(highDigit & 0xf);
	}
	else 
	{
		// alpha digit
		*buffer = 'a' + (char) (highDigit & 0xf) - 10;
	}

	char lowDigit = (number & 15);
	if ( (lowDigit & 0xf) < 10 )
	{
		*(buffer + 1)= '0' + (char)(lowDigit & 0xf);
	}
	else 
	{
		// alpha digit
		*(buffer + 1)= 'a' + (char) (lowDigit & 0xf) - 10;
	}

	// and assume this is the end of the string
	*(buffer + 2) = '\0';

	return (buffer + 2);
}

////////////////////////////////////////////////////////////////////////////////
//
// ultox
// Fast integer to hex string routine. You must provide the buffer. returns a 
// pointer to the first digit in the input buffer, 
// making it easy to use in a function parameter
////////////////////////////////////////////////////////////////////////////////
char *ultox( PT_ULONG number, char *buffer )
{
	char *digit;
	
	//work backwards in the buffer and return a pointer to the first digit
	buffer[255] = '\0';
	digit = &buffer[255];
	
	do 
	{
		digit--;

		// mod 16?
		if ( ( number & 0xf ) < 10) 
		{
			// numerical digit
			*digit = '0' + (char) (number & 0xf);
		} else 
		{
			// alpha digit
			*digit = 'a' + (char) (number & 0xf) - 10;
		}

		// divide by 16
		number = number >> 4;
	} while ( number );
	
	return(digit);
}



 




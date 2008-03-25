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

#ifndef __PT_UTIL_H
#define __PT_UTIL_H

#ifndef PT_MAX_PATH
#define PT_MAX_PATH		4096
#endif

#include "PT_Types.h"

//
// FastStringCat
// custom strcpy - returns ptr to next position in dest to be added
////////////////////////////////////////////////////////////////////////////////
char *FastStringCat( char *dest, const char *src );

//
// FastStringCopy
// takes variable args and prints out to a dest buffer
////////////////////////////////////////////////////////////////////////////////
void FastStringCreate( char *dest, char *first, ... );

//
// ultoascii
// Fast integer to ascii routines. You must provide the buffer. returns a 
// pointer to the first digit in the input buffer, 
// making it easy to use in a funciton parameter
////////////////////////////////////////////////////////////////////////////////
char *ultoascii( PT_ULONG number, char *buffer );

//
// ltoascii
// Fast integer to ascii routines. You must provide the buffer. returns a 
// pointer to the first digit in the input buffer, 
// making it easy to use in a funciton parameter
////////////////////////////////////////////////////////////////////////////////
char *ltoascii( long number, char *buffer );

//
// ultox
// Fast integer to hex string routine. You must provide the buffer. returns a 
// pointer to the first digit in the input buffer, 
// making it easy to use in a funciton parameter
////////////////////////////////////////////////////////////////////////////////
char *ultox( PT_ULONG number, char *buffer );

//
// bytetox
// Fast byte to hex string routine. You must provide the buffer. returns a 
// pointer to the next place in the buffer to place a byte, 
// making it easy to with outputtiing Byte arrays as addresses
////////////////////////////////////////////////////////////////////////////////
char *bytetox( PT_UCHAR number, char *buffer );

char *CreateMemMappedTmpFile();

char *ReturnMemMappedTmpFile();

#endif
  




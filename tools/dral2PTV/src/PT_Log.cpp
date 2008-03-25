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

#ifndef __PT_LOG_CPP
#define __PT_LOG_CPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PT_Log.h"

// reference count so that we don't try to reopen the same log file and 
// so we know when to close it
static int refCount = 0;

static FILE *g_pFile = NULL;

//
// log_init
// If not opened, open the log file for writing
// If opened, increase the reference count
////////////////////////////////////////////////////////////////////////////////
void log_init( const char* szFile )
{
	if ( !refCount )
	{
		if ( szFile == NULL || ( strcmp(szFile, "stderr") == 0 ))
		{
			g_pFile = stderr;
		}
		else
		{
			g_pFile = fopen( szFile, "wb" );
		}
		
		if ( g_pFile ) refCount++;
	}
	else
	{
		// log file already open
		refCount++;
	}
}

//
// log_uninit
// Decrement the reference count
// If zero, close the log file
////////////////////////////////////////////////////////////////////////////////
void log_uninit( )
{
	refCount--;
	
	if ( refCount <= 0 )
	{
		fclose( g_pFile );
		refCount = 0;
		g_pFile = NULL;
	}
}


//
// log_warning
// log a warning message
////////////////////////////////////////////////////////////////////////////////
void log_warning( const char *szMessage )
{
	if ( g_pFile && szMessage)
	{
		fwrite( szMessage, sizeof( char ), strlen( szMessage ), g_pFile );
		
		//if the last char isn't a \n, add it!
		if ( szMessage[ strlen( szMessage ) - 1] != '\n' )
		{
			fwrite( "\n", sizeof( char ), 1, g_pFile );
		}
	}
}

//
// log_fatal
// log a warning message and core dump
////////////////////////////////////////////////////////////////////////////////
void log_fatal( const char *szMessage )
{
	log_warning( szMessage );
	exit( -1 );
}

//
// log_assert
// log a warning message and core dump
////////////////////////////////////////////////////////////////////////////////
void log_assert( char result)
{
	if ( !result )
	{
		char outMsg[ 4096 ];
		strcpy( outMsg, "Assertion Failed: " );
		//strcat( outMsg, msg );
		log_fatal( outMsg );
	}
}


#endif	//__PT_LOG_CPP


 





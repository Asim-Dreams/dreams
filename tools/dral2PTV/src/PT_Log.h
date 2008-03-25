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

#ifndef __PT_LOG_H
#define __PT_LOG_H

#include <stdio.h>
#include <string.h>

// define some easy logging macros
#ifndef PT_INIT_LOGGING
#define	PT_INIT_LOGGING(XX)		log_init(XX);
#define	PT_UNINIT_LOGGING		log_uninit();
#define	PT_FATAL(XX)					{ log_fatal(XX); }
#define	PT_FATAL1(XX, AA)				{ char tmp[4096]; sprintf(tmp, XX, AA); log_fatal(tmp); }
#define	PT_FATAL2(XX, AA, BB)			{ char tmp[4096]; sprintf(tmp, XX, AA, BB ); log_fatal(tmp); }
#define	PT_FATAL3(XX, AA, BB, CC)		{ char tmp[4096]; sprintf(tmp, XX, AA, BB, CC ); log_fatal(tmp); }
#define	PT_FATAL4(XX, AA, BB, CC, DD)	{ char tmp[4096]; sprintf(tmp, XX, AA, BB, CC, DD); log_fatal(tmp); }

#define	PT_WARNING(XX)					{ log_warning(XX); }
#define	PT_WARNING1(XX, AA)				{ char tmp[4096]; sprintf(tmp, XX, AA ); log_warning(tmp); }
#define	PT_WARNING2(XX, AA, BB)			{ char tmp[4096]; sprintf(tmp, XX, AA, BB); log_warning(tmp); }
#define	PT_WARNING3(XX, AA, BB, CC)		{ char tmp[4096]; sprintf(tmp, XX, AA, BB, CC); log_warning(tmp); }
#define	PT_WARNING4(XX, AA, BB, CC, DD)	{ char tmp[4096]; sprintf(tmp, XX, AA, BB, CC, DD); log_warning(tmp); }

#ifdef ASSERTIONS_ON
#define	PT_ASSERT(XX)			log_assert(XX);
#else
#define	PT_ASSERT(XX)			
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
//
//	Logging functions
//
///////////////////////////////////////////////////////////////////////////////

//
// log_init
// If not opened, open the log file for writing
// If opened, increase the reference count
// "stderr" and "stdout" are valid file names, default is stderr
////////////////////////////////////////////////////////////////////////////////
extern void log_init( const char* file = NULL );

//
// log_uninit
// Decrement the reference count
// If zero, close the log file
////////////////////////////////////////////////////////////////////////////////
extern void log_uninit( );


//
// log_warning
// log a warning message
////////////////////////////////////////////////////////////////////////////////
extern void log_warning( const char *szMessage );

//
// log_fatal
// log a warning message and core dump
////////////////////////////////////////////////////////////////////////////////
extern void log_fatal( const char *szMessage );


//
// log_assert
// if (FALSE) log a warning message and core dump
////////////////////////////////////////////////////////////////////////////////
extern void log_assert( char result );

#endif	//__PT_LOG_H

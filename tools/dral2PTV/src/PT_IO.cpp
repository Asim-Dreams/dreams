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

#ifndef __PT_IO_CPP
#define __PT_IO_CPP

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "PT_IO.h"
#include "PT_Util.h"
#include <zlib.h>
#include <stdlib.h>

//use regexp parsing for header info
#include "rgx.h"


#define REMINDER(Q)		"#pragma message(Q)"

// ASCII strings to write
#define PIPE_OPEN                       "Open"
#define PIPE_CLOSE                      "Close"
#define PIPE_CONFIG_DATA                "ConfigData"
#define PIPE_CONFIG_EVENT               "ConfigEvent"
#define PIPE_CONFIG_RECORD              "ConfigRecord"
#define PIPE_CONFIG_STRUCT              "ConfigStruct"
#define PIPE_UPDATE_STRUCT              "Update"
#define PIPE_CLEAR_STRUCT               "Clear"
#define PIPE_COMMENT                    "Comment"

// Hash table configuration
#define FIRST_KEY                       24
#define HASH_SIZE                       (1024)          // MUST be power of 2
#define HASH_MASK                       (HASH_SIZE - 1) 

#define BIN_PUT_ULONG(x)    next_loc = put_word(x, next_loc)
#define BIN_PUT_UCHAR(x)    *((unsigned char *) (&m_buf[next_loc])) = ((unsigned char) (x) & 0xff); \
                            next_loc += sizeof(unsigned char);

// Hash Keys for standard tokens
#define PIPE_OPEN_KEY                       (1)
#define PIPE_CLOSE_KEY                      (2)
#define PIPE_CONFIG_DATA_KEY                (3)
#define PIPE_CONFIG_EVENT_KEY               (4)
#define PIPE_CONFIG_RECORD_KEY              (5)
#define PIPE_COMMENT_KEY                    (6)
#define PIPE_CONFIG_STRUCT_KEY              (7)
#define PIPE_UPDATE_STRUCT_KEY              (8)
#define PIPE_CLEAR_STRUCT_KEY               (9)

// File format version strings
#define PIPE_VERSION_STRING_3_0         "PipeTrace Format Version 3.0"
#define PIPE_VERSION_3_0                10

#define PIPE_VERSION_STRING_3_1         "PipeTrace Format Version 3.1"
#define PIPE_VERSION_3_1                11

#define PIPE_VERSION_STRING_3_2         "PipeTrace Format Version 3.2"
#define PIPE_VERSION_3_2                12

// 6/02 msblacks - added support for watch window structures
#define PIPE_VERSION_STRING_3_3         "PipeTrace Format Version 3.3"
#define PIPE_VERSION_3_3                13


////////////////////////////////////////////////////////////////////////////////
//
//	eqstr function for STL hash maps
//
////////////////////////////////////////////////////////////////////////////////
bool PT_eqstr::operator()(const char* s1, const char* s2) const
{
	return strcmp(s1, s2) < 0;
};


bool PT_eqstr::operator()(const PT_ULONG id1, const PT_ULONG id2 ) const
{
	return ( id1 < id2 );
};

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Writer::PT_WriteStream( )
// Description: Default Constructor. Must be called by all derived classes
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_WriteStream::PT_WriteStream( ) 
{
	PT_INIT_LOGGING( "stderr" );

	m_keySize = 1;
	m_nextKey = FIRST_KEY;

	m_DataCounter = 0;

	//do some init stuff
	m_file = NULL;
    m_szFileName[ 0 ] = '\0';

	m_nextRecord = 1;
	m_pStore = NULL;
//	m_lastRecord = -1;

//	m_HashTable = NULL;

//	m_pStore = new PT_DataStore( NULL );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Writer::~PT_WriteStream( )
// Description: Desctuctor. Must be called by all derived classes
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_WriteStream::~PT_WriteStream( )
{
	PT_WriteStream::Close( );

	if ( m_pStore )
	{
		delete m_pStore;
	}

	PT_UNINIT_LOGGING;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Writer::Open
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_WriteStream::Open( const char *szFileName, char *version )
{

	char *time_str;	//represent current time as a string

	m_nextRecord = 1;
	//m_lastRecord = -1;
	
	if ( m_file != NULL ) 
	{
		PT_WARNING1("Cannot open pipetrace %s for output. File already opened", szFileName);
		return PT_Error_FileOpenFailed;
	}

	// if filename is null, use "stdout"
//	if ( !szFileName ) 
//	{
//		strcpy( szFileName,"stdout" );
//	}

	// determine if it is gzipped and then open the file
	
#ifdef USE_ZLIB
	if ( szFileName == NULL || strcmp( szFileName, "stdout") == 0 )
	{
		// use standard output
		m_file = gzdopen( 1 /*stdout*/, "wb" );
	}
	else
	{
		m_file = gzopen( szFileName, "wb" );
	}
#else
        char *szTemp = (char *)szFileName;
	m_file = xfopen( szTemp, "wb" );
#endif 

	if ( !m_file )
	{
		PT_WARNING("Cannot open pipetrace for output.");
		m_file = NULL;
		return PT_Error_FileOpenFailed;
	}
	
	// OK, open succeeded, set up values
	if (szFileName) strcpy( m_szFileName, szFileName );

	// next up for derived classes: set version string, timestamp, and format
	// also, set up the hash table

	// Write a header. uses the virtual GetVersionInfo call to determine
#ifdef USE_ZLIB
	gzprintf( m_file, "%s\n", GetVersionInfo( ) );
#else
	fprintf( m_file, "%s\n", GetVersionInfo( ) );
#endif

	// Put a timestamp
	time_t now = time( NULL );
	time_str = ctime( &now );
	
	time_str[ strlen( time_str ) - 1 ] = '\0';
#ifdef USE_ZLIB
	gzprintf( m_file, "[Timestamp: %s]\n", time_str );

	// Internal constants line
	gzprintf( m_file, "[Hash: %d] [Key: %d]\n", HASH_SIZE, FIRST_KEY );

	// Format name - uses the virtual GetFormat call to determine
	gzprintf( m_file, "[Format: %s]\n", GetFormatString( ) );

	// if we are in a namespace other than std, print it out here
	if ( m_szNamespace != NULL )
		gzprintf( m_file, "[Namespace: %s] [NamespaceID: %d]\n", m_szNamespace, GetNameSpaceID( ) );

	// End the header.
	gzprintf( m_file, "BEGIN\n" );
#else
	fprintf( m_file, "[Timestamp: %s]\n", time_str );
	fprintf( m_file, "[Hash: %d] [Key: %d]\n", HASH_SIZE, FIRST_KEY );
	fprintf( m_file, "[Format: %s]\n", GetFormatString( ) );
	fprintf( m_file, "BEGIN\n" );
	if ( m_szNamespace != NULL )
		fprintf( m_file, "[Namespace: %s] [NamespaceID: %d]\n", m_szNamespace, GetNameSpaceID( ) );
#endif


	if ( ! m_pStore )
	{
		m_pStore = new PT_DataStore( NULL, PT_FALSE );
	}

	// make sure we are all set up
	m_keySize = 1;
	m_nextKey = FIRST_KEY;
	m_DataCounter = 0;
	m_nextRecord = 1;

//	if ( FAILED ( InitHashTable() ) )
//	{
//		return PT_Error_HashInitFailed;
//	}

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Writer::Close( )
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_WriteStream::Close( )
{
	//try to close the file
	if ( !m_file )
	{
		// already closed, not a bug
//		PT_WARNING("Cannot close output pipetrace for output.");
		return PT_Error_OK;
	}

	//flush the stream if necessary
	Flush( );

#ifdef USE_ZLIB
	gzclose( m_file );
#else
	xfclose( m_file );
#endif

//	REMINDER( "hash close" );
//	FreeHashTable( );
	if ( m_pStore )
	{
		delete m_pStore;
		m_pStore = NULL;
	}

	m_file = NULL;

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Writer::Flush
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_WriteStream::Flush( )
{
	//do nothing for now. if derived classes implement buffered output, 
	//then it should implement

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Writer::WriteStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_WriteStream::WriteStatement( PT_Record *current )
{
        //	REMINDER( "implement here" );

	return PT_Error_NotImpl;
}

// No longer needed
/*
////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_WriteStream::InitHashTable
// Description: sets up the hash table for inserting definitions
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////


PT_Error 
PT_WriteStream::InitHashTable( )
{
	PT_Error Error = PT_Error_OK;

	if ( !m_HashTable )
	{
		m_HashTable = 
			new hash_map< const char*, PT_Config *, hash< const char* >, PT_eqstr >( HASH_SIZE );

		if ( !m_HashTable )
		{	
			// alloc failed
			return PT_Error_OutOfMemory;
		}
	}

	return Error;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_WriteStream::FreeHashTable
// Description: frees up the hash table
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_WriteStream::FreeHashTable( )
{
	if ( m_HashTable )
	{
		m_HashTable->clear();
		delete m_HashTable;
	}

	return PT_Error_OK;
}
*/

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_WriteStream::GetNameSpaceID
// Description: 
// Hash the namespace name and return an ID 
// 
// Parameters: 
// Returns: 0 if in standard namesapace
//
////////////////////////////////////////////////////////////////////////////////
int PT_WriteStream::GetNameSpaceID( )
{
	if ( m_szNamespace == NULL ) return 0;

	// simple add up the ascii values in the name
	int val = 0;
	const char *current = m_szNamespace;
	while( *current != 0 )
	{
		val += *current;
		current++;
	}

	return val;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_WriteStream::ValidateName
// Description: 
// Make sure the definition name is kosher and wasn't previously defined
// 
// Parameters: 
// Returns: false if name is already found 
//
////////////////////////////////////////////////////////////////////////////////
PT_BOOL PT_WriteStream::ValidateName( const char *Name, PT_Config *Config )
{
//	PT_ASSERT( m_HashTable != NULL );
	PT_ASSERT( m_pStore != NULL );

	// Make sure it is not a reserved token
	if ( strcmp( Name, PIPE_OPEN ) == 0 ) return PT_FALSE;
	if ( strcmp( Name, PIPE_CLOSE ) == 0 ) return PT_FALSE;
	if ( strcmp( Name, PIPE_CONFIG_DATA ) == 0 ) return PT_FALSE;
	if ( strcmp( Name, PIPE_CONFIG_EVENT ) == 0 ) return PT_FALSE;
	if ( strcmp( Name, PIPE_CONFIG_RECORD ) == 0 ) return PT_FALSE;
	if ( strcmp( Name, PIPE_COMMENT ) == 0 ) return PT_FALSE;

	// Make sure it is a reasonable name
	// reasonable names have a letter as first char, then
	// the rest are alphanumerics or _ 
	const char *matches = Name;
	PT_BOOL first = PT_TRUE;

	while ( *matches ) 
	{
		if ( ( first && ! isalpha( *matches ) ) ||
		     ( !first && !( isalnum( *matches ) || *matches == '_') ) ) 
		{
			PT_FATAL("Invalid definition name in pipetrace");
		}
		matches++;
		first = PT_FALSE;
	}

	PT_Error Error = PT_Error_Fail;

	// Let the datastore assign the key value
	Config->m_key = NextKey( );

	if ( Config->GetType() == Pipe_Config_Record )
	{
		Error = m_pStore->OnConfigRecord( *(PT_Config_Record *)(Config) );
	}
	else if ( Config->GetType() == Pipe_Config_Event )
	{
		Error = m_pStore->OnConfigEvent( *(PT_Config_Event *)(Config) );
	}
	else if ( Config->GetType() == Pipe_Config_Data )
	{
		Error = m_pStore->OnConfigData( *(PT_Config_Data *)(Config) );
	}
	else if ( Config->GetType() == Pipe_Config_Struct )
	{
		Error = m_pStore->OnConfigStruct( *(PT_Config_Struct *)(Config) );
	}

	if ( Error != PT_Error_OK )
	{
		return PT_FALSE;
	}
	else
	{
		return PT_TRUE;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_WriteStream::NextKey
// Description: returns the next key for a config definition, and increases
//				the key size if necessary
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_KEY PT_WriteStream::NextKey( )
{
	//m_nextKey++;

	// Increase the key size if we're at 7, 15, or 22 bits long.
    // This allows us to ALWAYS keep the high bit clear, which is important for 
    // binary format
    if ( m_nextKey == ( ( 1 << ( m_keySize * 8 - 1 ) ) /*- 1*/) ) 
	{
		m_keySize++;
    }

	return m_nextKey++;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::PT_ASCIIWriteStream
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_ASCIIWriteStream::PT_ASCIIWriteStream( const char *szFile, PT_Error *Error, const char *szNamespace ) : PT_WriteStream( )
{
	m_szNamespace = szNamespace;

	if ( Error ) 
	{
		*Error = Open( szFile );
	}
	else
	{
		Open( szFile );
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::WriteStatement
// Description: 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_WriteStream::WriteStatement( PT_Statement &current, PT_ID *ID )
{
	char DebugMessage[ PT_MAX_PATH ];

	//do some sanity checking
	PT_ASSERT( m_file != NULL );

	PT_Error Error = PT_Error_OK;

	//PT_WARNING(current.ToString( DebugMessage ))

	if ( m_DataCounter != 0 )
	{
		// we recently saw an event, make sure we have EventData coming in
		if ( current.GetType( ) != Pipe_EventData )
		{
			PT_FATAL( "Fatal error. All EventData must be written directly after an event." );
		}

		// good, correct type, decrement reference counter
		m_DataCounter--;
	}

	//determine it's type and write it out
	switch( current.GetType( ) )
	{
		case Pipe_Open: 
			{
				PT_Open_Statement &st = ( PT_Open_Statement& ) current;

				// open a new record

				// this error isn't needed anymore b/c if you used a Reader to create the 
				// statements, it will already know the ID # to use
//				if ( ID == NULL ) 
//				{
//					PT_WARNING( "PT_Error_NullPointer: must supply ID var to open a record" )
//					PT_WARNING( st.ToString( DebugMessage ) );
//					return PT_Error_NullPointer;
//				}

				if ( !st.m_recordType ) 
				{
					PT_WARNING( "PT_Error_NullPointer: invalid open record statement" )
					PT_WARNING( st.ToString( DebugMessage ) );
					return PT_Error_NullPointer;
				}

				// compile a statement string and write to disk
				Error = HandleOpenStatement( st );

				// now assign the out param a new ID value
				if ( PT_SUCCEEDED( Error ) ) 
				{ 
					if ( ID ) 
					{
						*ID = m_nextRecord;
					}
					st.m_ID = m_nextRecord;
					m_nextRecord++;
				}
				break;
			}
		case Pipe_Close: 
			{
				PT_Close_Statement &st = ( PT_Close_Statement& ) current;

				// are we closing a valid record?
				// do a half-assed attempt at sanity checking
				if ( (st.m_ID >= m_nextRecord && st.m_namespace == 0) || st.m_ID == 0 )
				{
					PT_WARNING( "PT_Error_BadID: can't close this record" );
					PT_WARNING( st.ToString( DebugMessage ) );
					return PT_Error_BadID;
				}

				// compile a statement string and write to disk
				Error = HandleCloseStatement( st );
				break;
			}
		case Pipe_Comment:
			{
				PT_Comment_Statement &st = ( PT_Comment_Statement& ) current;

				// just write the string
				Error = HandleCommentStatement( st );
				break;
			}
		case Pipe_Config_Event: 
			{
				PT_Config_Event &st = ( PT_Config_Event &) current;

				if ( ValidateName( st.m_event_name, &st ) == PT_FALSE )
				{
					//name already exists!!! this is not legal
					PT_FATAL1( "Event definition name %s already exists! core dumping", st.m_event_name )
				}

				//make sure the event char is a readable character
				if (!isgraph( st.m_event_char )) 
				{
					PT_WARNING( "Invalid character for pipetrace event" );
					PT_WARNING( st.ToString( DebugMessage ) );
				}

				Error = HandleConfigEvent( st );
				break;
			}
		case Pipe_Config_Data: 
			{
				PT_Config_Data &st = ( PT_Config_Data& ) current;

				if ( ValidateName( st.m_name, &st ) == PT_FALSE )
				{
					// name already exists!!! this is not legal
					PT_FATAL1( "Data definition name %s already exists! Core dumping", st.m_name )
				}

				if ( ( st.m_datatype == Pipe_ByteArray || st.m_datatype == Pipe_Enumeration ) 
					 && ( st.m_arrayLen <= 0 ) )
				{
					// name already exists!!! this is not legal
					PT_FATAL1( "invalid byte array or enum definition %s! core dumping", st.m_name )
				}

				Error = HandleConfigData( st );

				if ( st.m_datatype == Pipe_Enumeration )
				{
					// to maintain backward compatibility with PSLib, we must
					// pretend like we are hashing enums by incrementing
					// the key value
					for ( unsigned int i = 0; i < st.m_arrayLen; i++ )
					{
						NextKey( );
					}
				}

				break;
			}
		case Pipe_Config_Record: 
		   {
				PT_Config_Record &st = ( PT_Config_Record& ) current;

				if ( ValidateName( st.m_name, &st ) == PT_FALSE )
				{
					// name already exists!!! this is not legal
					PT_FATAL1( "Record definition name %s already exists! Core dumping", st.m_name )
				}

				Error = HandleConfigRecord( st );
				break;
		   }
		case Pipe_Config_Struct: 
		   {
				PT_Config_Struct &st = ( PT_Config_Struct& ) current;

				if ( ValidateName( st.m_name, &st ) == PT_FALSE )
				{
					// name already exists!!! this is not legal
					PT_FATAL1( "Struct definition name %s already exists! Core dumping", st.m_name )
				}

				Error = HandleConfigStruct( st );
				break;
		   }
		case Pipe_Clear_Struct: 
		   {
				PT_Clear_Struct &st = ( PT_Clear_Struct& ) current;

				if (st.m_struct == NULL)
				{
					PT_FATAL( "Struct definition must be included in a clear! Core dumping" )
				}
				
				if (st.m_row >= st.m_struct->m_numRows)
				{
					PT_FATAL2( "Row number %d is out of bounds for struct %s", st.m_row, st.m_struct->m_name );
				}

				if (st.m_instanceNum >= st.m_struct->m_numInstances)
				{
					PT_FATAL2( "Instance number %d is out of bounds for struct %s", st.m_instanceNum, st.m_struct->m_name );
				}

				// recID is allowed to be 0 (so that we can jumpstart info into a struct)
				if (st.m_ID >= m_nextRecord && st.m_namespace == 0)
				{
					PT_WARNING1( "PT_Error_BadID: bad record ID (%d) for struct clear", st.m_ID )
					return PT_Error_BadID;
				}

				Error = HandleClearStruct( st );
				break;
		   }
		case Pipe_Update_Struct: 
		   {
				PT_Update_Struct &st = ( PT_Update_Struct& ) current;

				if (st.m_struct == NULL)
				{
					PT_FATAL( "Struct definition must be included in an event! Core dumping" )
				}
		
				if (st.m_row >= st.m_struct->m_numRows)
				{
					PT_FATAL2( "Row number %d is out of bounds for struct %s", st.m_row, st.m_struct->m_name );
				}

				if (st.m_instanceNum >= st.m_struct->m_numInstances)
				{
					PT_FATAL2( "Instance number %d is out of bounds for struct %s", st.m_instanceNum, st.m_struct->m_name );
				}

				// recID is allowed to be 0 (so that we can jumpstart info into a struct)
				if (st.m_ID >= m_nextRecord && st.m_namespace == 0)
				{
					PT_WARNING1( "PT_Error_BadID: bad record ID (%d) for struct update", st.m_ID )
					return PT_Error_BadID;
				}

				Error = HandleUpdateStruct( st );

				// now, do sanity checking to make sure that the next m_num_values
				// statements passed to WriteStatement are of type Pipe_EventData
				m_DataCounter = st.m_num_values;

				break;
		   }
		case Pipe_Event: 
			{
				PT_Event_Statement &st = ( PT_Event_Statement& ) current;

				// are working with a valid record?
				// do a half-assed attempt at sanity checking
				if ( (st.m_namespace == 0 && st.m_ID >= m_nextRecord) || st.m_ID == 0 )
				{
					PT_WARNING2( "PT_Error_BadID: bad record ID (%d) for event %s", st.m_ID, st.m_EventDef->m_event_name )
					return PT_Error_BadID;
				}

				if ( st.m_EventDef == NULL )
				{
					// event must have an attached definition
					PT_FATAL( "Event definition must be included in an event! Core dumping" )
				}

				Error = HandleEventStatement( st );

				// now, do sanity checking to make sure that the next m_num_values
				// statements passed to WriteStatement are of type Pipe_EventData
				m_DataCounter = st.m_num_values;

				break; 
			}
		case Pipe_EventData:
		case Pipe_Data:
			{
				// treat EventData and Data the same since they have the same data members, and
				// we've already decremented the reference count for EventData at the top of this
				// method
				PT_Data_Statement &st = ( PT_Data_Statement& ) current;

				// are working with a valid record?
				// do a half-assed attempt at sanity checking
				if ( (st.m_namespace == 0 && st.m_ID >= m_nextRecord) || st.m_ID == 0 )
				{
					PT_WARNING2( "PT_Error_BadID: bad record ID (%d) for data %s", st.m_ID, st.m_DataDef->m_name );
					return PT_Error_BadID;
				}

				if ( st.m_DataDef == NULL )
				{
					// event must have an attached definition
					PT_FATAL( "Data definition must be included with data! Core dumping" );
				}

				if ( (st.m_DataDef->m_datatype == Pipe_Enumeration) && 
					( *(( PT_ULONG *) ( st.m_pData )) > st.m_DataDef->m_arrayLen ))
				{
					PT_WARNING2( "PT_Error_BadIndex: enum %s has an invalid index for recID %d", st.m_DataDef->m_name, st.m_ID );
					return PT_Error_BadIndex;
				}
				
				Error = HandleDataStatement( st );
				break;
			}
		default: 
			{
				//invalid type
				PT_WARNING( "PT_Error_BadStatement: no such type" );
				return PT_Error_BadStatement;
			}
	}
	
	return Error;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::GetVersionInfo
// Description: returns file version to be written
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
const char *
PT_ASCIIWriteStream::GetVersionInfo( )
{ 
	//return PIPE_VERSION_STRING_3_2; 
	return PIPE_VERSION_STRING_3_3; 
} 

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::GetVersionInfo
// Description: returns file version to be written
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
const char *
PT_BinaryWriteStream::GetVersionInfo( )
{ 
	//return PIPE_VERSION_STRING_3_2; 
	return PIPE_VERSION_STRING_3_3; 
} 

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleOpenStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleOpenStatement( PT_Open_Statement &st )
{
	// Format:
	// xxxx RecordType
	//
	return FastStringWrite( ultoascii(m_nextRecord, m_buf), " ", st.m_recordType->m_name, "\n" );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleOpenStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleOpenStatement( PT_Open_Statement &st )
{
	// Format:
	// key [0 ID] 
	//
	if ( !st.m_recordType )
	{
		return PT_Error_BadStatement;
	}
	
	int next_loc = put_leader( m_nextRecord, *(st.m_recordType) );
	return BinaryWrite( m_buf, next_loc );
}



////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleCloseStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleCloseStatement( PT_Close_Statement &st )
{
	// Format:
	// xxxx Close
	//
	if (st.m_namespace == 0)
		return FastStringWrite( ultoascii( st.m_ID, m_buf ), " ", PIPE_CLOSE, "\n" );
	else
		return FastStringWrite( ultoascii( st.m_namespace, m_buf2 ), ":", ultoascii( st.m_ID, m_buf ), " ", PIPE_CLOSE, "\n" );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleCloseStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleCloseStatement( PT_Close_Statement &st )
{
	// Format:
	// Close [0 ID]
	//
	int next_loc = put_leader( st.m_ID, PIPE_CLOSE_KEY );
	return BinaryWrite( m_buf, next_loc );

}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleCommentStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleCommentStatement( PT_Comment_Statement &st )
{
	// Format:
	// xxxx Comment
	// 
	return FastStringWrite( "0", " ", PIPE_COMMENT, " ", st.m_szComment, "\n" );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleCommentStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleCommentStatement( PT_Comment_Statement &st )
{
	// Format:
	// Comment [0 ID]
	//
	int next_loc = put_leader( 0, PIPE_COMMENT_KEY );
	char *buf = st.m_szComment;
	while ( (m_buf[next_loc++] = *buf++) );
	
	return BinaryWrite( m_buf, next_loc );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleConfigEvent
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleConfigEvent( PT_Config_Event &st )
{
	//build a temp string
	char temp[2]; 
	temp[0] = st.m_event_char;
	temp[1] = '\0';
	
	return FastStringWrite(		"0", " ", 
								PIPE_CONFIG_EVENT, " ",
								st.m_event_name, " ",
								temp, " ",
								ltoascii((long) st.m_event_color, m_buf), " ", 
								st.m_description, "\n" );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleConfigEvent
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleConfigEvent( PT_Config_Event &st )
{
	// Format:
	// ConfigEvent [0 ID] Name 0 Character Color Description 0
	//
	const char *buf;

	int next_loc = put_leader( 0, PIPE_CONFIG_EVENT_KEY );
	buf = st.m_event_name;

	while ( (m_buf[next_loc++] = *buf++) );

	BIN_PUT_UCHAR( st.m_event_char );
	BIN_PUT_ULONG( st.m_event_color );
	
	buf = st.m_description;
	while ( (m_buf[next_loc++] = *buf++) );
	
	return BinaryWrite( m_buf, next_loc );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleConfigData
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleConfigData( PT_Config_Data &st )
{
	PT_Error Error;

	//
	// Format:
	// 0 ConfigData Name Type [type specific info] Description
	//

	// if it's an enum, do some prework and create a string
	char EnumString[ PT_MAX_PATH ];
	EnumString[ 0 ] = '\0';
	char *pEnum = EnumString;
	if ( st.m_datatype == Pipe_Enumeration )
	{		
		for ( PT_UINT i = 0; i < st.m_arrayLen; i++ )
		{
			pEnum = FastStringCat( pEnum, st.m_enumArray[ i ] );
			pEnum = FastStringCat( pEnum, " " );
		}
		
		// now we can write to disk
		Error = FastStringWrite(	"0", " ", 
									PIPE_CONFIG_DATA, " ",
									st.m_name, " ",
									DataTypeToString( st.m_datatype ), " ",
									ultoascii( st.m_arrayLen, m_buf ), " ",
									EnumString, /*" ",*/ st.m_description, "\n" );
	}
	// if it's a bytearray, add a length value
	else if ( st.m_datatype == Pipe_ByteArray )
	{
		Error = FastStringWrite(	"0", " ", 
									PIPE_CONFIG_DATA, " ",
									st.m_name, " ",
									DataTypeToString( st.m_datatype ), " ",
									ultoascii( st.m_arrayLen, m_buf ), " ",
									st.m_description, "\n" );

	}
	//it must be some other type
	else
	{					
		Error = FastStringWrite(	"0", " ", 
									PIPE_CONFIG_DATA, " ",
									st.m_name, " ",
									DataTypeToString( st.m_datatype ), " ",
									st.m_description, "\n" );

	}

	return Error;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleConfigData
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleConfigData( PT_Config_Data &st )
{
	//PT_Error Error;
	const char *buf;

	// Format:
	// ConfigData [0 ID] Name 0 Type Description 0
	//
	
	int next_loc = put_leader( 0, PIPE_CONFIG_DATA_KEY );

	buf = st.m_name;
	while ( (m_buf[ next_loc++ ] = *buf++) );
	BIN_PUT_ULONG( st.m_datatype );

	//
	// Datatype specific values.
	//
	if ( st.m_datatype == Pipe_Enumeration || st.m_datatype == Pipe_ByteArray ) 
	{
		BIN_PUT_ULONG( st.m_arrayLen );
	}

	buf = st.m_description;
	while ( (m_buf[next_loc++] = *buf++) );

	//
	// Enumeration values.
	// Yes, there is a reason these go at the end.
	// It makes the binary reader easier to deal with.
	//
	if ( st.m_datatype == Pipe_Enumeration ) 
	{
		//
		// Now, add the enumeration names
		//
		for ( unsigned int i = 0; i < st.m_arrayLen; i++ ) 
		{
			buf = st.m_enumArray[ i ];
			while ( (m_buf[next_loc++] = *buf++) );
		}
	}

	PT_ASSERT( next_loc < PT_MAX_PATH );
	
	return BinaryWrite( m_buf, next_loc );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleConfigRecord
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleConfigRecord( PT_Config_Record &st )
{

	// Format:
	// 0 ConfigRecord Name Description
	//

	return	FastStringWrite(	"0", " ", 
								PIPE_CONFIG_RECORD, " ",
								st.m_name, " ",
								st.m_description, "\n" );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleConfigRecord
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleConfigRecord( PT_Config_Record &st )
{

	// Format:
	// ConfigRecord [0 ID] Name 0 Description 0
	//
	const char *buf;
	int next_loc = put_leader( 0, PIPE_CONFIG_RECORD_KEY );

	buf = st.m_name;
	while ( ( m_buf[next_loc++] = *buf++ ) );

	buf = st.m_description;
	while ( (m_buf[next_loc++] = *buf++) ) ;
	
	return BinaryWrite( m_buf, next_loc );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleConfigStruct
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleConfigStruct( PT_Config_Struct &st )
{

	// Format:
	// 0 ConfigStruct name #instances #rows #cols {col0 col1 col2 col3} szDescription
	// (colX must be a predefined Pipe_Config_Data)
	char ColString[ PT_MAX_PATH ];
	ColString[ 0 ] = '\0';
	char *pCol = ColString;
	for ( PT_UINT i = 0; i < st.m_numCols; i++ )
	{
		pCol = FastStringCat( pCol, st.m_colArray[ i ]->m_name );
		pCol = FastStringCat( pCol, " " );
	}

	return	FastStringWrite(	"0", " ", 
								PIPE_CONFIG_STRUCT, " ",
								st.m_name, " ",
								ultoascii(st.m_numInstances, m_buf3), " ",
								ultoascii(st.m_numRows, m_buf2), " ",
								ultoascii(st.m_numCols, m_buf), " ",
								ColString, /*" ",*/
								st.m_description, "\n" );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleConfigStruct
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleConfigStruct( PT_Config_Struct &st )
{
	// Format:
	// 0 ConfigStruct [0 ID] Name 0 szDescription 0 #instances #rows #cols {colKey1 colKey2  colKey3 colKey4} 
	// (colX must be a predefined Pipe_Config_Data)

	const char *buf;
	int next_loc = put_leader( 0, PIPE_CONFIG_STRUCT_KEY );

	buf = st.m_name;
	while ( ( m_buf[ next_loc++ ] = *buf++ ));
	
	buf = st.m_description;
	while ( ( m_buf[ next_loc++ ] = *buf++ ) );
	
	
	BIN_PUT_ULONG( st.m_numInstances );
	BIN_PUT_ULONG( st.m_numRows );
	BIN_PUT_UCHAR( st.m_numCols );
	
	for( unsigned int i = 0; i < st.m_numCols; i++ )
		BIN_PUT_ULONG( st.m_colArray[ i ]->m_key );
	

	PT_ASSERT( next_loc < PT_MAX_PATH );
	
	return BinaryWrite( m_buf, next_loc );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleClearStruct
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleClearStruct( PT_Clear_Struct &st )
{

	// Format:
	// RecID Clear StructName Row Time ColNames
	// (if No ColNames provided, then clear entire row)
	// (if ROW = MAXROWS, then clear entire table)

	if (st.m_colList == NULL)
	{
		if (st.m_namespace == 0)
			return	FastStringWrite(	ultoascii(st.m_ID, m_buf), " ", 
										PIPE_CLEAR_STRUCT, " ",
										st.m_struct->m_name, " ",
										ultoascii(st.m_instanceNum, m_buf4), " ",
										ultoascii(st.m_row, m_buf2), " ",
										ultoascii(st.m_time, m_buf3), "\n" );
		else
			return	FastStringWrite(	ultoascii(st.m_namespace, m_buf5), ":",
										ultoascii(st.m_ID, m_buf), " ", 
										PIPE_CLEAR_STRUCT, " ",
										st.m_struct->m_name, " ",
										ultoascii(st.m_instanceNum, m_buf4), " ",
										ultoascii(st.m_row, m_buf2), " ",
										ultoascii(st.m_time, m_buf3), "\n" );
	}
	else
	{
		char ColString[ PT_MAX_PATH ];
		ColString[ 0 ] = '\0';
		char *pCol = ColString;
		for ( PT_UINT i = 0; i < st.m_numCols; i++ )
		{
			pCol = FastStringCat( pCol, st.m_colList[ i ]->m_name );
			pCol = FastStringCat( pCol, " " );
		}

		if (st.m_namespace == 0)
			return	FastStringWrite(	ultoascii(st.m_ID, m_buf), " ", 
										PIPE_CLEAR_STRUCT, " ",
										st.m_struct->m_name, " ",
										ultoascii(st.m_instanceNum, m_buf4), " ",
										ultoascii(st.m_row, m_buf2), " ",
										ultoascii(st.m_time, m_buf3), " ",
										ColString, "\n" );
		else
			return	FastStringWrite(	ultoascii(st.m_namespace, m_buf5), ":",
										ultoascii(st.m_ID, m_buf), " ", 
										PIPE_CLEAR_STRUCT, " ",
										st.m_struct->m_name, " ",
										ultoascii(st.m_instanceNum, m_buf4), " ",
										ultoascii(st.m_row, m_buf2), " ",
										ultoascii(st.m_time, m_buf3), " ",
										ColString, "\n" );
			
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleClearStruct
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleClearStruct( PT_Clear_Struct &st )
{
	// Format:
	// 0 ClearStruct [0 ID] RecID StructKey instanceNum row time {colx1 colx2} 0
	// (colX must be a predefined Pipe_Config_Data)
	// (if No ColNames provided, then clear entire row)
	// (if ROW = MAXROWS, then clear entire table)

	//const char *buf;
	int next_loc = put_leader( 0, PIPE_CLEAR_STRUCT_KEY );
	
	BIN_PUT_ULONG( st.m_ID );
	BIN_PUT_ULONG( st.m_struct->m_key );
	BIN_PUT_ULONG( st.m_instanceNum );
	BIN_PUT_ULONG( st.m_row );
	BIN_PUT_ULONG( st.m_time );
	
	if (st.m_colList != NULL )
	{
		for( unsigned int i = 0; i < st.m_numCols; i++ )
			BIN_PUT_ULONG( st.m_colList[ i ]->m_key );
	}
	
	BIN_PUT_ULONG( 0 );


	PT_ASSERT( next_loc < PT_MAX_PATH );
	
	return BinaryWrite( m_buf, next_loc );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleUpdateStruct
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleUpdateStruct( PT_Update_Struct &st )
{

	// Format:
	// RecID Update StructName Instance Row Time NumValues (must be followed by NumValues rows of EventData)
	if (st.m_namespace == 0)
		return	FastStringWrite(	ultoascii(st.m_ID, m_buf3), " ", 
									PIPE_UPDATE_STRUCT, " ",
									st.m_struct->m_name, " ",
									ultoascii(st.m_instanceNum, m_buf5), " ",
									ultoascii(st.m_row, m_buf2), " ",
									ultoascii(st.m_time, m_buf), " ",
									ultoascii(st.m_num_values, m_buf4), "\n" );
	else
		return	FastStringWrite(	ultoascii(st.m_namespace, m_buf6), ":",
									ultoascii(st.m_ID, m_buf3), " ", 
									PIPE_UPDATE_STRUCT, " ",
									st.m_struct->m_name, " ",
									ultoascii(st.m_instanceNum, m_buf5), " ",
									ultoascii(st.m_row, m_buf2), " ",
									ultoascii(st.m_time, m_buf), " ",
									ultoascii(st.m_num_values, m_buf4), "\n" );

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleUpdateStruct
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleUpdateStruct( PT_Update_Struct &st )
{

	// Format:
	// 0 UpdateStruct [0 ID] numvalues RecID StructKey instanceNum row time 

	int next_loc = put_leader( 0, PIPE_UPDATE_STRUCT_KEY );
	
		BIN_PUT_UCHAR( st.m_num_values );
		BIN_PUT_ULONG( st.m_ID );
		BIN_PUT_ULONG( st.m_struct->m_key );
		BIN_PUT_ULONG( st.m_instanceNum );
		BIN_PUT_ULONG( st.m_row );
		BIN_PUT_ULONG( st.m_time );

	PT_ASSERT( next_loc < PT_MAX_PATH );
	
	return BinaryWrite( m_buf, next_loc );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleEventStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleEventStatement( PT_Event_Statement &st )
{
	// Format:
	// xxxx EventName NumValues Duration Time
	//
	if (st.m_namespace == 0)
		return	FastStringWrite(	ultoascii( st.m_ID, m_buf ), " ", 
									st.m_EventDef->m_event_name, " ",
									ultoascii( st.m_num_values, m_buf2 ), " ",
									ultoascii( st.m_duration, m_buf3 ), " ",
									ultoascii( st.m_time, m_buf4 ), "\n" );
	else
		return	FastStringWrite(	ultoascii( st.m_namespace, m_buf5 ), ":",
									ultoascii( st.m_ID, m_buf ), " ", 
									st.m_EventDef->m_event_name, " ",
									ultoascii( st.m_num_values, m_buf2 ), " ",
									ultoascii( st.m_duration, m_buf3 ), " ",
									ultoascii( st.m_time, m_buf4 ), "\n" );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleEventStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleEventStatement( PT_Event_Statement &st )
{
	// Format:
	// key [0 ID] NumValues Duration Time
	//
	
	PT_Time delta;

	int next_loc = put_leader( st.m_ID, *(st.m_EventDef) );

	delta = st.m_time - m_lastTime;
	m_lastTime = st.m_time;

	BIN_PUT_UCHAR( st.m_num_values );
	BIN_PUT_ULONG( st.m_duration );
	BIN_PUT_ULONG( delta );

	return BinaryWrite( m_buf, next_loc );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::HandleDataStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::HandleDataStatement( PT_Data_Statement &st )
{
	// Format:
	// xxxx DataTypeName data
	//
	if (st.m_namespace == 0)
		return	FastStringWrite(	ultoascii( st.m_ID, m_buf ), " ", 
									st.m_DataDef->m_name, " ",
									st.DataToString( m_buf2 ), "\n" );			
	else
		return	FastStringWrite(	ultoascii( st.m_namespace, m_buf3 ), ":",
									ultoascii( st.m_ID, m_buf ), " ", 
									st.m_DataDef->m_name, " ",
									st.DataToString( m_buf2 ), "\n" );			

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::HandleDataStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::HandleDataStatement( PT_Data_Statement &st )
{
	// Format:
	// key [0 ID] data
	// key [0 ID] StringData 0
	//
	char *buf;
	int next_loc = put_leader( st.m_ID, *(st.m_DataDef));

	switch ( st.m_DataDef->m_datatype ) 
	{
	case Pipe_Boolean:
		BIN_PUT_UCHAR(* ((char *) st.m_pData)); break;
	case Pipe_Enumeration:
	case Pipe_Integer:
	case Pipe_Hex_Integer:
	case Pipe_Association:
		BIN_PUT_ULONG(* ((unsigned long *) st.m_pData) ); break;
	case Pipe_String:
		{
			buf = (char *) st.m_pData;
			while ( (m_buf[ next_loc++ ] = *buf++) );
			break;
		}
	case Pipe_ByteArray:
		{
			buf = (char *) st.m_pData;
			for ( unsigned int i = 0; i < st.m_DataDef->m_arrayLen; i++ )   
			{
				m_buf[ next_loc++ ] = *buf++;
			}
			break;
		}
	default:
		PT_FATAL("Unknown data type in HandleDataStatement()");
		break;
	}

	return BinaryWrite( m_buf, next_loc );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIWriteStream::FastStringWrite( )
// Description: build a string on the fly from a varg list and then write 
// to disk
// The last parameter must be "\n"
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIWriteStream::FastStringWrite( char *first, ... )
{
	va_list	ap;
	char *value;

	//parse the args list
	va_start(ap, first);
	
	char szOut[ PT_MAX_PATH ];
	char *dest = szOut;

	dest = FastStringCat( dest, first );
	if ( first[0] != '\n' )
	{
		int numEntries = 0;
		do 
		{
			//do a fast strcat of the next parameter. stop when we reach "\n"
			value = va_arg(ap, char *);
			dest = FastStringCat( dest, value);		

			numEntries++;
		}while (( value[ 0 ] != '\n' ) && ( numEntries < 256 ) );
	}

	// now, write to file
	size_t OutLen = strlen( szOut );
	
#ifdef USE_ZLIB
	size_t written = gzputs( m_file, szOut );
#else
	size_t written = fwrite( szOut, sizeof( char ), OutLen, m_file );
#endif

	if ( written != OutLen )
	{
		PT_WARNING2( "Num chars to write (%d) and num chars written (%d) don't match!", written, OutLen );
		return PT_Error_WriteFailed;
	}
	
	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::PT_BinaryWriteStream
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_BinaryWriteStream::PT_BinaryWriteStream( const char *szFile, PT_Error *Error, const char *szNamespace ) : PT_WriteStream( )
{
	m_lastTime = 0;
	m_lastRecord = ( PT_ULONG )(-1);

	m_szNamespace = szNamespace;

	if ( Error )
	{
		*Error = Open( szFile );
	}
	else 
	{
		Open( szFile );
	}
}



////////////////////////////////////////////////////////////////////////////////
//
// Util functions for Binary writers... this code is mostly taken from
// PSLib since if I tried mucking with it, I would most likely introduce errors,
// and the binary format is pretty complex
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// put_word()
//
// FUNCTION:
//  Put a word into the stream in few bytes.
//  Determines how many bytes it can pack this into, and
//  the stuffs it in.  Uses the high 2 bits of the first
//  byte to code how many bytes it needs.  Writes the
//  first word as all 1's in cases where it will need
//  the top two bits to be 1's, and where you
//  wouldn't be able to discriminate the top two bits
//  anyhow.
//  Always puts it into m_buf at next_loc.
//
// RETURNS:
//  where the next byte in the buffer will go.
//
////////////////////////////////////////////////////////////////////////////////
int PT_BinaryWriteStream::put_word( unsigned long value, int next_loc)
{
	if (value <= 0x0000003f) {
		PT_ASSERT((value & 0xc0) == 0);
		BIN_PUT_UCHAR(value);
	} else if (value <= 0x00003fff) {
		PT_ASSERT(((value >> 8) & 0xc0) == 0);
		BIN_PUT_UCHAR(((value >> 8) | 0x40));
		BIN_PUT_UCHAR(value);
	} else if (value <= 0x003fffff) {
		PT_ASSERT(((value >> 16) & 0xc0) == 0);
		BIN_PUT_UCHAR(((value >> 16) | 0x80));
		BIN_PUT_UCHAR((value >> 8));
		BIN_PUT_UCHAR(value);
	} else if (value <= 0x3fffffff && ((value & 0x3f000000) != 0x3f000000)) {
		PT_ASSERT(((value >> 24) & 0x80) == 0);
		BIN_PUT_UCHAR(((value >> 24) | 0xc0));
		BIN_PUT_UCHAR((value >> 16));
		BIN_PUT_UCHAR((value >> 8));
		BIN_PUT_UCHAR(value);
	} else {
		BIN_PUT_UCHAR(0xff);
		BIN_PUT_UCHAR((value >> 24));
		BIN_PUT_UCHAR((value >> 16));
		BIN_PUT_UCHAR((value >> 8));
		BIN_PUT_UCHAR(value);
	}
	return(next_loc);
} 

////////////////////////////////////////////////////////////////////////////////
//
// put_leader()
//
// FUNCTION:
//  Put a binary leader into the record.
//  The binary leader is a funky thing.
//  The bits look like this:
//  Xkkkkkkk [kkkkkkkk[kkkkkkkk[kkkkkkkk]]] [rrrrrrrr[rrrrrrrr[rrrrrrrr[rrrrrrrr]]]]
//  If the first bit, X, is set, there is a record ID field, r.
//  If the first bit, X, is clear, there is no record ID field.
//  k are the key bits.   This is from 1-4 bytes long (minus the high bit),
//  depending on the current key size.
//
////////////////////////////////////////////////////////////////////////////////
int PT_BinaryWriteStream::put_leader( PT_ID ID, PT_KEY key )
{
	int	next_loc;
	PT_ASSERT( key < 1 << ( m_keySize * 8 - 1) );

	next_loc = 0;

	if ( m_lastRecord != ID ) 
	{
		//
		// Set the "High" bit, signalling that we have to provide a record ID
		//
		key |= 1 << ( m_keySize * 8 - 1 );
	}

	//
	// Emit the modified key.
	//
	switch( m_keySize ) 
	{
		case	4:	BIN_PUT_UCHAR((key >> 24));
		case	3:	BIN_PUT_UCHAR((key >> 16));
		case	2:	BIN_PUT_UCHAR((key >> 8));
		case	1:	BIN_PUT_UCHAR(key);
	}
	
	// Emit the record ID, if there is one.
	if ( m_lastRecord != ID ) 
	{
		m_lastRecord = ID;
		BIN_PUT_ULONG( ID );
	} 
	return(next_loc);
} 


int PT_BinaryWriteStream::put_leader( PT_ID ID, PT_Config &st )
{
	return put_leader( ID, st.m_key );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryWriteStream::BinaryWrite
// Description: wraps around fwrite and returns a PT_Error
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryWriteStream::BinaryWrite( char *buffer, unsigned int length )
{
#ifdef USE_ZLIB
	size_t ret = gzwrite( m_file, buffer, length );
#else
	size_t ret = fwrite( buffer, sizeof( char ), length, m_file );
#endif 
	if ( ret != length ) 
	{
		return PT_Error_WriteFailed;
	}
	return PT_Error_OK;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function Name: get_chars( )
// Description: 
// Get characters out of the buffer and delineate them with
// a space.
// Not too fast, but only used at initialization type. 
// (Code taken straight from PSLib)
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
static
char *
get_chars( char *buffer, char	*line, int expr )
{
	//char	buffer[ 256 ];	// can't use static vars anymore - think multiple threads
	int	length;

	length = rgx_index_end( 1 ) - rgx_index_start( 1 );
	strncpy( buffer, &line[ rgx_index_start( 1 ) ], length );
	buffer[ length ] = '\0';
	return( buffer );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ReadStream::PT_ReadStream
// Description: Derived class' constructors should invoke this one for setup
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_ReadStream::PT_ReadStream( PT_DataStore *pDataStore, PT_Error *Error )
{
	PT_INIT_LOGGING( "stderr" );

	m_pDataStore = pDataStore;

	m_file = NULL;
	m_szFileName[ 0] = '\0';
	m_hashSize = 0;
	m_nextKey = 0;
	m_firstKey = 0;
	m_szTimeStamp[ 0 ] = '\0';
	m_szNamespace[ 0 ] = '\0';
	m_bIsNamespaced = false;

	m_nextRecord = 1;
	m_EventDataCount = 0;

	m_lastTime = 0;
	m_lastRecordID = 0;
	m_keySize = 1;

#ifdef USE_ZLIB
	m_pBufferedReader = NULL;
#endif

	SetupDefaultStatements( );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ReadStream::~PT_ReadStream
// Description: Default Constructor. Should only be called by class factory
//				Does not open file
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_ReadStream::~PT_ReadStream()	
{ 
	PT_UNINIT_LOGGING; 

	if (m_file)
		Close( );

	// this one is important, otherwise, destructor will crash
	DataStatement.m_pData = NULL;
	EventDataStatement.m_pData = NULL;
#ifdef USE_ZLIB
	if ( m_pBufferedReader != NULL ) 
	{
		delete m_pBufferedReader;
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIReadStream::PT_ASCIIReadStream
// Description: Default Constructor. Should only be called by class factory
//				Does not open file
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_ASCIIReadStream::PT_ASCIIReadStream(  ) : PT_ReadStream( NULL, NULL )
{

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryReadStream::PT_ReadStream
// Description: Default Constructor. Should only be called by class factory
//				Does not open file
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_BinaryReadStream::PT_BinaryReadStream(  ) : PT_ReadStream( NULL, NULL )
{

}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ReadStream::ParseHeaderInfo
// Description: Use regexp to parse header and make sure we are dealing with 
// a real pipetrace file
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
#ifdef USE_ZLIB
PT_Error
PT_ReadStream::ParseHeaderInfo( ZFILE file, char *format, PT_KEY *hashSize, 
								PT_KEY *hashKey, char *timestamp, char *szNamespace )
#else
PT_Error
PT_ReadStream::ParseHeaderInfo( FILE *file, char *format, PT_KEY *hashSize, 
								PT_KEY *hashKey, char *timestamp, char *szNamespace )
#endif
{
	char buffer[ PT_MAX_PATH ];
	char buffer2[ PT_MAX_PATH ];
	PT_BOOL bHashSize = PT_FALSE;
	PT_BOOL bHashKey = PT_FALSE;
	PT_BOOL bFormat = PT_FALSE;
	PT_BOOL bTimeStamp = PT_FALSE;
	
	// read from file until we get to the "BEGIN" statement
	//while ( fgets( buffer, PT_MAX_PATH, file ) && strcmp( buffer, "BEGIN\n" ) ) 
	
	// make sure we aren't searching forever
	int count = 0;

#ifdef USE_ZLIB
	while ( gzgets( file, buffer, PT_MAX_PATH ) && (strcmp( buffer, "BEGIN\n" ) && strcmp( buffer, "BEGIN\r\n" )) 
			&& (count < 1024 )) 
#else
	while ( fgets( buffer, PT_MAX_PATH, file ) && (strcmp( buffer, "BEGIN\n" ) && strcmp( buffer, "BEGIN\r\n" )) 
			&& (count < 1024 )) 
#endif
	{
		if ( rgx_simple_match( "\\[Hash: ([0-9]+)\\]", buffer ) ) 
		{
			*hashSize = atol( get_chars( buffer2, buffer, 1 ) );
			bHashSize = PT_TRUE;
		}
		if (rgx_simple_match( "\\[Key: ([0-9]+)\\]", buffer ) ) 
		{
			*hashKey = atol( get_chars( buffer2, buffer, 1 ) );
			bHashKey = PT_TRUE;
		}
		if (rgx_simple_match( "\\[Namespace: (.*)\\]", buffer ) )
		{
			strcpy( szNamespace, get_chars( buffer2, buffer, 1 ) );
PT_WARNING1("Namespace is %s", szNamespace);
		}
		if ( rgx_simple_match( "\\[Format: (ASCII|Binary)\\]", buffer ) ) 
		{
			if ( buffer[ rgx_index_start( 1 ) ] == 'A' ) 
			{
				// must be ascii
				strcpy( format, ASCII_FORMAT );
			} else 
			{
				// assume its a binary file
				strcpy( format, BINARY_FORMAT );
			}

			bFormat = PT_TRUE;
		}
		if ( rgx_simple_match( "\\[Timestamp: (.*)\\]", buffer ) ) 
		{
			strcpy( timestamp, get_chars( buffer2, buffer, 1 ) );
			bTimeStamp = PT_TRUE;
		}

		count++;
	}

	if ( ! ( bHashSize && bHashKey && bFormat /*&& bTimeStamp*/ ) )
	{
		return PT_Error_IncompleteHeader;
	}

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ReadStream::GetTimeStamp
// Description: return time stamp string from file header 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
const char *
PT_ReadStream::GetTimeStamp( )
{
	return m_szTimeStamp;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ReadStream::Open
// Description: Opens a pipetrace for reading. 
//				Will only succeed if the file format = ReadStream->Format()
//				Instead of calling this, use the class factory
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_ReadStream::Open( char *szFileName )
{

	char buffer[ PT_MAX_PATH ];

	// make sure we haven't done this already
	if ( m_file != NULL ) 
	{
		PT_WARNING1( "Cannot open pipetrace %s for input. File already opened", szFileName );
		return PT_Error_FileOpenFailed;
	}

	// if filename is null, use "stdin"
//	if ( !szFileName ) 
//	{
//		strcpy( szFileName,"stdout" );
//	}

#ifdef USE_ZLIB
	if ( szFileName == NULL || strcmp( szFileName, "stdin") == 0 )
	{
		// use standard input
		m_file = gzdopen( 0, "rb");
	}
	else
	{
		m_file = gzopen( szFileName, "rb");
	}
#else
	m_file = xfopen( szFileName, "rb");
#endif

	if (!m_file) 
	{
		PT_WARNING( "Couldn't open pipetrace for reading" );
		if ( szFileName ) PT_WARNING( szFileName );
		return PT_Error_FileOpenFailed;
	}
	
	// OK, open succeeded, set up values
	if ( szFileName ) strcpy( m_szFileName, szFileName );

	// Now, parse the header. Start by reading the first line
	// which should contain version info
	buffer[ 0 ] = '\0';

#ifdef USE_ZLIB
	gzgets( m_file, buffer, PT_MAX_PATH );
#else
	fgets(buffer, PT_MAX_PATH, m_file);
#endif

	// make us DOS file fmt compatible
	size_t verLen = strlen(buffer);
	buffer[ verLen - 1 ] = '\0';

	// extra carriage return char
	if ( (verLen > 1) && (buffer[ verLen - 2 ] == '\r') )
	{
		buffer[ verLen - 2 ] = '\0';
	}
	
	strcpy( m_versionStamp, buffer );
	
	buffer[strlen(buffer) - 1] = '\0';

	//
	// Make sure its an OK version...
	//
	if ( strcmp( m_versionStamp, PIPE_VERSION_STRING_3_0) != 0 &&
		 strcmp( m_versionStamp, PIPE_VERSION_STRING_3_1) != 0 &&
		 strcmp( m_versionStamp, PIPE_VERSION_STRING_3_2) != 0 &&
		 strcmp( m_versionStamp, PIPE_VERSION_STRING_3_3) != 0)
	{
		PT_FATAL( "Unable to read pipetrace. Unknown version" );
	}
	
	m_bIsVersion3_2 = false;
	m_bIsVersion3_3 = false;
	if ( strcmp( m_versionStamp, PIPE_VERSION_STRING_3_3 ) == 0 )
		m_bIsVersion3_3 = true;
	else if ( strcmp( m_versionStamp, PIPE_VERSION_STRING_3_2 ) == 0 )
		m_bIsVersion3_2 = true;

	// get format string
	char format[256];

	// parse the rest of the header until we reach the "Begin Statement"
	// or hit an error
//	while ( fgets(buffer, PT_MAX_PATH, file) && strcmp(buffer, "BEGIN\n") ) 
//	{
//		
//	}

	PT_Error Error = ParseHeaderInfo( m_file, format, &m_hashSize, &m_nextKey, m_szTimeStamp, m_szNamespace );
	{
		if (PT_FAILED ( Error ) )
		{
			Close();
			return Error;
		}
	}

	if ( m_szNamespace[ 0 ] != '\0' )
	{
		m_bIsNamespaced = true;
	}

	// now do sanity checking on the header
	if ( strcmp( format, GetFormatString() ) != 0 )
	{
		PT_WARNING2( "File format %s is not the same as this class' format (%s). Exiting! ", format, GetFormatString());
		Close();
		return PT_Error_FileOpenFailed;
	}

	m_firstKey = m_nextKey;

	if ( m_nextKey == 0 || m_nextKey >= m_hashSize )
	{
		PT_WARNING( "Hash info in this file is invalid. Exiting! ");
		Close();
		return PT_Error_FileOpenFailed;
	}
	
	// all OK, set us up for fast reading
#ifdef USE_ZLIB
	m_pBufferedReader = new PT_BufferedReader( m_file, &Error, 1 << 15 );
	if (PT_FAILED(Error))
		return Error;
#endif
	m_nextRecord = 1;
	SetupDefaultStatements( );

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ReadStream::Close
// Description: close the readstream, regardless of its format
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ReadStream::Close()
{
	//try to close the file
	if ( !m_file )
	{
		PT_WARNING("Cannot close pipetrace.");
		return PT_Error_FileCloseFailed;
	}

#ifdef USE__ZLIB
	if ( m_pBufferedReader != NULL )
	{
		delete m_pBufferedReader;
		m_pBufferedReader = NULL;
	}
#endif

#ifdef USE_ZLIB
	gzclose( m_file );
#else
	fclose( m_file );
#endif

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ReadStream::SetupDefaultStatements
// Description: 
// setup the internal statements for temporary storage. default constructor
// set values to nice defaults, now we set up the pointers
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ReadStream::SetupDefaultStatements()
{
	// Default string buffers to '\0'

	// Setup each statement. for string data, point it to one of the string 
	// buffers
	CommentStatement.m_szComment = m_szStringBuffers[ 0 ];
	DataStatement.m_pData = (void *)m_szStringBuffers[ 0 ];
	EventDataStatement.m_pData = (void *)m_szStringBuffers[ 0 ];
	
	ConfigData.m_name = m_szStringBuffers[ 0 ];
	ConfigData.m_description = m_szStringBuffers[ 1 ];
	ConfigData.m_enumArray = m_extraBuffer;
	
	// line up the extrabuffer entries with szStringBuffers so that
	// enum values are ready to go.
	for (int i = 0; i < 256; i++ )
	{
		m_extraBuffer[ i ] = m_szEnumChars[ i ];
	}

	ConfigRecord.m_name = m_szStringBuffers[ 0 ];
	ConfigRecord.m_description = m_szStringBuffers[ 1 ];
	
	ConfigEvent.m_event_name = m_szStringBuffers[ 0 ];
	ConfigEvent.m_description = m_szStringBuffers[ 1 ];

	ConfigStruct.m_name = m_szStringBuffers[ 0 ];
	ConfigStruct.m_description = m_szStringBuffers[ 1 ];
	ConfigStruct.m_colArray = m_StructColArray;

	ClearStruct.m_colList = m_StructColArray;

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ReadStream::CreateReader
// Description: 
// Static Class Factory
// This is the best way to create a reader, especially if you don't care
// what format or version the file is in, all you care about is data.
// The class factory will return the proper PT_ReadStream derived instance.
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ReadStream::CreateReader( char *szFileName, PT_DataStore *store, PT_ReadStream **ppReadStream )
{
	*ppReadStream = NULL;
	char buffer[ PT_MAX_PATH ];
	char versionStamp[ PT_MAX_PATH ];

#ifdef USE_ZLIB
	ZFILE file;
	if ( szFileName == NULL || strcmp( szFileName, "stdin") == 0 )
	{
		// use standard input
		file = gzdopen( 0, "rb6");
	}
	else
	{
		file = gzopen( szFileName, "rb6");
	}
#else
	FILE *file;
	file = xfopen( szFileName, "rb" );
#endif

	if (!file) 
	{
		PT_WARNING( "Couldn't open pipetrace for reading" );
		if (szFileName) PT_WARNING( szFileName );
		return PT_Error_FileOpenFailed;
	}	

	// Now, parse the header. Start by reading the first line
	// which should contain version info
	buffer[ 0 ] = '\0';

#ifdef USE_ZLIB
	gzgets( file, buffer, PT_MAX_PATH );
#else
	fgets(buffer, PT_MAX_PATH, file);
#endif

	// make us DOS file fmt compatible
	size_t verLen = strlen(buffer);
	buffer[ verLen - 1 ] = '\0';

	// extra carriage return char
	if ( ( verLen > 1 ) && (buffer[ verLen - 2 ] == '\r') )
	{
		buffer[ verLen - 2 ] = '\0';
	}
	
	strcpy( versionStamp, buffer );
	
	//buffer[strlen(buffer) - 1] = '\0';

	//
	// Make sure its an OK version...
	//
	if ( strcmp(versionStamp, PIPE_VERSION_STRING_3_0) != 0 &&
		 strcmp(versionStamp, PIPE_VERSION_STRING_3_1) != 0 &&
		 strcmp(versionStamp, PIPE_VERSION_STRING_3_2) != 0 &&
		 strcmp(versionStamp, PIPE_VERSION_STRING_3_3) != 0)
	{
//		PT_FATAL( "Unable to read pipetrace. Unknown version" );
#ifdef USE_ZLIB
	gzclose( file );
#else
	fclose(file);
#endif
	return PT_Error_Fail;

	}

	// returned values from ParseHeaderInfo
	char format[256];
	char timestamp[256];
	char szNamespace[256];
	szNamespace[0] = '\0';
	PT_KEY hashSize;
	PT_KEY hashKey;

	// parse the rest of the header until we reach the "Begin Statement"
	// or hit an error
	PT_Error Error = ParseHeaderInfo( file, format, &hashSize, &hashKey, timestamp, szNamespace );
	if ( PT_FAILED( Error ) )
	{
#ifdef USE_ZLIB
		// cleanup first
		gzclose( file );
#else
		fclose( file );
#endif

		// invalid header setup
		return Error;
	}

	if ( hashKey == 0 || hashKey >= hashSize )
	{
		PT_WARNING( "Hash info in this file is invalid. Exiting! ");
#ifdef USE_ZLIB
		gzclose( file );
#else
		fclose( file );
#endif
		return PT_Error_FileOpenFailed;
	}


	// now do sanity checking on the header
	if ( strcmp( format, ASCII_FORMAT ) == 0 )
	{
		*ppReadStream = new PT_ASCIIReadStream;
	}
	else
	{
		*ppReadStream = new PT_BinaryReadStream;
	}

	// OK, create succeeded, set up values inside the class
	// since we can't close this file and then invoke the proper constructor...
	// (if input is a pipe we can't go back)
	PT_ReadStream *read = *ppReadStream;

	if ( szFileName ) strcpy( read->m_szFileName, szFileName );
	read->m_file = file;
	read->m_hashSize = hashSize;
	read->m_nextKey = read->m_firstKey = hashKey;
	strcpy( read->m_versionStamp, versionStamp );
	strcpy( read->m_szTimeStamp, timestamp );

	read->m_nextRecord = 1;
	read->m_EventDataCount = 0;
	read->m_pDataStore = store;
	//read->m_versionStamp = versionStamp;


	if (szNamespace[0] != '\0')
	{
		strcpy( read->m_szNamespace, szNamespace );
		read->m_bIsNamespaced = true;
	}

	read->m_bIsVersion3_3 = false;
	read->m_bIsVersion3_2 = false;
	if ( strcmp( versionStamp, PIPE_VERSION_STRING_3_3 ) == 0 )
		read->m_bIsVersion3_3 = true;
	else if ( strcmp( versionStamp, PIPE_VERSION_STRING_3_2 ) == 0 )
		read->m_bIsVersion3_2 = true;

	// all OK, set us up for fast reading
#ifdef USE_ZLIB
	read->m_pBufferedReader = new PT_BufferedReader( read->m_file, &Error, 1 << 15 );
	if (PT_FAILED(Error))
		return Error;
#endif
	read->SetupDefaultStatements( );

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
// Taken from PSLib. Not converted to avoid bugs
//
// Macros for reading charcters quickly.
// All rely on there being a variable called nxt which
// points too the next character to read.
// Not pretty, but it works.  So there.
////////////////////////////////////////////////////////////////////////////////
#define PIPE_READ_SKIP_WS {while (isspace(*nxt)) nxt++;}
#define PIPE_READ_GET_NUM(x) { \
    PIPE_READ_SKIP_WS \
    tmp = nxt; \
    if (*nxt == '-')  nxt++; \
    while (isdigit(*nxt)) nxt++; \
    *nxt++ = '\0'; \
    x = myatoi(tmp); }
#define PIPE_READ_GET_HEX(x) { \
    PIPE_READ_SKIP_WS \
    tmp = nxt; \
    while ( isxdigit( *nxt ) ) nxt++; \
    *nxt++ = '\0'; \
    x = myxtoi( tmp ); }
#define PIPE_READ_MARK_NAME(name) {\
    PIPE_READ_SKIP_WS \
    name = nxt; \
    while ( !isspace( *nxt ) && *nxt ) nxt++; \
    *nxt++ = '\0'; }

////////////////////////////////////////////////////////////////////////////////
// Taken from PSLib. Not converted to avoid bugs
//
// Fast conversion routines for converting ascii strings
// to interers.
//////////////////////////////////////////////////////////////////////////////// 
static
unsigned long
myxtoi( char *x )
{
	unsigned long ret_val = 0;
	while ( *x ) 
	{
		ret_val = ret_val << 4;
		if (*x >= '0' && *x <= '9')
			ret_val += (unsigned long) (*x - '0');
		else if (*x >= 'a' && *x <= 'f')
			ret_val += (unsigned long) (*x - 'a') + 10;
		else if (*x >= 'A' && *x <= 'F')
			ret_val += (unsigned long) (*x - 'A') + 10;
		x++;
	}
	return (ret_val);
} 

static
unsigned long
myatoi(char	*x)
{
	unsigned long	ret_val = 0;
	PT_BOOL	neg = PT_FALSE;

	if (*x == '-') 
	{
		neg = PT_TRUE;
		x++;
	}
	while (*x) 
	{
		ret_val = (ret_val * 10) + (*x++ - '0');
	}
	if ( neg ) 
	{
		ret_val = -ret_val;
	}
	return ( ret_val );
} 

PT_ULONG PT_ReadStream::GetBytesRead( )
{ 
#ifdef USE_ZLIB
	if (m_pBufferedReader) 
		return m_pBufferedReader->GetBytesRead( ); 
	else return 0;
#else
        return 0;
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ReadStream::NextKey
// Description: returns the next key for a config definition, and increases
//				the key size if necessary
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_KEY PT_ReadStream::NextKey( )
{
	//m_nextKey++;

	// Increase the key size if we're at 7, 15, or 22 bits long.
    // This allows us to ALWAYS keep the high bit clear, which is important for 
    // binary format
    if ( m_nextKey == ( ( 1 << ( m_keySize * 8 - 1 ) ) - 1 ) ) 
	{
		m_keySize++;
    }

	return m_nextKey++;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ASCIIReadStream::ReadStatement
// Description: 
// Meant to be used in a while loop... returns PT_Error_EOF when end of
// file reached
// If insertInfo is false, then the record isn't inserted into the 
// datastore
// Most of this code is taken from PSLib, since a rewrite would be dangerous
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ASCIIReadStream::ReadStatement( PT_Statement **statement, PT_BOOL bInsertInfo )
{
	PT_ID currentID = (PT_ID)-1;	// record ID associated w/ current statement
	PT_ID currentNamespace = 0;
	PT_Config *st;
	char *name;				// read in type name

	// it's ok for the user to pass in null, if they don't care what the statement is
	// (they want datastore access instead). so hook us up to a dummy statement
	PT_Statement *tempStatement;
	if ( statement == NULL )
	{
		statement = &tempStatement;
	}

	// Get the next ASCII line
	// char *nxt = fgets( m_szLine, PT_MAX_PATH, m_file );
/*
#ifdef USE_ZLIB
	char *nxt = gzgets( m_file, m_szLine, PT_MAX_PATH );
#else
	char *nxt = fgets( m_szLine, PT_MAX_PATH, m_file );
#endif
*/
#ifdef USE_ZLIB
	char *nxt = m_pBufferedReader->PT_gets( m_szLine, PT_MAX_PATH );
#else
        char *nxt = fgets( m_szLine, PT_MAX_PATH, m_file );
#endif

	// used in macro operations
	char *tmp = m_szBuf;
	
	PT_Error Error = PT_Error_OK;

	// see if we are at the end of the file
	if ( !nxt ) 
	{ 
		if ( m_pDataStore )
		{
			m_pDataStore->OnEndOfTrace( );
		}
		return( PT_Error_EOF );
	}

	// to read statements, we will need a DataStore to hash definitions
	if ( m_pDataStore == NULL )
	{
		return PT_Error_Fail;
	}
	
	// chop off the \n
	size_t lineLen = strlen(m_szLine);

	m_szLine[lineLen - 1] = '\0'; 
	if (lineLen - 1 == 0) 
	{
		//blank line
		*statement = NULL; 
		return PT_Error_OK;
	}

	// and if a DOS file fmt, get rid of the \r too
	if (( lineLen > 1) && (m_szLine[ lineLen - 2 ] == '\r' ))
	{
		m_szLine[ lineLen - 2 ] = '\0';
		if (lineLen - 2 == 0) 
		{
			//blank line
			*statement = NULL; 
			return PT_Error_OK;
		}
	}

	// Read the record ID number.
	if ( !m_bIsNamespaced )
	{
		PIPE_READ_GET_NUM( currentID );
	}
	else
	{
		PIPE_READ_SKIP_WS
		tmp = nxt;
		if (*nxt == '-')  nxt++;
		while (isdigit(*nxt)) nxt++;
		
		if (*nxt == ':')
		{
			*nxt = '\0';
			currentNamespace = myatoi(tmp);

			// now, get ID
		    nxt++;
			tmp = nxt;
			if (*nxt == '-')  nxt++;
			while (isdigit(*nxt)) nxt++;
			*nxt++ = '\0'; 
			currentID = myatoi(tmp);
		}
		else
		{
			//recID is in current namespace
			tmp = nxt;
			if (*nxt == '-')  nxt++;
			while (isdigit(*nxt)) nxt++;
			*nxt++ = '\0'; 
			currentID = myatoi(tmp);
		}
	}

	// Read the type descriptor
	PIPE_READ_MARK_NAME( name );

	// Find what type of statement key this is.
	Statement_Type eType = Pipe_Invalid_Type;
	
	// First, see if the parsed name matched on of the reserved tokens
	// ToDo, do a hash on these, or some speedup mechanism?
	// Probably no need to fix, ASCII is supposed to be slow, use a 
	// switch statement for binary
	
	bool foundMatch = false;

	// hack - avoid doing 9 strcmps MOST of the time
	if ( name[0] == 'C' || name[0] == 'O' || name[0] == 'U' )
	{
		if ( strcmp( name, PIPE_UPDATE_STRUCT ) == 0 )
		{
			eType = Pipe_Update_Struct;
			foundMatch = true;
		}
		else if ( strcmp( name, PIPE_CLEAR_STRUCT ) == 0 )
		{
			eType = Pipe_Clear_Struct;
			foundMatch = true;
		}
		else if ( strcmp( name, PIPE_COMMENT ) == 0 )
		{
			eType = Pipe_Comment;
			foundMatch = true;
		}
		else if ( strcmp( name, PIPE_CONFIG_RECORD ) == 0 )
		{
			eType = Pipe_Config_Record;
			foundMatch = true;
		}
		else if ( strcmp( name, PIPE_CONFIG_EVENT ) == 0 )
		{
			eType = Pipe_Config_Event;
			foundMatch = true;
		}
		else if ( strcmp( name, PIPE_CONFIG_DATA ) == 0 )
		{
			eType = Pipe_Config_Data;
			foundMatch = true;
		}
		else if ( strcmp( name, PIPE_OPEN ) == 0 )
		{
			eType = Pipe_Open;
			foundMatch = true;
		}
		else if ( strcmp( name, PIPE_CLOSE ) == 0 )
		{
			eType = Pipe_Close;
			foundMatch = true;
		}
		else if ( strcmp( name, PIPE_CONFIG_STRUCT ) == 0 )
		{
			eType = Pipe_Config_Struct;
			foundMatch = true;
		}
	}
	
	if (!foundMatch)
	{
		Error = m_pDataStore->FindConfigByName( name, &st );
		
		if ( PT_FAILED( Error ) ) 
		{
			PT_WARNING1( "Definition Lookup for %s failed!", name );
			return Error;
		}


		Statement_Type type = st->GetType( );

		if ( type == Pipe_Config_Event )
		{
			eType = Pipe_Event;
		}
		else if ( type == Pipe_Config_Data )
		{
			eType = Pipe_Data;
		}
		else if ( type == Pipe_Config_Record )
		{
			// must be a new record since it has a config record
			eType = Pipe_Open;
		}
	}
	
	//
	// Make sure the ID was valid
	if ( currentID >= m_nextRecord && eType != Pipe_Open ) 
	{
		PT_WARNING1( "Attempting to add information to recID %d which has not yet been opened. Exiting\n", currentID );
		return PT_Error_InvalidReadStatement;
	}

	// if we are expecting event data, it must appear now.
	if ( m_EventDataCount != 0 && eType != Pipe_Data )
	{
		PT_WARNING1( "Invalid Statement -> event data must be here! recID = %d ", currentID );
		return PT_Error_InvalidReadStatement;
	}
	
		
	// now parse based on type
	switch( eType ) 
	{
		case Pipe_Open:
			{
				//
				// Error check the record ID.  It MUST be the next one in order.
				if ( currentID < m_nextRecord) 
				{
					PT_WARNING1( "Attempting to reopen a previously opened record (%d)", currentID );
					return PT_Error_InvalidReadStatement;
				} 
				else if ( currentID != m_nextRecord ) 
				{
					PT_WARNING1( "Attempting to open records out of order. currentID = %d \n", currentID);
					return PT_Error_InvalidReadStatement;
				}

				// assign ID
				OpenStatement.m_ID = currentID;
				OpenStatement.m_recordType = (PT_Config_Record *) st;

				// if the caller wanted to see the last statement, return it
				*statement = &OpenStatement;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnOpenStatement( OpenStatement );
				}
				
				m_nextRecord++;

				break;
			}
		case Pipe_Close:
			{
				//
				// Error check the record ID.
				if ( currentID >= m_nextRecord && currentNamespace == 0) 
				{
					PT_WARNING1( "Attempting to close an unopened record (%d)", currentID );
					return PT_Error_InvalidReadStatement;
				} 

				CloseStatement.m_ID = currentID;
				CloseStatement.m_namespace = currentNamespace;

				// if the caller wanted to see the last statement, return it
				*statement = &CloseStatement;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnCloseStatement( CloseStatement );
				}
				
				break;
			}
		case Pipe_Comment:
			{
				// fill in the comment string
				strcpy( CommentStatement.m_szComment, nxt );
				
				// if the caller wanted to see the last statement, return it
				*statement = &CommentStatement;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnCommentStatement( CommentStatement );
				}
				break;
			}
		case Pipe_Event:
			{
				EventStatement.m_EventDef = ( PT_Config_Event * )st;
				PIPE_READ_GET_NUM( EventStatement.m_num_values );
				PIPE_READ_GET_NUM( EventStatement.m_duration );
				PIPE_READ_GET_NUM( EventStatement.m_time ); 

				m_EventDataCount = EventStatement.m_num_values;
				
				//
				// Error check the record ID.
				//if ( currentID >= m_nextRecord) 
				//{
				//	PT_WARNING( "Attempting to add an event to an unopened statement" );
				//	return PT_Error_InvalidReadStatement;
				//} 

				EventStatement.m_ID = currentID;
				EventStatement.m_namespace = currentNamespace;	
				//
				// Make sure this number is reasonable
				if ( m_EventDataCount > 1<<15 )
				{
					PT_WARNING1("Invalid Statement, too many data members listed for event %s.", EventStatement.m_EventDef->m_event_name );
					m_EventDataCount = 0;
					return PT_Error_InvalidReadStatement;
				}

				// if the caller wanted to see the last statement, return it
				*statement = &EventStatement;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnEventStatement( EventStatement );
				}

				Error = PT_Error_OK;
				break;
			}
		case Pipe_Data:
			{
				PT_Data_Statement *DS;

				// should we be working with a DataStatement or an 
				// EventData statement? it matters if we are going to 
				// try to write out what we read in...
				if ( m_EventDataCount != 0 )
				{
					m_EventDataCount--;
					
					// let's work with the dataevent
					DS = &EventDataStatement;
				}
				else
				{
					// if we ARE zero, it's still ok, it's record data
					DS = &DataStatement;
				}

				DS->m_DataDef = ( PT_Config_Data * )st; 
				DS->m_ID = currentID;
				DS->m_namespace = currentNamespace;

				//char *strLoc;			
				bool foundOne = false;

				// now get the data
				// just cast m_pData into whatever it really is
				switch ( DS->m_DataDef->m_datatype )
				{
					case Pipe_Integer:
						PIPE_READ_GET_NUM( * ((long *) (DS->m_pData) ) ); break;
					case Pipe_Association:
						PIPE_READ_GET_NUM( * ((PT_ULONG *) (DS->m_pData) ) ); break;
					case Pipe_String:
						PIPE_READ_SKIP_WS;
						strcpy( ((char *) (DS->m_pData) ), nxt); break;
					case Pipe_Hex_Integer:
						PIPE_READ_GET_HEX( *((PT_ULONG *) (DS->m_pData) ) ); break;
					case Pipe_Boolean:
						PIPE_READ_SKIP_WS;
						*((char *) (DS->m_pData) ) = (*nxt == 'T') ? PT_TRUE:PT_FALSE; break;
					case Pipe_Enumeration:
						{
							// add a null to our buffer so strcmp treats it as a string
//							FastStringCat( nxt, "\0" );

							for( unsigned char i = 0; i < DS->m_DataDef->m_arrayLen; i++ )
							{
								// once we find a match, just store the index of the enum, not the string
								if ( strcmp( nxt, DS->m_DataDef->m_enumArray[ i ] ) == 0 )
								{
									(* ((PT_ULONG *) (DS->m_pData) )) = i;
									foundOne = true;
									break;
								}
							}

							if (!foundOne) 
							{
								int here = 1;
							}

							break;
						}
					case Pipe_ByteArray:
						{
							PT_ULONG color;
							
							PT_ASSERT( DS->m_DataDef->m_arrayLen );

							for (unsigned int i = 0; i < DS->m_DataDef->m_arrayLen; i++) 
							{
								PIPE_READ_GET_HEX( color );		// Read into an unused variable
								((char *) ( DS->m_pData ))[ i ] = color;
							}
							break;
						}
					default:
						{
							PT_FATAL("Unknown type for Pipe_Data");
						}
				}

				// if the caller wanted to see the last statement, return it
				// also, a writer will care if it is just data or event data, so differentiate
				*statement = DS;

				// and send off to the DataStore if asked
				
				if ( bInsertInfo )
				{
					if ( DS == &EventDataStatement )
					{
						m_pDataStore->OnEventDataStatement( EventDataStatement );
					}
					else
					{
						m_pDataStore->OnDataStatement( DataStatement );
					}
				}
				break;
			}

		case Pipe_Update_Struct:
			{
				char *tmpStruct = m_szBuf2;

				// get the struct name
				PIPE_READ_MARK_NAME( tmpStruct );

				// and convert it to a configdata
				if (tmpStruct[0]!='\0')
				{
					PT_Config *outConfig;
					Error = m_pDataStore->FindConfigByName( tmpStruct, &outConfig );
					
					if (PT_SUCCEEDED(Error) && outConfig->GetType( ) == Pipe_Config_Struct )
					{
						UpdateStruct.m_struct = (PT_Config_Struct *)outConfig;
					}
					else
					{
						PT_WARNING1("Invalid struct name (%s) provided for update!", UpdateStruct.m_struct->m_name );
						return PT_Error_Fail;
					}
				}

				PIPE_READ_GET_NUM( UpdateStruct.m_instanceNum );

				if ( UpdateStruct.m_instanceNum >= UpdateStruct.m_struct->m_numInstances )
				{
					PT_WARNING2( "Invalid instance number %d for update to %s!", UpdateStruct.m_instanceNum, UpdateStruct.m_struct->m_name );
					return PT_Error_Fail;
				}

				PIPE_READ_GET_NUM( UpdateStruct.m_row );
				
				if ( UpdateStruct.m_row >= UpdateStruct.m_struct->m_numRows )
				{
					PT_WARNING2( "Invalid row number %d for update to %s!", UpdateStruct.m_row, UpdateStruct.m_struct->m_name );
					return PT_Error_Fail;
				}

				PIPE_READ_GET_NUM( UpdateStruct.m_time );
				PIPE_READ_GET_NUM( UpdateStruct.m_num_values ); 

				m_EventDataCount = UpdateStruct.m_num_values;
				
				UpdateStruct.m_ID = currentID;
				UpdateStruct.m_namespace = currentNamespace;

				//
				// Make sure this number is reasonable
				if ( m_EventDataCount > 1<<15 )
				{
					PT_WARNING1("Invalid Statement, too many data members listed for struct update to %s.", UpdateStruct.m_struct->m_name);
					m_EventDataCount = 0;
					return PT_Error_InvalidReadStatement;
				}

				// if the caller wanted to see the last statement, return it
				*statement = &UpdateStruct;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnUpdateStruct( UpdateStruct );
				}

				Error = PT_Error_OK;
				break;
			}
		case Pipe_Clear_Struct:
			{
				char *tmpStruct = m_szBuf2;

				// get the struct name
				PIPE_READ_MARK_NAME( tmpStruct );

				// and convert it to a configdata
				if (tmpStruct[0]!='\0')
				{
					PT_Config *outConfig;
					Error = m_pDataStore->FindConfigByName( tmpStruct, &outConfig );
					
					if (PT_SUCCEEDED(Error) && outConfig->GetType( ) == Pipe_Config_Struct )
					{
						ClearStruct.m_struct = (PT_Config_Struct *)outConfig;
					}
					else
					{
						PT_WARNING1("Invalid struct name %s provided for clear!", ClearStruct.m_struct->m_name);
						return PT_Error_Fail;
					}
				}

				PIPE_READ_GET_NUM( ClearStruct.m_instanceNum );

				if ( ClearStruct.m_instanceNum >= ClearStruct.m_struct->m_numInstances )
				{
					PT_WARNING2( "Invalid instance number %d for clear to %s!", ClearStruct.m_instanceNum, ClearStruct.m_struct->m_name );
					return PT_Error_Fail;
				}

				PIPE_READ_GET_NUM( ClearStruct.m_row );
				
				if ( ClearStruct.m_row >= ClearStruct.m_struct->m_numRows )
				{
					PT_WARNING2( "Invalid row number %d for clear to %s!", ClearStruct.m_row, ClearStruct.m_struct->m_name );
					return PT_Error_Fail;
				}

				PIPE_READ_GET_NUM( ClearStruct.m_time );
				
				// see if the clear is of explicit cells
				int numFound = 0;
				
				PIPE_READ_MARK_NAME( tmpStruct );
				while( tmpStruct[0]!='\0' && tmpStruct[0]!='\r' && tmpStruct[0]!='\n' )
				{
					ClearStruct.m_colList = m_StructColArray;
					
					// find the config data for this item
					PT_Config *outConfig;
					Error = m_pDataStore->FindConfigByName( tmpStruct, &outConfig );
					
					if (PT_SUCCEEDED(Error) && outConfig->GetType( ) == Pipe_Config_Data )
					{
						ClearStruct.m_colList[ numFound ] = (PT_Config_Data *)outConfig;
					}
					else
					{
						PT_WARNING2("Invalid col data name %s provided for clear to %s!", "(unknown)", ClearStruct.m_struct->m_name);
						return PT_Error_Fail;
					}					
					
					numFound++;	
					PIPE_READ_MARK_NAME( tmpStruct );
				}

				if (numFound == 0)
				{
					// clear entire row
					ClearStruct.m_colList = NULL;
					ClearStruct.m_numCols = 0;
				}

				ClearStruct.m_numCols = numFound;

				// if the caller wanted to see the last statement, return it
				*statement = &ClearStruct;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnClearStruct( ClearStruct );
				}

				Error = PT_Error_OK;
				break;
			}
		case Pipe_Config_Struct:
			{
				PIPE_READ_MARK_NAME( ConfigStruct.m_name );
				
				PIPE_READ_GET_NUM( ConfigStruct.m_numInstances );
				PIPE_READ_GET_NUM( ConfigStruct.m_numRows );
				PIPE_READ_GET_NUM( ConfigStruct.m_numCols );

				// now, read in the columns, and grab the ConfigData that this corresponds to
				for( PT_UINT i = 0; i < ConfigStruct.m_numCols; i++)
				{
					char *curCol = m_szBuf2;
					PIPE_READ_MARK_NAME( curCol );

					if (curCol[0]!='\0')
					{
						PT_Config *outConfig;
						Error = m_pDataStore->FindConfigByName( curCol, &outConfig );
						
						if (PT_SUCCEEDED(Error) && outConfig->GetType( ) == Pipe_Config_Data )
						{
							ConfigStruct.m_colArray[ i ] = (PT_Config_Data *)outConfig;
						}
						else
						{
							PT_WARNING1("Invalid column names provided for config struct %s!", ConfigStruct.m_name);
							return PT_Error_Fail;
						}
					}
					else
					{
						PT_WARNING1("Not enough column names provided for config struct %s!", ConfigStruct.m_name);
						return PT_Error_Fail;
					}
				}

				PIPE_READ_SKIP_WS;
				
				ConfigStruct.m_key = NextKey( );
				
				// read the description
                //char *x_desc = (char *) malloc(sizeof(char *) * strlen(nxt));
                //strcpy (x_desc, nxt);
                //ConfigEvent.m_description = x_desc;

				//strcpy( ConfigStruct.m_description, nxt );
				strcpy( m_szStringBuffers[ 1 ], nxt );

				// if the caller wanted to see the last statement, return it
				*statement = &ConfigStruct;

				// and ALWAYS send off to the DataStore
				if ( m_pDataStore )
				{
					m_pDataStore->OnConfigStruct( ConfigStruct );
				}
				break;
				
			}
		case Pipe_Config_Event:
			{
				PIPE_READ_MARK_NAME( ConfigEvent.m_event_name );
				ConfigEvent.m_event_char = *nxt++;
				
				PT_ULONG color;
				PIPE_READ_GET_NUM( color );
				ConfigEvent.m_event_color = ( Pipe_Color )color;

				PIPE_READ_SKIP_WS;
				
				ConfigEvent.m_key = NextKey( );
				
				// read the description
//                char *x_desc = (char *) malloc(sizeof(char *) * strlen(nxt));
//                strcpy (x_desc, nxt);
//                ConfigEvent.m_description = x_desc;
				strcpy( m_szStringBuffers[ 1 ], nxt );

				// if the caller wanted to see the last statement, return it
				*statement = &ConfigEvent;

				// and ALWAYS send off to the DataStore
				if ( /*bInsertInfo &&*/ m_pDataStore )
				{
					m_pDataStore->OnConfigEvent( ConfigEvent );
				}
				break;			
			}
		case Pipe_Config_Record:
			{
				PIPE_READ_MARK_NAME( ConfigRecord.m_name );
				PIPE_READ_SKIP_WS;

				// read the description
//                char *x_desc = (char *) malloc(sizeof(char *) * strlen(nxt));
//                strcpy (x_desc, nxt);
//                ConfigEvent.m_description = x_desc;

				strcpy( m_szStringBuffers[ 1 ], nxt );

				ConfigRecord.m_key = NextKey( );
				
				// if the caller wanted to see the last statement, return it
				*statement = &ConfigRecord;

				// and ALWAYS send off to the DataStore 
				if ( /*bInsertInfo &&*/ /*m_pDataStore*/ true )
				{
					m_pDataStore->OnConfigRecord( ConfigRecord );
				}
				break;

			}
		case Pipe_Config_Data:
			{
				PIPE_READ_MARK_NAME( ConfigData.m_name );
				PIPE_READ_SKIP_WS;
				tmp = nxt;
				while ( !isspace( *nxt ) ) nxt++;
		
				// add a new key value
				ConfigData.m_key = NextKey( );

				//
				// Check the first character of the descriptor string to 
				// figure out base type
				switch( *tmp ) 
				{
					case 'B':
						// B could be Bool or ByteArray.. only ambiguous case
						if ( *( tmp + 1 ) == 'o' ) 
						{ 
							ConfigData.m_datatype = Pipe_Boolean; 
						}
						else 
						{
							ConfigData.m_datatype = Pipe_ByteArray;
							
							// Get num_values extra param
							PIPE_READ_GET_NUM( ConfigData.m_arrayLen );
							PT_ASSERT( ConfigData.m_arrayLen );
						}
						break;
					case 'H': ConfigData.m_datatype = Pipe_Hex_Integer; break;
					case 'I': ConfigData.m_datatype = Pipe_Integer; break;
					case 'S': ConfigData.m_datatype = Pipe_String; break;
					case 'A': ConfigData.m_datatype = Pipe_Association; break;
					case 'E': ConfigData.m_datatype = Pipe_Enumeration;
						{
							// read all enumeration entries
							PIPE_READ_GET_NUM( ConfigData.m_arrayLen );
							PT_ASSERT( ConfigData.m_arrayLen );
							PT_ASSERT( ConfigData.m_arrayLen < 1024 );
							for ( unsigned int i = 0; i < ConfigData.m_arrayLen; i++ ) 
							{
								PIPE_READ_MARK_NAME( ConfigData.m_enumArray[ i ] );
							
								// *important* in PSLib, they insert enum values into the 
								// the hash array. since we assume that enum lengths are going
								// to be small, and it is rare (never) that we will ever have to match
								// enum names in fast searches instead of values, we don't do this.
								// but, to maintain backward compatibility, must increse Key Value
								NextKey( );
							}
							break;
						}
					default:
						PT_FATAL("Unrecognized data type");
						break;
				}

				PIPE_READ_SKIP_WS;

				// finally, read description in
//                                char *x_desc = (char *) malloc(sizeof(char *) * strlen(nxt));
//                                strcpy (x_desc, nxt);
//                                ConfigData.m_description = x_desc;

				strcpy( m_szStringBuffers[ 1 ], nxt );

				// if the caller wanted to see the last statement, return it
				*statement = &ConfigData;

				// and ALWAYS send off to the DataStore
				if ( /*bInsertInfo &&*/ m_pDataStore )
				{
					m_pDataStore->OnConfigData( ConfigData );

					if (ConfigData.m_datatype == Pipe_Enumeration)
					{
						ConfigData.m_enumArray = m_extraBuffer;
						
						// line up the extrabuffer entries with szStringBuffers so that
						// enum values are ready to go.
						for (int i = 0; i < 256; i++ )
						{
							m_extraBuffer[ i ] = m_szEnumChars[ i ];
						}						
					}
				}
				break;

				// in this implementation, don't add enum to hash table, no reason to?
			}
        default:
                break;
	}

	return Error;
}
		
////////////////////////////////////////////////////////////////////////////////
// The following stuff was taken directly from PSLib, so as not to create
// really elusive bugs
////////////////////////////////////////////////////////////////////////////////

//msblacks - modified to use zlib
#define BIN_GET_ULONG(x)    get_word(x)

#ifdef USE_ZLIB
//#define BIN_GET_UCHAR(x)    (unsigned char) ( gzgetc(x) )	
#define BIN_GET_UCHAR(x)    (unsigned char) ( m_pBufferedReader->PT_getc( ) )	
#else
#define BIN_GET_UCHAR(x)    (unsigned char) ( fgetc(x) )	
#endif

////////////////////////////////////////////////////////////////////////////////
// get_word()
//
// FUNCTION:
//  Read a compressed word from the pipetrace.
//  See put_word() in pipe_output.c for details of how this is
//  encoded.
////////////////////////////////////////////////////////////////////////////////
unsigned long
#ifdef USE_ZLIB
PT_ReadStream::get_word( ZFILE file )
#else
PT_ReadStream::get_word( FILE *file )
#endif
{
	unsigned long	retval = 0;
	unsigned char	c;
	unsigned char	flag;

	c = BIN_GET_UCHAR( file );
	if (c == 0xff) {
		flag = 0xc0;
		c = BIN_GET_UCHAR( file );
	} else {
		flag = c & 0xc0;
		c &= 0x3f;
	}
	switch(flag) {
	case 0xc0: retval  = c << 24; c = BIN_GET_UCHAR(file);
	case 0x80: retval += c << 16; c = BIN_GET_UCHAR(file);
	case 0x40: retval += c << 8;  c = BIN_GET_UCHAR(file);
	case 0x00: retval += c;  break;
	default:
		PT_FATAL( "Invalid field in get_word" );
	}
	return( retval );
} 

////////////////////////////////////////////////////////////////////////////////
// End hack code
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryReadStream::ReadStatement
// Description: 
// Meant to be used in a while loop... returns PT_Error_EOF when end of
// file reached
// If insertInfo is false, then the record isn't inserted into the 
// datastore
// Most of this code is taken from PSLib, since a rewrite would be dangerous
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_BinaryReadStream::ReadStatement( PT_Statement **statement, PT_BOOL bInsertInfo )
{
	PT_ID currentID;		// record ID associated w/ current statement
	PT_Config *st;
	PT_KEY key = 0;
	PT_Error Error = PT_Error_OK;
	PT_BOOL bGetID = PT_FALSE;

	// it's ok for the user to pass in null, if they don't care what the statement is
	// (they want datastore access instead). so hook us up to a dummy statement
	PT_Statement *tempStatement;
	if ( statement == NULL )
	{
		statement = &tempStatement;
	}

	// get the first byte of the key. if its top bit is set, we'll need to get an ID too
	unsigned char c = BIN_GET_UCHAR( m_file );
	if ( c & 0x80 ) 
	{
		//
		// High bit is set. Turn it off, and mark that we have
		// a record ID to read.
		c &= 0x7f;
		bGetID = PT_TRUE;
	}

	// now read the rest of the key. the size of the key depends on
	// how many definitions we have in the hash table. 
	switch ( m_keySize ) 
	{
		case 4:		key += ((int) c << 24);	c = BIN_GET_UCHAR( m_file );
		case 3:		key += ((int) c << 16);	c = BIN_GET_UCHAR( m_file );
		case 2:		key += ((int) c << 8);	c = BIN_GET_UCHAR( m_file );
		case 1:		key += ((int) c);
			break;
		default:
			PT_FATAL("Internal error - invalid key size" );
	}

	// See if we got anything or if we reached end of file
	//
/*
#ifdef USE_ZLIB
	if ( gzeof( m_file ) ) 
#else
	if ( feof( m_file ) ) 
#endif
*/
#ifdef USE_ZLIB
	if (m_pBufferedReader->PT_eof( ))
	{
		if ( m_pDataStore )
		{
			m_pDataStore->OnEndOfTrace( );
		}
		return PT_Error_EOF;
	}
#else
        if (feof( m_file))
        {
		if ( m_pDataStore )
		{
			m_pDataStore->OnEndOfTrace( );
		}
		return PT_Error_EOF;
        }
#endif

	// If the high bit of the key was there, we need read a record ID.
	// Otherwise, we just use the previous record ID.
	if ( bGetID == PT_TRUE ) 
	{
		currentID = BIN_GET_ULONG( m_file );
		m_lastRecordID = currentID;
	} 
	else 
	{
		currentID = m_lastRecordID;
	}

	// do some sanity checking on our IDs
	PT_ASSERT( key < (1 << (m_keySize * 8)) );

	// now determine the statement type
	Statement_Type eType = Pipe_Invalid_Type;
        //        cerr<<key<<endl;
	switch( key )
	{
		case PIPE_CLEAR_STRUCT_KEY: eType = Pipe_Clear_Struct; break;
		case PIPE_UPDATE_STRUCT_KEY: eType = Pipe_Update_Struct; break;
		case PIPE_OPEN_KEY: eType = Pipe_Open; break;
		case PIPE_CLOSE_KEY: eType = Pipe_Close; break;
		case PIPE_CONFIG_DATA_KEY: eType = Pipe_Config_Data; break;
		case PIPE_CONFIG_EVENT_KEY: eType = Pipe_Config_Event; break;
		case PIPE_CONFIG_RECORD_KEY: eType = Pipe_Config_Record; break;
		case PIPE_CONFIG_STRUCT_KEY: eType = Pipe_Config_Struct; break;
		case PIPE_COMMENT_KEY: eType = Pipe_Comment; break;
		default:
			{
				// look it up!
				Error = m_pDataStore->FindConfigByKey( ( PT_KEY ) key, &st );

				if ( PT_FAILED( Error ) ) 
				{
					PT_WARNING1( "Definition Lookup failed for key = %d !", key );
					return Error;
				}

				Statement_Type type = st->GetType();

				if ( type == Pipe_Config_Event )
				{
					eType = Pipe_Event;
				}
				else if ( type == Pipe_Config_Data )
				{
					eType = Pipe_Data;
				}
				else if ( type == Pipe_Config_Record )
				{
					// must be a new record since it has a config record
					eType = Pipe_Open;
				}
			}
	}

	//
	// Make sure the ID was valid
	if ( currentID >= m_nextRecord && eType != Pipe_Open ) 
	{
		PT_WARNING1( "Attempting to add information to a record (%d) which has not yet been opened. Exiting\n", currentID );
		return PT_Error_InvalidReadStatement;
	}

	// if we are expecting event data, it must appear now.
	if ( m_EventDataCount != 0 && eType != Pipe_Data )
	{
		PT_WARNING1( "Invalid Statement -> must be event data here for recID %d!", currentID );
		return PT_Error_InvalidReadStatement;
	}
	
		
	// now parse based on type
	switch( eType ) 
	{
		case Pipe_Open:
			{
				//
				// Error check the record ID.  It MUST be the next one in order.
				if ( currentID < m_nextRecord ) 
				{
					PT_WARNING1( "Attempting to reopen a previously opened record (%d)\n", currentID );
					return PT_Error_InvalidReadStatement;
				} 
				else if ( currentID != m_nextRecord ) 
				{
					PT_WARNING1( "Attempting to open records out of order (recID = %d).\n", currentID );
					return PT_Error_InvalidReadStatement;
				}

				// assign ID
				OpenStatement.m_ID = currentID;
				OpenStatement.m_recordType = (PT_Config_Record *) st;

				// if the caller wanted to see the last statement, return it
				*statement = &OpenStatement;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnOpenStatement( OpenStatement );
				}
				
				m_nextRecord++;

				break;
			}
		case Pipe_Close:
			{
				//
				// Error check the record ID.
				if ( currentID >= m_nextRecord) 
				{
					PT_WARNING1( "Attempting to close an unopened statement (recID = %d )", currentID );
					return PT_Error_InvalidReadStatement;
				} 

				CloseStatement.m_ID = currentID;

				// if the caller wanted to see the last statement, return it
				*statement = &CloseStatement;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnCloseStatement( CloseStatement );
				}
				
				break;
			}
		case Pipe_Comment:
			{
				//
				// Copy the string
				BinGetString( CommentStatement.m_szComment );

				// if the caller wanted to see the last statement, return it
				*statement = &CommentStatement;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnCommentStatement( CommentStatement );
				}
				break;
			}
		case Pipe_Event:
			{
				EventStatement.m_EventDef = ( PT_Config_Event * )st;
				EventStatement.m_num_values = BIN_GET_UCHAR( m_file );
				EventStatement.m_duration = BIN_GET_ULONG( m_file );
				EventStatement.m_time = BIN_GET_ULONG( m_file );
				//
				// Convert delta time to current time, in version 3.2 pipetraces
				// (YES greater) - msblacks
				if ( m_bIsVersion3_2 || m_bIsVersion3_3 )
				{					
					EventStatement.m_time += m_lastTime;
				}
				m_lastTime = EventStatement.m_time;

				m_EventDataCount = EventStatement.m_num_values;
				
				//
				// Error check the record ID.
				//if ( currentID >= m_nextRecord) 
				//{
				//	PT_WARNING( "Attempting to add an event to an unopened statement" );
				//	return PT_Error_InvalidReadStatement;
				//} 

				EventStatement.m_ID = currentID;

				//
				// Make sure this number is reasonable
				if ( m_EventDataCount > 1<<15 )
				{
					PT_WARNING2("Invalid Statement, too many data members listed for event %s, recID = %d.", EventStatement.m_EventDef->m_event_name, currentID);
					m_EventDataCount = 0;
					return PT_Error_InvalidReadStatement;
				}

				// if the caller wanted to see the last statement, return it
				*statement = &EventStatement;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnEventStatement( EventStatement );
				}

				Error = PT_Error_OK;
				break;
			}
		case Pipe_Data:
			{
				PT_Data_Statement *DS;

				// should we be working with a DataStatement or an 
				// EventData statement? it matters if we are going to 
				// try to write out what we read in...
				if ( m_EventDataCount != 0 )
				{
					m_EventDataCount--;
					
					// let's work with the dataevent
					DS = &EventDataStatement;
				}
				else
				{
					// if we ARE zero, it's still ok, it's record data
					DS = &DataStatement;
				}

				DS->m_DataDef = ( PT_Config_Data * )st; 
				DS->m_ID = currentID;


				//char *strLoc;			

				// now get the data
				// just cast m_pData into whatever it really is
				switch ( DS->m_DataDef->m_datatype )
				{
					case Pipe_Integer:
					case Pipe_Hex_Integer:
					case Pipe_Association:
						( * ((long *) (DS->m_pData) ) ) = BIN_GET_ULONG( m_file ); break;
					case Pipe_String:
						BinGetString( (char *) (DS->m_pData) ); break;
					case Pipe_Boolean:
						*((char *) (DS->m_pData) ) = ( BIN_GET_UCHAR( m_file ) == 1 ) ? PT_TRUE:PT_FALSE; break;
					case Pipe_Enumeration:
						{
							PT_ULONG i = BIN_GET_ULONG( m_file );
							
							// enum is just stored as an index
							if ( i >= DS->m_DataDef->m_arrayLen )
							{
								//invalid enum index
								PT_WARNING2( "Invalid enumeration index for %s, recID = %d", DS->m_DataDef->m_name, currentID );
								return PT_Error_InvalidReadStatement;
							}
							
							( * ((PT_ULONG *) (DS->m_pData) ) ) = i;
							break;
						}
					case Pipe_ByteArray:
						{
							PT_ASSERT( DS->m_DataDef->m_arrayLen );

							for (unsigned int i = 0; i < DS->m_DataDef->m_arrayLen; i++) 
							{
								((char *) ( DS->m_pData ))[ i ] = BIN_GET_UCHAR( m_file );
							}

							break;
						}
					default:
						{
							PT_FATAL("Unknown type for Pipe_Data");
						}
				}

				// if the caller wanted to see the last statement, return it
				// also, a writer will care if it is just data or event data, so differentiate
				*statement = DS;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					if ( DS == &EventDataStatement )
					{
						m_pDataStore->OnEventDataStatement( EventDataStatement );
					}
					else
					{
						m_pDataStore->OnDataStatement( DataStatement );
					}
				}

				break;
			}

		case Pipe_Update_Struct:
			{
				// 0 UpdateStruct [0 ID] numvalues RecID StructKey instanceNum row time 
				UpdateStruct.m_num_values = BIN_GET_UCHAR( m_file );
				UpdateStruct.m_ID = BIN_GET_ULONG( m_file );
				
				// retrieve the struct key
				key = BIN_GET_ULONG( m_file );
				PT_Config *outConfig;
				Error = m_pDataStore->FindConfigByKey( key, &outConfig );
				
				if (PT_SUCCEEDED(Error) && outConfig->GetType( ) == Pipe_Config_Struct )
				{
					UpdateStruct.m_struct = (PT_Config_Struct *)outConfig;
				}
				else
				{
					PT_WARNING1("Invalid struct key %d provided for update!", key);
					return PT_Error_Fail;
				}

				UpdateStruct.m_instanceNum = BIN_GET_ULONG( m_file );
				if ( UpdateStruct.m_instanceNum >= UpdateStruct.m_struct->m_numInstances )
				{
					PT_WARNING2( "Invalid instance %d number for update to %s!", UpdateStruct.m_instanceNum, UpdateStruct.m_struct->m_name );
					return PT_Error_Fail;
				}

				UpdateStruct.m_row = BIN_GET_ULONG( m_file );
				if ( UpdateStruct.m_row >= UpdateStruct.m_struct->m_numRows )
				{
					PT_WARNING2( "Invalid row number %d for update to %s!", UpdateStruct.m_row, UpdateStruct.m_struct->m_name );
					return PT_Error_Fail;
				}

				UpdateStruct.m_time = BIN_GET_ULONG( m_file );

				m_EventDataCount = UpdateStruct.m_num_values;

				//
				// Make sure this number is reasonable
				if ( m_EventDataCount > 1<<15 )
				{
					PT_WARNING("Invalid Statement, too many data members listed for struct update.");
					m_EventDataCount = 0;
					return PT_Error_InvalidReadStatement;
				}

				// if the caller wanted to see the last statement, return it
				*statement = &UpdateStruct;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnUpdateStruct( UpdateStruct );
				}

				Error = PT_Error_OK;
				break;
			}
		case Pipe_Clear_Struct:
			{
				// 0 ClearStruct [0 ID] RecID StructKey instanceNum row time {colx1 colx2} 0
				UpdateStruct.m_ID = BIN_GET_ULONG( m_file );

				// retrieve the struct key
				key = BIN_GET_ULONG( m_file );

				// and convert it to a configdata
				PT_Config *outConfig;
				Error = m_pDataStore->FindConfigByKey( key, &outConfig );
				
				if (PT_SUCCEEDED(Error) && outConfig->GetType( ) == Pipe_Config_Struct )
				{
					ClearStruct.m_struct = (PT_Config_Struct *)outConfig;
				}
				else
				{
					PT_WARNING1("Invalid struct key %d provided for clear!", key);
					return PT_Error_Fail;
				}

				ClearStruct.m_instanceNum = BIN_GET_ULONG( m_file );

				if ( ClearStruct.m_instanceNum >= ClearStruct.m_struct->m_numInstances )
				{
					PT_WARNING2( "Invalid instance number %d for clear to %s!", ClearStruct.m_instanceNum, ClearStruct.m_struct->m_name );
					return PT_Error_Fail;
				}

				ClearStruct.m_row = BIN_GET_ULONG( m_file );
				
				if ( ClearStruct.m_row >= ClearStruct.m_struct->m_numRows )
				{
					PT_WARNING2( "Invalid row number %d for clear to %s!", ClearStruct.m_row, ClearStruct.m_struct->m_name  );
					return PT_Error_Fail;
				}

				ClearStruct.m_time = BIN_GET_ULONG( m_file );
				
				// see if the clear is of explicit cells
				int numFound = 0;
				PT_ULONG curColKey;

				curColKey = BIN_GET_ULONG( m_file );
				while( curColKey != 0 )
				{
					ClearStruct.m_colList = m_StructColArray;
					
					// find the config data for this item
					PT_Config *outConfig;
					Error = m_pDataStore->FindConfigByKey( curColKey, &outConfig );
					
					if (PT_SUCCEEDED(Error) && outConfig->GetType( ) == Pipe_Config_Data )
					{
						ClearStruct.m_colList[ numFound ] = (PT_Config_Data *)outConfig;
					}
					else
					{
						PT_WARNING2("Invalid col data key %d provided for clear to %s!", curColKey, ClearStruct.m_struct->m_name);
						return PT_Error_Fail;
					}					
					
					numFound++;	
					curColKey = BIN_GET_ULONG( m_file );
				}

				if (numFound == 0)
				{
					// clear entire row
					ClearStruct.m_colList = NULL;
					ClearStruct.m_numCols = 0;
				}

				ClearStruct.m_numCols = numFound;

				// if the caller wanted to see the last statement, return it
				*statement = &ClearStruct;

				// and send off to the DataStore if asked
				if ( bInsertInfo )
				{
					m_pDataStore->OnClearStruct( ClearStruct );
				}

				Error = PT_Error_OK;
				break;
			}
		case Pipe_Config_Event:
			{
                                //char *event_name = (char *) malloc(sizeof(char *) * PT_MAX_PATH);
				BinGetString( m_szStringBuffers[ 0 ] );
                                //ConfigEvent.m_event_name = event_name;
				
				ConfigEvent.m_event_char = BIN_GET_UCHAR( m_file );
				ConfigEvent.m_event_color = ( Pipe_Color ) BIN_GET_ULONG( m_file );
				
				ConfigEvent.m_key = NextKey( );
				
				// read the description
                                //char *description = (char *) malloc(sizeof(char *) * PT_MAX_PATH);
				BinGetString( m_szStringBuffers[ 1 ] );
                                //ConfigEvent.m_description = description;

				// if the caller wanted to see the last statement, return it
				*statement = &ConfigEvent;

				// and ALWAYS send off to the DataStore
				if ( m_pDataStore )
				{
					m_pDataStore->OnConfigEvent( ConfigEvent );
				}
				break;			
			}
		case Pipe_Config_Record:
			{
                                //char *name = (char *) malloc(sizeof(char *) * PT_MAX_PATH);
                                
				BinGetString( m_szStringBuffers[ 0 ] );
                                //ConfigRecord.m_name = name;
                                
                                //char *description = (char *) malloc(sizeof(char *) * PT_MAX_PATH);
				BinGetString( m_szStringBuffers[ 1 ] );
                                //ConfigEvent.m_description = description;

				ConfigRecord.m_key = NextKey( );
				
				// if the caller wanted to see the last statement, return it
				*statement = &ConfigRecord;

				// and ALWAYS send off to the DataStore 
				if ( m_pDataStore )
				{
					m_pDataStore->OnConfigRecord( ConfigRecord );
				}
				break;

			}
		case Pipe_Config_Struct:
			{
				// 0 ConfigStruct [0 ID] Name 0 szDescription 0 #instances #rows #cols {colKey1 colKey2  colKey3 colKey4} 
				BinGetString( m_szStringBuffers[ 0 ] ); //ConfigStruct.m_name
				BinGetString( m_szStringBuffers[ 1 ] ); //ConfigStruct.m_description
				
				ConfigStruct.m_numInstances = BIN_GET_ULONG( m_file );
				ConfigStruct.m_numRows = BIN_GET_ULONG( m_file );
				ConfigStruct.m_numCols = BIN_GET_UCHAR( m_file );

				// now, read in the columns, and grab the ConfigData that this corresponds to
				for( PT_UINT i = 0; i < ConfigStruct.m_numCols; i++)
				{
					PT_ULONG curColKey;
					curColKey = BIN_GET_ULONG( m_file );

					if (curColKey!=0)
					{
						PT_Config *outConfig;
						Error = m_pDataStore->FindConfigByKey( curColKey, &outConfig );
						
						if (PT_SUCCEEDED(Error) && outConfig->GetType( ) == Pipe_Config_Data )
						{
							ConfigStruct.m_colArray[ i ] = (PT_Config_Data *)outConfig;
						}
						else
						{
							PT_WARNING1("Invalid column names provided for struct %s!", ConfigStruct.m_name);
							return PT_Error_Fail;
						}
					}
					else
					{
						PT_WARNING1("Not enough column names provided for struct %s!", ConfigStruct.m_name);
						return PT_Error_Fail;
					}
				}

				ConfigStruct.m_key = NextKey( );
				
				// if the caller wanted to see the last statement, return it
				*statement = &ConfigStruct;

				// and ALWAYS send off to the DataStore
				if ( m_pDataStore )
				{
					m_pDataStore->OnConfigStruct( ConfigStruct );
				}
				break;
				
			}
	case Pipe_Config_Data:
			{
                                //char *name = (char *) malloc(sizeof(char *) * PT_MAX_PATH);
				BinGetString( m_szStringBuffers[ 0 ] );
                                //ConfigData.m_name = name;
				ConfigData.m_datatype = ( Base_Data_Type )BIN_GET_ULONG( m_file );

				if ( ConfigData.m_datatype >= Pipe_Max_Data_Types )
				{
					// something screwy is going on
					PT_FATAL("invalid data type value");
				}
				
				// if its a special type, then the num elements comes next
				if ( ConfigData.m_datatype == Pipe_Enumeration || ConfigData.m_datatype == Pipe_ByteArray) 
				{
					ConfigData.m_arrayLen = BIN_GET_ULONG( m_file );
				} 
				else 
				{
					ConfigData.m_arrayLen = 0;
				}
		
				// msblacks- weird, this is out of order from ASCII, but description
				// comes next
                                //char *description = (char *) malloc(sizeof(char *) * PT_MAX_PATH);
				BinGetString( m_szStringBuffers[ 1 ] );
                                //ConfigData.m_description = description;

				// add a new key value
				ConfigData.m_key = NextKey( );

				// now, if it's an enum, get the enum values
				if ( ConfigData.m_datatype == Pipe_Enumeration ) 
				{
					for (PT_ULONG i = 0; i < ConfigData.m_arrayLen; i++) 
					{
						BinGetString( ConfigData.m_enumArray[ i ] );

						// *important* in PSLib, they insert enum values into the 
						// the hash array. since we assume that enum lengths are going
						// to be small, and it is rare (never) that we will ever have to match
						// enum names in fast searches instead of values, we don't do this.
						// but, to maintain backward compatibility, must increse Key Value
						NextKey( );
					}
				}

				// if the caller wanted to see the last statement, return it
				*statement = &ConfigData;

				// and ALWAYS send off to the DataStore
				if ( m_pDataStore )
				{
					m_pDataStore->OnConfigData( ConfigData );

					if (ConfigData.m_datatype == Pipe_Enumeration)
					{
						ConfigData.m_enumArray = m_extraBuffer;
						
						// line up the extrabuffer entries with szStringBuffers so that
						// enum values are ready to go.
						for (int i = 0; i < 256; i++ )
						{
							m_extraBuffer[ i ] = m_szEnumChars[ i ];
							
							// for debug
//							memset( m_extraBuffer[ i ], 0, 256 );
						}						
					}

				}
				break;

				// in this implementation, don't add enum to hash table, no reason to?
			}
        default:
                break;

	}

	return Error;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BinaryReadStream::BinGetString
// Description: read the next null-terminated string from the file
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void
PT_BinaryReadStream::BinGetString( char *szString )
{
	char *current = szString;
	
	// read string one char at a time until we hit a '\0'
	do {
		*current = BIN_GET_UCHAR( m_file );
	} while ( *(current++) != '\0' );
}



////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BufferedReader::PT_BufferedReader
// Description: Contructor
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
#ifdef USE_ZLIB
PT_BufferedReader::PT_BufferedReader( ZFILE file, PT_Error *Error, PT_UINT bufferSize )

        //PT_BufferedReader::PT_BufferedReader( FILE *file, PT_Error *Error, PT_UINT bufferSize )								       
{
	*Error = PT_Error_OK;

	m_file = file;
	m_bufferSize = bufferSize;

	m_indexPointer = 0;

	m_pBuffer = new char[bufferSize];

	m_bEOF = false;

	m_bytesRead = 0;

	// do an initial fill on the buffer
	int ret = gzread( m_file, m_pBuffer, m_bufferSize );

	m_bytesRead += ret;

	if ( ret == 0 )
	{
		*Error = PT_Error_EOF;
		m_bEOF = true;
		return;
	}
	else if ( ret == -1 )
	{
		*Error = PT_Error_Fail;
		return;
	}

	m_bufferSize = ret;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: ~PT_BufferedReader::PT_BufferedReader
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_BufferedReader::~PT_BufferedReader( )
{
	if ( m_pBuffer )
	{
		delete m_pBuffer;
	}
}


//////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BufferedReader::PT_gets
// Description: call this instead of fgets or gzgets
// 
// 
// Parameters: 
// Returns: 
//
//////////////////////////////////////////////////////////////////////////////// 
char *PT_BufferedReader::PT_gets( char *buffer, PT_UINT bufSize )
{
	// fill up the return buffer
	unsigned int pos = 0;

	unsigned int destEnd = bufSize - 2;

	while ( (	m_indexPointer < m_bufferSize ) 
				&& ( m_pBuffer[m_indexPointer] != '\n' ) 
				&& ( m_pBuffer[m_indexPointer] != '\0' ) 
				&& ( pos < destEnd ) ) 
	{
		buffer[ pos++ ] = m_pBuffer[ m_indexPointer++ ];
	}

	if ( m_indexPointer >= m_bufferSize )
	{
		// need to read more from buffer
		int ret = gzread( m_file, m_pBuffer, m_bufferSize );

		if ( ret == 0 )
		{
			m_bEOF = true;
			return NULL;	
		}

		m_bytesRead += ret;

		m_bufferSize = ret;	
		m_indexPointer = 0;

		// recursively get the rest
		PT_gets( &(buffer[pos]), bufSize - pos );

		return buffer;
	}
	else if ( m_pBuffer[m_indexPointer] == '\n' ) 
	{
		// found end of line
		buffer[ pos ] = '\n';
		buffer[ pos + 1 ] = '\0';
		m_indexPointer++;
		return buffer;
	}
	else if ( m_pBuffer[m_indexPointer] == '\0' ) 
	{
		// found end of stream
		buffer[ pos ] = '\n';
		buffer[ pos + 1 ] = '\n';
		m_bEOF = true;
		return buffer;
	}
	else if ( pos == ( bufSize - 2 ) )
	{
		// filled the return buffer
		buffer[ pos ] = '\n';
		buffer[ pos + 1 ] = '\0';
		
		// find end of line anyway
		while( (m_indexPointer < m_bufferSize) &&
			   ( m_pBuffer[m_indexPointer] != '\n' ) &&
			   ( m_pBuffer[m_indexPointer] != '\0' ) )
			pos++;

		if ( m_pBuffer[m_indexPointer] == '\n' ) pos++;

		return buffer;
	}
	else	// not reachable, but get rid of warning
		return buffer;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_BufferedReader::PT_gets
// Description: call this instead of fgetc or gzgetc
// 
// 
// Parameters: 
// Returns: 
//
//////////////////////////////////////////////////////////////////////////////// 
char PT_BufferedReader::PT_getc( )
{
	if ( m_indexPointer < m_bufferSize )
	{
		return m_pBuffer[ m_indexPointer++ ];
	}
	else
	{
		// need to read more from buffer
		int ret = gzread( m_file, m_pBuffer, m_bufferSize );

		m_indexPointer = 0;
		m_bufferSize = ret;

		m_bytesRead = ret;

		if ( ret == 0 )
		{
			m_bEOF = true;
			return 0;	
		}

		else return m_pBuffer[ m_indexPointer++ ];
	}
}

int PT_BufferedReader::PT_eof( )
{
	return ( m_bEOF ) ? 1:0;
}
#endif


#endif //__PT_IO_CPP

  




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

#ifndef __PT_TYPES_CPP
#define __PT_TYPES_CPP

#include <string.h>
#include <stdio.h>

#include "PT_Log.h"
#include "PT_Util.h"
#include "PT_Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: DataTypeToString
// Description: return a Base_Data_Type as a string
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
const char *DataTypeToString( Base_Data_Type dt )
{
	switch( dt )
	{
	case Pipe_Boolean:		return "Boolean";
	case Pipe_Integer:		return "Integer";
	case Pipe_Hex_Integer:	return "Hex";
	case Pipe_String:		return "String";
	case Pipe_Association:  return "Association";
	case Pipe_Enumeration:	return "Enumeration";
	case Pipe_ByteArray:	return "ByteArray";
	default: return "";
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//	Constructor Implementations for Statements
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Comment_Statement::PT_Comment_Statement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Comment_Statement::PT_Comment_Statement( char * szComment ) : 
										m_szComment( szComment )
{
	/*
	size_t size = strlen( event_name );
	if ( size )
	{
		// dynamically create the string and copy...
		// thought about just copying the pointer, but this means we have to 
		// worry about the user allocating a string
		m_event_name = new char[ strlen( event_name ) ];
		strcpy( m_event_name, event_name );
	}
	else 
	{
		m_event_name = NULL;
	}
	*/
}



////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Config_Event::PT_Config_Event
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Config_Event::PT_Config_Event(	const char * event_name, char event_char, 
                                        Pipe_Color event_color, const char *description ) :
	m_event_name( event_name ),  
        m_event_char( event_char ),
        m_event_color( event_color ),
	m_description( description )
//	m_firstEvent(NULL), m_lastEvent(NULL)
{
		/*
	size_t size = strlen( event_name );
	if ( size )
	{
		// dynamically create the string and copy...
		// thought about just copying the pointer, but this means we have to 
		// worry about the user allocating a string
		m_event_name = new char[ strlen( event_name ) ];
		strcpy( m_event_name, event_name );
	}
	else 
	{
		m_event_name = NULL;
	}
	*/
}

/*
PT_Config_Event::~PT_Config_Event( )
{
	if ( m_event_name ) 
	{
		delete m_event_name;
	}
}
*/

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Config_Data::PT_Config_Data
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Config_Data::PT_Config_Data(	const char *name, Base_Data_Type datatype, const char *description,
								int arrayLen, char **enumArray ) :
        m_name( name ),
        m_datatype( datatype ),
        m_description( description ), 
        m_arrayLen( arrayLen ),
        m_enumArray( enumArray ),
        m_bRecordData(false)

{

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Config_Record::PT_Config_Record
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Config_Record::PT_Config_Record( const char *name, const char *description ) :
					m_name( name ), m_description( description )
{
	
}

PT_Config_Struct::PT_Config_Struct( const char *name, const char *description, PT_ULONG numInstances, PT_ULONG numRows, PT_ULONG numCols, PT_Config_Data **colArray ) :
	m_name( name ), m_description( description ), m_numRows( numRows ), m_numCols( numCols ), m_colArray( colArray ), m_numInstances( numInstances )
{

}

PT_Clear_Struct::PT_Clear_Struct(	PT_ID ID,  PT_Config_Struct *StructDef, PT_ULONG instanceNum,
                                        PT_Time time, PT_ULONG row, PT_Config_Data **colList, PT_ULONG numCols, PT_ID namespaceID  ) :
	m_struct( StructDef ),
        m_ID( ID ),  
        m_time( time ), 
        m_row( row ), 
        m_colList( colList ), 
        m_numCols( numCols ),
	m_instanceNum(instanceNum),
        m_namespace( namespaceID )
{

}

PT_Update_Struct::PT_Update_Struct(	PT_ID ID,  PT_Config_Struct *StructDef, PT_ULONG instanceNum,
								   PT_Time time, PT_ULONG row, PT_ULONG numData, PT_ID namespaceID ) :
        m_struct( StructDef ),
	m_ID( ID ), 
        m_namespace( namespaceID ),
        m_time( time ), 
        m_row( row ), 
        m_num_values( numData ),
        m_instanceNum(instanceNum)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Open_Statement::PT_Open_Statement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Open_Statement::PT_Open_Statement( PT_Config_Record *def ) :
        m_ID( 0 ), m_recordType( def )
{

}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Event_Statement::PT_Event_Statement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Event_Statement::PT_Event_Statement( PT_ID ID,  PT_Config_Event *EventDef, 
					PT_Time time, PT_Time duration, PT_ULONG num_values, PT_ID namespaceID ) :
		m_ID(ID), m_EventDef( EventDef ), m_time( time ), m_duration( duration ),
		m_num_values( num_values ), m_namespace( namespaceID )
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Data_Statement::PT_Data_Statement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Data_Statement::PT_Data_Statement( PT_ID ID, PT_Config_Data *DataDef, void *data, PT_ID namespaceID ):
		m_ID( ID ), m_DataDef( DataDef ), m_namespace( namespaceID )
{
	m_pData = data;
}

PT_Data_Statement::~PT_Data_Statement()
{
//	if ( m_pData != NULL ) delete ((void *)m_pData);
}

////////////////////////////////////////////////////////////////////////////////
//
//	ToString Implementations for Statements
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Comment_Statement::ToString
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Comment_Statement::ToString( char *szIn )
{
	FastStringCreate( szIn, "Comment Statement: ", m_szComment, "\n" );
	return szIn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Config_Event::ToString
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Config_Event::ToString( char *szIn )
{
	char myChar[2];
	myChar[0] = m_event_char;
	myChar[1] = '\0';

	char buffer[256];

	FastStringCreate( szIn, "Config Event Statement: ", "Name:", m_event_name, 
					  " Description:", m_description, " Color:", 
					  ultoascii((unsigned long)m_event_color, buffer), " Char:", myChar, 
					  "\n" );
	return szIn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Config_Data::ToString
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Config_Data::ToString( char *szIn )
{
	FastStringCreate( szIn, "Config Data Statement: ", "Name:", m_name, 
					  " Type:", DataTypeToString( m_datatype ), " Description:", m_description, "\n" );
	return szIn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: char *PT_Config_Record::ToString( char *szIn )
//
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Config_Record::ToString( char *szIn )
{
	FastStringCreate( szIn, "Config Record Statement: ", "Name:" ,m_name, " Description:",m_description, "\n" );
	return szIn;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: char *PT_Config_Struct::ToString( char *szIn )
//
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Config_Struct::ToString( char *szIn )
{
	FastStringCreate( szIn, "Config Struct Statement: ", "Name:" ,m_name, " Description:",m_description, " Num Rows:", m_numRows, " Num Cols: ", m_numCols, "\n" );
	return szIn;
}

char *PT_Clear_Struct::ToString( char *szIn )
{
	FastStringCreate( szIn, "Clear Struct Statement: ", "RecID:" ,m_ID, " Struct:",m_struct->m_name, " Row:", m_row, " Time: ", m_time, "\n" );
	return szIn;
}

char *PT_Update_Struct::ToString( char *szIn )
{
	FastStringCreate( szIn, "Update Struct Statement: ", "RecID:" ,m_ID, " Struct:",m_struct->m_name, " Row:", m_row, " Time: ", m_time, "\n" );
	return szIn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Open_Statement::ToString
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Open_Statement::ToString( char *szIn )
{
	char buffer[256];
	FastStringCreate( szIn, "Open Record Statement: ", "ID:" ,ultoascii((unsigned long)m_ID, buffer), "(Type on next line)","\n");
	
	//kind of a lazy cheat, but let the config print itself out
	char *next = szIn + strlen( szIn );
	m_recordType->ToString( next );

	return szIn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Close_Statement::ToString
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Close_Statement::ToString( char *szIn )
{
	char buffer[256];
	FastStringCreate( szIn, "Close Record Statement: ", "ID:" ,ultoascii(m_ID, buffer), "\n");

	return szIn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Event_Statement::ToString
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Event_Statement::ToString( char *szIn )
{
	char buffer[256];
	char buffer2[256];
	char buffer3[256];

	FastStringCreate( szIn, "Event Statement: ", "ID:" ,ultoascii(m_ID, buffer3), 
					  " time", ultoascii(m_time, buffer), " duration", ultoascii(m_duration, buffer2), " (Type on next line)","\n");
	
	//kind of a lazy cheat, but let the config print itself out
	char *next = szIn + strlen( szIn );
	m_EventDef->ToString( next );	

	return szIn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Data_Statement::ToString
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Data_Statement::ToString( char *szIn )
{
	char temp[ PT_MAX_PATH ];
	DataToString( temp );

	char buffer[256];

	FastStringCreate(	szIn, "Data Statement: ", "ID:" ,ultoascii(m_ID, buffer), 
						" Data:",temp,  "\n");

	//kind of a lazy cheat, but let the config print itself out
	char *next = szIn + strlen( szIn );
	m_DataDef->ToString( next );

	return szIn;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_EventData_Statement::ToString
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_EventData_Statement::ToString( char *szIn )
{
	char temp[ PT_MAX_PATH ];
	DataToString( temp );

	char buffer[256];

	FastStringCreate(	szIn, "EventData Statement: ", "ID:" ,ultoascii(m_ID, buffer), 
						" Data:",temp,  "\n");

	//kind of a lazy cheat, but let the config print itself out
	char *next = szIn + strlen( szIn );
	m_DataDef->ToString( next );

	return szIn;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Data_Statement::DataToString
// Description: 
// String rep of data - can be used for ASCII traces, or for debug
// Returned ptr is same as in ptr, useful for using in a function call
//
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *PT_Data_Statement::DataToString( char *szIn )
{
	// do not make this static - need to be threadsafe
	char buffer[ 256 ];

	if ( !m_DataDef /*|| !m_pData*/)
	{
		return NULL;
	}

	switch( m_DataDef->m_datatype )
	{
		case Pipe_Boolean: 
			{
				// cast to a 'BOOL' and represent as a string
				strcpy( szIn, ( *((PT_ULONG *) m_pData) != 0) ? "True":"False" ); 
				break;
			}
		case Pipe_Integer: 
			{
				// cast to a long
				strcpy( szIn, ltoascii( *((long *) m_pData), buffer ) ); 
				break;
			}
		case Pipe_Hex_Integer: 
			{
				// cast to unsigned long and then represent as a hex string
				strcpy( szIn, ultox( *((PT_ULONG *) m_pData), buffer ) ); 
				break;
			}
		case Pipe_String: 
			{
				// just use it as a char *
				strcpy( szIn, (char *) m_pData ); 
				break;
			}

		case Pipe_Association: 
			{
				// cast to unsigned long ( PT_ID )
				char *temp = ultoascii( *((PT_ULONG *) m_pData), buffer);
				strcpy( szIn, temp ); 
				break;
			}
		case Pipe_Enumeration: 
			{				
				if ( *((PT_ULONG *) m_pData ) > m_DataDef->m_arrayLen)
				{
					PT_WARNING( "Invalid enum index!" );
					return NULL;
				}

				// cast to ulong and then use it as an index into the enum array
				strcpy( szIn, m_DataDef->m_enumArray[ (*(PT_ULONG *) m_pData) ] ); 
				break;
			}
		case Pipe_ByteArray: 
			{
				char *current = szIn;

				for ( PT_UINT i = 0; i < m_DataDef->m_arrayLen; i++ )
				{
					current = FastStringCat( current, ultox( ( PT_ULONG )( ( ( (char *)m_pData )[ i ] ) & 0xff ), buffer ) ) ;
					if ( i != ( m_DataDef->m_arrayLen - 1 ) )
					{
						current = FastStringCat( current, " " );
					}
				}

				break;
			}
        default:
                break;
	}

	return szIn;
}


#endif	//__PT_TYPES_CPP
  




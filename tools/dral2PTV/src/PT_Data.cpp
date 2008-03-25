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

#ifndef __PT_DATA_CPP
#define __PT_DATA_CPP

#include "PT_Data.h"

// nice start size for lookup tables... tables can grow automatically if 
// necessary
#define DEFAULT_TABLE_SIZE	(2048)

// how many events can start in one clock cycle? need this to store temp
// event list
#define MAX_EVENTS_PER_TIME	(5000)

// we'll use 1 MB "pages" of data memory. Large enough that not a lot
// of overhead when growing, small enough that we won't be "overallocating"
// too much
#define DATA_TABLE_PAGE_SIZE	(1024 * 1024)

// 2 MB "pages" of event memory, since there are so many more events, and
// events need to be able to be acccesed sequentially
#define EVENT_TABLE_PAGE_SIZE	(2 * 1024 * 1024)

#define RECORD_TABLE_PAGE_SIZE 20000

// cache event pointers for the last 50K times
#define NUM_CACHED_TIMES	(50 * 1024)

#define THREAD_NAME "Thread"
#define PARENT_NAME "Parent"
#define INSTID_NAME "Inst_ID"

// for osiris
#define INSTID_NAME2 "ia_inst_count"

#define UID_NAME "UID"

// the following defines, correspond to bits in the char returned by
// PT_Sorter::RegisterEvents
#define RECORD_COMPLETE_BIT			0x01
#define EVENT_COMPLETE_BIT			0x02
#define RECORD_CONFIG_COMPLETE_BIT	0x04
#define EVENT_CONFIG_COMPLETE_BIT	0x08
#define DATA_CONFIG_COMPLETE_BIT	0x10

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::PT_DataStore
// Description: DataStore constructor. This should be called by the
//				derived class' constructor
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_DataStore::PT_DataStore( PT_Sorter *pSorter, PT_BOOL doWatch ) 
{
	// good guess at definition size ?
	m_KeyTable = new KeyMap;	
	
	// good guess at definition size ?
	m_NameTable = new NameMap;

	m_bDoWatch = (doWatch == PT_TRUE) ? true:false;

	if (m_bDoWatch)
	{
		m_StructTable = new StructMap;
	}
	else
	{
		m_StructTable = NULL;
	}
	
	m_EnumTable = NULL;
	m_StringTable = NULL;

	// setup the string and enum tables
	InitLookupTables();

	for( int i = 0; i < MAX_SORTERS; i++ )
	{
		m_RecordSorters[ i ] = NULL;
		m_EventSorters[ i ] = NULL;
		m_ConfigRecordSorters[ i ] = NULL;
		m_ConfigEventSorters[ i ] = NULL;
		m_ConfigDataSorters[ i ] = NULL;
		m_AllSorters[ i ] = NULL;
	}

	if ( pSorter )
	{
		// set update to false, cuz there's nothing to update yet
		AddSorter( pSorter, PT_FALSE );
	}

	m_numRecordSorters = m_numEventSorters = m_numConfigRecordSorters = 
					m_numConfigEventSorters = m_numConfigDataSorters = 
					m_numAllSorters = 0;

	m_bTraceDone = false;
	m_bInterruptFlag = false;
	m_numStructTypes = 0;

	m_threadKey = -1;
	m_parentKey = -1;
	m_InstIDKey = -1;
	m_UIDKey = -1;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::RemoveSorter
// Description: removes a sorter from all notifiers based on pointer compare.
// dumb implementation, this function does not have to be that fast, since it
// is rarely called and works on small amounts of data
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_DataStore::RemoveSorter( PT_Sorter *pSorter )
{
	for ( int i = 0; i < MAX_SORTERS; i++ )
	{
		if ( m_RecordSorters[ i ] == pSorter )
		{
			m_RecordSorters[ i ] = NULL;
			
			//compact the rest of the array
			for (unsigned int j = i + 1; j < m_numRecordSorters; j++)
			{
				m_RecordSorters[ j - 1 ] = m_RecordSorters[ j ];
			}
			m_numRecordSorters--;
			
			//last old element becomes null
			m_RecordSorters[ m_numRecordSorters ] = NULL;

		}
		if ( m_EventSorters[ i ] == pSorter )
		{
			m_EventSorters[ i ] = NULL;
			
			//compact the rest of the array
			for (unsigned int j = i + 1; j < m_numEventSorters; j++)
			{
				m_EventSorters[ j - 1 ] = m_EventSorters[ j ];
			}
			m_numEventSorters--;
			
			//last old element becomes null
			m_EventSorters[ m_numEventSorters ] = NULL;
		}
		if ( m_ConfigRecordSorters[ i ] == pSorter )
		{
			m_ConfigRecordSorters[ i ] = NULL;
			
			//compact the rest of the array
			for (unsigned int j = i + 1; j < m_numConfigRecordSorters; j++)
			{
				m_ConfigRecordSorters[ j - 1 ] = m_ConfigRecordSorters[ j ];
			}
			m_numConfigRecordSorters--;
			
			//last old element becomes null
			m_ConfigRecordSorters[ m_numConfigRecordSorters ] = NULL;
		}
		if ( m_ConfigEventSorters[ i ] == pSorter )
		{
			m_ConfigEventSorters[ i ] = NULL;
			
			//compact the rest of the array
			for (unsigned int j = i + 1; j < m_numConfigEventSorters; j++)
			{
				m_ConfigEventSorters[ j - 1 ] = m_ConfigEventSorters[ j ];
			}
			m_numConfigEventSorters--;
			
			//last old element becomes null
			m_ConfigEventSorters[ m_numConfigEventSorters ] = NULL;
		}
		if ( m_ConfigDataSorters[ i ] == pSorter )
		{
			m_ConfigDataSorters[ i ] = NULL;
			
			//compact the rest of the array
			for (unsigned int j = i + 1; j < m_numConfigDataSorters; j++)
			{
				m_ConfigDataSorters[ j - 1 ] = m_ConfigDataSorters[ j ];
			}
			m_numConfigDataSorters--;
			
			//last old element becomes null
			m_ConfigDataSorters[ m_numConfigDataSorters ] = NULL;
		}
		if ( m_AllSorters[ i ] == pSorter )
		{
			m_AllSorters[ i ] = NULL;
			
			//compact the rest of the array
			for (unsigned int j = i + 1; j < m_numAllSorters; j++)
			{
				m_AllSorters[ j - 1 ] = m_AllSorters[ j ];
			}
			m_numAllSorters--;
			
			//last old element becomes null
			m_AllSorters[ m_numAllSorters ] = NULL;
		}
	}

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::AddSorter
// Description: 
// Instead of passing a sorter into the constructor, you 
// can also just add multiple of them here.
// Setting bUpdateAll to TRUE forces the DataStore to run through all 
// records and events thus far, and call the sorters handlers for
// RecordComplete etc...
// AddSorter invokes the protected virtual function UpdateAll, which
// must be implemented by the derived class
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_DataStore::AddSorter( PT_Sorter *pSorter, PT_BOOL bUpdateAll )
{
	// first, get the events that this sorter is registering for
	PT_UCHAR notify = pSorter->RegisterEvents( );

	if ( notify & RECORD_COMPLETE_BIT )
	{
		if ( m_numRecordSorters >= MAX_SORTERS )
		{
			PT_WARNING( "Can't add sorter. Only allowed to add 256 sorters for this notify" );
			return PT_Error_TooManySorters;
		}

		// add notifys on completed records
		m_RecordSorters[ m_numRecordSorters ] = pSorter;
		m_numRecordSorters++;
	}
	if ( notify & EVENT_COMPLETE_BIT )
	{
		if ( m_numEventSorters >= MAX_SORTERS )
		{
			PT_WARNING( "Can't add sorter. Only allowed to add 256 sorters for this notify" );
			return PT_Error_TooManySorters;
		}

		// add notifys on completed records
		m_EventSorters[ m_numEventSorters ] = pSorter;
		m_numEventSorters++;
	}
	if ( notify & RECORD_CONFIG_COMPLETE_BIT )
	{
		if ( m_numConfigRecordSorters >= MAX_SORTERS )
		{
			PT_WARNING( "Can't add sorter. Only allowed to add 256 sorters for this notify" );
			return PT_Error_TooManySorters;
		}

		// add notifys on completed records
		m_ConfigRecordSorters[ m_numConfigRecordSorters ] = pSorter;
		m_numConfigRecordSorters++;
	}
	if ( notify & EVENT_CONFIG_COMPLETE_BIT )
	{
		if ( m_numConfigEventSorters >= MAX_SORTERS )
		{
			PT_WARNING( "Can't add sorter. Only allowed to add 256 sorters for this notify" );
			return PT_Error_TooManySorters;
		}

		// add notifys on completed records
		m_ConfigEventSorters[ m_numConfigEventSorters ] = pSorter;
		m_numConfigEventSorters++;
	}
	if ( notify & DATA_CONFIG_COMPLETE_BIT )
	{
		if ( m_numConfigDataSorters >= MAX_SORTERS )
		{
			PT_WARNING( "Can't add sorter. Only allowed to add 256 sorters for this notify" );
			return PT_Error_TooManySorters;
		}

		// add notifys on completed records
		m_ConfigDataSorters[ m_numConfigDataSorters ] = pSorter;
		m_numConfigDataSorters++;
	}

	//always add to all sorters
	{
		if ( m_numAllSorters >= MAX_SORTERS )
		{
			PT_WARNING( "Can't add sorter. Only allowed to add 256 sorters for this notify" );
			return PT_Error_TooManySorters;
		}

		// add notifys on completed records
		m_AllSorters[ m_numAllSorters ] = pSorter;
		m_numAllSorters++;
	}

	// finally, if this sorter wants to be updated immediately, do so
	if ( bUpdateAll )
	{
		return UpdateAll( pSorter );
	}

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// The following methods notify all registered sorters of any new events
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_DataStore::SendEventComplete(  PT_Event *event )
{
	PT_Error Error = PT_Error_OK;

	for( unsigned int i = 0; i < m_numEventSorters; i++ )
	{
		Error = m_EventSorters[ i ]->OnEventComplete( event );

		if ( Error == PT_Error_NotImpl )
		{
			PT_WARNING("Warning: sorter said it implemented OnEventComplete but it didn't");
		}
	}
	return PT_Error_OK;
}

PT_Error PT_DataStore::SendRecordComplete( PT_Record *record )
{	
	PT_Error Error = PT_Error_OK;

	for( unsigned int i = 0; i < m_numRecordSorters; i++ )
	{
		Error = m_RecordSorters[ i ]->OnRecordComplete( record );
		
		if ( Error == PT_Error_NotImpl )
		{
			PT_WARNING("Warning: sorter said it implemented OnRecordComplete but it didn't");
		}
	}
	return PT_Error_OK;
}

PT_Error PT_DataStore::SendConfigRecordComplete( PT_Config_Record *configR )
{
	PT_Error Error = PT_Error_OK;

	for( unsigned int i = 0; i < m_numConfigRecordSorters; i++ )
	{
		Error = m_ConfigRecordSorters[ i ]->OnRecordConfigComplete( configR );

		if ( Error == PT_Error_NotImpl )
		{
			PT_WARNING("Warning: sorter said it implemented OnRecordConfigComplete but it didn't");
		}	
	}
	return PT_Error_OK;
}

PT_Error PT_DataStore::SendConfigEventComplete( PT_Config_Event *configE )
{
	PT_Error Error = PT_Error_OK;

	for( unsigned int i = 0; i < m_numConfigEventSorters; i++ )
	{
		Error = m_ConfigEventSorters[ i ]->OnEventConfigComplete( configE );

		if ( Error == PT_Error_NotImpl )
		{
			PT_WARNING("Warning: sorter said it implemented OnEventConfigComplete but it didn't");
		}	
	}
	return PT_Error_OK;
}

PT_Error PT_DataStore::SendConfigDataComplete( PT_Config_Data *configD )
{
	PT_Error Error = PT_Error_OK;

	for( unsigned int i = 0; i < m_numConfigDataSorters; i++ )
	{
		Error = m_ConfigDataSorters[ i ]->OnDataConfigComplete( configD );

		if ( Error == PT_Error_NotImpl )
		{
			PT_WARNING("Warning: sorter said it implemented OnDataConfigComplete but it didn't");
		}	
	}
	return PT_Error_OK;
}

PT_Error PT_DataStore::SendEndOfTrace( )
{
	PT_Error Error = PT_Error_OK;

	for( unsigned int i = 0; i < m_numAllSorters; i++ )
	{
		Error = m_AllSorters[ i ]->OnTraceComplete();
	}
	return Error;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::UpdateAll
// Description: Handles UpdateAll for Configs, but not records or events
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_DataStore::UpdateAll( PT_Sorter *pSorter )
{
	PT_Config *current;

	bool bGetConfigRecord;
	bool bGetConfigEvent;
	bool bGetConfigData;

	// figure out which notifications we will need to do some work for
	PT_UCHAR notify = pSorter->RegisterEvents( );
	bGetConfigRecord = ((RECORD_CONFIG_COMPLETE_BIT & notify) != 0 );
	bGetConfigEvent = ((EVENT_CONFIG_COMPLETE_BIT & notify) != 0 );
	bGetConfigData = ((DATA_CONFIG_COMPLETE_BIT & notify) != 0 );

	// quick out if nothing to do
	if ( ( !bGetConfigRecord ) && ( !bGetConfigEvent ) && ( !bGetConfigData ) )
	{
		return PT_Error_OK;
	}

	// do an STL iterate through the hash map, and send out notifications
	for ( KeyMap::iterator it = m_KeyTable->begin();
		  it != m_KeyTable->end(); ++it )
	{
		  current = (*it).second;
		  
		  // now figure out which type of config it is, cast it, and send it out for the 
		  if ( bGetConfigRecord && current->GetType( ) == Pipe_Config_Record )
		  {
			  pSorter->OnRecordConfigComplete( (PT_Config_Record *)current );
		  }
		  else if ( bGetConfigEvent && current->GetType( ) == Pipe_Config_Event )
		  {
			  pSorter->OnEventConfigComplete( (PT_Config_Event *)current );
		  }
		  else if ( bGetConfigData && current->GetType( ) == Pipe_Config_Data )
		  {
			  pSorter->OnDataConfigComplete( (PT_Config_Data *)current );
		  }
	}

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::InitLookupTables
// Description: initialize the tables with nice start sizes
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_DataStore::InitLookupTables()
{
	PT_ASSERT( m_EnumTable == NULL );
	PT_ASSERT( m_StringTable == NULL );

	m_EnumTable = new char **[ DEFAULT_TABLE_SIZE ];
	if ( m_EnumTable == NULL )
	{
		PT_WARNING( "Could alloc lookup tables! Out of memory" );
		return PT_Error_OutOfMemory;
	}

	m_StringTable = new char *[ DEFAULT_TABLE_SIZE ];
	if ( m_StringTable == NULL )
	{
		PT_WARNING( "Could alloc lookup tables! Out of memory" );
		return PT_Error_OutOfMemory;
	}

	// now null everything out
	for ( int i = 0; i < DEFAULT_TABLE_SIZE; i++ )
	{
		m_StringTable[ i ] = NULL;
		m_EnumTable[ i ] = NULL;
	}

	m_enumNumElements = 0;
	m_stringNumElements = 0;

	m_enumRealSize = DEFAULT_TABLE_SIZE;
	m_stringRealSize = DEFAULT_TABLE_SIZE;

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::AddToEnumTable
// Description: copy input enum table to a newly allocated buffer, then assign ptr
//				to the in/out param
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_DataStore::AddToEnumTable( char ***enumArray, PT_ULONG arrayLen )
{
	size_t size = arrayLen;

	if ( m_enumNumElements == m_enumRealSize )
	{
		//time to grow the array
		PT_ULONG newRealSize = m_enumRealSize * 2;
		char ***newArray = new char **[ newRealSize ];

		if ( newArray == NULL )
		{
			PT_WARNING( "Couldn't grow lookup tables! Out of memory" );
			return PT_Error_OutOfMemory;
		}

		//copy data over
		for ( unsigned int i = 0; i < newRealSize; i++ )
		{
			if ( i < m_enumNumElements )
			{
				newArray[ i ] = m_EnumTable[ i ];
			}
			else 
			{
				newArray[ i ] = NULL;
			}
		}

		//great now we're all grown up 
		m_enumRealSize = newRealSize;
		
		char ***old = m_EnumTable;
		m_EnumTable = newArray;
		delete [ ]old;
	}

	// create a place for our string
	m_EnumTable[ m_enumNumElements ] = new char *[ size ];

	// now copy it over
	for( unsigned int j = 0 ; j < size; j++ )
	{
		m_EnumTable[ m_enumNumElements ][ j ] = (*enumArray)[ j ];
	}
	//memcpy( m_EnumTable[ m_enumNumElements ], *enumArray, size );
	*enumArray = m_EnumTable[ m_enumNumElements ];

	m_enumNumElements++;

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::AddToStringTable
// Description: copy input string to a newly allocated buffer, then assign ptr
//				to the in/out param
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_DataStore::AddToStringTable( const char **szVal )
{
	size_t size = strlen( *szVal ) + 1;

	if ( m_stringNumElements == m_stringRealSize )
	{
		//time to grow the array
		PT_ULONG newRealSize = m_stringRealSize * 2;
		char **newArray = new char *[ newRealSize ];

		if ( newArray == NULL )
		{
			PT_WARNING( "Couldn't grow lookup tables! Out of memory" );
			return PT_Error_OutOfMemory;
		}

		//copy data over
		for ( unsigned int i = 0; i < newRealSize; i++ )
		{
			if ( i < m_stringNumElements )
			{
				newArray[ i ] = m_StringTable[ i ];
			}
			else 
			{
				newArray[ i ] = NULL;
			}
		}

		//great now we're all grown up 
		m_stringRealSize = newRealSize;
		
		char **old = m_StringTable;
		m_StringTable = newArray;
		delete [ ]old;

	}

	// create a place for our string
	m_StringTable[ m_stringNumElements ] = new char[ size ];

	// now copy it over
	strcpy( m_StringTable[ m_stringNumElements ], *szVal );
	*szVal = m_StringTable[ m_stringNumElements ];

	m_stringNumElements++;

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::FreeStringTable
// Description: de-alloc a lot of lookup memory
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_DataStore::FreeStringTable()
{
	PT_ASSERT ( m_StringTable != NULL );

	// first, release all the strings
	for( unsigned int i = 0; i < m_stringNumElements; i++ )
	{
		if ( m_StringTable[ i ] )
			delete m_StringTable[ i ];
		m_StringTable[ i ] = NULL;
	}

	// now, release the table
	delete [] m_StringTable;

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::FreeEnumTable
// Description: de-alloc a lot of lookup memory
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_DataStore::FreeEnumTable()
{
	PT_ASSERT ( m_EnumTable != NULL );

	for (unsigned int i = 0; i < m_enumNumElements; i++ )
	{
		if (m_EnumTable[i])
			delete m_EnumTable[i];
		m_EnumTable[i] = NULL;
	}

	// now, release the table
	delete [] m_EnumTable;

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::~PT_DataStore
// Description: virtual Destructor
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_DataStore::~PT_DataStore(  ) 
{
	// perform a delete on all statements added to the hashtables
	for ( KeyMap::iterator it = m_KeyTable->begin();
		  it != m_KeyTable->end(); ++it )
		  {
			//if ((*it).next != NULL)
				delete ((*it).second);
			//delete ((*it).first);
		  }

	
	if (m_StructTable != NULL)
	{
		for ( StructMap::iterator it2 = m_StructTable->begin();
			  it2 != m_StructTable->end(); ++it2 )
			  {
				//if ((*it).next != NULL)
					delete ((*it2).second);
				//delete ((*it).first);
			  }
		delete m_StructTable;
	}

	FreeStringTable();
	FreeEnumTable();

	delete m_KeyTable;
	delete m_NameTable;
}

PT_StructArray::PT_StructArray( )
{
	m_array = NULL;
	m_arrayLen = 0;
}

PT_StructArray::~PT_StructArray( )
{
	if (m_arrayLen != 0 && m_array != NULL)
	{

		// auto delete the PT_Structures inside of us!!
		for( PT_UINT i = 0; i < m_arrayLen; i++ )
		{
			if ( m_array[ i ] != NULL )
			{
				delete m_array[ i ];
				m_array[ i ] = NULL;
			}
		}

		delete []m_array;
	}
}

// No implementation in base class
PT_Error PT_DataStore::OnOpenStatement( const PT_Open_Statement &st )
{
	return PT_Error_OK;
}

// No implementation in base class
PT_Error PT_DataStore::OnCloseStatement( const PT_Close_Statement &st )
{
	return PT_Error_OK;
}

// No implementation in base class
PT_Error PT_DataStore::OnCommentStatement( const PT_Comment_Statement &st )
{
	return PT_Error_OK;
}

// No implementation in base class
PT_Error PT_DataStore::OnEventStatement( const PT_Event_Statement &st )
{
	return PT_Error_OK;
}

// No implementation in base class
PT_Error PT_DataStore::OnUpdateStruct( const PT_Update_Struct &st )
{
	return PT_Error_OK;
}

// No implementation in base class
PT_Error PT_DataStore::OnClearStruct( const PT_Clear_Struct &st )
{
	return PT_Error_OK;
}

// No implementation in base class
PT_Error PT_DataStore::OnDataStatement( const PT_Data_Statement &st )
{
	return PT_Error_OK;
}

// No implementation in base class
PT_Error PT_DataStore::OnEventDataStatement( const PT_EventData_Statement &st )
{
	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::OnConfigRecord
// Description: 
// Parameters: 
// Returns: PT_Error_HashExists if already found
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_DataStore::OnConfigRecord( const PT_Config_Record &st )
{
	// If name already exists, then this definition is invalid!
	if ( m_NameTable->count( st.m_name ) != 0 )
	{
		PT_WARNING("Definition already exists in hash table!!!");
		return PT_Error_HashExists;
	}

	// OK, add to hash table
	PT_Config_Record *config = new PT_Config_Record( st );

	// also, replicate string values in the string table
	AddToStringTable( (& ( config->m_name )) );
	AddToStringTable( (& ( config->m_description )) );
	
	(*m_KeyTable)[ config->m_key ] = config;
	(*m_NameTable)[ config->m_name ] = config;

	// and finally, notify all sorters that are registered for ConfigRecords
	SendConfigRecordComplete( config );

	return PT_Error_OK;
}


PT_UINT PT_DataStore::GetStructTypeNames( const char **outNames )
{
	PT_UINT currentIndex = 0;
	PT_StructArray *current;

	// do an STL iterate through the hash map, and send out notifications
	for ( StructMap::iterator it = m_StructTable->begin();
		  it != m_StructTable->end(); ++it )
	{
		current = (*it).second;
		PT_Config_Struct *config = current->m_array[ 0]->GetStructureDefinition( );
		outNames[ currentIndex++ ] = config->m_name;
	}


	return currentIndex;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::OnConfigData
// Description: 
// Parameters: 
// Returns: PT_Error_HashExists if already found
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_DataStore::OnConfigData( const PT_Config_Data &st )
{
	// If name already exists, then this definition is invalid!
	if ( ( m_NameTable->count( st.m_name )) != 0 )
	{
		PT_WARNING("Definition already exists in hash table!!!");
		return PT_Error_HashExists;
	}

	// OK, add to hash table

PT_Config_Data *config;
	config = new PT_Config_Data( st );
		
	// also, replicate string values in the string table
	AddToStringTable( & ( config->m_name ) );
	AddToStringTable( & ( config->m_description ) );
	
	// if its an enum, we also have to copy enum vals
	if ( config->m_datatype == Pipe_Enumeration )
	{
		char **enumArray = config->m_enumArray;

		AddToEnumTable( &enumArray, config->m_arrayLen );

		for( unsigned int i = 0; i < config->m_arrayLen; i++ )
		{
            char *temp = (char *)(enumArray[i]/*config->m_enumArray[i]*/);
            const char **temp2 = (const char **)(&temp);

			AddToStringTable( temp2 );

			enumArray[i]/*config->m_enumArray[i]*/ = (char *)(*temp2);
		}
				
		config->m_enumArray = enumArray;
/*		
		for( PT_UINT i = 0; i < config->m_arrayLen; i++ )
		{
			AddToStringTable( & ( config->m_enumArray[i] ) );
		}
		AddToEnumTable( &(config->m_enumArray), config->m_arrayLen );
*/		
	}

	(*m_KeyTable)[ config->m_key ] = config;
	(*m_NameTable)[ config->m_name ] = config;

	// and finally, notify all sorters that are registered for ConfigData
	SendConfigDataComplete( config );

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::OnConfigEvent
// Description: 
// Parameters: 
// Returns: PT_Error_HashExists if already found
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_DataStore::OnConfigEvent( const PT_Config_Event &st )
{

	// If name already exists, then this definition is invalid!
	if ( m_NameTable->count( st.m_event_name ) != 0 )
	{
		PT_WARNING("Definition already exists in hash table!!!");
		return PT_Error_HashExists;
	}

	// OK, add to hash table
	PT_Config_Event *config = new PT_Config_Event( st );

	// also, replicate string values in the string table
	AddToStringTable( & ( config->m_event_name ) );
	AddToStringTable( & ( config->m_description ) );
	
	(*m_KeyTable)[ config->m_key ] = config;
	(*m_NameTable)[ config->m_event_name ] = config;

	// and finally, notify all sorters that are registered for ConfigEvent
	SendConfigEventComplete( config );

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::OnConfigStruct
// Description: 
// Parameters: 
// Returns: PT_Error_HashExists if already found
//			PT_Error_ConfigNotFound if one of the column configs is not an existing
//			ConfigData
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_DataStore::OnConfigStruct( const PT_Config_Struct &st )
{
	// If name already exists, then this definition is invalid!
	if ( m_NameTable->count( st.m_name ) != 0 )
	{
		PT_WARNING("Definition already exists in hash table!!!");
		return PT_Error_HashExists;
	}

	// OK, add to hash table
	PT_Config_Struct *config = new PT_Config_Struct( st );

	// also, replicate string values in the string table
	AddToStringTable( & config->m_name );
	AddToStringTable( & config->m_description );
	
	PT_Config_Data **enumArray = config->m_colArray;

	// need to store this somewhere, so reuse the EnumTable!

	AddToEnumTable( (char ***)(&enumArray), config->m_numCols);// * (sizeof (PT_Config_Data **)));
	config->m_colArray = enumArray;
	
	(*m_KeyTable)[ config->m_key ] = config;
	(*m_NameTable)[ config->m_name ] = config;

	//SendConfigEventComplete( config );

	if ( m_bDoWatch )
	{
		// alloc watch window PT_Structures for this structure!
		PT_StructArray *newStructArray = new PT_StructArray;
		newStructArray->m_array = new PT_Structure *[ st.m_numInstances ];
		newStructArray->m_arrayLen = st.m_numInstances;

		for( PT_UINT i = 0; i < st.m_numInstances; i++ )
		{
			newStructArray->m_array[ i ] = new PT_Structure( config );
		}

		(*m_StructTable)[ config->m_key ] = newStructArray;
	}

	m_numStructTypes++;

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::OnEndOfTrace
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_DataStore::OnEndOfTrace( )
{
	m_bTraceDone = true;

	// and finally, notify all sorters
	SendEndOfTrace( );

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::OnEndOfTrace
// Description: force close all open records
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_StandardDataStore::OnEndOfTrace( )
{
//cout<<"Small = "<< m_numSmallEvents<<endl;
//cout<<"Large = "<< m_numLargeEvents<<endl;
//cout<<"Cached = "<< m_numCachedTypes<<endl;
//cout<<"Non = "<< m_numNonCachedTypes<<endl;

//debug
/*for( int t = 0; t < 4096; t++ )
{
	if ( m_dataKeyTableSize[ t ] != 0 )
	{
		cout<<t<<": "<<" size = " << m_dataKeyTableSize[ t ] << " vals = ";
		for( int t2= 0; t2 < m_dataKeyTableSize[ t ]; t2++ )
		{
			PT_Config_Data *cfg; 
			this->FindConfigByKey( m_dataKeyTable[ t ][ t2 ], (PT_Config **)(&cfg) );
			
			cout<< " " << cfg->m_name; 
		}
		cout<<endl;
	}
}
*/
	// force close all open records
	for( unsigned int i = 1; i < ( m_numAllocRecords + 1); i++ )
	{
		// make sure the record is closed
		PT_Record *record = GetRecordByID( i );
		
		if ( !record->GetBoolVal( RECORD_CLOSED_BIT ) )
		{
			// it wasn't closed, but now it is!
			ForceClose( record );
		}
	}

	return PT_DataStore::OnEndOfTrace( );

}

void PT_StandardDataStore::ForceClose( PT_Record *record )
{
	// lock this record, just in case
	record->SetBoolVal( RECORD_LOCKED_BIT, true );

	// now convert the PT_RecordDataList struct to a PT_DataList!
	PT_RecordDataList *linkedList = record->GetRecordDataList( );

	// it could be null if no data!
	if ( linkedList != NULL )
	{
		PT_DataList *pDataList = ( PT_DataList * )AllocData( PT_DataList::GetDataByteSize( linkedList->NumElements( ) ) );

		if ( pDataList == NULL )
		{
			PT_WARNING("Ran out of memory!");
			record->SetBoolVal( RECORD_LOCKED_BIT, false );
			return;
		}

		pDataList->Init( linkedList->NumElements( ), this );

		// copy data over
		linkedList->FillDataList( pDataList, this );

		// and finally, destroy the list. the destructor will recursively 
		// destroy all nodes in the list
		delete linkedList;

		record->SetDataList( pDataList );
	}

	record->SetBoolVal( RECORD_LOCKED_BIT, false );

	// Close this thing, once and for all!
	record->SetBoolVal( RECORD_CLOSED_BIT, true );

	// change the end time of this record to be current time, not the time of last event
//	record->m_maxTime = m_maxTime;

	// Update all sorters of this recent development
	SendRecordComplete( record );

	//return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::FindConfigByName
// Description: 
// Useful method for definition lookup
// Returns PT_Error_HashNotFound if the hash value doesn't exist
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_DataStore::FindConfigByName( const char *NameKey, PT_Config **outConfig )
{
	PT_Config *retVal = (*m_NameTable)[ NameKey ];
	if ( (!retVal) || retVal->GetType( ) == Pipe_Invalid_Type )
	{
		// hash wasn't found, so STL returned data_type() == PT_Config instance
		return PT_Error_HashNotFound;
	}

	*outConfig = retVal;
	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::FindStructArrayByKey
// Description: 
// Useful method for struct array lookup
// Returns PT_Error_HashNotFound if the hash value doesn't exist
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_DataStore::FindStructArrayByKey( const PT_KEY ValueKey, PT_StructArray **outArray )
{
	PT_StructArray *retVal = (*m_StructTable)[ ValueKey ];
	if ( (!retVal) || retVal->m_array == NULL )
	{
		// hash wasn't found, so STL returned data_type() == PT_Config instance
		return PT_Error_HashNotFound;
	}

	*outArray = retVal;
	return PT_Error_OK;
}
	
////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataStore::FindConfigByKey
// Description: 
// Useful method for definition lookup
// Returns PT_Error_NotFound if the hash value doesn't exist
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_DataStore::FindConfigByKey( const PT_KEY ValueKey, PT_Config **outConfig )
{
	PT_Config *retVal = (*m_KeyTable)[ ValueKey ];
	if ( (!retVal) || retVal->GetType( ) == Pipe_Invalid_Type )
	{
		// hash wasn't found, so STL returned data_type() == PT_Config instance
		return PT_Error_HashNotFound;
	}

	*outConfig = retVal;
	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::PT_StandardDataStore
// Description: Main constructor for PT_StandardDataStore, pass a 
// sorter you want to do work. Invokes InitDataStructures( ) to set
// up the mem alloc tables
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_StandardDataStore::PT_StandardDataStore( PT_Sorter  *pSorter,
											  PT_Error *Error ) : PT_DataStore( pSorter ) 
{
	PT_Error tempErr;

	if ( Error == NULL )
	{
		Error = &tempErr;
	}

	// counters for footprint analysis
	m_numSmallEvents = 0;
	m_numLargeEvents = 0;

	m_numNonCachedTypes = 0;
	m_numCachedTypes = 0;


	*Error = PT_Error_OK;
	m_CurrentEvent = NULL;
	m_veryFirstEvent = NULL;
	m_minTime = 0;
	m_maxTime = (PT_Time)-1;//0; // mgd changed this from -1 crossing my fingers
	TimeDictionary = NULL;

	for( int i = 0; i < NUM_MEM_TABLES; i++ )
	{
		DataTable[ i ] = NULL;
		EventTable[ i ] = NULL;
		RecordTable[ i ] = NULL;

		EventIndexTable[ i ] = 0;
	}


	for( int i2 = 0; i2 < 8192; i2++ )
	{
		for( int j2 = 0; j2 < STRING_REP_TO_SEARCH; j2++ )
		{
			m_StringCompressionTable[ i2 ][ j2 ] = NULL; 
		}
	}

	//PT_USHORT *m_dataKeyTable[ 4096 ];
	for( int i3 = 0; i3 < 8192; i3++ )
	{
		m_dataKeyTable[ i3 ] = NULL;
		m_dataKeyTableSize[ i3 ] = NULL;
	}

	m_currentRecordPage = m_currentEventPage = m_currentDataPage = 0;
	m_currentEventPageSize = m_currentDataPageSize = 0;
	m_numAllocRecords = 0;
	m_bIsEvent = true;

	m_pCurrentStruct = NULL;
	m_CurrentStructTime = 0;
	m_CurrentStructRow = 0;

//	m_lastEventTime = 0;
	m_pCurrentEvent	= NULL;

	// Let someone else do the initilization work
	*Error = InitDataStructures( );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::~PT_StandardDataStore
// Description: Calls FreeDataStructures to do all the cleanup
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_StandardDataStore::~PT_StandardDataStore( )
{
	// Let someone else do the destruction work
	FreeDataStructures( );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::InitDataStructures
// Description: Initializes the memory allocation data structures. Note
// lots of member vars keeping track of stuff. need to be very careful setting
// initial values and updating
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_StandardDataStore::InitDataStructures( )
{
	// allocate the timetable
	TimeDictionary = new PT_Event *[ NUM_CACHED_TIMES ];

	if ( TimeDictionary == NULL ) 
	{
		PT_WARNING( "Out of Memory!: Couldn't Allocate a new TimeTable" );
		FreeDataStructures( );
		return PT_Error_OutOfMemory;
	}
	for( int i = 0; i < NUM_CACHED_TIMES; i++ )
	{
		TimeDictionary[ i ] = NULL;
	}

	// and now, the data tables
	PT_ASSERT( DataTable[ 0 ] == NULL ); 
	PT_ASSERT( RecordTable[ 0 ] == NULL ); 
	PT_ASSERT( EventTable[ 0 ] == NULL ); 

	// 
	DataTable[ 0 ] = new char[ DATA_TABLE_PAGE_SIZE ];
	m_currentDataPage = 0;
	m_currentDataPageSize = 0;

	if ( DataTable[ 0 ] == NULL ) 
	{
		PT_WARNING( "Out of Memory!: Couldn't Allocate a new Data Table page" );
		FreeDataStructures( );
		return PT_Error_OutOfMemory;
	}

	// alloc of record table is fixed size multiple of sizeof(PT_Record)
	RecordTable[ 0 ] = new PT_Record[ RECORD_TABLE_PAGE_SIZE ];
	m_currentRecordPage = 0;
	m_numAllocRecords = 0;

	if ( RecordTable[ 0 ] == NULL )
	{
		PT_WARNING( "Out of Memory!: Couldn't Allocate a new Record Table page" );
		FreeDataStructures( );
		return PT_Error_OutOfMemory;
	}

	EventTable[ 0 ] = new char[ EVENT_TABLE_PAGE_SIZE ];
	m_currentEventPage = 0;
	m_currentEventPageSize = 0;
	
	if ( EventTable[ 0 ] == NULL )
	{
		PT_WARNING( "Out of Memory!: Couldn't Allocate a new Event Table page" );
		FreeDataStructures( );
		return PT_Error_OutOfMemory;
	}

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::FreeDataStructures
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_StandardDataStore::FreeDataStructures( )
{
	if ( TimeDictionary != NULL )
	{
		delete TimeDictionary;
	}

	// now, we have to iterate through all records, and if we find open ones,
	// we have to delete their DataNodes and PT_RecordLists since these were
	// dynamically allocated
	for( unsigned int j = 1; j <= m_numAllocRecords; j++ )
	{
		PT_Record *record = GetRecordByID( j );
		if ( (record->GetBoolVal( RECORD_CLOSED_BIT )) == false )
		{
			// still open
			PT_RecordDataList *pRecordDataList = record->GetRecordDataList( );
			
			if ( pRecordDataList )
			{
				delete pRecordDataList;
				pRecordDataList = NULL;
			}
		}
	}

	// free up lots and lots of memory for all table pages that are being used
	for( int i = 0; i < NUM_MEM_TABLES; i++ )
	{
		if ( DataTable[ i ] != NULL )
		{
			delete [] (DataTable[ i ]); 
			DataTable[ i ] = NULL;
		}
		if ( EventTable[ i ] != NULL )
		{
			delete [] (EventTable[ i ]); 
			EventTable[ i ] = NULL;
		}
		if ( RecordTable[ i ] != NULL )
		{
			delete [] (RecordTable[ i ]); 
			RecordTable[ i ] = NULL;
		}
	}

	for( int i3 = 0; i3 < 8192; i3++ )
	{
		if ( m_dataKeyTable[ i3 ] != NULL )
		{
			delete [ ](m_dataKeyTable[ i3 ]);
			m_dataKeyTable[ i3 ] = NULL;
		}
	}

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::AllocNewRecord
// Description: allocation method for records. doesn't call new or malloc, just
// gives us the next available chunk in the record table. records are ligned
// up sequentially by ID
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Record *
PT_StandardDataStore::AllocNewRecord( )
{
	if ( ((m_numAllocRecords % RECORD_TABLE_PAGE_SIZE) == 0) && (m_numAllocRecords != 0) )
	{
		// we ran out of space in our current record page. time to 
		// allocate a new one!
		m_currentRecordPage++;
		
		// if we fail this, we've allocated WAY too much stuff
		PT_ASSERT( m_currentRecordPage < NUM_MEM_TABLES );

		RecordTable[ m_currentRecordPage ] = new PT_Record[ RECORD_TABLE_PAGE_SIZE ]; 
		
		if ( RecordTable[ m_currentRecordPage ] == NULL )
		{
			PT_WARNING( "Out of Memory!: Couldn't Allocate a new Record Table page" );
			return NULL;
		}

		// increment our total count
		m_numAllocRecords++;

		// return base address of new page
		return RecordTable[ m_currentRecordPage ];
	}
	else
	{
		// unlike other case, first increment count to get address offset
		PT_Record *ret = &( RecordTable[ m_currentRecordPage ][ m_numAllocRecords % RECORD_TABLE_PAGE_SIZE ] );
		m_numAllocRecords++;
		return ret;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::AllocNewEvent
// Description: allocation method for events. doesn't call new or malloc, just
// gives us the next available chunk in the event table. Event are ligned
// up sequentially by time, HOWEVER, PT_Events are essentially variable size
// since we may have an additional pointer on the end, so we have to make our 
// array out of char instead of PT_Event
//
// Parameters:  
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Event *
PT_StandardDataStore::AllocNewEvent( bool AddDataPtr, bool isSmallEvent )
{
	// if AddDataPtr then PT_Event + 1 ptr
	PT_UCHAR newSize;
		
	if (isSmallEvent)
	{
		newSize = ( AddDataPtr ) ? (PT_UCHAR)(sizeof( PT_Event ) + sizeof( PT_SmallEvent) + sizeof( PT_DataList *)) : (PT_UCHAR)(sizeof( PT_Event ) + sizeof( PT_SmallEvent) );
		m_numSmallEvents++;
	}
	else
	{
		newSize = ( AddDataPtr ) ? (PT_UCHAR)(sizeof( PT_Event ) + sizeof( PT_LargeEvent) + sizeof( PT_DataList *)) : (PT_UCHAR)(sizeof( PT_Event ) + sizeof( PT_LargeEvent) );
		m_numLargeEvents++;
	}

	//if (AddDataPtr) m_numSmallEvents++; else m_numLargeEvents++;

	// check if we hit a page boudary in our table
	if ( (m_currentEventPageSize + newSize + 30) >= EVENT_TABLE_PAGE_SIZE )
	{
		// add a page

		m_currentEventPage++;
		m_currentEventPageSize = 0;

		// if we fail this, we've allocated WAY too much stuff
		PT_ASSERT( m_currentEventPage < NUM_MEM_TABLES );

		EventTable[ m_currentEventPage ] = new char[ EVENT_TABLE_PAGE_SIZE ]; 

		if ( EventTable[ m_currentEventPage ] == NULL )
		{
			PT_WARNING( "Out of Memory!: Couldn't Allocate a new Event Table page" );
			return NULL;
		}

		// save the max index found in this page to make it easy for
		// finding events by index
		EventIndexTable[ m_currentEventPage ] = EventIndexTable[ m_currentEventPage - 1 ] + 1;

		m_currentEventPageSize = newSize;
		return (PT_Event *)( EventTable[ m_currentEventPage ] );
	}
	else
	{
		// return next slot in current page, and then add to pagesize
		char *retVal = &( EventTable[ m_currentEventPage ][ m_currentEventPageSize ] );
		m_currentEventPageSize += newSize;

		// save the max index found in this page to make it easy for
		// finding events by index
		EventIndexTable[ m_currentEventPage ]++;

		return (PT_Event *) retVal;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::AllocData
// Description: Allocation method for data. Doesn't call new or malloc, just
// gives us the next available chunk in the data table. 
//
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
char *
PT_StandardDataStore::AllocData( PT_ULONG dataSize ) 
{
	// quick sanity check
	if ( dataSize >= DATA_TABLE_PAGE_SIZE )
	{
		PT_WARNING( "Are you kidding? DataSize alloc request is bigger than an entire page!")
		return NULL;
	}

	// check if we're about to hit a page boudary in our table
	if ( (m_currentDataPageSize + dataSize) >= DATA_TABLE_PAGE_SIZE )
	{
		// add a page

		m_currentDataPage++;

		// if we fail this, we've allocated WAY too much stuff
		if ( m_currentDataPage >= NUM_MEM_TABLES )
		{
			PT_FATAL("Alloced too many Data pages!!!");
		}

		DataTable[ m_currentDataPage ] = new char[ DATA_TABLE_PAGE_SIZE ]; 

		if ( DataTable[ m_currentDataPage ] == NULL )
		{
			PT_WARNING( "Out of Memory!: Couldn't Allocate a new Data Table page" );
			return NULL;
		}

		m_currentDataPageSize = dataSize;
		return DataTable[ m_currentDataPage ];
	}
	else
	{
		// return next slot in current page, and then add to pagesize
		char *retVal = &( DataTable[ m_currentDataPage ][ m_currentDataPageSize ] );
		m_currentDataPageSize += dataSize;

		return retVal;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::GetRecordByID
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Record *PT_StandardDataStore::GetRecordByID( PT_ID ID )
{	
	// make sure its a valid index
	// remmeber, ID starts at 1
	if ( ID > m_numAllocRecords ) return NULL;

	// TRICKY! record ID 1 is stored in RecordTable location 0!
	ID--;
	
	// ok, its valid
	PT_ULONG pageNum = ID / RECORD_TABLE_PAGE_SIZE;
	PT_ULONG indexNum = ID % RECORD_TABLE_PAGE_SIZE;

	return &(RecordTable[ pageNum ][ indexNum ]);
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::OnOpenStatement
// Description: Handler for PT_Open_Statement. 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_StandardDataStore::OnOpenStatement( const PT_Open_Statement &st )
{
	// remember, ID 1 goes in loc 0 so do st.m_ID - 1
	if ( ( st.m_ID - 1 )!= m_numAllocRecords )
	{
		PT_WARNING( "Error: Open Statement must have next sequential record ID" );
		return PT_Error_BadStatement;
	}
	
	PT_Record *record = AllocNewRecord( );

	// set up this record with nice initial values
	if (record != NULL)
	{
		record->Init( &st );
		return PT_Error_OK;
	}
	else
	{
		PT_WARNING( "Error: Ran out of memory for record alloc!" );
		return PT_Error_OutOfMemory;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::OnUpdateStruct
// Description: update a cell in a watch window, more details to follow
// must communicate well with events, since both use EventData
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_StandardDataStore::OnUpdateStruct( const PT_Update_Struct &st )
{
	// get ahold of this struct
	PT_StructArray *structArray = (*m_StructTable)[ st.m_struct->m_key ];

	if (structArray == NULL) return PT_Error_HashNotFound;
	if (st.m_instanceNum >= structArray->m_arrayLen) return PT_Error_IndexOutOfBounds;

	PT_Structure * theStruct = structArray->m_array[ st.m_instanceNum ];

	if ( st.m_num_values == 0 )
	{
		// just update the owner
		return theStruct->UpdateRowOwner( st.m_row, st.m_ID, st.m_time );
	}
	else
	{
		m_pCurrentStruct	= theStruct;
		m_CurrentStructTime = st.m_time;
		m_CurrentStructRow = st.m_row;

		m_EventDataLeft = st.m_num_values;

		// also, let the EventData know that it is working with a struct, not an event!
		m_bIsEvent = false;
	}

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::OnClearStruct
// Description: clear a cell or a row in a watch window, more details to follow
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_StandardDataStore::OnClearStruct( const PT_Clear_Struct &st )
{
	// get ahold of this struct
	PT_StructArray *structArray = (*m_StructTable)[ st.m_struct->m_key ];

	if (structArray == NULL) return PT_Error_HashNotFound;
	if (st.m_instanceNum >= structArray->m_arrayLen) return PT_Error_IndexOutOfBounds;

	PT_Structure * theStruct = structArray->m_array[ st.m_instanceNum ];

	PT_Error Error;

	if ( st.m_numCols == 0 || st.m_colList == NULL )
	{
		// clear an entire row
		return theStruct->ClearRow( st.m_row, st.m_time );
	}
	else
	{
		// for each cell entry in colList, call ClearCell
		for( PT_UINT i = 0; i < st.m_numCols; i++ )
		{
			Error = theStruct->ClearCell( st.m_colList[ i ], st.m_row, st.m_time );
			if (PT_FAILED( Error ))
			{
				return Error;
			}
		}
	}

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::OnEventStatement
// Description: Handler for PT_Event_Statement
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_StandardDataStore::OnEventStatement( const PT_Event_Statement &st )
{
	PT_Event *event = NULL;	
	PT_Event *first = NULL;

	if (st.m_time < m_minTime || (m_numSmallEvents + m_numLargeEvents == 0)) m_minTime = st.m_time;
	if ( (m_maxTime == ((PT_Time)-1) ) || ((st.m_time + st.m_duration - 1) > m_maxTime ) )
		m_maxTime = st.m_time + st.m_duration - 1;

	PT_Record *rec = GetRecordByID( st.m_ID );
	bool isLowest = (( first = rec->GetFirstEvent( )) == NULL || first->GetStartTime( rec ) > st.m_time );
	bool isSmallEvent = PT_SmallEvent::IsWorthyOfSmallEvent( &st, rec, isLowest );

	if ( st.m_num_values == 0 )
	{
		// no data attached to event, so no need to alloc an extra pointer
		event = AllocNewEvent( false, isSmallEvent );
	}
	else
	{
		// need to alloc sizeof(PT_Event + 4) for extra data pointer
		event = AllocNewEvent( true,  isSmallEvent );

		// also, let the EventData know that it is working with an event, not a struct!
		m_bIsEvent = true;	
	}

	if ( event != NULL )
	{
		// set up this record with nice initial values
		event->Init( rec,&st, isSmallEvent );

		// add it to the record!
		PT_Record *record = GetRecordByID( st.m_ID );
		
		if ( record == NULL )
		{
			PT_WARNING( "Invalid record ID on current event!" )
			return PT_Error_BadStatement;
		}

		record->AddEvent( event, &st, this );

//		m_lastEventTime = st.m_time;

		if ( st.m_num_values != 0 )
		{
			// for adding event data
			m_pCurrentEvent = event;
			m_EventDataLeft = st.m_num_values;

			// set up data event
			// for now, our datalist pointer is just a temp buffer, but once we get
			// all the data, we will do the actual alloc and memcopy... why?
			// because when we have all the data, we will know whether this event
			// uses the same data keys as every other version, so we don't have to waste 
			// bytes storing data type info
			PT_DataList *seq = (PT_DataList *)(&TempDataList);
			//(PT_DataList *) AllocData( PT_DataList::GetDataByteSize( st.m_num_values ) );
			/*
			if ( seq == NULL )
			{
				PT_WARNING( "Error: Ran out of memory for alloc event data sequence!" );
				return PT_Error_OutOfMemory;
			}
			*/

			event->SetDataPtr( seq );
			seq->Init( &st, this );
		}
		else
		{
			// no data, so tell the sorters we're done with this event
			// also, do an insertion of this event by its start time
//			SortEventByTime( event );
//			SortEventByType( event );
			SendEventComplete( event );
			
			m_pCurrentEvent = NULL;
			
			m_pCurrentEvent = NULL;		// set this to NULL for sanity checking
			m_EventDataLeft = 0;
		} 

		return PT_Error_OK;
	}
	else
	{
		PT_WARNING( "Error: Ran out of memory for event alloc!" );
		return PT_Error_OutOfMemory;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::SortEventByType
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
/*
PT_Error PT_StandardDataStore::SortEventByType( PT_Event *event )
{
	PT_Config_Event *configEvent;
	PT_Error Error = FindConfigByKey( event->m_definitionKey, (PT_Config **)(&configEvent) );

	if (PT_FAILED( Error ) ) 
	{
		PT_WARNING( " SortEventByType: definition lookup failed! " );
		return PT_Error_Fail;
	}

	// ok, we have a valid config
	if ( configEvent->m_firstEvent == NULL )
	{
		// case 1: first event of this type
		PT_ASSERT( configEvent->m_lastEvent == NULL );
		configEvent->m_lastEvent = configEvent->m_firstEvent = event;
	}
	else if ( )
	{
	}
}
*/
/*deprecated
////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::SortEventByTime
// Description: 
// When an event is complete, do a fast insertion of it based upon it's start time
// and record ID (2ndary criteria)
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_StandardDataStore::SortEventByTime( PT_Event *event )
{
	// first check to see if the new event goes at the very end of the list
	// this might seem expensive to do first every time, but it is a common
	// ocurrence
	PT_Time startTime = event->GetStartTime( );

	if ( m_veryFirstEvent == NULL )
	{
		m_veryFirstEvent = event;
		m_maxTime = startTime;
		event->m_nextEventInTime = NULL;

		TimeDictionary[ startTime % NUM_CACHED_TIMES ] = event;
	}
	else if ( startTime < m_veryFirstEvent->GetStartTime() )
	{
		event->m_nextEventInTime = m_veryFirstEvent;
		m_veryFirstEvent = event;
		TimeDictionary[ startTime % NUM_CACHED_TIMES ] = event;
	}
	else if ((startTime == m_veryFirstEvent->GetStartTime()) &&
			( event->GetRecordID() < m_veryFirstEvent->GetRecordID( ) ))
	{
		event->m_nextEventInTime = m_veryFirstEvent;
		m_veryFirstEvent = event;
	}

	// next check to see if the new event is the new "last time"
	else if ( startTime > m_maxTime )
	{	
		PT_Event *oldLastEvent = TimeDictionary[ m_maxTime % NUM_CACHED_TIMES ];

		// null out any time in between old last and new last. if we run into 
		// a null later we'll have to look for last non-null time. we could have
		// set these times to the old last event, but this run into lots of problems
		// when we wrap around
		for( PT_Time i = m_maxTime + 1; i < startTime; i++ )
		{
			TimeDictionary[ i % NUM_CACHED_TIMES ] = NULL;
		}

		TimeDictionary[ startTime % NUM_CACHED_TIMES ] = event;
		oldLastEvent->m_nextEventInTime = event;
		event->m_nextEventInTime = NULL;
		m_maxTime = startTime;
	}

	// did we cache a pointer to the first event with this time?
	else if ( (m_maxTime - startTime) < NUM_CACHED_TIMES )
	{
		PT_Event **current = &TimeDictionary[ startTime % NUM_CACHED_TIMES ];

		if ( *current == NULL )
		{
			// first event at this time, also may be first event in a whole gap 
			// of time... 
			*current = event;
			
			// find the last event before us...
			PT_Time i = startTime - 1;
			while( TimeDictionary[ i % NUM_CACHED_TIMES ] == NULL )
			{
				i--;
			}

			//what to do here... we've wrapped around & the time gap is too huge!!!!!!!!
			if ( TimeDictionary[ i % NUM_CACHED_TIMES ] == event )
			{
				PT_WARNING("dangerous error here with SortEventByTime!!!");
				PT_ASSERT(PT_FALSE);
				//return PT_Error_Fail;
			}

			else
			{
				PT_Event *prev = TimeDictionary[ i % NUM_CACHED_TIMES ];
				PT_Event *temp = prev->m_nextEventInTime;
				prev->m_nextEventInTime = event;
				event->m_nextEventInTime = temp;
			}
		}
		else
		{
			if ( (*current)->GetRecordID() <= event->GetRecordID() )
			{
				//insert at the end for this time
				PT_Event *temp = (*current)->m_nextEventInTime;
				(*current)->m_nextEventInTime = event;
				event->m_nextEventInTime = temp;
				(*current) = event;
			}
			else if ( event->GetStartTime( ) == m_veryFirstEvent->GetStartTime( ))
			{
				// special case, don't need to add m_nextEventInTime to anything

				// find place to insert - we already know we aren't adding to tail
				PT_Event *current = m_veryFirstEvent;
				
				do
				{
					PT_Event *temp = current->m_nextEventInTime;
					PT_ASSERT( current != NULL );
					if (temp->GetRecordID() >= event->GetRecordID() )
					{
						// insert here
						current->m_nextEventInTime = event;
						event->m_nextEventInTime = temp;
						break;
					}
					else
					{
						current = temp;
					}
				}while( PT_TRUE );

			}
			else
			{
				//find the position in this time to insert at
				PT_Time i = startTime - 1;
				while( TimeDictionary[ i % NUM_CACHED_TIMES ] == NULL )
				{
					i--;
				}

				//what to do here... we've wrapped around & the time gap is too huge!!!!!!!!
				if ( (i % NUM_CACHED_TIMES) == (startTime % NUM_CACHED_TIMES) )
				{
					PT_WARNING("dangerous error here with SortEventByTime!!!");
					PT_ASSERT(PT_FALSE);
					//return PT_Error_Fail;
				}

				PT_ASSERT( TimeDictionary[ i % NUM_CACHED_TIMES ] != NULL );
				PT_Event *firstEvent = TimeDictionary[ i % NUM_CACHED_TIMES ]->m_nextEventInTime;
				PT_ASSERT( firstEvent != NULL );

				if ( event->GetRecordID() <= firstEvent->GetRecordID() )
				{
					// insert at head
					TimeDictionary[ i % NUM_CACHED_TIMES ]->m_nextEventInTime = event;
					event->m_nextEventInTime = firstEvent;
				}
				else
				{
					// find place to insert
					PT_Event *current = firstEvent;
					
					do
					{
						PT_Event *temp = current->m_nextEventInTime;
						PT_ASSERT( current != NULL );
						if (temp->GetRecordID() >= event->GetRecordID() )
						{
							// insert here
							current->m_nextEventInTime = event;
							event->m_nextEventInTime = temp;
							break;
						}
						else
						{
							current = temp;
						}
					}while( PT_TRUE );
				}
			}
		}
	}
	else
	{
		// we've gone too far back in time, we haven't cached this!!!!
		// crash for now unless I see this is necessary
		PT_ASSERT( PT_FALSE );
		return PT_Error_Fail;
	}


	return PT_Error_OK;
//what if second event < 1st event???
//what if new time wipes entire table
//what if need to update veryFirstTime?
}
*/
////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::GetDataVoid
// Description: given a DataStatement or EventDataStatement, return the data value 
// as a void *.. Handles the allocs of strings and byte arrays as well!
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_StandardDataStore::GetDataVoid( const PT_Data_Statement &st, void **retVal )
{
	void *data = 0;
	PT_Error Error = PT_Error_OK;
	
	switch ( st.m_DataDef->m_datatype )
	{
		case Pipe_Integer:
		case Pipe_Hex_Integer:
		case Pipe_Association:
		case Pipe_Enumeration:
			{
				// m_pData points to an address where a long is stored
				// or unsigned, but we don't really care right now
				data = (void *)( * ((PT_ULONG *) (st.m_pData)) );
				break;
			}
		case Pipe_Boolean:
			{
				// m_pData points to an address where a char is stored
				// but we need to convert it to a long so that it is the 
				// same size as a void *
				PT_UCHAR val = ( * ((PT_UCHAR *) (st.m_pData)) );
				PT_ULONG val2 = val;

				data = (void *)val2;
				break;
			}
		case Pipe_String:
			{
				// need to allocate space for the string data
				char *src = ((char *) (st.m_pData));
				size_t size = strlen( src );

				// search for repetitions of certain strings
				// use somewhat of an LRU algo
				bool foundMatch = false;

				int ind = 0;
				PT_KEY key = st.m_DataDef->m_key;
				char *szval;
				do{
					szval = m_StringCompressionTable[ key ][ ind ];
					if ( szval == NULL ) break;

					if ( strcmp( szval, src ) == 0 )
					{
						foundMatch = true;
						break;
					}

					ind++;
				}while( ind < STRING_REP_TO_SEARCH );

				if (!foundMatch)
				{
					// alloc just enough space + NULL char
					char *dest = AllocData( size + 1 );

					if (ind < STRING_REP_TO_SEARCH) 
					{
						//add to  
						m_StringCompressionTable[ key ][ ind ] = dest;
					}
					else
					{
						// update LRU to make this most recent
						for( int i = 1; i < STRING_REP_TO_SEARCH; i++ )
						{
							m_StringCompressionTable[ key ][ i ] = m_StringCompressionTable[ key ][ i - 1 ];
						}
						m_StringCompressionTable[ key ][ 0 ] = dest;
					}

					if ( dest == NULL )
					{
						PT_WARNING( "Error: Ran out of memory for string alloc!" );
						return PT_Error_OutOfMemory;
					}
					else
					{
						// now copy string over
						strcpy( dest, src );

						data = (void *)dest;
						break;
					}
				}
				else
				{
					// should we update LRU here. or is algo close enough?
					for( int i = ind; i > 0; i-- )
					{
						m_StringCompressionTable[ key ][ i ] = m_StringCompressionTable[ key ][ i - 1 ];
					}
					
					m_StringCompressionTable[ key ][ 0 ] = szval;

					data = (void *)szval;
					break;
				}
			}
		case Pipe_ByteArray:
			{
				// need to allocate space for byte array
				char *src = ((char *) (st.m_pData));

				// alloc just enough space
				char *dest = AllocData( st.m_DataDef->m_arrayLen );

				if ( dest == NULL )
				{
					PT_WARNING( "Error: Ran out of memory for bytearray alloc!" );
					Error = PT_Error_OutOfMemory;
				}
				else
				{
					// now copy values over
					memcpy( dest, src, st.m_DataDef->m_arrayLen );

					data = (void *)dest;
					break;
				}
			}
		default:
			{
				PT_WARNING( "Error: Event Data has bad basetype" );
				Error = PT_Error_Fail;
			}
	}

	*retVal = data;
	return Error;
}
	

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::OnDataStatement
// Description: 
// at this point, the record is still open, so use the PT_RecordDataList linked
// list data struct to maintain the new data, and use 'new' to alloc this data.
// 'new' and 'delete' waste 8 bytes or so per alloc. but this doesn't matter.
// i did a study of traces, and on average, we have about 600 records open at
// a time (being liberal with 600). 600 * 20 data per record (again liberal) *
// 8 = 96KB. so no big deal!!!
//
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_StandardDataStore::OnDataStatement( const PT_Data_Statement &st )
{

	PT_Record *record = GetRecordByID( st.m_ID ); 

	if ( record == NULL ) 
	{
		PT_WARNING("DataStatement has invalid record ID!");
		return PT_Error_Fail;
	}

	// figure out what the key for "Thread" is, so that we don't have
	// to look it up every time we see a data entry
	if ( m_threadKey == -1 && strcmp(st.m_DataDef->m_name, THREAD_NAME) == 0)
	{
		m_threadKey = st.m_DataDef->m_key;
	}
	// figure out what the key for "Parent" is, so that we don't have
	// to look it up every time we see a data entry
	else if ( m_parentKey == -1 && strcmp(st.m_DataDef->m_name, PARENT_NAME) == 0)
	{
		m_parentKey = st.m_DataDef->m_key;
	}
	else if ( m_UIDKey == -1 && strcmp(st.m_DataDef->m_name, UID_NAME) == 0)
	{
		m_UIDKey = st.m_DataDef->m_key;
	}
	
	else if ( m_InstIDKey == -1 && strcmp(st.m_DataDef->m_name, INSTID_NAME) == 0)
	{
		m_InstIDKey = st.m_DataDef->m_key;
	}

	else if ( m_InstIDKey == -1 && strcmp(st.m_DataDef->m_name, INSTID_NAME2) == 0)
	{
		m_InstIDKey = st.m_DataDef->m_key;
	}

	// if it's thread info, add its info!
	if (st.m_DataDef->m_key == m_threadKey )
	{
		record->m_threadID = (PT_UCHAR)( * ((PT_ULONG *) (st.m_pData)) );
	}
	// if it's parent info, add its info!
	else if (st.m_DataDef->m_key == m_parentKey )
	{
		int ID = (PT_ULONG)( * ((PT_ULONG *) (st.m_pData)) );
		PT_Record * parent = GetRecordByID( ID );
		
		if (parent != NULL)
		{
			record->SetParent( parent );

			// also, set our thread to our parent's if we don't think we've been
			// initialized - or don't know (set to 0)
			if ( record->m_threadID == 0 )
			{
				record->m_threadID = parent->m_threadID;
			}
		}
	}
	else if (st.m_DataDef->m_key == m_InstIDKey)
	{
		record->m_instID = (PT_ID)( * ((PT_ULONG *) (st.m_pData)) );
	}
	else if (st.m_DataDef->m_key == m_UIDKey)
	{
		record->m_uopID = (PT_ID)( * ((PT_ULONG *) (st.m_pData)) );
	}

	// lock down this record while we do pointer manipulation
	// so that another thread doesn't get confused
	record->SetBoolVal( RECORD_LOCKED_BIT, true );

	if ( record->GetRecordDataList( ) == NULL )
	{
		// this is first data attached to the record
		PT_RecordDataList *list = new PT_RecordDataList;

		// do ugly cast! TODO: is there a cleaner way to store this without
		// wasting space and time?
		record->SetDataList( (PT_DataList *)list );
	}

	// update config data to reflect that this is record data
	st.m_DataDef->m_bRecordData = true;

	if ( record->GetBoolVal( RECORD_CLOSED_BIT ) == true )
	{
		record->SetBoolVal( RECORD_LOCKED_BIT, false );
		return PT_Error_Fail;
	}
	
	// add a new node
	// do ugly cast! 
	PT_RecordDataList *list = record->GetRecordDataList( );

	// and add to tail of list!
	void *data;
	PT_Error Error = GetDataVoid( st, &data );
	
	if ( PT_FAILED( Error ) )
	{
		record->SetBoolVal( RECORD_LOCKED_BIT, false );
		return Error;
	}
//if( record->GetRecordID( ) == 1 )
//{
//	int here = 1;
//}
	list->AddData( st.m_DataDef->m_key, data );


	record->SetBoolVal( RECORD_LOCKED_BIT, false );
	return PT_Error_OK;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::OnEventDataStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error
PT_StandardDataStore::OnEventDataStatement( const PT_EventData_Statement &st )
{
	// does this eventdata belong to a PT_UPDATE_STRUCT or a PT_EVENT?
	if ( m_bIsEvent )
	{
		PT_DataList *pDataList = m_pCurrentEvent->GetDataList( );

		// bad event setup
		if ( pDataList == NULL )
		{
			PT_WARNING( "Error: trying to add event data to an event with no data specified" );
			return PT_Error_Fail;
		}

		// index to add is derived as follows
		PT_USHORT index = pDataList->NumElements( this ) - m_EventDataLeft;
		
		// insertion of data is different for each type
		void *data;
		PT_Error Error = GetDataVoid( st, &data );

		if ( PT_FAILED( Error ) )
		{
			return Error;
		}

		// now, insert data
		pDataList->SetDataByIndex( index, st.m_DataDef->m_key, data, this );

		m_EventDataLeft--;

		// finally, if this is the last piece of EventData in the event, the 
		// event is complete. Send notification to the Sorters
		if ( m_EventDataLeft == 0 ) 
		{
	//		SortEventByTime( m_pCurrentEvent );
	//		SortEventByType( m_pCurrentEvent );

			bool needType = true;
			PT_KEY _key = m_pCurrentEvent->GetDefinitionKey( );
			
			if ( TypesEqual( _key ) ) needType = false;

			if (needType) m_numNonCachedTypes++;
			else m_numCachedTypes++;

			PT_DataList *seq = ( PT_DataList * ) AllocData( PT_DataList::GetDataByteSize( 
							m_pCurrentEvent->GetDataList( )->NumElements( this ), needType ) );			
			if ( seq == NULL )
			{
				PT_WARNING( "Error: Ran out of memory for alloc event data sequence!" );
				return PT_Error_OutOfMemory;
			}

			// see if this is the first time we have seen this event
			if ( m_dataKeyTable[ _key ] == NULL )
			{
				PT_DataList *list = ( PT_DataList * )( &TempDataList );
				m_dataKeyTable[ _key ] = new PT_KEY[ list->NumElements( this ) ];
				m_dataKeyTableSize[ _key ] = list->NumElements( this );

				for( PT_USHORT i = 0; i < list->NumElements( this ); i++ )
				{
					m_dataKeyTable[ _key ][ i ] = list->GetDefinitionKey( i, this );
				}
			}

			// copy data over
			CopyData( _key, seq, needType );

			// finally, change the data pointer... doing this last minimizes the risk for
			// thread timing issues
			m_pCurrentEvent->SetDataPtr( seq );
			
			SendEventComplete( m_pCurrentEvent );
			m_pCurrentEvent = NULL;
		}

		return PT_Error_OK;
	}
	else
	{
		// insertion of data is different for each type
		void *data;
		PT_Error Error = GetDataVoid( st, &data );

		if ( PT_FAILED( Error ) )
		{
			return Error;
		}

		// its an update struct
		Error = m_pCurrentStruct->UpdateCell( st.m_DataDef, st.m_ID, data, m_CurrentStructRow, m_CurrentStructTime );

		if ( PT_FAILED( Error ) )
		{
			return Error;
		}

		m_EventDataLeft--;

		if ( m_EventDataLeft == 0 ) 
		{
			m_pCurrentStruct = NULL;
		}

		return PT_Error_OK;
	}
}

// helper function
bool
PT_StandardDataStore::TypesEqual( PT_KEY key )
{
	PT_DataList *list = ( PT_DataList * )( &TempDataList );
	
	if (m_dataKeyTableSize[ key ] == 0) return true;	// first instance of 

	int num = list->NumElements( this );
	if (num != m_dataKeyTableSize[ key ]) return false;

	// loop through to make sure all is the same
	for( PT_UINT i = 0; i < num; i++ )
	{
		if ( list->GetDefinitionKey( i, this ) != m_dataKeyTable[ key ][ i ] )
			return false;
	}

	return true;
}

// helper function
void 
PT_StandardDataStore::CopyData( PT_KEY key, PT_DataList *seq, bool needType )
{
	if (needType)
	{
		PT_DataList *list = ( PT_DataList * )( &TempDataList );
		seq->m_numElements = list->NumElements( this );
		for( PT_USHORT i = 0; i < list->NumElements( this ); i++ )
		{
			seq->AddData( list->GetDefinitionKey( i, this ), (void *)(* (list->GetDataByIndex( i, this ))), i, this );
		}
	}
	else
	{
		PT_DataList *list = ( PT_DataList * )( &TempDataList );

		// mask the key with a bit that tells us it is a keytabled list
		seq->m_numElements = key | DATA_KEYTABLE_MASK;
		for( PT_USHORT i = 0; i < list->NumElements( this ); i++ )
		{
			seq->AddData( list->GetDefinitionKey( i, this ), (void *)(* (list->GetDataByIndex( i, this ))), i, this );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::OnCloseStatement
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_StandardDataStore::OnCloseStatement( const PT_Close_Statement &st )
{
	PT_Record *record = GetRecordByID( st.m_ID );
	if ( record == NULL ) 
		return PT_Error_BadID;

	// if we are being blocked from the recorddatalist, don't close now,
	// close will happen OnTraceComplete
	if (record->GetBoolVal( RECORD_LOCKED_BIT ))
	{
		PT_WARNING("Can't close, Record is locked!");
		return PT_Error_OK;
	}

//if( record->GetRecordID( ) == 1 )
//{
//	int here = 1;
//}

	// lock this record from access just in case!
	record->SetBoolVal( RECORD_LOCKED_BIT, true );

	// now convert the PT_RecordDataList struct to a PT_DataList!
	PT_RecordDataList *linkedList = record->GetRecordDataList( );


	// it could be null if no data!
	if ( linkedList != NULL )
	{
		PT_DataList *pDataList = ( PT_DataList * )AllocData( PT_DataList::GetDataByteSize( linkedList->NumElements( ) ) );

		if ( pDataList == NULL )
		{
			record->SetBoolVal( RECORD_LOCKED_BIT, false );
			return PT_Error_OutOfMemory;
		}

		pDataList->Init( linkedList->NumElements( ), this );

		// copy data over
		linkedList->FillDataList( pDataList, this );

		// and finally, destroy the list. the destructor will recursively 
		// destroy all nodes in the list
		delete linkedList;

		record->SetDataList( pDataList );
	}


	// Close this thing, once and for all!
	record->SetBoolVal( RECORD_CLOSED_BIT, true );
	record->SetBoolVal( RECORD_LOCKED_BIT, false );

	// Update all sorters of this recent development
	SendRecordComplete( record );

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::UpdateAll
// Description: Handles UpdateAll for Configs, but not records or events
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error 
PT_StandardDataStore::UpdateAll( PT_Sorter *pSorter )
{
	bool bGetRecord;
	bool bGetEvent;

	// first, handle the PT_Configs
	PT_Error Error = PT_DataStore::UpdateAll( pSorter );
	
	if ( PT_FAILED( Error ) )
	{
		return Error;
	}

	// now for the records and events

	// figure out which notifications we will need to do some work for
	PT_UCHAR notify = pSorter->RegisterEvents( );
	bGetRecord = ( (RECORD_COMPLETE_BIT & notify) != 0 );
	bGetEvent = ( (EVENT_COMPLETE_BIT & notify) != 0 );

	// stop now?
	if (m_bInterruptFlag)
		return PT_Error_Interrupted;

	if ( (!bGetEvent) && (!bGetRecord) )
	{
		if (m_bTraceDone)
			pSorter->OnTraceComplete( );
		return PT_Error_OK;
	}

	// case 1: just get records - assumption is that order they arrived in
	// is easier for the sorter to work with than order they were closed
	if ( bGetRecord && !bGetEvent )
	{
		PT_Record *record;

		for( unsigned int i = 1; i < ( m_numAllocRecords + 1); i++ )
		{
			// stop now?
			if (m_bInterruptFlag) return PT_Error_Interrupted;
			
			// make sure the record is closed
			record = GetRecordByID( i );
			
			PT_ASSERT( record != NULL );
			if ( record->GetBoolVal( RECORD_CLOSED_BIT ) )
			{
				// notify the sorter
				pSorter->OnRecordComplete( record );
			}
		}
	}

	// case 2: get events and maybe records too - events are presorted in the 
	// order they came in. if we are sending records as well, order we send
	// might be important to the sorter, so do the following: send events until
	// you find the last one belonging to a record. then, if that record is closed
	// send it as well
	if ( bGetEvent )
	{ 
		// start with the first event
		PT_ULONG currentPage = 0;
		PT_Event *currentEvent = m_veryFirstEvent;

		PT_Record *record = NULL;

        PT_ULONG index = 0;

		// first flush out all records that don't have any events
		if( bGetRecord )
		{
			for( unsigned int i = 1; i < ( m_numAllocRecords + 1); i++ )
			{
				// stop now?
				if (m_bInterruptFlag) return PT_Error_Interrupted;

				
				// make sure the record is closed
				record = GetRecordByID( i );
				
				if ( (record->GetFirstEvent() == NULL) && record->GetBoolVal( RECORD_CLOSED_BIT ) )
				{
					// notify the sorter
					pSorter->OnRecordComplete( record );
				}
			}
		}

		do {
			// stop now?
			if (m_bInterruptFlag) return PT_Error_Interrupted;
			
			pSorter->OnEventComplete( currentEvent );

			// now, if the event is the last in its record, check to see
			// if the record has been closed. if so, send it too
			if ( bGetRecord && ( currentEvent->GetNextEvent( ) == NULL ) )
			{
				record = GetRecordByID( currentEvent->GetRecordID() );

				if ( record != NULL && record->GetBoolVal( RECORD_CLOSED_BIT ) )
				{
					// now send notification for this record
					pSorter->OnRecordComplete( record );
				}
			}
		//} while ( (currentEvent = GetNextEventInTime()) != NULL );
		} while ( (currentEvent = GetNextEvent( &currentPage, &index, currentEvent )) != NULL );
	
	}

	if ( m_bTraceDone && !m_bInterruptFlag) 
	{
		pSorter->OnTraceComplete( );
	}

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_StandardDataStore::GetNextEvent
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Event  *PT_StandardDataStore::GetNextEvent( PT_ULONG *currentPage, PT_ULONG *currentIndex, PT_Event *event )
{
	PT_Event *retVal;

	// are we at a page boundary (and not at the last event)?
	if ( (EventIndexTable[ *currentPage ] == *currentIndex) )
	{
		if (*currentPage >= m_currentEventPage)
		{
			// last event
			return NULL;
		}

		(*currentPage)++;
		(*currentIndex)++;

		// rare case, but make sure this isn't the last one
		if ((*currentPage) == NUM_MEM_TABLES || EventIndexTable[ *currentPage ] == 0 )
		{
			return NULL;
		}

		return (PT_Event *)(EventTable[ *currentPage ]);
	}
	else
	{
		(*currentIndex)++;

		if ( EventIndexTable[ *currentPage ] <= (*currentIndex) )
		{
			return NULL;
		}

		// let the event determine if this is a short or long event
		retVal = event->GetNextEventInTime( );

		return retVal;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordDataList::AddData
// Description: 
// Add a data node to the end of the list
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_RecordDataList::AddData( PT_KEY dataKey, void *data )
{
	if ( m_tailNode == NULL )
	{
		PT_ASSERT( m_rootNode == NULL && m_numElements == 0 );

		// create our first node
		DataNode *newnode = new DataNode( dataKey, data );
		m_rootNode = m_tailNode = newnode;

		m_numElements++;
	}
	else
	{
		// just add to tail
		PT_ASSERT( m_tailNode->nextNode == NULL );

		m_tailNode->nextNode = new DataNode( dataKey, data );
		m_tailNode = m_tailNode->nextNode;

		m_numElements++;
	}
}

// Returns a key for the definition of data[ index ]
// Returns zero if invalid index
PT_KEY PT_RecordDataList::GetDefinitionKey( PT_USHORT index )
{
	if (index >= m_numElements) return 0;

	//else, chase pointers
	DataNode *current = m_rootNode;

	for( int i = 0; i < index - 1; i++ )
	{
		PT_ASSERT(current != NULL);
		current = current->nextNode;
	}

	return current->dataDef;
}

// Returns address of the data for the associated definition. If doesn't exist
// returns NULL
void  **PT_RecordDataList::GetDataByDefinition( PT_KEY definitionKey )
{
	//else, chase pointers
	DataNode *current = m_rootNode;

	while( current != NULL )
	{
		if ( current->dataDef == definitionKey )
		{
			return &(current->data);
		}

		current = current->nextNode;
	}

	return NULL;
}

// Returns the data for the associated index. If doesn't exist, 
// returns NULL
void  **PT_RecordDataList::GetDataByIndex( PT_USHORT index )
{
	if (index >= m_numElements) return NULL;

	//else, chase pointers
	DataNode *current = m_rootNode;

	for( int i = 0; i < index - 1; i++ )
	{
		PT_ASSERT(current != NULL);
		current = current->nextNode;
	}

	return &(current->data);
}

	
////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordDataList::FillDataList
// Description: 
// helper function to help DataStores convert from a PT_RecordDataList to a 
// PT_DataList
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_RecordDataList::FillDataList( PT_DataList *pDataList, PT_StandardDataStore *store )
{
	PT_USHORT outSize = pDataList->NumElements( store );

	// sanity check
	PT_ASSERT( outSize == m_numElements );

	DataNode *current = m_rootNode;

	PT_KEY *keyTable = pDataList->GetKeyTable( store );
	void **dataTable = pDataList->GetDataTable( store );

	// now iterate through linked list and copy over to data list
	for( int i = 0; i < outSize; i++ )
	{
		PT_ASSERT( current != NULL );
		

		keyTable[ i ] = current->dataDef;
		dataTable[ i ] = current->data;

		current = current->nextNode;
	}

	PT_ASSERT( current == NULL);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::PT_DataList
// Description: Default Contructor
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_DataList::PT_DataList()
{
	m_numElements = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::PT_DataList
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_DataList::PT_DataList( PT_Event_Statement *event, PT_StandardDataStore *store )
{
	Init( event, store );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::Init
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void
PT_DataList::Init( const PT_Event_Statement *event, PT_StandardDataStore *store )
{
	Init( ( PT_USHORT )event->m_num_values, store );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::GetDataByIndex
// Description: 
// Returns the data for the associated index. If doesn't exist, 
// returns NULL
//
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void **PT_DataList::GetDataByIndex( PT_USHORT index, PT_StandardDataStore *store )
{
	if ( index >= NumElements( store ) ) return 0;

	// see above for pointer math description
	void **dataTable = GetDataTable( store );
	return &(dataTable[ index ]);
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::GetDataByIndex
// Description: 
// Returns the data for the associated index. If doesn't exist, 
// returns NULL
//
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_USHORT 
PT_DataList::NumElements( PT_StandardDataStore *store )
{
	if ( HasKeyTable( ) ) return m_numElements;
	else return ( store->GetKeyDataSize( m_numElements ^ DATA_KEYTABLE_MASK ) );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::GetDefinitionKey
// Description: Returns a key for the definition of data[ index ]
// Returns zero if invalid index
//
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_KEY PT_DataList::GetDefinitionKey( PT_USHORT index, PT_StandardDataStore *store )
{
	if ( index >= NumElements( store ) ) return 0;
	
	// now do ugly pointer math to find definition
	// key must be stored directly after the struct in 
	// allocated memory
	PT_KEY *keyTable = GetKeyTable( store );
	return keyTable[ index ];
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::GetKeyTable
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_KEY *PT_DataList::GetKeyTable( PT_StandardDataStore *store )
{
	PT_KEY *keyTable;
	if ( HasKeyTable( ) )
		keyTable = (PT_KEY *)( this + 1 );
	else 
	{
		keyTable = store->GetKeyTable( m_numElements ^ DATA_KEYTABLE_MASK );
		PT_ASSERT( keyTable != NULL );
	}

	return keyTable;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::GetDataTable
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void **PT_DataList::GetDataTable( PT_StandardDataStore *store )
{
	void **dataTable;
	
	if ( HasKeyTable( ) )
		dataTable = (void **)( GetKeyTable( store ) + m_numElements /** sizeof( PT_KEY )*/ );
	else 
		dataTable = (void **)((PT_KEY *)(this + 1));	// ugly cast - fix this
	
	return dataTable;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::Init
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void
PT_DataList::Init( const PT_USHORT size, PT_StandardDataStore *store )
{
	m_numElements = size;

	// using ugly pointer math, clear the memory after us
	// this will be very useful for sanity checking
	PT_KEY *key = GetKeyTable( store );
	void  **data = GetDataTable( store );

	for( int i = 0; i < m_numElements; i++ )
	{
		key[ i ] = 0;
		data[ i ] = NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::TypesEqual
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
bool 
PT_DataList::TypesEqual( PT_DataList *other, PT_StandardDataStore *store )
{
	PT_USHORT num = NumElements( store );
	if (other->NumElements( store ) != num ) return false;

	for( PT_USHORT i = 0; i < num; i++ )
	{
		if ( GetDefinitionKey( i, store ) != other->GetDefinitionKey( i, store ) ) return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::GetDataByteSize
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_ULONG PT_DataList::GetDataByteSize( PT_ULONG numElements, bool needType )
{
	if ( needType )
		return ( sizeof( PT_DataList ) + ( numElements * sizeof( PT_KEY ) ) + ( numElements * sizeof( void *) ));
	else
		return ( sizeof( PT_DataList ) + ( numElements * sizeof( void *) ) );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::GetDataByDefinition
// Description: 
// Returns the data for the associated definition. If doesn't exist
// returns NULL
//
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void **PT_DataList::GetDataByDefinition( PT_KEY definitionKey, PT_StandardDataStore *store )
{
	// do an O(n) search through all data keys

	// now do ugly pointer math to find definition.
	// key must be stored directly after the struct in 
	// allocated memory
	PT_KEY *keyTable = GetKeyTable( store );
	for (int index = 0; index < NumElements( store ); index++ )
	{
		if ( keyTable[ index ] == definitionKey )
		{
			// found it!, now return the data associated with that address
			// more ugly pointer math here. data must be stored directly after
			// the key table
			void **dataTable = GetDataTable( store );
			return &(dataTable[ index ]);
		}
	}

	// didn't find a match
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::AddData
// Description: 
// pre: we are not adding a data element to a list that is already
// full ( == to m_numElements )
//
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_DataList::AddData( PT_KEY key, void *data, PT_USHORT index, PT_StandardDataStore *store )
{
	PT_ASSERT( index < NumElements( store ) );

	if ( HasKeyTable( ) )
	{
		PT_KEY *keyTable = GetKeyTable( store );
		keyTable[ index ] = key;
	}

	void **dataTable = GetDataTable( store );
	dataTable[ index ] = data;

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Event::PT_Event
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Event::PT_Event()
{ 
/*
	m_definitionKey = 0;
	m_eventTime = 0;
	m_eventDuration = 0;
	m_nextEvent = NULL;
//	m_nextEventInTime = NULL;
	m_recordIndex = 0;
*/
	lowWord = 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Event::SetDataPtr
// Description: Use some ugly pointer math and set the pointer located right
// after the PT_Event
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void 
PT_Event::SetDataPtr( PT_DataList *seq )
{
	if ( ! ( lowWord & SMALL_SIZE_MASK ) )
	{
		PT_DataList **pDataList = (PT_DataList **)( ((char *)this) + sizeof(lowWord) + sizeof( PT_LargeEvent ) );
		*pDataList = seq;
	}
	else
	{
		PT_DataList **pDataList = (PT_DataList **)( ((char *)this) + sizeof(lowWord) + sizeof( PT_SmallEvent ) );
		*pDataList = seq;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Event::GetNextEventInTime
// Description: 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Event *PT_Event::GetNextEventInTime( )
{	
	// check to see if there is data attached.
	// if no return NULL
	if ( ! ( lowWord & DATAKEY_MASK ) )
	{
		if ( ! ( lowWord & SMALL_SIZE_MASK ) )
		{
			return (PT_Event *) ( ((char *)this) + sizeof(lowWord) + sizeof( PT_LargeEvent ) );
		}
		else
		{
			return (PT_Event *) ( ((char *)this) + sizeof(lowWord) + sizeof( PT_SmallEvent ) );
		}
	}
	else 
	{
		// there is data, so there is a dataseq pointer

		// cast it to a char* so that we can work in unit pointer math
		if ( ! ( lowWord & SMALL_SIZE_MASK ) )
		{	
			char *ptr = ((char *)this) + sizeof( PT_LargeEvent ) + sizeof( PT_DataList *);
			
			// now cast it back
			return (PT_Event *)ptr;
		}
		else
		{	
			char *ptr = ((char *)this) + sizeof( PT_SmallEvent ) + sizeof( PT_DataList *);
			
			// now cast it back
			return (PT_Event *)ptr;
		}
	}
}

void PT_Event::UpdateStartTime( PT_Time diff )
{
	if ( ( lowWord & SMALL_SIZE_MASK ) )
	{	
		lowWord += diff;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Event::Init
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_Event::Init( PT_Record *r, const PT_Event_Statement *event, bool isSmallEvent )
{
	PT_Event *first;

	// see if we can do a small event or if we need a large one
	if (isSmallEvent)
	{
		bool isLowest = (( first = r->GetFirstEvent( )) == NULL || first->GetStartTime( r ) > event->m_time );

		if ( isLowest )
		{
			// this is the new first event
			lowWord = 0;
		}
		else
		{
			lowWord = event->m_time - first->GetStartTime( r );
		}
		
		PT_USHORT duration = event->m_duration;

		// 3/4/02 some models have bugs where they set duration to 0 or -1. adjust this 
		// to be 1.
		if (( duration == 0 ) || (duration == (PT_Time)-1))
		{
			duration = 1;
		}

		PT_ASSERT( duration <= 2 );

		duration = (duration - 1)<< (PT_USHORT)13;
		lowWord += duration;

		PT_SmallEvent *se = (PT_SmallEvent *)(this + 1);
		// fill in the structore based on the event statement
		se->m_nextEvent = NULL; 
		
		PT_ASSERT( event->m_EventDef->m_key < ( 1 << 12 ) );
		PT_ASSERT( event->m_ID < ( 1 << 20 ) );

		se->midDword = 0;
		se->midDword += (event->m_EventDef->m_key << 20);
		se->midDword += event->m_ID;
		
		
		PT_ASSERT( (lowWord & DATAKEY_MASK) == 0 );
		PT_ASSERT( (lowWord & SMALL_SIZE_MASK) == 0 );

		if ( event->m_num_values > 0 )
		{
			// since we can't allocate a DataList ourselves
			// lets set the pointer to NULL for now, and let the
			// allocator fill it in

			// set the bool bit
			lowWord =  lowWord | (PT_USHORT)DATAKEY_MASK;

			// list pointer is located right after us in memory
			PT_DataList **list = (PT_DataList **)( (se + 1));//sizeof( PT_Event ) ));
			*list = NULL;
		}	
		lowWord |= SMALL_SIZE_MASK;

	}
	else
	{
		//m_definitionKey
		lowWord = event->m_EventDef->m_key;

		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		
		// fill in the structore based on the event statement
		le->m_eventTime = event->m_time;
		le->m_eventDuration = event->m_duration;

		// 3/4/02 some models have bugs where they set duration to 0 or -1. adjust this 
		// to be 1.
		if (( le->m_eventDuration == 0 ) || (le->m_eventDuration == (PT_Time)-1))
		{
			le->m_eventDuration = 1;
		}

		le->m_nextEvent = NULL;
		le->m_recordIndex = event->m_ID;

		// there should be NO definitions that use the high bit
		PT_ASSERT( (lowWord & DATAKEY_MASK) == 0 );
		PT_ASSERT( (lowWord & SMALL_SIZE_MASK) == 0 );

		if ( event->m_num_values > 0 )
		{
			// since we can't allocate a DataList ourselves
			// lets set the pointer to NULL for now, and let the
			// allocator fill it in

			// set the bool bit
			lowWord =  lowWord | DATAKEY_MASK;

			// list pointer is located right after us in memory
			PT_DataList **list = (PT_DataList **)( (le + 1));//sizeof( PT_Event ) ));
			*list = NULL;
		}	
	
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_DataList::SetDataByIndex
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_DataList::SetDataByIndex( PT_USHORT index, PT_KEY definitionKey, void *data, PT_StandardDataStore *store )
{
	PT_ASSERT( index < NumElements( store ) ); 
	
	void  **dTable = GetDataTable( store );

	if ( HasKeyTable( ) )
	{
		PT_KEY *kTable = GetKeyTable( store );
		kTable[ index ] = definitionKey;
	}
	
	dTable[ index ] = data;
}




////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordDataList::PT_RecordDataList
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_RecordDataList::PT_RecordDataList()
{

	m_numElements = 0;
	m_rootNode = NULL;
	m_tailNode = NULL;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordDataList::~PT_RecordDataList
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_RecordDataList::~PT_RecordDataList()
{
	DataNode *current = m_rootNode;
	DataNode *next = NULL;

	// traverse down the list and delete each node
	while ( current != NULL )
	{
		next = current->nextNode;
		delete current;
		current = next;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::PT_Record
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Record::PT_Record( )
{ 
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::~PT_Record
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
//PT_Record::~PT_Record( )
//{
//	int here = 1;
//}

void PT_Record::UpdateEventStartTimes( const PT_Event_Statement *eventStatement, PT_StandardDataStore *dataStore )
{
	// go through list, skipping the head
	int diff = m_minTime - eventStatement->m_time;
	PT_ASSERT( m_minTime == 0 || diff >= 0 );

	PT_Event *cur = m_pFirstEvent->GetNextEvent( );
	PT_Event *prev = m_pFirstEvent;

	while ( cur != NULL )
	{
		// if time will now extend over 14 bits we are in trouble
		if ( cur->GetIsShortEvent( ) && ( cur->GetStartTime( this ) - m_minTime + diff ) > 16383 )
		{
			// big problem... must convert this event to a long event
			// need to get this from the datastore
			PT_Event *newcur = dataStore->AllocNewEvent( ( cur->GetDataList( ) != NULL ), false );
			
			PT_Event_Statement dummy;
			dummy.m_duration = cur->GetDuration( );
			dataStore->FindConfigByKey( cur->GetDefinitionKey( ), (PT_Config **)(&dummy.m_EventDef) );
			dummy.m_ID = cur->GetRecordID( );
			dummy.m_namespace = 0;
			dummy.m_num_values = ( cur->GetDataList( ) != NULL ) ? 1 : 0;
			dummy.m_time = cur->GetStartTime( this );

			newcur->Init( this, &dummy , false );
			
			if ( cur->GetDataList( ) != NULL )
				newcur->SetDataPtr( cur->GetDataList( ) );
			
			newcur->SetNextEvent( cur->GetNextEvent( ) );
			prev->SetNextEvent( newcur );

			cur = newcur;
		}
			
		cur->UpdateStartTime( (PT_Time)diff );		
		prev = cur;
		cur = cur->GetNextEvent( );
	}
	
	m_minTime = eventStatement->m_time;

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::AddEvent
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_Record::AddEvent( PT_Event *event, const PT_Event_Statement *eventStatement, PT_StandardDataStore *dataStore )
{
	if ( m_pLastEvent == NULL )
	{
		// this is the first event
		PT_ASSERT( m_pFirstEvent == NULL );
		
		m_pLastEvent = m_pFirstEvent = event;
		event->SetNextEvent( NULL );
		
		// therefore all our offsets are off! update them.
		UpdateEventStartTimes( eventStatement, dataStore );
	}
	else
	{	
		// see if we can just add to tail
		if ( m_pLastEvent->GetStartTime( this ) <= eventStatement->m_time )
		{
			event->SetNextEvent( NULL );
			m_pLastEvent->SetNextEvent( event );
			m_pLastEvent = event;
		}
		else
		{
			// need to do an expensive insertion, this should rarely happen though
			if ( m_pFirstEvent->GetStartTime( this ) >= eventStatement->m_time )
			{
				event->SetNextEvent( m_pFirstEvent );
				m_pFirstEvent = event;
				//m_minTime = event->GetStartTime( this );

				// therefore all our offsets are off! update them.
				UpdateEventStartTimes( eventStatement, dataStore );

			}
			else
			{
				PT_Event *last = m_pFirstEvent;
				PT_Event *current = m_pFirstEvent->GetNextEvent( );

				PT_Time stTime = event->GetStartTime( this );
				while( (current != NULL) && (current->GetStartTime( this ) < stTime) )
				{
					last = current;
					current = current->GetNextEvent( );
				}

				PT_ASSERT( last != m_pLastEvent );

				// ok found a place to insert
				last->SetNextEvent( event );
				event->SetNextEvent( current );
			}
		}
	}

	// update MaxTime if necessary
	PT_Time endTime = event->GetEndTime( this );
	if (  endTime > m_maxTime ) 
	{
		m_maxTime = endTime;
	}
}
/*
////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::GetMaxTime
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Time PT_Record::GetMaxTime( )
{ 
	return m_maxTime; 
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::GetMinTime
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Time PT_Record::GetMinTime( )
{
	if ( m_pFirstEvent )
	{
		return m_pFirstEvent->GetStartTime( );
	}
	return 0;
}
*/

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::Init
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_Record::Init( const PT_Open_Statement *st )
{
	m_recordID = st->m_ID;
	m_uopID = m_instID = 0;
	m_pFirstEvent = m_pLastEvent = NULL;
	m_maxTime = 0;
	m_pDataList = NULL;


	PT_ASSERT( st->m_recordType != NULL );
	m_definitionKey = st->m_recordType->m_key;

	m_minTime = 0;
	m_threadID = 0;
	m_boolVector = 0x00;
	m_pParent = m_pChild = m_pLastChild = m_pNextSibling = /*m_pNextThreadSibling =*/ NULL;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::GetBoolVal
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
bool PT_Record::GetBoolVal( PT_UCHAR index )
{ 
	PT_ASSERT(index < 8); 
	return ((m_boolVector & (1 << index)) != 0); 
}


void PT_Record::SetBoolVal( PT_UCHAR index, bool val)
{ 
	if (index < 8)
	{
		if ( !val )
		{
			unsigned char mask = ((PT_UCHAR)1 << index);
			mask = 0xff - mask;
			m_boolVector = m_boolVector & mask;
		}
		else m_boolVector |= (1 << index); 
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::SetParent
// Description: 
// sets the parent pointer to parent, adds this to parent's sibling list
// setting to null, means no parent
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_Record::SetParent( PT_Record *parent )
{
	m_pParent = parent;

	if ( parent != NULL )
	{
		if (parent->m_pChild == NULL)
		{
			PT_ASSERT(parent->m_pLastChild == NULL);
			parent->m_pChild = this;
			parent->m_pLastChild = this;
			m_pNextSibling = NULL;
		}
		else
		{
			// insert in order by recID
			if (this->m_recordID < parent->m_pChild->m_recordID )
			{
				// insert at head
				this->m_pNextSibling = parent->m_pChild;
				parent->m_pChild = this;
			}
			else if (this->m_recordID > parent->m_pLastChild->m_recordID )
			{
				this->m_pNextSibling = NULL;
				parent->m_pLastChild->m_pNextSibling = this;
				parent->m_pLastChild = this;
			}
			else
			{
				PT_Record *pCurrent = parent->m_pChild;
				PT_Record *pPrevious = NULL;
				while( pCurrent != NULL && (this->m_recordID > pCurrent->m_recordID ))
				{
					pPrevious = pCurrent;
					pCurrent = pCurrent->m_pNextSibling;
				}

				PT_ASSERT( pPrevious != NULL );
				this->m_pNextSibling = pPrevious->m_pNextSibling;
				pPrevious->m_pNextSibling = this;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Sorter::RegisterEvents
// Description: 
// Notifies the DataStore which events the derived sorter implements, so that 
// the DataStore doesn't waste a lot of time calling methods that aren't
// implemented. The derived class implements this by implementing
// SetImplementBools( ). RegisterEvents should not be overrided.
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_UCHAR PT_Sorter::RegisterEvents( )
{
	PT_UCHAR outChar = 0;

	SetImplementBools();

	// set the return bits based upon which methods the derived class has 
	// announced it will implement
	if ( m_bImplementsRecordComplete ) 
	{
		outChar |= RECORD_COMPLETE_BIT;
	}
	if ( m_bImplementsEventComplete )
	{
		outChar |= EVENT_COMPLETE_BIT;
	}
	if ( m_bImplementsRecordConfigComplete )
	{
		outChar |= RECORD_CONFIG_COMPLETE_BIT;
	}
	if ( m_bImplementsEventConfigComplete )
	{
		outChar |= EVENT_CONFIG_COMPLETE_BIT;
	}
	if ( m_bImplementsDataConfigComplete )
	{
		outChar |= DATA_CONFIG_COMPLETE_BIT;
	}

	if ( outChar == 0 )
	{
		// stupid is as stupid does
//		PT_WARNING( "Sorter implemented to register for zero notifications!" );
	}

	return outChar;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Sorter::GetByIndex
// Description: Default "Getters" for PT_Sorters. Return E_NOTIMPL and sets
// *retVal to NULL
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_Sorter::GetByIndex( PT_UINT index, PT_Record **retVal )
{
	*retVal = NULL;
	return PT_Error_NotImpl;
}

PT_Error PT_Sorter::GetByIndex( PT_UINT index, PT_Event **retVal )
{
	*retVal = NULL;
	return PT_Error_NotImpl;
}

PT_Error PT_Sorter::GetByIndex( PT_UINT index, PT_Config_Record **retVal )
{
	*retVal = NULL;
	return PT_Error_NotImpl;
}

PT_Error PT_Sorter::GetByIndex( PT_UINT index, PT_Config_Event **retVal )
{
	*retVal = NULL;
	return PT_Error_NotImpl;
}

PT_Error PT_Sorter::GetByIndex( PT_UINT index, PT_Config_Data **retVal )
{
	*retVal = NULL;
	return PT_Error_NotImpl;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Sorter::OnRecordComplete
// Description: Default handlers for PT_Sorters. Returns E_NOTIMPL
// Derived classes must implement the handlers they register
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_Sorter::OnRecordComplete( PT_Record *pNewRecord)
{
	return PT_Error_NotImpl;
}

PT_Error PT_Sorter::OnEventComplete( PT_Event *pNewRecord)
{
	return PT_Error_NotImpl;
}

PT_Error PT_Sorter::OnRecordConfigComplete( PT_Config_Record *pNewRecord)
{
	return PT_Error_NotImpl;
}

PT_Error PT_Sorter::OnEventConfigComplete( PT_Config_Event *pNewRecord)
{
	return PT_Error_NotImpl;
}

PT_Error PT_Sorter::OnDataConfigComplete( PT_Config_Data *pNewRecord)
{
	return PT_Error_NotImpl;
}

PT_Error PT_Sorter::OnTraceComplete( )
{
	return PT_Error_OK;
}

// define these 2 globals so that we can pass them into sortvectors. they contain no data
// members, so this is safe
WatchCellComparer g_CellCompare;
WatchRowComparer g_RowCompare;

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Structure::PT_Structure
// Description: Configures a PT_Structure with correct rows and cols
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Structure::PT_Structure( PT_Config_Struct *st )
{
	m_StructureDefinition = st;
	
	m_numRows = st->m_numRows;
	m_numCols = st->m_numCols;

	// now create our SortVectors
	m_StructureRows = new PT_SortVector< StructureRowNode > *[ m_numRows ];
	m_StructureCells = new PT_SortVector< StructureCellNode > **[ m_numRows ];
	
	for( PT_UINT i = 0; i < m_numRows; i++ )
	{
		m_StructureCells[ i ] = new PT_SortVector< StructureCellNode > *[ m_numCols ];

		// start with an initial size of 2! this is good for large structures like caches, where
		// some entries may never get updated in the course of a small trace
		m_StructureRows[ i ] = new PT_SortVector< StructureRowNode >( &g_RowCompare, 2, true );

		for( PT_UINT j = 0; j < m_numCols; j++ )
		{
			m_StructureCells[ i ][ j ] = new PT_SortVector< StructureCellNode >( &g_CellCompare, 2, true );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Structure::PT_Structure
// Description: Destructor
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Structure::~PT_Structure( )
{
	for( PT_UINT i = 0; i < m_numRows; i++ )
	{
		for( PT_UINT j = 0; j < m_numCols; j++ )
			delete m_StructureCells[ i ][ j ];

		delete [] m_StructureCells[ i ];
		delete m_StructureRows[ i ];
	}

	delete []m_StructureRows;
	delete []m_StructureCells;
}

// don't update any cells, just the row owner
PT_Error PT_Structure::UpdateRowOwner( PT_ULONG rowNum, PT_ID recID, PT_Time time )
{
	if (rowNum >= m_numRows)
	{
		return PT_Error_IndexOutOfBounds;
	}

	StructureRowNode tmp;
	tmp.m_owner = recID;
	tmp.m_ownerTime = time; 

	m_StructureRows[ rowNum ]->InsertElementAtEnd( tmp );

	return PT_Error_OK;
}

// update the contents of a cell at said time. updates the row owner if necessary
PT_Error PT_Structure::UpdateCell( PT_Config_Data *config, PT_ID recID, void *data, PT_ULONG rowNum, PT_Time time )
{
	if (rowNum >= m_numRows)
	{
		return PT_Error_IndexOutOfBounds;
	}
	
	// figure out the column number
	int col = -1;

	for( PT_UINT i = 0; i < m_numCols; i++ )
	{
		if ( m_StructureDefinition->m_colArray[ i ]->m_key == config->m_key )
		{
			col = i; 
			break;
		}
	}

	if (col == -1) 
	{
		// not found...
		return PT_Error_IndexOutOfBounds;
	}

	// now, update the cell with the right value
	StructureCellNode CellNode;
	CellNode.m_allocTime = time;
	CellNode.m_pData = data;

	m_StructureCells[ rowNum ][ col ]->InsertElementAtEnd( CellNode );

	// finally, see if we need to update the owner of this cell
	PT_UINT len = m_StructureRows[ rowNum ]->Length( );
	PT_UINT lastRec = 0; 

	if (len != 0)
	{
		StructureRowNode *tmp = m_StructureRows[ rowNum ]->GetLastElement( );
		lastRec = tmp->m_owner;

		if (tmp->m_ownerTime > time )
		{
			PT_WARNING( "Watch window events must be chronological for a cell!!!" );
			PT_ASSERT( 0 );
		}
	}

	if (len == 0 || recID != lastRec )
	{
		// ownership has just changed!

		StructureRowNode tmp2;
		tmp2.m_owner = recID;
		tmp2.m_ownerTime = time; 

		m_StructureRows[ rowNum ]->InsertElementAtEnd( tmp2 );
	}

	return PT_Error_OK;
}
	
// clear the contents of a cell at said time. updates the row owner if necessary
PT_Error PT_Structure::ClearCell( PT_Config_Data *config, PT_ULONG rowNum, PT_Time time )
{
	if (rowNum >= m_numRows)
	{
		return PT_Error_IndexOutOfBounds;
	}

	// figure out the column number
	int col = -1;

	for( PT_UINT i = 0; i < m_numCols; i++ )
	{
		if ( m_StructureDefinition->m_colArray[i]->m_key == config->m_key )
		{
			col = i; 
			break;
		}
	}

	if (col == -1) 
	{
		// not found...
		return PT_Error_IndexOutOfBounds;
	}

	PT_UINT len = m_StructureCells[ rowNum ][ col ]->Length( );

	if (len == 0)
	{
		// no need to do an explicit clear here, there's nothing there!
		return PT_Error_OK;
	}
	else
	{
		StructureCellNode *tmp = (m_StructureCells[ rowNum ][ col ]->GetLastElement( ));
		if (tmp->m_clearTime == (PT_Time)-1)
		{
			// sanity check that we're not going backwards in time!!!
			if ( time < tmp->m_allocTime )
			{
				PT_WARNING( "Watch window events must be chronological for a cell!!!" );
				PT_ASSERT( 0 );
			}
			
			// clear happens now
			tmp->m_clearTime = time;

			// if this cell has been cleared, and the whole row is now empty, 
			// remove the row owner
			bool foundOne = false;
			
			for( PT_UINT i = 0; i < m_numCols; i++ )
			{
				len = m_StructureCells[ rowNum ][ i ]->Length( );
				if ( len != 0 )
				{
					tmp = m_StructureCells[ rowNum ][ i ]->GetLastElement( );
					if (tmp->m_clearTime == (PT_Time)-1)
					{
						// found a cell that hasn't been cleared!
						foundOne = true;
						break;
					}
				}
			}

			if (!foundOne)
			{
				// remove row owner at this time...
				StructureRowNode tmp;
				tmp.m_owner = 0;
				tmp.m_ownerTime = time; 

				m_StructureRows[ rowNum ]->InsertElementAtEnd( tmp );
			}

			return PT_Error_OK;
		}
		else
		{
			// no clear takes place, b/c the cell has already been cleared!
			return PT_Error_OK;
		}
	}
}

// clear an entire row!
PT_Error PT_Structure::ClearRow( PT_ULONG rowNum, PT_Time time )
{
	if (rowNum >= m_numRows)
	{
		return PT_Error_IndexOutOfBounds;
	}

	StructureCellNode *tmp;

	PT_UINT len;

	// clear each cell
	for( PT_UINT i = 0; i < m_numCols; i++ )
	{
		len = m_StructureCells[ rowNum ][ i ]->Length( );
		if ( len != 0 )
		{
			tmp = m_StructureCells[ rowNum ][ i ]->GetLastElement( );
			if (tmp->m_clearTime == (PT_Time)-1)
			{
				// sanity check that we're not going backwards in time!!!
				if ( time < tmp->m_allocTime )
				{
					PT_WARNING( "Watch window events must be chronological for a cell!!!" );
					PT_ASSERT( 0 );
				}
				
				// found a cell that hasn't been cleared, clear it!
				tmp->m_clearTime = time;
			}
		}
	}

	// finally, make sure that the row owner is cleared!
	StructureRowNode tmp2;
	tmp2.m_owner = 0;
	tmp2.m_ownerTime = time; 

	m_StructureRows[ rowNum ]->InsertElementAtEnd( tmp2 );

	return PT_Error_OK;
}

// Returns start time of the event
PT_Time PT_Event::GetStartTime( class PT_StandardDataStore *store )
{ 
	if ( ! ( lowWord & SMALL_SIZE_MASK ) )
	//if (lowWord < (1 << 15))
	{	
		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		return le->m_eventTime;
	}
	else
	{	
		//PT_SmallEvent *se = (PT_SmallEvent *)(this + 1);
		//PT_ID recID = (se->midDword & 0x000fffff);

		PT_Time start = store->GetRecordStartTime( GetRecordID( )/* recID*/ );
		start = start + (lowWord & 0x1fff);
		return start;
		//return 0;
	}	
}

bool PT_SmallEvent::IsWorthyOfSmallEvent( const PT_Event_Statement *event, const PT_Record *r, bool isLowestTime )
{ 
	PT_Record *r2 = (PT_Record *)r;
	return	event->m_duration <= 2 && 
			event->m_EventDef->m_key < (1 << 12) &&
		    event->m_ID < (1 << 20) && 
			(isLowestTime || (event->m_time - r2->GetMinTime( ) < (1 << 13)) );
}


#endif //__PT_DATA_CPP
  
  




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

#ifndef __PT_DATA_H
#define __PT_DATA_H

#include "PT_Types.h"
#include "PT_IO.h"

// use STL
/*
#ifdef ICC
#include <stllib/hash_map.h>
#else
#include <hash_map.h>
#endif
*/
#include <map>

class PT_Sorter;
class PT_Structure;
class PT_StructArray;
class PT_Event;
class PT_Record;
class PT_DataElement;

typedef std::map< const PT_KEY, PT_Config *, PT_eqstr > KeyMap;
typedef std::map< const char *, PT_Config *, PT_eqstr > NameMap;
typedef std::map< const PT_KEY, PT_StructArray *, PT_eqstr > StructMap;

// keep events in a timetable when we first receive them, until
// we receive all events of the same time
#define TIME_TABLE_SIZE		4096

// num of mem tables we keep around for each structure
#define NUM_MEM_TABLES		1024

// max number of sorters allowed to attach to 1 datastore
#define MAX_SORTERS 			256

// for compression of strings that appear over and over again
#define STRING_REP_TO_SEARCH	5

template < class T >
class PT_Comparer
{
public:
	// Compare two items.
	// Returns: 0 if they are equal, 1 if _one > _two, -1 if _one < _two
	virtual int Compare( T _one, T _two ) = 0;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Template Class PT_SortVector
//	A utility class that makes it very easy to write a custom sorter.
//	Simply provide the Sorter with your own PT_Comparer class and an expected size
//	and it does the following for you:
// 
//	1) It handles the internal array list and grows itself if necessary 
//	   (at a significant perf hit, so choose your initial size wisely).
//	2) It is templated so no casting from void*
//	3) It handles inserting a new element into a sorted array in O(n) worst case 
//	   time. (Also checks to add to end first so in many sorts, expected case 
//	   could be O(1))
//	4) If the array is unsorted, it can QuickSort the elements, expected case
//	   O(n log n)
//
////////////////////////////////////////////////////////////////////////////////
template < class T >
class PT_SortVector
{
public:
	////////////////////////////////////////////////////////////////////////////
	// Contructors and Destructors
	////////////////////////////////////////////////////////////////////////////

	// Primary constructor. You must implement your own PT_Comparer to help
	// with the sort ( see PT_RecordIDSorter example )
	// sets initial size to suggestedSize
	// slow growth means that grow vector resizes by 1.2X not by 2x
	PT_SortVector( PT_Comparer< T > *Comparer, PT_UINT suggestedSize = 4096, bool doSlowGrowth = false );	
	
	// Copy constructor
	PT_SortVector( PT_SortVector< T > *pCopyVector );		  
	
	virtual ~PT_SortVector( ){ if (m_pData) delete m_pData; }

	////////////////////////////////////////////////////////////////////////////
	// User methods
	////////////////////////////////////////////////////////////////////////////
	
	// Add an element to the vector. Uses your compare class to insertion sort.
	// Grows Array if necessary
	PT_Error InsertElement( T pItem );

	// return the location we inserted the element as an out param
	PT_Error InsertElement( T pItem, int *outPos );

	PT_Error InsertElementAtEnd( T pItem );

	// if you are fairly certain that the item you are inserting is either right at 
	// the end, or very close, you may want to use this algorithm
	PT_Error InsertElementNearEnd( T pItem, int *outPos );

	// Just add an element to the vector at the end. Some sorters (like 
	// 'searchers' may not care that the list is sorted). Also, it may be
	// a valid idea to quickly add the element now, and then do the quicksort
	// later.
	// Grows Array if necessary
	inline PT_Error InsertElementNoSort( T pItem ); 
		
	inline PT_Error RemoveAllElements( ){ m_numElements = 0; return PT_Error_OK; }

	inline bool IsEmpty( ){ return m_numElements == 0; }

	// Perform a QuickSort (Assuming it is unsorted)
	PT_Error QuickSortArray( ){return QuickSortArray(0,m_numElements - 1);}
	
	// How many elements in the array?
	inline PT_UINT Length(){ return m_numElements; }

	inline PT_UINT GetAllocatedSize(){ return m_size; }

	// returns NULL if unsuccessful (array index out of bounds)
	// otherwise a POINTER to a T
	inline T  *GetElement( PT_UINT index );
	inline T  *operator[]( PT_UINT index );

	inline T *GetLastElement( ){ return &(m_pData[m_numElements - 1]); }

	void InsertElementAt( T item, PT_UINT index );

private:
	// helper for insertion sort
	void InsertAt( T item, PT_UINT index );

	PT_Error GrowVector( );

	//partition for quicksort
	int Partition( int left, int right );

	// Recursive function
	PT_Error QuickSortArray( int left, int right);

	////////////////////////////////////////////////////////////////////////////
	// User methods
	////////////////////////////////////////////////////////////////////////////

	PT_UINT m_size;				// actual size of the array
	PT_UINT m_numElements;		// number of elements currently in the array

	T*	 m_pData;				// internal storage array

	PT_Comparer< T > *m_Comparer;	// contains comparison function, 
	bool m_bDoSlowGrowth;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Class PT_StructArray
//
//	a simple storage struct that is the value type for the StructHash in the 
//  datastore, that maps from a key to a StructArray
//
////////////////////////////////////////////////////////////////////////////////
class PT_StructArray
{
public:
	PT_StructArray( );
	
	// auto deletes the PT_Structures inside of us!!!
	~PT_StructArray( );

	PT_Structure	**m_array;
	PT_UINT			m_arrayLen;
};


////////////////////////////////////////////////////////////////////////////////
//
//	Class PT_DataStore
//
//	Base class for a data manager for storing record/event info. Derived classes
//  should implement a way to store data with a small memory footprint.
//  Base class implementation does hashing of definitions, so if this is all you
//  need, you can use the base class.
//  Communicates with sorters.
//
////////////////////////////////////////////////////////////////////////////////
class PT_DataStore
{
	friend class PT_RecordIDSorter;
public:
	////////////////////////////////////////////////////////////////////////////
	// Contructors and Destructors
	////////////////////////////////////////////////////////////////////////////

	// Allows you to provide one sorter here. The rest can be adder (right away
	// or later from the AddSorter function)
	// if doWatch = true, then watch window structures will be allocated. otherwise,
	// they won't. set this to PT_FALSE for PT_Writers since they don't need to alloc
	PT_DataStore( PT_Sorter *pSorter, PT_BOOL doWatch = PT_TRUE );
	virtual ~PT_DataStore();

	// Instead of passing a sorter into the constructor, you 
	// can also just add multiple of them here.
	// Setting bUpdateAll to TRUE forces the DataStore to run through all 
	// records and events thus far, and call the sorters handlers for
	// RecordComplete etc...
	// AddSorter invokes the protected virtual function UpdateAll, which
	// must be implemented by the derived class
	PT_Error AddSorter( PT_Sorter *pSorter, PT_BOOL bUpdateAll = PT_FALSE );
	
	// Removes a sorter from lists by matching pointers...
	PT_Error RemoveSorter( PT_Sorter *pSorter );

	////////////////////////////////////////////////////////////////////////////
	//
	// Statement Handlers. These will get called by a ReadStream as it is
	// parsing statements.
	////////////////////////////////////////////////////////////////////////////
	virtual PT_Error OnOpenStatement( const PT_Open_Statement &st );
	virtual PT_Error OnCloseStatement( const PT_Close_Statement &st );
	virtual PT_Error OnCommentStatement( const PT_Comment_Statement &st );
	virtual PT_Error OnEventStatement( const PT_Event_Statement &st );
	virtual PT_Error OnDataStatement( const PT_Data_Statement &st );
	virtual PT_Error OnEventDataStatement( const PT_EventData_Statement &st );
	virtual PT_Error OnConfigRecord( const PT_Config_Record &st );
	virtual PT_Error OnConfigData( const PT_Config_Data &st );
	virtual PT_Error OnConfigEvent( const PT_Config_Event &st );
	virtual PT_Error OnConfigStruct( const PT_Config_Struct &st );
	virtual PT_Error OnUpdateStruct( const PT_Update_Struct &st );
	virtual PT_Error OnClearStruct( const PT_Clear_Struct &st );

	// done reading trace !
	virtual PT_Error OnEndOfTrace( );

	// not implemented in base class
	virtual PT_Event *GetVeryFirstEvent( ){ return NULL; }

	////////////////////////////////////////////////////////////////////////////
	// User methods
	////////////////////////////////////////////////////////////////////////////

	// Useful method for definition lookup
	// Returns PT_Error_NotFound if the hash value doesn't exist
	PT_Error FindConfigByName( const char *NameKey, PT_Config **outConfig );
	PT_Error FindConfigByKey( const PT_KEY ValueKey, PT_Config **outConfig );
	PT_Error FindStructArrayByKey( const PT_KEY ValueKey, PT_StructArray **outArray );

	// How much memory (or disk space) has this DataStore allocated?
	long GetAllocatedSize( );
	
	void SetInterruptFlag( ){m_bInterruptFlag = true;}
	bool GetInterruptFlag( ){return m_bInterruptFlag;}

	PT_UINT GetNumStructTypes( ){ return m_numStructTypes; }
	
	// returns num names found
	PT_UINT GetStructTypeNames( const char **outNames );

	//The reason why we implement this at all is so certain sorters are much faster
	//at retrieving records
	virtual PT_Record *GetRecordByID( PT_ID ID ){ return NULL;} //notimpl
protected:
	
	// are we done reading in the trace
	bool m_bTraceDone;

	// interrupt the trace load
	bool m_bInterruptFlag;

	// derived class must implement, and must call Event and Record complete
	// notifies in the order they were received!!!
	virtual PT_Error UpdateAll( PT_Sorter *pSorter );

	// implement the notification stuff for our derived classes enjoyment
	// only sends notifications to those handlers that sorters register
	PT_Error SendEventComplete(  PT_Event *event );
	PT_Error SendRecordComplete( PT_Record *record );
	PT_Error SendConfigRecordComplete( PT_Config_Record *config );
	PT_Error SendConfigEventComplete( PT_Config_Event *config );
	PT_Error SendConfigDataComplete( PT_Config_Data *config );

	PT_Error SendEndOfTrace( );

	// Hash Definitions both by Key and by Name
	// Key is necessary for compact storage of Definitions... Keys are only
	// 2 bytes long (2^16 definitions), whereas pointers to definitions are 4 bytes.
	// So this saves 2 bytes of data for every record, event, and data element, 
	// meaning that you could easily save 100 bytes in a record...
	// The tradeoff is lookup performance. However, you should never have to perform
	// O(n) lookups in a search for specific records. For example, if you want all
	// records of type t, just search each record for record.definitionKey = key( t )
	KeyMap *m_KeyTable;

	// Hashing by Name is used by PT_ReadStreams to parse statements. 
	// Since there aren't that many definitions, this isn't wasting much space
	NameMap *m_NameTable;

	// Hash by PT_Config_Struct key to get ahold of the PT_Structures created for it
	StructMap *m_StructTable;

	// Resource management: must store string arrays somewhere in datastore. 
	// Keep it small, and then grow the array if necessary.
	// PSLib used a mempool to do all this, but the inefficiencies of the mempool
	// almost equal the overhead of using 'new'. plus there aren't that many definitions
	// in the grand scheme of things
	PT_Error FreeStringTable();
	PT_Error FreeEnumTable();
	PT_Error InitLookupTables();
	PT_Error AddToStringTable( const char **szVal );	
	PT_Error AddToEnumTable( char ***enumArray, PT_ULONG arrayLen );

	char	***m_EnumTable;			// contains arrays that points to enum elements
//	std::vector<char **> m_EnumTable;
	PT_ULONG   m_enumNumElements;	// how many items currently allocated?
	PT_ULONG   m_enumRealSize;		// how big is the array (need to grow it?)

	char	 **m_StringTable;		// contains string allocations
//	std::vector<char *> m_StringTable;
	PT_ULONG   m_stringNumElements;	// how many items currently allocated?
	PT_ULONG   m_stringRealSize;	// how big is the array (need to grow it?)

	// list of all sorters that want RecordComplete notifications
	PT_Sorter *m_RecordSorters[ MAX_SORTERS ]; 
	PT_UINT	  m_numRecordSorters;

	// list of all sorters that want EventComplete notifications
	PT_Sorter *m_EventSorters[ MAX_SORTERS ];
	PT_UINT	  m_numEventSorters;

	// list of all sorters that want ConfigRecordComplete notifications
	PT_Sorter *m_ConfigRecordSorters[ MAX_SORTERS ];
	PT_UINT	  m_numConfigRecordSorters;

	// list of all sorters that want ConfigEventComplete notifications
	PT_Sorter *m_ConfigEventSorters[ MAX_SORTERS ];
	PT_UINT	  m_numConfigEventSorters;

	// list of all sorters that want ConfigDataComplete notifications
	PT_Sorter *m_ConfigDataSorters[ MAX_SORTERS ]; 
	PT_UINT	  m_numConfigDataSorters;

	// to send end of trace notifies
	PT_Sorter *m_AllSorters[ MAX_SORTERS ]; 
	PT_UINT	  m_numAllSorters;

	// alloc watch window structures? writers should say NO
	bool	  m_bDoWatch;
	PT_UINT	  m_numStructTypes;

	// quick lookup for config key for "Thread"
	int m_threadKey;

	// quick lookup for config key for "Parent"
	int m_parentKey;

	// quick lookup for config key for "Inst_ID"
	int m_InstIDKey;

	// quick lookup for config key for "UID"
	int m_UIDKey;

};


////////////////////////////////////////////////////////////////////////////////
//
//	Class PT_StandardDataStore
//
//	Standard implementation of a datastore. Robust, to be used
//  in a PTV type application. Perfchecker too?
//
////////////////////////////////////////////////////////////////////////////////
class PT_StandardDataStore : public PT_DataStore
{
public:
	friend class PT_Record;
	
	// Allows you to provide one sorter here. The rest can be added (right away
	// or later from the AddSorter function)
	PT_StandardDataStore( PT_Sorter *pSorter = NULL, 
						   PT_Error *Error = NULL
						   );

	// set up the data tables. called by constructor
	PT_Error InitDataStructures( );	

	virtual ~PT_StandardDataStore();

	virtual PT_Error OnOpenStatement( const PT_Open_Statement &st );
	virtual PT_Error OnCloseStatement( const PT_Close_Statement &st );
	virtual PT_Error OnEventStatement( const PT_Event_Statement &st );
	virtual PT_Error OnDataStatement( const PT_Data_Statement &st );
	virtual PT_Error OnEventDataStatement( const PT_EventData_Statement &st );
	virtual PT_Error OnUpdateStruct( const PT_Update_Struct &st );
	virtual PT_Error OnClearStruct( const PT_Clear_Struct &st );

	// done reading trace !
	virtual PT_Error OnEndOfTrace( );

	virtual PT_Event *GetVeryFirstEvent( ){ return m_veryFirstEvent; };

	// for keeping track of mem footprint size
	virtual int GetEventPages( ){ return m_currentEventPage; }
	virtual int GetDataPages( ){ return m_currentDataPage; }

	inline PT_Time GetRecordStartTime( PT_ID recID ){ return GetRecordByID( recID )->GetMinTime( );}
	// The reason why we implement this at all is so certain sorters are much faster
	// at retrieving records
	virtual PT_Record *GetRecordByID( PT_ID ID );

	inline PT_Time GetMinTime( ){ return m_minTime; }
	inline PT_Time GetMaxTime( ){ return m_maxTime; }

	// should only be called by PT_DataList to get its key table if it is using a cached one
	inline PT_USHORT GetKeyDataSize( PT_KEY key ){ return m_dataKeyTableSize[ key ]; }
	inline PT_KEY *GetKeyTable( PT_KEY key ){ return m_dataKeyTable[ key ]; }

protected:
	// derived class must implement, and must call Event and Record complete
	// notifies in the order they were received!!!
	virtual PT_Error UpdateAll( PT_Sorter *pSorter );
	

	// force close an open record
	virtual void ForceClose( PT_Record *record );

	// See above description
	// Param 1 is an in/out parameter, that increments if the next event
	// is on a different page, currentIndex is in/out as well, return val is
	// NULL if no next event
	virtual PT_Event  *GetNextEvent( PT_ULONG *currentPage, PT_ULONG *currentIndex, 
									 PT_Event *currentEvent );

	// When an event is complete, do a fast insertion of it based upon it's start time
	// and record ID (2ndary criteria)
	PT_Error SortEventByTime( PT_Event *event );

	// First event in time in the entire trace
	PT_Event *m_veryFirstEvent;
	
	// largest time value we have seen
	PT_Time m_maxTime;
	
	// frees data tables. called by destructor
	PT_Error FreeDataStructures( );

	// copies DataList from temp buffer into actual one... algo depends on
	// if we are using a Key table, or reusing the cached one
	void CopyData( PT_KEY key, PT_DataList *seq, bool needType );
	bool TypesEqual( PT_KEY key );

	// store the last 5 times a string appears for a given key, since it will repeat often,
	// we should be able to avoid a lot of strign redundancy
	char *m_StringCompressionTable[ 8192 ][ STRING_REP_TO_SEARCH ];

	// take the data from a statement, convert it to a void*, and alloc
	// memory in the data table for strings and byte arrays
	PT_Error GetDataVoid( const PT_Data_Statement &st, void **retVal );

	// allocation methods. some of the internal allocating is fairly complex..
	// especially w/ events b/c of the TimeTable
	char *AllocData( PT_ULONG dataSize );

	// if AddDataPtr, then alloc sizeof(PT_Event) + room for a pointer to a 
	// datalist
	PT_Event  *AllocNewEvent( bool AddDataPtr, bool isSmallEvent );

	PT_Record *AllocNewRecord( );

	PT_Event	*m_CurrentEvent;	//pointer to the last event we worked with
	
	// Events in the EventTable are sorted by start time. However, within the same
	// start time, we sort by record ID, (and then a 3rd sort?), so we keep a dictionary
	// here for fast insertions by time
	PT_Event	**TimeDictionary;

	// counters for footprint analysis
	PT_UINT m_numSmallEvents;
	PT_UINT m_numLargeEvents;
	PT_UINT m_numNonCachedTypes;
	PT_UINT m_numCachedTypes;

	// while event data is still coming in, store the PT_DataList buffer temporarily here
	PT_UCHAR TempDataList[ 4096 ];

	// since most events with data of the same type use the same exact data types
	// reuse them in almost all cases by caching here
	PT_KEY    *m_dataKeyTable[ 8192 ];
	PT_USHORT m_dataKeyTableSize[ 8192 ];

	// here's where we keep our super huge data tables.
	
	// PT_DataList, string data and ByteArray data go here!
	char		*DataTable[ NUM_MEM_TABLES ];
	PT_USHORT	m_currentDataPage;	  // Current page we are allocating data on

	// How much memory have we used up in the current page?
	PT_ULONG	m_currentDataPageSize;

	// PT_Events go here. Its a shame we have to use char * instead of PT_Event *, but
	// b/c we store the extra pointer if necessary at the end of the event, we need
	// to do pointer math
	char		*EventTable[ NUM_MEM_TABLES ];
	PT_USHORT	m_currentEventPage;	 // Current page we are allocating data on

	// How much memory have we used up in the current page?
	PT_ULONG	m_currentEventPageSize;

	PT_ULONG	EventIndexTable[ NUM_MEM_TABLES ];
	
	// PT_Records go here
	PT_Record	*RecordTable[ NUM_MEM_TABLES ];
	PT_USHORT	m_currentRecordPage; // Current page we are allocating data on
	PT_ULONG	m_numAllocRecords;	
	
	// Keep track of last event for the sake of adding EventData
	PT_Event	*m_pCurrentEvent;

	// Keep track of current EventData to receive in order to add data to 
	// the right index
	PT_ULONG	m_EventDataLeft;

	// both Update_Structs and Event_Statements use EventData, so let the event data
	// know which one it is working with
	bool		m_bIsEvent;	

	// Keep track of last struct for the sake of adding EventData on UpdateStructs
	PT_Structure	*m_pCurrentStruct;
	PT_Time			m_CurrentStructTime;
	PT_UINT			m_CurrentStructRow;

	PT_Time m_minTime;
};


////////////////////////////////////////////////////////////////////////////////
//
// Class PT_Sorter
//
// Wheras PT_DataStores provide an efficient way of storing record information, 
// PT_Sorters provide an efficient way of retrieving information. They allow 
// the user to enumerate records, events, data or definitions. Several sorters 
// are provided with the PTLib+ library. They include record sorters by ID#, 
// event sorters by start time, and others. Users can access records by ID index 
// using the the GetByIndex method or the operate []. Many application will want 
// to write custom sorters, so implementing a Sorter has been made as simple as 
// possible. Default implementations of Sorter does nothing for all handlers. 
// PTLib also provides utility routines for insertion sort, mergesort and 
// quicksort.
// 
////////////////////////////////////////////////////////////////////////////////
class PT_Sorter
{
public:
	
	////////////////////////////////////////////////////////////////////////////
	// Contructors and Destructors
	////////////////////////////////////////////////////////////////////////////
	virtual ~PT_Sorter() {}

	////////////////////////////////////////////////////////////////////////////
	// The following methods are used by the DataStore
	// to notify the sorter of events
	// The sorter must override at least one of these methods if it wants to 
	// handle update notifications from the DataStore
	////////////////////////////////////////////////////////////////////////////	
	virtual PT_Error OnRecordComplete( PT_Record *pNewRecord);
	virtual PT_Error OnEventComplete( PT_Event *pNewEvent);
	virtual PT_Error OnRecordConfigComplete( PT_Config_Record *pNewConfig);
	virtual PT_Error OnEventConfigComplete( PT_Config_Event *pNewConfig);
	virtual PT_Error OnDataConfigComplete( PT_Config_Data *pNewConfig);
	
	// always receive this notification regardless
	virtual PT_Error OnTraceComplete( );

	// Notifies the DataStore which events the derived sorter implements, so that 
	// the DataStore doesn't waste a lot of time calling methods that aren't
	// implemented. The derived class implements this by implementing
	// SetImplementBools( ). RegisterEvents should not be overrided.
	PT_UCHAR RegisterEvents( );
	
	// SetImplementBools is kept pure virtual here as a 
	// reminder to the derived class implementor
	virtual void SetImplementBools( ) = 0;

	////////////////////////////////////////////////////////////////////////////
	// The following methods are exposed to the user for enumerating the 
	// items currently in the sorter
	////////////////////////////////////////////////////////////////////////////
	
	// How many items are there so far? Remember, this number can grow as the 
	// search continues
	virtual PT_UINT Length() = 0;

	// -The following methods retrieve an item from the Sorter.
	// -The Sorter must override at least one of these methods
	// -It is up to the user to know what type of Sorter they are using, since 
	//  the sorter will likely only override one of the following functions
	//  (i.e. a RecordSorter will only override the PT_Record 
	//  version of GetByIndex). The rest will return PT_Error_NotImpl
	//  Out Param will be returned as NULL if the item at the index has not
	//  been filled by the sorter yet
	virtual PT_Error GetByIndex( PT_UINT index, PT_Record **retVal );
	virtual PT_Error GetByIndex( PT_UINT index, PT_Event **retVal );
	virtual PT_Error GetByIndex( PT_UINT index, PT_Config_Record **retVal );
	virtual PT_Error GetByIndex( PT_UINT index, PT_Config_Event **retVal );
	virtual PT_Error GetByIndex( PT_UINT index, PT_Config_Data **retVal );

	//  Side note about above functions. At first, I was going to make 
	//  PT_Sorter a templated class instead. But this complicates things when 
	//  you are passing a list of sorters the DataStore. Another option would be 
	//  to create a base class for all types, but this would consume too 
	//  much space

protected:
	//  These values must be set by the derived class in SetImplementBools
	bool m_bImplementsRecordComplete;
	bool m_bImplementsEventComplete;
	bool m_bImplementsRecordConfigComplete;
	bool m_bImplementsEventConfigComplete;
	bool m_bImplementsDataConfigComplete;
};

// see PT_Structure
class StructureCellNode
{
public:
	StructureCellNode( ){m_allocTime = m_clearTime = (PT_Time)-1; }

	void *m_pData;	// if String or array then this is a pointer, else its the actual
					// data
	PT_Time	m_allocTime;	// time this node was updated to this value
	PT_Time m_clearTime;	// if next cell action was a clear, this value is the time of clear. else = -1
};

// see PT_Structure - since only 1 record can "own" a structs row, we maintain the owner here, and not
// in the cell to save memory
class StructureRowNode
{
public:
	StructureRowNode( ){ m_owner = 0; m_ownerTime = 0; }

	PT_ID	m_owner;		// record that owns the row, if = 0, then no owner
	PT_Time m_ownerTime;
};


// does nothing for now
class WatchRowComparer : public PT_Comparer< StructureRowNode > 
{
	virtual int Compare( StructureRowNode _one, StructureRowNode _two )
	{
		return 0; //one == two
	}
};

// does nothing for now
class WatchCellComparer : public PT_Comparer< StructureCellNode > 
{
	virtual int Compare( StructureCellNode _one, StructureCellNode _two )
	{
		return 0; //one == two
	}
};

////////////////////////////////////////////////////////////////////////////////
//
// Class PT_Structure
//
//	PT_Structures provide the storage mechanism for watch windows. they are 
//  fairly lightweight, and make it easy to efficiently find the next and previous 
//  update of a window in time
// 
////////////////////////////////////////////////////////////////////////////////
class PT_Structure
{
public:
	// default constructor
	PT_Structure( PT_Config_Struct *st );

	virtual ~PT_Structure( );

	PT_SortVector< StructureCellNode > *GetCellSortVector( PT_UINT row, PT_UINT col )
	{
		if (row >= m_numRows || col >= m_numCols) return NULL;
		else return m_StructureCells[ row ][ col ];
	}

	PT_SortVector< StructureRowNode > *GetRowSortVector( PT_UINT row )
	{
		if (row >= m_numRows) return NULL;
		else return m_StructureRows[ row ];
	}

	// update the contents of a cell at said time. updates the row owner if necessary
	PT_Error UpdateCell( PT_Config_Data *config, PT_ID recID, void *data, PT_ULONG rowNum, PT_Time time );
	
	// clear the contents of a cell at said time. updates the row owner if necessary	
	// don't update any cells, just the row owner
	PT_Error UpdateRowOwner( PT_ULONG rowNum, PT_ID recID, PT_Time time );

	PT_Error ClearCell( PT_Config_Data *st, PT_ULONG rowNum, PT_Time time );

	// clear an entire row!
	PT_Error ClearRow( PT_ULONG rowNum, PT_Time time );

	PT_Config_Struct	*GetStructureDefinition( ){ return m_StructureDefinition;}

private:
	PT_SortVector< StructureCellNode >	***m_StructureCells;	// this 2D array holds all updates and 
																// clears chronologically to each cell
	
	PT_SortVector< StructureRowNode >	**m_StructureRows;	// holds changes to the owner of a cell

	PT_ULONG m_numRows;
	PT_ULONG m_numCols;

	PT_Config_Struct	*m_StructureDefinition;
};


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_SortVector::PT_SortVector
// Description: Standard Constructor
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
template<class T>
PT_SortVector< T >::PT_SortVector( PT_Comparer< T > *Comparer, PT_UINT suggestedSize, bool doSlowGrowth )
{
	m_Comparer = Comparer;
	m_size = suggestedSize;
	m_numElements = 0;
	m_bDoSlowGrowth = doSlowGrowth;

	// now alloc memory
	m_pData = new T[ suggestedSize ];

	if ( m_pData == NULL )
	{
		// uh oh, low on memory!!!
		PT_FATAL("PT_SortVector::Ran out of memory!");
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_SortVector::GetElement and operator[]
// Description: accessor methods, the latter allows you to do
//				T myT = MyVector[ 5 ]
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
template< class T >
T  *PT_SortVector<T>::GetElement( PT_UINT index )
{
	if ( index >= m_numElements ) return NULL;

	return &(m_pData[ index ]);
}

template< class T >
T  *PT_SortVector<T>::operator[]( PT_UINT index )
{
	return GetElement( index );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_SortVector::InsertElement
// Description: 
// Add an element to the vector. Uses your compare class to insertion sort.
// Grows Array if necessary
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
template<class T>
PT_Error PT_SortVector<T>::InsertElement( T pItem )
{
	int ret; 
	return InsertElement( pItem, &ret );
}

template<class T>
PT_Error PT_SortVector<T>::InsertElement( T pItem, int *outPos )
{	

	if ( m_size == m_numElements )
	{
		PT_Error Error = GrowVector( );
		if (PT_FAILED( Error )) return Error;
	}

	// now do the actual insert, quite often, the items will 
	// be already sorted, so always check the last element first
	// so that we can make insertion sort O(n) and not O(n^2)
	if ((m_numElements == 0) || ( m_Comparer->Compare( pItem, m_pData[ m_numElements - 1 ] ) == 1 ))
	{
		//pItem is greater than the previously largest element. insert at end
		InsertAt( pItem, m_numElements);
		*outPos = m_numElements;
	}
	else
	{

//		// do the insertion sort -> always O(n) per insertion :-(
//#define SLOW_INSERT 
#ifdef SLOW_INSERT
		T temp;
		T temp2;

		for( int i = 0; i < m_numElements; i++ )
		{
			if (m_Comparer->Compare( pItem, m_pData[ i ]) == -1 )
			{
				//insert pItem at this position and move the rest down
				temp = m_pData[ i ];
				m_pData[ i ] = pItem;
				
				while( i < m_numElements )
				{
					temp2 = temp;
					temp = m_pData[ i + 1 ];
					m_pData[ i + 1 ] = temp2;
					i++;
				}

				m_numElements++;

				break;
			}
			else if ( i == (m_numElements - 1) )
			{
				// add to tail
				m_pData[ m_numElements ] = pItem;
				m_numElements++;
			}
		}
#else
		// 2/25/02 Too Damn slow!!! Switched this to a binary insertion so that each
		// insertion is log( n )
		int lowElement = 0;
		int highElement = m_numElements - 1;
		int midElement = 0;
		int compareVal;

		while( lowElement <= highElement )
		{
			midElement = ( lowElement + highElement ) / 2;

			compareVal = m_Comparer->Compare( pItem, m_pData[ midElement ]);

			if ( compareVal == -1 )
			{
				highElement = midElement - 1;
			}
			else if ( compareVal == 1 )
			{
				lowElement = midElement + 1;
			}

			else
			{
				// its equal to 
				InsertAt( pItem, midElement );
				*outPos = midElement;
				return PT_Error_OK;
			}

		}

		// do one final compare to find insertion point

		if ( highElement == lowElement )
		{
			if ( m_Comparer->Compare( pItem, m_pData[ highElement ] ) == -1  )
			{
				//less than, insert at this position
				InsertAt( pItem, highElement );
				*outPos = midElement;
			}
			else
			{
				InsertAt( pItem, highElement + 1);
				*outPos = midElement;
			}
		}

		// if not equal, high is really < low
		else 
		{
			InsertAt( pItem, lowElement );
			*outPos = midElement;
		}
/*		
		else if ( (highElement >= 0) && (m_Comparer->Compare( pItem, m_pData[ highElement ] ) == -1)  )
		{
			if (highElement < 0)
				InsertAt( pItem, 0 );
			else
				InsertAt( pItem, highElement );
		}

		else if ( m_Comparer->Compare( pItem, m_pData[ lowElement ] ) == -1  )
		{
			InsertAt( pItem, lowElement );
		}

		else
		{ 
			InsertAt( pItem, lowElement + 1 );
		}
*/
#endif
	}

	return PT_Error_OK;
}

template< class T >
void PT_SortVector<T>::InsertAt( T item, PT_UINT index )
{
	T temp = m_pData[ index ];
	T temp2;

	m_pData[ index ] = item;
	
	//memmove( &m_pData[ index + 1 ], &m_pData[ index ], (m_numElements - index) * sizeof(T) );

	while( index < m_numElements )
	{
		temp2 = temp;
		temp = m_pData[ index + 1 ];
		m_pData[ index + 1 ] = temp2;
		index++;
	}

	m_numElements++;
}

template<class T>
void PT_SortVector<T>::InsertElementAt( T item, PT_UINT index )
{	

	if ( m_size == m_numElements )
	{
		PT_Error Error = GrowVector( );
		if (PT_FAILED( Error )) return;
	}

	InsertAt( item, index );
}

// if you are fairly certain that the item you are inserting is either right at 
// the end, or very close, you may want to use this algorithm
template<class T>
PT_Error PT_SortVector<T>::InsertElementNearEnd( T pItem, int *outPos )
{
	if ( m_size == m_numElements )
	{
		PT_Error Error = GrowVector( );
		if (PT_FAILED( Error )) return Error;
	}

	// best case ( hopefully expected case - 1 compare, worst case O(n)
	int index = (int)m_numElements;
	
	while( index > 0 && m_Comparer->Compare( pItem, m_pData[ index - 1 ] ) == -1 )
	{
		index--;
	}

	InsertAt( pItem, index );
	*outPos = index;

	return PT_Error_OK;
}

template<class T>
PT_Error PT_SortVector<T>::InsertElementAtEnd( T pItem )
{
	if ( m_size == m_numElements )
	{
		PT_Error Error = GrowVector( );
		if (PT_FAILED( Error )) return Error;
	}

	InsertAt( pItem, m_numElements );

	return PT_Error_OK;
}

template< class T >
PT_Error PT_SortVector<T>::GrowVector( )
{
	// need to grow the array

	if (!m_bDoSlowGrowth)
	{
		T *tempData = new T[ m_size * 2 ];

		if ( tempData == NULL ) 
		{
			PT_WARNING("PT_SortVector::Ran out of memory!");
			return PT_Error_OutOfMemory;
		}

		// copy data
		for( PT_UINT i = 0; i < m_numElements; i++ )
		{
			tempData[ i ] = m_pData[ i ];
		}

		m_size = m_size * 2;

		T* temp2 = m_pData;
		m_pData = tempData;
		delete []temp2;
	}
	else
	{
		// grow slowly
		T *tempData = new T[ (m_size * 5) / 4 + 1 ];

		if ( tempData == NULL ) 
		{
			PT_WARNING("PT_SortVector::Ran out of memory!");
			return PT_Error_OutOfMemory;
		}

		// copy data
		for( PT_UINT i = 0; i < m_numElements; i++ )
		{
			tempData[ i ] = m_pData[ i ];
		}

		m_size = (m_size * 5) / 4 + 1;

		T* temp2 = m_pData;
		m_pData = tempData;
		delete []temp2;
	}

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_SortVector::InsertElementNoSort
// Description: Just sticks it on the end, grows if necessary
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
template<class T>
PT_Error PT_SortVector<T>::InsertElementNoSort( T pItem )
{	
	if ( m_size == m_numElements )
	{
		PT_Error Error = GrowVector( );
		if (PT_FAILED( Error )) return Error;
	}

	//pItem is greater than the previously largest element. insert at end
	m_pData[ m_numElements ] = pItem;
	m_numElements++;

	return PT_Error_OK;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_SortVector::PT_SortVector
// Description: Copy Constructor - copies all mem
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
template< class T >
PT_SortVector<T>::PT_SortVector( PT_SortVector< T > *pCopyVector )
{
	m_Comparer = pCopyVector->m_Comparer;
	m_size = pCopyVector->suggestedSize;
	m_numElements = pCopyVector->m_numElements;

	// now alloc memory
	m_pData = new T[ m_size ];

	if ( m_pData == NULL )
	{
		// uh oh, low on memory!!!
		PT_FATAL("PT_SortVector::Ran out of memory!");
	}

	for ( int i = 0; i < m_numElements; i++ )
	{
		m_pData[ i ] = pCopyVector->m_pData[ i ];
	}
}

template< class T >
int PT_SortVector<T>::Partition( int left, int right )
{
	static int THRESHOLD = 0;

	if ( (right - left) > THRESHOLD ) 
	{

		T pivot = m_pData[ (left + right) / 2 ];
		T temp;
		int L = left;
		int R = right;

		while( L < R )
		{
			if (m_Comparer->Compare( pivot, m_pData[L] ) > 0) L++;
			else
			{
				R--;
				temp = m_pData[ L ];
				m_pData[ L ] = m_pData[ R ];
				m_pData[ R ] = temp;
			}
		}

		temp = m_pData[ R ];
		m_pData[ R ] = m_pData[ right ];
		m_pData[ right ] = temp;

		return L;
	}
	else
	{
	}
/*
	int pivotIndex = (left + right)/2;
	int diff = right - left;
	
	

	T pivot;
	T pivot2;
	T pivot3; 
	T pivot4;
	int c1, c2, c3;

	if (diff >= THRESHOLD)
	{
		pivot4 =  m_pData[ left ];
		pivot2 = m_pData[ pivotIndex ];
		pivot3 = m_pData[ right ];
		
		c1 = m_Comparer->Compare( pivot4, pivot2 );
		if ( c1 > 0 )
		{
			c2 = m_Comparer->Compare( pivot4, pivot3 );
			if (c2 > 0)
			{
				c3 = m_Comparer->Compare( pivot2, pivot3 );
				if (c3 > 0) pivot = pivot2;
				else pivot = pivot3;
			}
			else pivot = pivot4; 
		}
		else
		{
			c2 = m_Comparer->Compare( pivot4, pivot3 );
			if (c2 >= 0) pivot = pivot4; 
			else 
			{
				c3 = m_Comparer->Compare( pivot2, pivot3 );
				if (c3 > 0) pivot = pivot3;
				else pivot = pivot2;
			}
		}
	
		int L = left;
		int R = right;
		T temp;
		int middle;

		// we already calculated this
		if ( (diff >= THRESHOLD) && (L < R) && (L <= right) && (pivot == pivot4) ) L++;
		if ( (diff >= THRESHOLD) && (L < R) && (L <= right) && (pivot == pivot3) ) R--;

		while( L < R ) 
		{
			while( ( L <= right ) && (m_Comparer->Compare( pivot, m_pData[L] ) >= 0) ) L++;
			while( ( R >= left ) && (m_Comparer->Compare( pivot, m_pData[R] ) < 0) ) R--;

			if ( L < R )
			{
				// swap
				temp = m_pData[ L ];
				m_pData[ L ] = m_pData[ R ];
				m_pData[ R ] = temp;
			}
		}

		middle = R;

		// swap pivot with X[middle]
		temp = m_pData[ left ];
		m_pData[ left ] = m_pData[ middle ];
		m_pData[ middle ] = temp;

		return middle;
	}
	else
	{
		//less than threshold, do some other sort
	}*/
}

template< class T >
PT_Error PT_SortVector<T>::QuickSortArray( int left, int right)
{
	if ( left < right )
	{
		PT_UINT middle = Partition(left, right);
		QuickSortArray(left, middle - 1);
		QuickSortArray(middle + 1, right);
	}

	return PT_Error_OK;
}


#endif  //__PT_DATA_H
  






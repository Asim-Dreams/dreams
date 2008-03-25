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

#ifndef __PT_TYPES_H
#define __PT_TYPES_H

#ifndef PT_MAX_PATH
#define PT_MAX_PATH		4096
#endif


///////////////////////////////////////////////////////////////////////////////
// Simple types
///////////////////////////////////////////////////////////////////////////////

// Convenient notation for unsigned types.
typedef	unsigned int	PT_UINT;
typedef	unsigned long	PT_ULONG;
typedef	unsigned short	PT_USHORT;
typedef	unsigned char	PT_UCHAR;

#ifndef NULL
#define NULL	0L
#endif

// This type is used to specify which record to emit events/data to.
#ifndef PT_ID	
#define PT_ID	PT_ULONG
#endif

// This type is used to specify which record to emit events/data to.
#ifndef PT_Time	
#define PT_Time	PT_UINT
#endif

#ifndef PT_KEY
#define PT_KEY	PT_USHORT
#endif

// BOOL type
typedef enum{
	PT_FALSE = 0,
	PT_TRUE = 1
} PT_BOOL;

///////////////////////////////////////////////////////////////////////////////
// Enumerated types
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Types of "statements" in pipetrace files. This used to be the pipe_field_t
// type, and was mostly used for writing and reading statements.
// However, the new lib uses PT_Statements for reading and writing type.
// This enum is kept around so that the user can get type information from 
// some class derived from PT_Statement (to do static casting). 
///////////////////////////////////////////////////////////////////////////////

// Enum of all Error types
// PT_Errors are returned by almost all PTLib+ methods
///////////////////////////////////////////////////////////////////////////////

typedef enum {
	PT_Error_OK					= 0,	// No Error
	PT_Error_Fail				= 1,	// Generic Fail
	PT_Error_NotImpl			= 2,	// Not implemented yet
	PT_Error_FileOpenFailed		= 3,
	PT_Error_FileCloseFailed	= 4,
	PT_Error_NullPointer		= 5,	// NULL pointer can't be used as a param here
	PT_Error_BadID				= 6,	// Bad Record ID was used in a Statement
	PT_Error_BadStatement		= 7,	// Statement was not initialized properly
	PT_Error_BadIndex			= 8,	// Array index is invalid
	PT_Error_WriteFailed		= 9,	// Generic write failure
	PT_Error_HashInitFailed		= 10,
	PT_Error_OutOfMemory		= 11,
	PT_Error_IncompleteHeader	= 12,	// File header does not contain enough info
	PT_Error_HashExists			= 13,	// Value already exists in table, can't add this new value
	PT_Error_HashNotFound		= 14,	// Value not found when looking up...
	PT_Error_EOF				= 15,	// End of file reached
	PT_Error_InvalidReadStatement = 16,	// Parsed statement isn't proper format or syntax
	PT_Error_EventOutOfOrder	  = 17, // Events must be passed into DataStores in sequential time order
	PT_Error_TooManySorters		  = 18, // A DataStore can only be passed a max of 256 sorters
	PT_Error_Interrupted		  = 19, // Operation was canceled while processing
	PT_Error_IndexOutOfBounds	  = 20, // Index into a watch window structure was out of bounds
	PT_Error_MaxError 
} PT_Error;

// Success and failure macros 
#define PT_SUCCEEDED( x )	( x == PT_Error_OK ) ? 1:0
#define PT_FAILED( x )		( x != PT_Error_OK ) ? 1:0


typedef enum {
        Pipe_Invalid_Type   = 0,
        Pipe_Open           = 1,
        Pipe_Close          = 2,
        Pipe_Data           = 3,
        Pipe_Event          = 4,
        Pipe_Config_Data    = 5,	// Configuration record for a data element 
        Pipe_Config_Event   = 6,    // Configuration record for an event element 
        Pipe_Config_Record  = 7,    // Configuration record for an record type element
        Pipe_Record         = 8,    // Internal use only. NOT a statement type
        Pipe_Enum_Value     = 9,    // Internal use only. NOT a statement type
		Pipe_Comment		= 10,
		Pipe_EventData		= 11,	// msblacks- new type, allows sanity checking for events
		Pipe_Config_Struct	= 12,	// msblacks- new type, define a watch window struct
		Pipe_Update_Struct	= 13,	// msblacks- update (alloc) a watch window row or cell
		Pipe_Clear_Struct	= 14,	// msblacks- clear (dealloc) a watch window row or cell
        Pipe_Max_Record_Types
} Statement_Type;

#include "PT_Base_Data.h"
#include "PT_Colors.h"

class PT_StandardDataStore;

// Print out data type as a string representation (like "ByteArray")
const char *DataTypeToString( Base_Data_Type dt );

//
// Pipe File Formats supported. Currently, just ASCII and binary
///////////////////////////////////////////////////////////////////////////////
typedef enum {
		Pipe_ASCII_Format			= 0,
		Pipe_Binary_Format			= 1,
		Pipe_Max_Formats
} Pipe_Format;


///////////////////////////////////////////////////////////////////////////////
// "Statement" types. A statement is equivalent to one line in an ASCII 
// pipetrace and has a type that corresponds to one of the Statement_Types.
// Other than a type, a statement various data specific to its type. 
//
// All statement types are derived from PT_Statement
///////////////////////////////////////////////////////////////////////////////

//
// PT_Statement
///////////////////////////////////////////////////////////////////////////////
class PT_Statement 
{
public:

	friend class PT_ReaderStream;
	friend class PT_WriterStream;

	virtual ~PT_Statement() {}

	////////////////////////////////////////////////////////////////////////////
	// User Methods
	////////////////////////////////////////////////////////////////////////////
	
	//get type info about the statement
	virtual Statement_Type GetType(  ) = 0;

	// fills an input string with string info about this object suitable for 
	// debug
	// returns same pointer as in, just used for easy function passing
	virtual char *ToString( char *szIn ) = 0;
};


//
// PT_Comment_Statement - Emit a comment to the pipetrace
///////////////////////////////////////////////////////////////////////////////
class PT_Comment_Statement : public PT_Statement
{
public:	

	PT_Comment_Statement( ) : m_szComment( NULL ) {}

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Comment_Statement( const PT_Comment_Statement &st ) { *this = st; } 

	// Constructor 
	PT_Comment_Statement( char * szComment );

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Comment; }
	
	// Debug String
	char *ToString( char *szIn );

	////////////////////////////////////////////////////////////////////////////
	// Data Members.
	// Kept public so that it is easy to access - not an issue since statements
	// usually have very short lifetimes
	////////////////////////////////////////////////////////////////////////////
	char	*m_szComment;
};

//
// PT_Config - Definition type - Config_Data, Config_Event, or Config_Record
// This class does nothing beyond PT_Statement, but is used for improved
// compile time type checking when a Config type is needed instead of just
// any statement
///////////////////////////////////////////////////////////////////////////////
class PT_Config : public PT_Statement
{
public:
	PT_Config() { m_key = 0; }

	// Statement Type
	virtual Statement_Type GetType( ) { return Pipe_Invalid_Type; }
	virtual char *ToString( char *szIn ) { return "PT_Config class\n"; }

	PT_KEY m_key;
};


//
// PT_Config_Event - Declare a new event definition
// Note that this name does not have the underscore attached to it.
// That's because Configs are used as by both writers as statements,
// *and* by Sorters for enumerations
///////////////////////////////////////////////////////////////////////////////
class PT_Config_Event : public PT_Config
{
public:	

	PT_Config_Event( ) : m_event_name( NULL ),  
                m_event_char( '\0' ), 
                m_event_color( Pipe_No_Color ),
                m_description( NULL ){ }

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Config_Event( const PT_Config_Event &st ) { *this = st; } 
	
	// Constructor 
	PT_Config_Event( const char * event_name, char event_char, 
                         Pipe_Color event_color, const char *description );

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Config_Event; }
	
	// Debug String
	char *ToString( char *szIn );

	////////////////////////////////////////////////////////////////////////////
	// Data Members.
	// Kept public so that it is easy to access - 
	// change this for Configs?
	////////////////////////////////////////////////////////////////////////////
        const char                      *m_event_name;		//name of event
        char                             m_event_char;		//char in a graphic display
        Pipe_Color			 m_event_color;		//color in a graphic display
        const char                      *m_description;		//description of the event

//	PT_Event			*m_firstEvent;		//first event in with trace with this type
//	PT_Event			*m_lastEvent;		//last event in trace with this type
};

//
// PT_Config_Data - Declare a new data definition
// Note that this name does not have the underscore attached to it.
// That's because Configs are used as by both writers as statements,
// *and* by Sorters for enumerations
///////////////////////////////////////////////////////////////////////////////
class PT_Config_Data : public PT_Config
{
public:	

	PT_Config_Data() :  m_name( NULL ), m_datatype( Pipe_Invalid_Data ),
						m_description( NULL ), m_arrayLen( 0 ),
						m_enumArray( NULL), m_bRecordData(false) {}

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Config_Data( const PT_Config_Data &st )
	{
		m_name = st.m_name;
		m_key = st.m_key;
		m_datatype = st.m_datatype;
		m_description = st.m_description;
		m_arrayLen = st.m_arrayLen;
		m_enumArray = st.m_enumArray;
		m_bRecordData = st.m_bRecordData;
	} 

	// Constructor... has two extra params for enumeration and array types
	// ByteArray types set arrayLen to array size
	// Enum types pass a enum length plus a string array with 'arrayLen' 
	// strings
	PT_Config_Data( const char *name, Base_Data_Type datatype, const char *description,
                        int arrayLen = 0, char **enumArray = NULL );

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Config_Data; }
	
	// Debug String
	char *ToString( char *szIn );


	////////////////////////////////////////////////////////////////////////////
	// Data Members.
	// Kept public so that it is easy to access - 
	// change this for Configs?
	////////////////////////////////////////////////////////////////////////////
    const char       *m_name;
    Base_Data_Type    m_datatype;
    const char       *m_description;
	
	PT_ULONG		  m_arrayLen;
	char			**m_enumArray;

	// does this data appear in any records, or just events?
	bool			  m_bRecordData;
};


//
// PT_Config_Record - Declare a new record
// Note that this name does not have the underscore attached to it.
// That's because Configs are used as by both writers as statements,
// *and* by Sorters for enumerations
///////////////////////////////////////////////////////////////////////////////
class PT_Config_Record : public PT_Config
{
public:	

	PT_Config_Record() : m_name( NULL ), m_description( NULL ) {}

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Config_Record( const PT_Config_Record &st ) { *this = st; } 

	// Constructor
	PT_Config_Record( const char *name, const char *description );

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Config_Record; }
	
	// Debug String
	char *ToString( char *szIn );

	////////////////////////////////////////////////////////////////////////////
	// Data Members.
	// Kept public so that it is easy to access - 
	// change this for Configs?
	////////////////////////////////////////////////////////////////////////////
        const char *m_name;
        const char *m_description;
};

//
// PT_Config_Struct - Declare a new watch window struct
// Note that this name does not have the underscore attached to it.
// That's because Configs are used as by both writers as statements,
// *and* by Sorters for enumerations
///////////////////////////////////////////////////////////////////////////////
class PT_Config_Struct : public PT_Config
{
public:	

	PT_Config_Struct( ) : m_name( NULL ), m_description( NULL ), m_numRows( 0 ), m_numCols(0), m_colArray(NULL), m_numInstances(0) {}

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Config_Struct( const PT_Config_Struct &st ) { *this = st; } 

	// Constructor
	PT_Config_Struct( const char *name, const char *description, PT_ULONG numInstances, PT_ULONG numRows, PT_ULONG numCols, PT_Config_Data	**colArray );

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Config_Struct; }
	
	// Debug String
	char *ToString( char *szIn );

    const char		*m_name;
    const char		*m_description;
	PT_ULONG		 m_numRows;
	PT_ULONG		 m_numCols;
	PT_Config_Data	**m_colArray;
	PT_ULONG		m_numInstances;		// how many copies of this struct exist in the simulation?
};

//
// PT_Update_Strutc - update a watch window struct's row or cell
///////////////////////////////////////////////////////////////////////////////
class PT_Update_Struct : public PT_Config
{
public:	

	PT_Update_Struct( ) : m_struct( NULL ), m_ID( 0 ), m_namespace( 0 ), m_time( 0 ), m_row( 0 ), m_num_values( 0 ), m_instanceNum( 0 ) {}

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Update_Struct( const PT_Update_Struct &st ) { *this = st; } 

	// Constructor
	PT_Update_Struct(	PT_ID ID,  PT_Config_Struct *StructDef, PT_ULONG instanceNum,
						PT_Time time, PT_ULONG row, PT_ULONG numValues, PT_ID namespaceID = 0 );

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Update_Struct; }
	
	// Debug String
	char *ToString( char *szIn );

    PT_Config_Struct					*m_struct;		// struct id info
    PT_ID								m_ID;			// record that issued the update
	PT_ID								m_namespace;	// namespace that issued the update
    PT_Time								m_time;			// time of the clear
	PT_ULONG							m_row;			// row to update
	PT_ULONG							m_num_values;	// how many cols will be filled in
	PT_ULONG							m_instanceNum;	// which instance of the watch window are we working with?
};

//
// PT_Clear_Struct - clear a watch window struct's row or cell
// if colList is null then clear whole row!
///////////////////////////////////////////////////////////////////////////////
class PT_Clear_Struct : public PT_Config
{
public:	

	PT_Clear_Struct( ) : m_struct( NULL ), m_ID(0), m_time( 0 ), m_row( 0 ), m_colList( NULL ), m_instanceNum( 0 ), m_namespace( 0 ) {}

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Clear_Struct( const PT_Clear_Struct &st ) { *this = st; } 

	// Constructor
	PT_Clear_Struct(	PT_ID ID,  PT_Config_Struct *StructDef, PT_ULONG instanceNum,
						PT_Time time, PT_ULONG row, PT_Config_Data **colList = NULL, PT_ULONG numCols = 0, PT_ID namespaceID = 0 );

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Clear_Struct; }
	
	// Debug String
	char *ToString( char *szIn );

    PT_Config_Struct					*m_struct;		// struct id info
	PT_ID								m_ID;			// record that issued the clear
    PT_Time								m_time;			// time of the clear
	PT_ULONG							m_row;			// row to clear - if row = max_rows for struct, then 
														// whole table will be cleared
	PT_Config_Data						**m_colList;		// columns - values are column names
														// if m_colList is NULL, then clear entire row
	PT_ULONG							m_numCols;
	PT_ULONG							m_instanceNum;	// which instance of the watch window are we working with?
	PT_ID								m_namespace;	// namespace that issued the clear
};

//
// PT_Open_Statement - Open a new record
///////////////////////////////////////////////////////////////////////////////
class PT_Open_Statement : public PT_Statement
{
public:	

	PT_Open_Statement( ) : m_ID( 0 ), m_recordType( NULL ) { }

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Open_Statement( const PT_Open_Statement &st ) { *this = st; } 

	// Constructor 
	PT_Open_Statement( PT_Config_Record *def );

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Open; }
	
	// Debug String
	char *ToString( char *szIn );

	////////////////////////////////////////////////////////////////////////////
	// Data Members.
	// Kept public so that it is easy to access - not an issue since statements
	// usually have very short lifetimes
	////////////////////////////////////////////////////////////////////////////
	PT_ID	m_ID;	 // Don't set yourself, generated by a writer

	// you can't open a record in a different namespace than your own, only access one

	PT_Config_Record *m_recordType;
};

//
// PT_Close_Statement - Close a record
///////////////////////////////////////////////////////////////////////////////
class PT_Close_Statement : public PT_Statement
{
public:	
	PT_Close_Statement( ) : m_ID( 0 ), m_namespace( 0 ) { }

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Close_Statement( const PT_Close_Statement &st ) { *this = st; } 

	// Constructor 
	PT_Close_Statement( PT_ID ID, PT_ID nameSpaceID = 0 ) : m_ID( ID ), m_namespace( nameSpaceID ) { }
	PT_Close_Statement( PT_Open_Statement _Open ) : m_ID( _Open.m_ID ) { }

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Close; }
	
	// Debug String
	char *ToString( char *szIn );

	////////////////////////////////////////////////////////////////////////////
	// Data Members.
	// Kept public so that it is easy to access - not an issue since statements
	// usually have very short lifetimes
	////////////////////////////////////////////////////////////////////////////
	PT_ID	m_ID;
	PT_ID	m_namespace;
};

//
// PT_Event_Statement - Declare a new event
///////////////////////////////////////////////////////////////////////////////
class PT_Event_Statement : public PT_Statement
{
public:	
	PT_Event_Statement( ) : m_ID( 0 ), m_EventDef( NULL ), m_time( 0 ), 
							m_duration( 0 ), m_num_values( 0 ), m_namespace( 0 ) { }

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Event_Statement( const PT_Event_Statement &st ) { *this = st; } 

	// Constructor 
	PT_Event_Statement( PT_ID ID,  PT_Config_Event *EventDef, 
						PT_Time time, PT_Time duration, PT_ULONG num_values, PT_ID namespaceID = 0 );

	// Statement Type
	Statement_Type GetType( ) { return Pipe_Event; }
	
	// Debug String
	char *ToString( char *szIn );

	////////////////////////////////////////////////////////////////////////////
	// Data Members.
	// Kept public so that it is easy to access - not an issue since statements
	// usually have very short lifetimes
	////////////////////////////////////////////////////////////////////////////
    PT_ID								m_ID;
    PT_Config_Event					   *m_EventDef;
    PT_Time								m_time;
    PT_Time								m_duration;
	PT_ULONG							m_num_values;
	PT_ID								m_namespace;
};


//
// PT_Data_Statement - Declare a data element
// Use this statement if you are using this data in a record
// Use PT_Event_StatementData instead if you are using this data in an event
//
///////////////////////////////////////////////////////////////////////////////
class PT_Data_Statement : public PT_Statement
{
public:	
	PT_Data_Statement( ) : m_ID( 0 ), m_DataDef( NULL ), m_pData( NULL ), m_namespace( 0 ) { }

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_Data_Statement( const PT_Data_Statement &st ) { *this = st; } 

	// Constructor 
	PT_Data_Statement( PT_ID ID, PT_Config_Data *DataDef, void *data, PT_ID namespaceID = 0 );
	virtual ~PT_Data_Statement( );

	// Statement Type
	virtual Statement_Type GetType( ) { return Pipe_Data; }
	
	// Debug String
	virtual char *ToString( char *szIn );

	// String rep of data - can be used for ASCII traces, or for debug
	// Returned ptr is same as in ptr, useful for using in a function call
	char *DataToString( char *szIn );

	////////////////////////////////////////////////////////////////////////////
	// Data Members.
	// Kept public so that it is easy to access - not an issue since statements
	// usually have very short lifetimes
	////////////////////////////////////////////////////////////////////////////
    PT_ID								m_ID;
    PT_Config_Data					   *m_DataDef;
	void							   *m_pData;
	PT_ID								m_namespace;
};


//
// PT_EventData_Statement - Declare a data element attached to an event
// Use this statement if you are using this data in an event
///////////////////////////////////////////////////////////////////////////////
class PT_EventData_Statement : public PT_Data_Statement
{
public:
	
	PT_EventData_Statement() : PT_Data_Statement() { } 

	// Copy Constructor - copy constructors just copy pointers,
	// so if you want to keep separate strings, you will have to 
	// move the string over to your own buffer and do a strcpy
	PT_EventData_Statement( const PT_EventData_Statement &st ) { *this = st; } 

	// Constructor 
	PT_EventData_Statement( PT_ID ID, PT_Config_Data *DataDef, void *data, PT_ID namespaceID = 0 ) :
		PT_Data_Statement( ID, DataDef, data, namespaceID ) {}
	//~PT_EventData_Statement( ) { PT_Data_Statement::~PT_Data_Statement(); }

	// Statement Type
	Statement_Type GetType( ) { return Pipe_EventData; }
	
	// Debug String
	char *ToString( char *szIn );

};


///////////////////////////////////////////////////////////////////////////////
// DataStore types. 
// 
// Hide internal storage mechanism, but at the same time, create an interface
// that lends itself to a very compact memory footprint but still efficient
// sorting algos
// Does not use virtual functions, as that would waste space 
//
///////////////////////////////////////////////////////////////////////////////

#define DATA_KEYTABLE_MASK 0x8000

// each event w/ at least 1 data element... save 7 bytes.
// each data elament... save 2 bytes on type 

//
//	PT_DataList - class for accessing data in an event or a record once it
//  has been closed. When the record is still open, we use the less efficient
//  (both speed and space-wise) PT_RecordDataList b/c we don't have a pre-determined
//  number of data elements. Once the record is closed, create a PT_DataList,
//  and transfer the info
///////////////////////////////////////////////////////////////////////////////
class PT_DataList
{
public:
	friend class PT_DataStore;
	friend class PT_StandardDataStore;

	PT_DataList();
	PT_DataList( PT_Event_Statement *event, PT_StandardDataStore *store );
	~PT_DataList() { }

	inline void Init( const PT_Event_Statement *event, PT_StandardDataStore *store );
	inline void Init( const PT_USHORT numElements, PT_StandardDataStore *store );

	// How many data elements are attached to this record or event?
	PT_USHORT NumElements( PT_StandardDataStore *st );

	// Returns a key for the definition of data[ index ]
	// Returns zero if invalid index
	PT_KEY GetDefinitionKey( PT_USHORT index, PT_StandardDataStore *st );
	
	// Returns address of the data for the associated definition. If doesn't exist
	// returns NULL
	void  **GetDataByDefinition( PT_KEY definitionKey, PT_StandardDataStore *st );

	// Returns the data for the associated index. If doesn't exist, 
	// returns NULL
	void  **GetDataByIndex( PT_USHORT index, PT_StandardDataStore *store );

	// pre: we are not adding a data element to a list that is already
	// full ( == to m_numElements )
	inline void AddData( PT_KEY key, void *data, PT_USHORT index, PT_StandardDataStore *st );

	// prints out data info to a buffer and 
	// this buffer should be big!
	char *ToString( char *buffer );

	// returns pointer to array of size NumElements of type PT_Key. Each key is the 
	// definition key of data[ i ]. Definition can be retrieved by calling the
	// FindConfigByName in the DataStore
	inline PT_KEY *GetKeyTable( PT_StandardDataStore *st );

	// returns pointer to array of size NumElements of type void*. Each element
	// can be cast into its proper data type by fist figuring out it's definition
	inline void **GetDataTable( PT_StandardDataStore *st );

	// how much memory do i require on an alloc?
	static inline PT_ULONG GetDataByteSize( PT_ULONG numElements, bool needType = true );

	// TODO?: only let our friends SetData
	inline void SetDataByIndex( PT_USHORT index, PT_KEY definitionKey, void *data, PT_StandardDataStore *st );

	// given two datalists, do they have the same exact data types in the same exact
	// order???
	bool TypesEqual( PT_DataList *other, PT_StandardDataStore *st );

	// is the keytable stored sequentially in memory after this struct?
	inline bool HasKeyTable( ){ return ! (m_numElements & ((PT_USHORT)DATA_KEYTABLE_MASK)); }

private:

	// if the high bit of m_numElements is set, then this value is actually
	// an index into a lookup table for retrieving the keytable
	PT_USHORT m_numElements;
};


//
//	PT_RecordDataList - class for accessing data in a record. Implementation
//  is as a linked list.
//  See description below under AddData for more info
//  Record struct should have pointers into the important data elements
///////////////////////////////////////////////////////////////////////////////
class PT_RecordDataList
{
public:
	PT_RecordDataList();
	~PT_RecordDataList();
	
	// Returns a key for the definition of data[ index ]
	// Returns zero if invalid index
	PT_KEY GetDefinitionKey( PT_USHORT index );
	
	// Returns address of the data for the associated definition. If doesn't exist
	// returns NULL
	void  **GetDataByDefinition( PT_KEY definitionKey );

	// Returns the data for the associated index. If doesn't exist, 
	// returns NULL
	void  **GetDataByIndex( PT_USHORT index );

	// at this point, the record is still open, so use the PT_RecordDataList linked
	// list data struct to maintain the new data, and use 'new' to alloc this data.
	// 'new' and 'delete' waste 8 bytes or so per alloc. but this doesn't matter.
	// i did a study of traces, and on average, we have about 600 records open at
	// a time (being liberal with 600). 600 * 20 data per record (again liberal) *
	// 8 = 96KB. so no big deal!!!

	// Add a data node to the end of the list, 
	inline void AddData( PT_KEY dataKey, void *data );
	
	// helper function to help DataStores convert from a PT_RecordDataList to a 
	// PT_DataList
	void FillDataList( PT_DataList *pDataList, PT_StandardDataStore *store );

	inline PT_USHORT NumElements( ){ return m_numElements; }

private:
	// Make this a DWORD in order to keep things DWORD aligned, or 
	// not important?
	PT_USHORT m_numElements;

	// Internal struct for storing data as a linked list
	class DataNode
	{
	public:
		DataNode()
			{ dataDef = 0; this->data = NULL; nextNode = NULL; }
		DataNode( PT_KEY dataKey, void *data )
			{ dataDef = dataKey; this->data = data; nextNode = NULL; }

		void *data;
		DataNode *nextNode;
		PT_KEY dataDef;
	};

	DataNode *m_rootNode;
	DataNode *m_tailNode;
};


// 28 bytes down to 16!!

// top bit of m_definitionKey is whether or not this event has data
// attached to it so mask it out
#define DEFKEY_MASK		0x3fff
#define DATAKEY_MASK	0x8000
#define SMALL_SIZE_MASK	0x4000

class PT_Event;

class PT_LargeEvent
{
public:
	// Key to definition. To get actual definition involves a hash
	// table lookup, but any time you are searching for a specific
	// definition, you can just search by key anyway since keys
	// are unique
	// Actually: (and this next part is a hack to save 4 bytes per event) 
	// the top bit of m_definitionKey is a bool that tells us whether this
	// event has any data attached. If yes, there is a pointer to a
	// PT_EventDataList right after this struct in memory. This limits
	// max possible definitions to 2^15, but I seriously doubt we will
	// ever need more than 32,000 definitions (willy currently has ~300)!
	// *******next to top bit tells us whether we are actually a PT_Event or a 
	// PT_ShortEvent
	//PT_KEY	  m_definitionKey;

	// Duration can be a short - VERIFY!
	PT_USHORT m_eventDuration;

	// Start Time of event
	PT_Time	  m_eventTime; 

	PT_Event *m_nextEvent;
	
	// deprecated
	//PT_Event *m_nextEventInTime;

	// next event in time with this event type
	//PT_Event *m_nextEventOfType;

	// Associated record. Is this needed? Yes. Search for an event and 
	// you'll want to know what instruction is was
	// Why store an index and not a pointer? Well, if you use a pointer 
	// to a record, and then later need to know it's ID, this implies
	// that the record needs to store its ID within its struct, unless
	// you do some ugly pointer math... 1/26/01 Actually, we'll need this
	// ugly pointer math anyway. Need to think more on this.
	PT_ID	m_recordIndex;

	// Data List - do not include this in the struct. If the event contains
	// data, this pointer will be located right after the struct in memory.
	// 4 bytes may not sound like a lot to save, but because events are 
	// the most common types of statements, these 4 bytes are the difference
	// between a 29% and 38% memory footprint improvement in PTV!!!
	//PT_DataList *m_pDataList; 
};

class PT_Record; 

//
//	PT_SmallEvent - takes advantage of the properties of most events to 
//  create a really compact version of an event for small mem footprint
///////////////////////////////////////////////////////////////////////////////
class PT_SmallEvent
{
public:
	// does this statement have the right properties to be a compact event?
	static bool IsWorthyOfSmallEvent( const PT_Event_Statement *event, const PT_Record *r, bool isLowestTime );

	// 10 bytes!!!!!!!!!!

	// byte 0 - 1 (IN PT_EVENT) 
	// the top bit is a bool that tells us whether this
	// event has any data attached. If yes, there is a pointer to a
	// PT_EventDataList right after this struct in memory. 
	// next to top bit tells us whether we are actually a PT_Event or a 
	// PT_SmallEvent
	// the next 2 bits tell us the duration (from 0 to 2 )
	// the next 12 bits tell us the time offset from start (0 to 2048 (is this enough???))

	// bytes 2, 3, 4, 5
	// first 12 bits are the key (0 to 2048)
	// next 20 are the recID
	PT_ULONG midDword;

	// bytes 6 - 9 pointer to nextInRecord
	PT_Event *m_nextEvent;
};

//
//	PT_Event - class for accessing events in a record
//  Events are the most common statement, so this needs to be stored
//  *efficiently*
///////////////////////////////////////////////////////////////////////////////
class PT_Event
{
public:
//	friend class PT_DataStore;
//	friend class PT_StandardDataStore;
//	friend class PT_Record;

	PT_Event();
	//PT_Event( const PT_Event_Statement *event ){ Init( event );}
	~PT_Event(){ }

	inline void Init( PT_Record *r, const PT_Event_Statement *event, bool isSmallEvent );

	inline bool GetIsShortEvent( ){return (bool)(lowWord & SMALL_SIZE_MASK);}

	// return a ptr to our DataList, if there is no data, returns null
	// see m_definitionKey below for important implementation details
	inline PT_DataList *GetDataList();

	// Returns a key for the event definition
	// see m_definitionKey below for important implementation details
	inline PT_KEY GetDefinitionKey( );// { return (m_definitionKey & DEFKEY_MASK); }

	// Returns start time of the event
	PT_Time GetStartTime( class PT_StandardDataStore *store );//{ return m_eventTime; }
	inline PT_Time GetStartTime( class PT_Record *rec );//{ return m_eventTime; }
	inline PT_Time GetStartTime( class PT_Record *rec, bool isSmallEvent );//{ return m_eventTime; }

	// Returns end time of the event = start + duration - 1 (duration 1 is where start==end)
	PT_Time GetEndTime( class PT_StandardDataStore *store ){ return GetStartTime( store ) + GetDuration( ) - 1; }
	inline PT_Time GetEndTime( class PT_Record *rec ){ return GetStartTime( rec ) + GetDuration( ) - 1; }
	
	inline void UpdateStartTime( PT_Time diff );
	
	// Gets event duration
	inline PT_Time GetDuration( );//{ return m_eventDuration; }
	inline PT_Time GetDuration( bool isSmallEvent );

	// Returns next event associated with this record. 
	// Returns NULL if this is the last event
	inline PT_Event *GetNextEvent( );//{ return m_nextEvent; }

	inline void SetNextEvent( PT_Event *nextEvent );

	// Returns record ID associated with this event
	inline PT_ID GetRecordID( );//{ return m_recordIndex; }
	inline PT_ID GetRecordID( bool isSmallEvent );

	// prints out data info to a buffer and 
	// this buffer should be big!
	char *ToString( char *buffer );

	//update our data pointer
	inline void SetDataPtr( PT_DataList *seq );

	PT_Event *GetNextEventInTime( );

private:
	PT_USHORT lowWord;

};


// old was 16 * 4 = 64 + 5 bools = 69 bytes per record!
// new... down to 40

// the high bit of the bool vector is reserved for telling whether or not 
// the record is closd yet
#define RECORD_CLOSED_BIT 7

// if one thread is accessing our PT_RecordDataList, prevent another thread from
// closing the record, which would delete the PT_RecordDataList and convert it 
// to a PT_DataList
#define RECORD_LOCKED_BIT 6

//
//	PT_Record - class for working with records
///////////////////////////////////////////////////////////////////////////////
class PT_Record
{
public:
	friend class PT_StandardDataStore;
	
	PT_Record( );
	PT_Record( PT_Open_Statement *st );

// removed this b/c boundschecker didn't like it, and we weren't using it
// anyways
//	~PT_Record( );

	inline void Init( const PT_Open_Statement *st );

	PT_KEY GetDefinitionKey( ){ return m_definitionKey; } 

	// currently O(n)
	PT_USHORT GetNumEvents( ); 
	
	// method for getting data lists. if the record is still
	inline PT_DataList *GetDataList( );
	
	inline PT_ID GetRecordID( ){ return m_recordID; }

	// returns 0 if doesn't make sense
	inline PT_ID GetUID( ){ return m_uopID; }

	// returns 0 if doesn't make snese
	inline PT_ID GetInst_ID( ){ return m_instID; }

	// return first event in the record, null if there are no events
	inline PT_Event *GetFirstEvent( ){ return m_pFirstEvent; }

	// return last event in the record, null if there are no events
	inline PT_Event *GetLastEvent( ){ return m_pLastEvent; }

	// return time of earliest event O(1)
	// returns 0 if no events
	inline PT_Time GetMinTime( ){ return m_minTime; }
	
	// return latest end time O(1)
	// return 0 if no events
	inline PT_Time GetMaxTime( ){ return m_maxTime; }

	// prints out data info to a buffer and 
	// this buffer should be big!
	char *ToString( char *buffer );

	inline PT_UCHAR GetThreadID( ){ return m_threadID; }

	// we keep around one char of bits to store boolean data.
	// this way, you can have up to 8 bools used for your custom purposes
	void SetBoolVal( PT_UCHAR index, bool val);
	bool GetBoolVal( PT_UCHAR index );

	// just sets the parent pointer, doesn't update sibling info
	void SetParent( PT_Record *parent );

	// returns NULL if no parent record
	inline PT_Record *GetParent( ){ return m_pParent; }				
	
	// returns NULL if no children
	inline PT_Record *GetFirstChild( ){ return m_pChild; }				
	
	// returns NULL if this is the last sibling
	inline PT_Record *GetNextSibling( ){ return m_pNextSibling; }
	
	// return NULL if this is the last in thread
//	inline PT_Record *GetNextThreadSibling( ){ return m_pNextThreadSibling; }	

	// ToDo? only let our friends cast our data list to PT_RecordDataList
	inline PT_RecordDataList *GetRecordDataList( );

	// Setter methods for updating a record
	inline void AddEvent( PT_Event *event, const PT_Event_Statement *eventStatement, PT_StandardDataStore *store );
	inline void SetDataList( PT_DataList *list ){ m_pDataList = list; }

	// given another record that we know is one of our first level children,
	// insert it into the list
//	void InsertChild( PT_Record *newChild );

private:
	
	void UpdateEventStartTimes( const PT_Event_Statement *eventStatement, PT_StandardDataStore *store );

	// DWORD Aligned

	// TODO: Do not store record ID here? The record array is stored by ID#,
	// so you can just do hackish pointer math here if you want to get 
	// record ID.
	PT_ID	 m_recordID;

	PT_ID	 m_uopID;
	PT_ID	 m_instID;

	// Pointer to event linked list
	PT_Event *m_pFirstEvent; 

	// LastEvent and LastEventTime... Are they needed? LastEvent is 
	// nice for fast insertion of new events. LastEventTime is not
	// really needed, just used in insertion, we can just do the 
	// lookup + math
	PT_Event *m_pLastEvent; 

	// minTime is not needed since it is equal to 
	// m_pFirstEvent->m_eventTime
	PT_Time		m_minTime;	// start time of this record
	
	// maxTime is needed b/c we aren't storing m_pLastEvent around, and 
	// it may not be equal to m_pLastEvent anyway b/c of durations
	PT_Time		m_maxTime;

	// Data List
	PT_DataList *m_pDataList; 

	// Key to definition
	PT_KEY	 m_definitionKey;

	PT_UCHAR m_threadID;				// MAX_THREADS is 16 so 1 byte 
										// should suffice

	PT_UCHAR m_boolVector;				// Store bools as bits for efficiency

	PT_Record	*m_pParent;				// Parent record
	PT_Record	*m_pChild;				// First child record's ID
	PT_Record	*m_pLastChild;			// Last child record's ID
	PT_Record	*m_pNextSibling;		// Next Sibling 
//	PT_Record	*m_pNextThreadSibling;	

	// TODO: need this or no?
	//int		   sort_order;
};


////////////////////////////////////////////////////////////////////////////////
//
//	eqstr function for STL hash maps
//
////////////////////////////////////////////////////////////////////////////////
struct PT_eqstr
{
  // for using names as keys
  bool operator()( const char* s1, const char* s2 ) const;
  
  // for using "keys" as keys
  bool operator()( const PT_ULONG id1, const PT_ULONG id2 ) const;
};


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Event::GetDataList
// Description: 
// return a ptr to our DataList, if there is no data, returns null
// see m_definitionKey below for important implementation details
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_DataList *PT_Event::GetDataList()
{
	// check to see if there is data attached.
	// if no return NULL
	if ( ! ( lowWord & DATAKEY_MASK ) )
	{
		return NULL;
	}
	else 
	{
		if ( ! ( lowWord & SMALL_SIZE_MASK ) )
		{
			// its a big record
			PT_DataList **list = (PT_DataList **)( ((char *)this) + sizeof(lowWord) + sizeof( PT_LargeEvent ) );
			return *list;
		}
		else
		{
			PT_DataList **list = (PT_DataList **)( ((char *)this) + sizeof(lowWord) + sizeof( PT_SmallEvent ) );
			return *list;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::GetDataList
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_DataList *PT_Record::GetDataList( )
{ 
	// make sure the top bit is set to indicate that the record is closed!
	// otherwise, this call could be dangerous!
	if ( !GetBoolVal( RECORD_CLOSED_BIT ) )
	{
		return NULL; 
	}
	else
	{
		return m_pDataList;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_Record::GetRecordDataList
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_RecordDataList *PT_Record::GetRecordDataList( )
{ 
	// make sure the top bit is set to indicate that the record is closed!
	// otherwise, this call could be dangerous!
	if ( GetBoolVal( RECORD_CLOSED_BIT ) )
	{
		return NULL; 
	}
	else
	{
		return (PT_RecordDataList *)m_pDataList;
	}
}


// Returns a key for the event definition
// see m_definitionKey below for important implementation details
PT_KEY PT_Event::GetDefinitionKey( )
{
	if ( ! ( lowWord & SMALL_SIZE_MASK ) )
	{	
		return (PT_KEY)(lowWord & DEFKEY_MASK); 
	}
	else
	{	
		PT_SmallEvent *se = (PT_SmallEvent *)(this + 1);
		return (PT_KEY)( se->midDword >> 20 );
		//return 0;
	}	
}


// Returns start time of the event
PT_Time PT_Event::GetStartTime( class PT_Record *rec )
{ 
	if ( ! ( lowWord & SMALL_SIZE_MASK ) )
	{	
		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		return le->m_eventTime;
	}
	else
	{	
		PT_Time start = rec->GetMinTime( );
		start = start + (lowWord & 0x1fff);
		return start;
		//return 0;
	}	
}

// Returns start time of the event
PT_Time PT_Event::GetStartTime( class PT_Record *rec, bool isSmall )
{ 
	if ( isSmall )
	{	
		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		return le->m_eventTime;
	}
	else
	{	
		PT_Time start = rec->GetMinTime( );
		start = start + (lowWord & 0x1fff);
		return start;
		//return 0;
	}	
}

void PT_Event::SetNextEvent( PT_Event *nextEvent )
{
	if ( ! ( lowWord & SMALL_SIZE_MASK ) )
	{	
		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		le->m_nextEvent = nextEvent;
	}
	else
	{	
		PT_SmallEvent *se = (PT_SmallEvent *)(this + 1);
		se->m_nextEvent = nextEvent;
		return;
	}	
}

// Gets event duration
PT_Time PT_Event::GetDuration( )
{
	if ( ! ( lowWord & SMALL_SIZE_MASK ) )
	{	
		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		return le->m_eventDuration;
	}
	else
	{	
		// extract the low 3rd bit from the top nibble
		return (lowWord & 0x2000) ? 2:1;
	}	//{ return m_eventDuration; }
}

// Gets event duration
PT_Time PT_Event::GetDuration( bool isSmall )
{
	if ( ! ( isSmall ) )
	{	
		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		return le->m_eventDuration;
	}
	else
	{	
		// extract the low 3rd bit from the top nibble
		return (lowWord & 0x2000) ? 2:1;
	}	//{ return m_eventDuration; }
}

// Returns next event associated with this record. 
// Returns NULL if this is the last event
PT_Event *PT_Event::GetNextEvent( )
{
	if ( ! ( lowWord & SMALL_SIZE_MASK ) )
	{	
		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		return le->m_nextEvent;
	}
	else
	{	
		PT_SmallEvent *se = (PT_SmallEvent *)(this + 1);
		return se->m_nextEvent;
	}	
}//{ return m_nextEvent; }

// Returns record ID associated with this event
PT_ID PT_Event::GetRecordID( )
{
	if ( ! ( lowWord & SMALL_SIZE_MASK ) )
	{	
		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		return le->m_recordIndex;
	}
	else
	{	
		PT_SmallEvent *se = (PT_SmallEvent *)(this + 1);
		return (se->midDword & 0x000fffff);
	}	
}

// Returns record ID associated with this event
PT_ID PT_Event::GetRecordID( bool isSmall )
{
	if ( ! ( isSmall ) )
	{	
		PT_LargeEvent *le = (PT_LargeEvent *)(this + 1);
		return le->m_recordIndex;
	}
	else
	{	
		PT_SmallEvent *se = (PT_SmallEvent *)(this + 1);
		return (se->midDword & 0x000fffff);
	}	
}

#endif //__PT_TYPES_H







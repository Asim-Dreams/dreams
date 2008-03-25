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

#ifndef __PT_IO_H
#define __PT_IO_H

#include <stdio.h>
#include "PT_Types.h"
#include "PT_Log.h"
#include "PT_Data.h"

#ifdef WIN32
#define NO_ZLIB
#endif

#ifndef NO_ZLIB
#define USE_ZLIB
#endif

#ifdef USE_ZLIB
	// Use ZLib for working with gzip files
	#include <zlib.h>
	#define ZFILE	gzFile
#else
	#include "xfopen.h"
#endif 

// Format Strings
#define BINARY_FORMAT	"Binary"
#define ASCII_FORMAT	"ASCII"

class PT_DataStore;

#ifdef USE_ZLIB
class PT_BufferedReader;
#endif

////////////////////////////////////////////////////////////////////////////////
//	Readers
////////////////////////////////////////////////////////////////////////////////
class PT_ReadStream
{
public:

	////////////////////////////////////////////////////////////////////////////
	// Contructors and Destructors
	////////////////////////////////////////////////////////////////////////////
	
	// All derived classes should call this constructor
	// If the DataStore is null, then no data store is used, and 
	// ReadAndParseStatement will fail
	PT_ReadStream( PT_DataStore *pDataStore = NULL, PT_Error *Error = NULL );	

	// Virtual Destructor - all derived classes are responsible for their own
	// cleanup
	virtual	~PT_ReadStream();

	////////////////////////////////////////////////////////////////////////////
	// User Methods
	////////////////////////////////////////////////////////////////////////////

	// Opens an input stream from a file. 
	// If szFileName is NULL, stdin is used.
	// This should be automatically called by all constructors
	virtual	PT_Error Open( char *szFileName );
	
	// This should automatically be called by the destructor
	virtual PT_Error Close(); 

	// Meant to be used in a while loop... returns PT_Error_EOF when end of
	// file reached
	// If insertInfo is false, then the record isn't inserted into the 
	// datastore
	// Important: returned statements have a very limited lifetime. The next
	// ReadStatement may overwrite its comments. If you want to store 
	// statements, you should use the copy constructor.
	virtual PT_Error ReadStatement( PT_Statement **statement, PT_BOOL bInsertInfo = PT_TRUE ) = 0;

	// returns the file version 
	virtual const char *GetVersionInfo( ) = 0;

	// returns 'mytype' as a string "ASCII" or "Binary"
	virtual const char *GetFormatString( ) = 0;
	virtual Pipe_Format GetFormat( ) = 0;

	// returns timestamp found in pipetrace file header
	virtual const char *GetTimeStamp( );

	////////////////////////////////////////////////////////////////////////////
	// Static Class Factory
	// This is the best way to create a reader, especially if you don't care
	// what format or version the file is in, all you care about is data.
	// The class factory will return the proper PT_ReadStream derived instance.
	////////////////////////////////////////////////////////////////////////////
	static PT_Error CreateReader( char *szFileName, PT_DataStore *store, PT_ReadStream **ppReadStream );	

	PT_ULONG GetBytesRead( );

protected:
	// Use regexp to parse header and make sure we are dealing with 
	// a real pipetrace file
#ifdef USE_ZLIB
	static PT_Error ParseHeaderInfo(	ZFILE file, char *format, PT_KEY *hashSize, 
										PT_KEY *hashKey, char *timestamp, char *szNamespace );
	ZFILE			m_file;							// file pointer
#else
	static PT_Error ParseHeaderInfo(	FILE *file, char *format, PT_KEY *hashSize, 
										PT_KEY *hashKey, char *timestamp, char *szNamespace );
	FILE			*m_file;
#endif

	unsigned long 
#ifdef USE_ZLIB
	get_word( ZFILE file );
#else
	get_word( FILE *file );
#endif


	bool m_bIsVersion3_2;
	bool m_bIsVersion3_3;

	// setup the internal statements for temporary storage
	PT_Error SetupDefaultStatements( );

	inline PT_KEY NextKey( );						// retrieve a new key for 
													// the new definition
	char	m_szNamespace[256];						//if null, then we are in the standard namespace (normal pipetraces)
	bool m_bIsNamespaced;

	PT_DataStore	*m_pDataStore;

	char			m_szFileName[ PT_MAX_PATH ];	// current file name

	PT_KEY			m_hashSize;						// min size of our hash table. 
													// parsed from header
	PT_KEY			m_nextKey;						// current hash key found. each time 
													// we find a definition
													// we incremnent
	PT_KEY			m_firstKey;						// lowest hash key index. 
													// parsed from header

	char			m_szTimeStamp[ PT_MAX_PATH ];
	char			m_versionStamp[ PT_MAX_PATH ];	//version string in header file

	PT_ID			m_nextRecord;					// for sanity checking
	PT_UINT			m_EventDataCount;				// for sanity checking

	// for binary only, but keep here so that the class factory has an
	// easier time setting things up, and we do all the nice initialization
	PT_ULONG		m_lastTime;						// version 3.2 uses deltas
													// instead of times for events
	PT_UCHAR		m_keySize;
	PT_ID			m_lastRecordID;

	// for faster IO
#ifdef USE_ZLIB
	PT_BufferedReader *m_pBufferedReader;
#endif

	////////////////////////////////////////////////////////////////////////////
	// Keep a copy of each statement type around. If you find a statement of 
	// type t, fill in the statement class. This is better than the alternative
	// of making a new statement, and letting the user delete it when they are 
	// ready (Lots of potential for mem leaks). 
	////////////////////////////////////////////////////////////////////////////
	
	char m_szStringBuffers[ 2 ][ PT_MAX_PATH ];		// attach the char * inside
													// statements to these buffers
													
	char m_szEnumChars[ 256 ][ 256 ];				// we're already set up for 
													// the worst case : 256 enums

	PT_Config_Data *m_StructColArray[ 2048 ];		// array of pointer to column defs

	char *m_extraBuffer[ 256 ];

	PT_Close_Statement		CloseStatement;
	PT_Open_Statement		OpenStatement;
	PT_Comment_Statement	CommentStatement;
	PT_Event_Statement		EventStatement;
	PT_Data_Statement		DataStatement;
	PT_EventData_Statement	EventDataStatement;
	PT_Config_Event			ConfigEvent;
	PT_Config_Data			ConfigData;
	PT_Config_Record		ConfigRecord;
	PT_Config_Struct		ConfigStruct;
	PT_Update_Struct		UpdateStruct;
	PT_Clear_Struct			ClearStruct;
};


////////////////////////////////////////////////////////////////////////////////
//	Reader for ASCII format files
////////////////////////////////////////////////////////////////////////////////
class PT_ASCIIReadStream : public PT_ReadStream
{
public:

	////////////////////////////////////////////////////////////////////////////
	// Contructors and Destructors
	////////////////////////////////////////////////////////////////////////////
	
	// Default Constructor. Should only be called by class factory
	// Does not open a file
	PT_ASCIIReadStream(  );
	
	// NULL file name = stdin
	PT_ASCIIReadStream( char *szFileName, PT_Error *Error = NULL );	
	//~PT_ASCIIReadStream();

	////////////////////////////////////////////////////////////////////////////
	// User Methods
	////////////////////////////////////////////////////////////////////////////

	// returns the file version 
	const char *GetVersionInfo( ){ return m_versionStamp; }
	const char *GetFormatString( ){ return ASCII_FORMAT; }
	Pipe_Format GetFormat( ){ return Pipe_ASCII_Format; }

	// opens the file and validates the format
	// PT_Error Open( char *szFileName );

	// Meant to be used in a while loop... returns PT_Error_EOF when end of
	// file reached
	// If insertInfo is false, then the record isn't inserted into the 
	// datastore
	// Important: returned statements have a very limited lifetime. The next
	// ReadStatement may overwrite its comments. If you want to store 
	// statements, you should use the copy constructor if you know it's type.
	PT_Error ReadStatement( PT_Statement **statement, 
							PT_BOOL bInsertInfo = PT_TRUE );

private:
	char m_szLine[ PT_MAX_PATH ];
	char m_szBuf[ PT_MAX_PATH ];		// temp buffer for parsing
	char m_szBuf2[ PT_MAX_PATH ];		// temp buffer for parsing

};


////////////////////////////////////////////////////////////////////////////////
//	Reader for Binary format files
////////////////////////////////////////////////////////////////////////////////
class PT_BinaryReadStream : public PT_ReadStream
{
public: 
	
	////////////////////////////////////////////////////////////////////////////
	// Contructors and Destructors
	////////////////////////////////////////////////////////////////////////////
	
	// Default Constructor. Should only be called by class factory
	// Does not open a file
	PT_BinaryReadStream(  );

	//NULL file name = stdin
	PT_BinaryReadStream( char *szFileName, PT_Error *Error = NULL );	
	//~PT_BinaryReadStream();

	////////////////////////////////////////////////////////////////////////////
	// User Methods
	////////////////////////////////////////////////////////////////////////////

	// returns the file version 
	const char *GetVersionInfo( ){ return m_versionStamp; }
	const char *GetFormatString( ){ return BINARY_FORMAT; }
	Pipe_Format GetFormat( ){ return Pipe_Binary_Format; }

	// opens the file and validates the format
	// PT_Error Open( char *szFileName );

	// Meant to be used in a while loop... returns PT_Error_EOF when end of
	// file reached
	// If insertInfo is false, then the record isn't inserted into the 
	// datastore
	// Important: returned statements have a very limited lifetime. The next
	// ReadStatement may overwrite its comments. If you want to store 
	// statements, you should use the copy constructor if you know it's type.
	PT_Error ReadStatement( PT_Statement **statement, 
							PT_BOOL bInsertInfo = PT_TRUE );

protected:
	// read the next null-terminated string from the file
	void BinGetString( char *szString );

};


class PT_DataStore;

////////////////////////////////////////////////////////////////////////////////
//	Writers 
////////////////////////////////////////////////////////////////////////////////
class PT_WriteStream
{
public:
	////////////////////////////////////////////////////////////////////////////
	// Contructors and Destructors
	////////////////////////////////////////////////////////////////////////////
	PT_WriteStream( );
	virtual	~PT_WriteStream();
	
	////////////////////////////////////////////////////////////////////////////
	// User Methods
	////////////////////////////////////////////////////////////////////////////

	// Opens an output stream to a file and writes header info (version, etc..) 
	// If szFileName is NULL, stdout is used.
	// If version is NULL, default version is used
	// This should be automatically called by all constructors
	virtual PT_Error Open( const char *szFileName, char *version = NULL );
	
	// Close the stream (if it is currently open)
	// This should automatically be called by the destructor
	virtual PT_Error Close();

	// Send anything being buffered down to disk
	virtual PT_Error Flush();

	// Write a statement to file
	// Implemented for all streams, including lots of sanity checking
	// All ASCII, Binary streams have to do is implement the Handle_XXX functions 
	// for specific types. 
	// The second parameter is an out parameter. If the PT_Statement
	// is an Open_Record then this must be provided to get the new record ID 
	// generated by the WriteStream. Otherwise, it can be left NULL
	virtual PT_Error WriteStatement( PT_Statement &current, PT_ID *ID = NULL );

	// Less used, write statement. If you have a complete record, you can 
	// serialize it into the file ( this function will break it into 
	// statements)
	virtual PT_Error WriteStatement( PT_Record *current );

	// returns the file version 
	virtual const char *GetVersionInfo( ) = 0;

	// returns 'mytype' as a string "ASCII" or "Binary"
	virtual const char *GetFormatString( ) = 0;
	virtual Pipe_Format GetFormat( ) = 0;

	const char *GetNameSpaceName( ){ return m_szNamespace; }
	
	// the PT_Writer will hash the name to try to get a unique namespace ID for this namespace
	int GetNameSpaceID( );

protected:
#ifdef USE_ZLIB	
	ZFILE	m_file;	// file pointer
#else
	FILE	*m_file;
#endif

    char	m_szFileName[ PT_MAX_PATH ];

	const char	*m_szNamespace;	//if null, then we are in the standard namespace (normal pipetraces)

	PT_ID	m_nextRecord;
	
	// mostly for binary writers
	char		m_keySize;
	PT_KEY		m_nextKey;
	
	// Use STL hash map class to see if definition is valid, and to associate
	// value associated with a name string is the type definition 
	// (i.e. PT_Config_Data)
	// it with a key in the table for binary writers
	//hash_map< const char*, PT_Config *, hash< const char* >, PT_eqstr > *m_HashTable;

	// 1/21 - instead of hash map - use DataStore instead. This way, one code base,
	// plus, it handles storage and deallocation of string values
	PT_DataStore *m_pStore;

	// for sanity checking - see WriteStatement code
	PT_ULONG	m_DataCounter;

	// Do initial setup of the hash table
	// PT_Error InitHashTable( );

	// Do cleanup of hash table
	// PT_Error FreeHashTable( );
	
	// Make sure the definition name is kosher and wasn't previously defined
	PT_BOOL ValidateName( const char *Name, PT_Config *Config );

	inline PT_KEY NextKey( );

	////////////////////////////////////////////////////////////////////////////
	//
	// Handler methods. Instead of writing your own WriteStatement, just
	// implement the HandleXXXX methods and let the base class WriteStatement
	// do all the type stuff and lots of sanity checking... adds a lot of code
	// reuse and makes writing a new Stream much easier
	//
	////////////////////////////////////////////////////////////////////////////
	virtual PT_Error HandleOpenStatement( PT_Open_Statement &st ) = 0;
	virtual PT_Error HandleCloseStatement( PT_Close_Statement &st ) = 0;
	virtual PT_Error HandleCommentStatement( PT_Comment_Statement &st ) = 0;
	virtual PT_Error HandleConfigEvent( PT_Config_Event &st ) = 0;
	virtual PT_Error HandleConfigData( PT_Config_Data &st ) = 0;
	virtual PT_Error HandleConfigRecord( PT_Config_Record &st ) = 0;
	virtual PT_Error HandleEventStatement( PT_Event_Statement &st ) = 0;
	virtual PT_Error HandleDataStatement( PT_Data_Statement &st ) = 0;
	virtual PT_Error HandleConfigStruct( PT_Config_Struct &st ) = 0;
	virtual PT_Error HandleUpdateStruct( PT_Update_Struct &st ) = 0;
	virtual PT_Error HandleClearStruct( PT_Clear_Struct &st ) = 0;
};


////////////////////////////////////////////////////////////////////////////////
//	ASCII Writer
////////////////////////////////////////////////////////////////////////////////
class PT_ASCIIWriteStream : public PT_WriteStream
{
public:
	////////////////////////////////////////////////////////////////////////////
	// Contructors and Destructors
	////////////////////////////////////////////////////////////////////////////
	
	// Specify a file name. NULL = stdout
	PT_ASCIIWriteStream( const char *szFile, PT_Error *Error, const char *szNamespace = NULL );
	//~PT_ASCIIWriteStream();
	
	////////////////////////////////////////////////////////////////////////////
	// User Methods
	////////////////////////////////////////////////////////////////////////////

	// Opens an output stream to a file and writes header info (version, etc..) 
	// If szFileName is NULL, stdout is used.
	// If version is NULL, default version is used
	// This should be automatically called by all constructors
	// PT_Error Open( char *szFileName, char *version = NULL );

	// Send anything being buffered down to disk
	//PT_Error Flush( );

	// Write a statement to file
	// Implemented for all streams, including lots of sanity checking
	// All ASCII, Binary streams have to do is implement the Handle_XXX functions 
	// for specific types. 
	// The second parameter is an out parameter. If the PT_Statement
	// is an Open_Record then this must be provided to get the new record ID 
	// generated by the WriteStream. Otherwise, it can be left NULL
	// PT_Error WriteStatement( PT_Statement &current, PT_ID *ID = NULL );

	// Returns the file version 
	const char *GetVersionInfo( );

	const char *GetFormatString( ) { return ASCII_FORMAT; }
	Pipe_Format GetFormat( ) { return Pipe_ASCII_Format; }

protected:
	////////////////////////////////////////////////////////////////////////////
	//
	// Handler methods. Instead of writing your own WriteStatement, just
	// implement the HandleXXXX methods and let the base class WriteStatement
	// do all the type stuff and lots of sanity checking... adds a lot of code
	// reuse and makes writing a new Stream much easier
	//
	////////////////////////////////////////////////////////////////////////////
	PT_Error HandleOpenStatement( PT_Open_Statement &st );
	PT_Error HandleCloseStatement( PT_Close_Statement &st );
	PT_Error HandleCommentStatement( PT_Comment_Statement &st );
	PT_Error HandleConfigEvent( PT_Config_Event &st );
	PT_Error HandleConfigData( PT_Config_Data &st );
	PT_Error HandleConfigRecord( PT_Config_Record &st );
	PT_Error HandleEventStatement( PT_Event_Statement &st );
	PT_Error HandleDataStatement( PT_Data_Statement &st );
	PT_Error HandleConfigStruct( PT_Config_Struct &st );
	PT_Error HandleUpdateStruct( PT_Update_Struct &st );
	PT_Error HandleClearStruct( PT_Clear_Struct &st );

private:

	////////////////////////////////////////////////////////////////////////////
	// Helper functions
	////////////////////////////////////////////////////////////////////////////
		
	// build a string on the fly from a varg list and write to disk
	// the last parameter must be "\n"
	PT_Error FastStringWrite( char *first, ... );

	// temp buffers for ascii conversion
	char		m_buf[ 256 ];
	char		m_buf2[ 256 ];
	char		m_buf3[ 256 ];
	char		m_buf4[ 256 ];
	char		m_buf5[ 256 ];
	char		m_buf6[ 256 ];
};


////////////////////////////////////////////////////////////////////////////////
//	Binary Writer 
////////////////////////////////////////////////////////////////////////////////
class PT_BinaryWriteStream : public PT_WriteStream
{
public:
	////////////////////////////////////////////////////////////////////////////
	// Contructors and Destructors
	////////////////////////////////////////////////////////////////////////////

	// Specify a file name. NULL = stdout
	PT_BinaryWriteStream( const char *szFile, PT_Error *Error, const char *szNamespace = NULL );
	virtual	~PT_BinaryWriteStream()	{ }
	
	////////////////////////////////////////////////////////////////////////////
	// User Methods
	////////////////////////////////////////////////////////////////////////////

	// Opens an output stream to a file and writes header info (version, etc..) 
	// If szFileName is NULL, stdout is used.
	// If version is NULL, default version is used
	// This should be automatically called by all constructors
	// PT_Error Open( char *szFileName, char *version = NULL );
	
	// Send anything being buffered down to disk
	//PT_Error Flush();

	// Write a statement to file
	// Implemented for all streams, including lots of sanity checking
	// All ASCII, Binary streams have to do is implement the Handle_XXX functions 
	// for specific types. 
	// The second parameter is an out parameter. If the PT_Statement
	// is an Open_Record then this must be provided to get the new record ID 
	// generated by the WriteStream. Otherwise, it can be left NULL
	// PT_Error WriteStatement( PT_Statement &current, PT_ID *ID = NULL );

	// returns the file version 
	const char *GetVersionInfo( );

	// returns 'mytype' as a string "ASCII" or "Binary"
	const char *GetFormatString( ) { return BINARY_FORMAT; }
	Pipe_Format GetFormat( ) { return Pipe_Binary_Format; }

protected:
	////////////////////////////////////////////////////////////////////////////
	//
	// Handler methods. Instead of writing your own WriteStatement, just
	// implement the HandleXXXX methods and let the base class WriteStatement
	// do all the type stuff and lots of sanity checking... adds a lot of code
	// reuse and makes writing a new Stream much easier
	//
	////////////////////////////////////////////////////////////////////////////
	PT_Error HandleOpenStatement( PT_Open_Statement &st );
	PT_Error HandleCloseStatement( PT_Close_Statement &st );
	PT_Error HandleCommentStatement( PT_Comment_Statement &st );
	PT_Error HandleConfigEvent( PT_Config_Event &st );
	PT_Error HandleConfigData( PT_Config_Data &st );
	PT_Error HandleConfigRecord( PT_Config_Record &st );
	PT_Error HandleEventStatement( PT_Event_Statement &st );
	PT_Error HandleDataStatement( PT_Data_Statement &st );
	PT_Error HandleConfigStruct( PT_Config_Struct &st );
	PT_Error HandleUpdateStruct( PT_Update_Struct &st );
	PT_Error HandleClearStruct( PT_Clear_Struct &st );

	PT_Error BinaryWrite( char *buffer, unsigned int length );

private:
	// taken from PSLib... put a word into the stream as compact as possible
	inline int put_word( unsigned long value, int next_loc );
	
	// Put a binary leader into the record for compactly identifying records
	// and statement types
	inline int put_leader( PT_ID record, PT_KEY key );
	inline int put_leader( PT_ID ID, PT_Config &st );

	char m_buf[ PT_MAX_PATH ];
	PT_Time		m_lastTime;		//times are stored as deltas

	PT_ID		m_lastRecord;	//last record we sent to file. if current = last
								//then this file format can ignore Record ID
};

////////////////////////////////////////////////////////////////////////////////
//	Buffered Reader - so that you're not reading one byte at a time
////////////////////////////////////////////////////////////////////////////////
#ifdef USE_ZLIB
class PT_BufferedReader
{
public: 


	PT_BufferedReader(	ZFILE file, PT_Error *Error, PT_UINT bufferSize = 4096 );
	ZFILE			m_file;							// file pointer

	virtual ~PT_BufferedReader( );

	// call this instead of fgets or gzgets. 
	char *PT_gets( char *buffer, PT_UINT bufSize );

	// call this instead of fgetc or gzgetc. 
	char PT_getc( );

	// call this instead of feof or gzeof. 
	int PT_eof( );
	
	inline PT_ULONG	GetBytesRead( ){ return m_bytesRead; }

protected:
	
	// have we hit end of file?
	bool m_bEOF;

	// 
	PT_UINT m_indexPointer;

	// our actual store buffer
	PT_UINT m_bufferSize;

	// store buffer
	char *m_pBuffer;

	// total # of bytes read so far
	PT_ULONG	m_bytesRead;

};
#endif


#endif //__PT_IO_H
  






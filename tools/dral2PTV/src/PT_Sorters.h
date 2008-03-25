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

#ifndef __PT_SORTERS_H
#define __PT_SORTERS_H

#include "PT_Data.h"

// java support
#include <jni.h>


////////////////////////////////////////////////////////////////////////////////
//
//	Class PT_ConfigRecordSorter: Provides a nice template for writing a 
//  sorter of PT_Config_Records. Abstract class
//
////////////////////////////////////////////////////////////////////////////////
class PT_ConfigRecordSorter : public PT_Sorter
{
public:
	// standard constructor
	PT_ConfigRecordSorter( PT_Comparer< PT_Config_Record * > *compare );

	// OnRecordConfigComplete default implementation adds all to the list
	// If you want to add based on criteria, you must override!!!
	// handler method
	virtual PT_Error OnRecordConfigComplete( PT_Config_Record *pNewRecord);

	// just implement OnRecordConfigComplete
	virtual void SetImplementBools( );

	// just return sorter.Length
	virtual PT_UINT Length(){ return m_sorter.Length(); }

	// "Getter method"
	virtual PT_Error GetByIndex( PT_UINT index, PT_Config_Record **retVal );

	// another way to access the RecordArray: thisRecord = MySorter[ index ];
	// if you go out of bounds this will return NULL
	PT_Config_Record *operator[]( PT_UINT index );

private:
	PT_SortVector< PT_Config_Record * > m_sorter;
	PT_Comparer< PT_Config_Record * > *m_compare;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Class PT_ConfigDataSorter: Provides a nice template for writing a 
//  sorter of PT_Config_Data. Abstract class
//
////////////////////////////////////////////////////////////////////////////////
class PT_ConfigDataSorter : public PT_Sorter
{
public:
	// standard constructor
	PT_ConfigDataSorter( PT_Comparer< PT_Config_Data * > *compare );

	// OnDataConfigComplete default implementation adds all to the list
	// If you want to add based on criteria, you must override!!!
	// handler method
	virtual PT_Error OnDataConfigComplete( PT_Config_Data *pNewData);

	// When records are Finished, update all its config datas to reflect
	// that they are record data
//	virtual PT_Error OnRecordComplete( PT_Record *pNewRecord);

	// just implement OnDataConfigComplete
	virtual void SetImplementBools( );

	// just return sorter.Length
	virtual PT_UINT Length(){ return m_sorter.Length(); }

	// "Getter method"
	virtual PT_Error GetByIndex( PT_UINT index, PT_Config_Data **retVal );

	// another way to access the DataArray: thisData = MySorter[ index ];
	// if you go out of bounds this will return NULL
	PT_Config_Data *operator[]( PT_UINT index );

private:
	PT_SortVector< PT_Config_Data * > m_sorter;
	PT_Comparer< PT_Config_Data * > *m_compare;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Class PT_ConfigEventSorter: Provides a nice template for writing a 
//  sorter of PT_Config_Events. Abstract class
//
////////////////////////////////////////////////////////////////////////////////
class PT_ConfigEventSorter : public PT_Sorter
{
public:
	// standard constructor
	PT_ConfigEventSorter( PT_Comparer< PT_Config_Event * > *compare );

	// OnEventConfigComplete default implementation adds all to the list
	// If you want to add based on criteria, you must override!!!
	// handler method
	virtual PT_Error OnEventConfigComplete( PT_Config_Event *pNewEvent);

	// just implement OnEventConfigComplete
	virtual void SetImplementBools( );

	// just return sorter.Length
	virtual PT_UINT Length(){ return m_sorter.Length(); }

	// "Getter method"
	virtual PT_Error GetByIndex( PT_UINT index, PT_Config_Event **retVal );

	// another way to access the EventArray: thisEvent = MySorter[ index ];
	// if you go out of bounds this will return NULL
	PT_Config_Event *operator[]( PT_UINT index );

private:
	PT_SortVector< PT_Config_Event * > m_sorter;
	PT_Comparer< PT_Config_Event * > *m_compare;
};

////////////////////////////////////////////////////////////////////////////////
//
//	Class PT_RecordIDSorter
//	A PT_Sorter that sorts all records by increasing ID#.
//	It does NOT base itself upon the RecordTemplateSorter b/c it can gain
//  efficiency by being friends with the datastore
//
////////////////////////////////////////////////////////////////////////////////
class PT_RecordIDSorter : public PT_Sorter
{
public:

	// Standard Constructor
	// Requiring a DataStore means that you can't pass this sorter into 
	// the constructor of a DataStore, you must use AddSorter
	PT_RecordIDSorter( PT_DataStore *store );

	// handler method
	virtual PT_Error OnRecordComplete( PT_Record *pNewRecord );

	// just implement OnRecordComplete
	virtual void SetImplementBools( );

	// just returns the highest ID of a closed record + 1. this way
	// it makes it easy to use this Sorter in a for loop. To get
	// number of closed records call NumClosed
	virtual PT_UINT Length(){ return m_length; }

	// number of closed records
	virtual PT_UINT NumClosed(){ return m_numClosed; }

	// "Getter method" - is actually "GetByID". If a record hasn't been 
	// closed (or opened) yet, it sets retVal to NULL
	// Remember - There is no record with ID 0!!!
	virtual PT_Error GetByIndex( PT_UINT index, PT_Record **retVal );

	// Same as GetByIndex except returns any record that exists, whether
	// or not it has been closed
	virtual PT_Error GetByIndexRegardless( PT_UINT index, PT_Record **retVal );

	// how many threads have we seen so far?
	inline PT_UCHAR GetNumThreads( ) {return (PT_UCHAR)(m_maxThread + 1);} 

	// index by ID. returns NULL if not open or closed yet
	// Remember - There is no record with ID 0!!!
	PT_Record *operator[]( PT_UINT index );

private:
	PT_DataStore *m_Store;

	PT_UCHAR	m_maxThread;	//largest thread we have seen so far
	PT_UINT		m_length;
	PT_UINT		m_numClosed;
};

class PTV_JavaComparer;
class PTV_OccurrenceComparer;

// sorter that maintains ordering of events in time, horizontal occurrence sorter
class PTV_OccurrenceSorter : public PT_Sorter
{
public:
	// standard constructor

	PTV_OccurrenceSorter( PT_StandardDataStore *store, PT_UINT maxNumEventTypes = 2048 );
	virtual ~PTV_OccurrenceSorter();

	// just implement OnEventConfigComplete and OnEventComplete
	virtual void SetImplementBools( );

	// these two methods will just call the JVM
	virtual PT_Error OnEventConfigComplete( PT_Config_Event *pNewConfig);
	virtual PT_Error OnEventComplete( PT_Event *pNewEvent);

	virtual PT_Error OnTraceComplete( );

	// E_NOTIMPL
	virtual PT_UINT Length(){ return 0; }

	virtual PT_UINT Length( PT_KEY eventIndex );

	// "Getter method" - E_NOTIMPL
	virtual PT_Error GetByIndex( PT_UINT index, int *retVal ){ return PT_Error_NotImpl; }

	// This is the real "Getter"
	virtual PT_Error GetByKey( PT_KEY eventKey, PT_UINT index, PT_Event **retVal );
 
	// If you know the index, this one is faster...
	virtual PT_Error GetByIndex( PT_KEY tableIndex, PT_UINT index, PT_Event **retVal );

	virtual PT_UINT GetIndexFromKey( PT_KEY key );

	// how much space do we use here?
	int GetNumBytes( )
	{	
		int count = 0;
		for(PT_UINT i = 0; i < m_maxTypes; i++)
		{
			count+= m_sorter[ i ]->GetAllocatedSize( ) * 4 /*todo use sizeof*/;
		}
		return count;
	}

private:

	// if we have lots of insert elements in middle, merge sort of two lists will 
	// be faster
	void MergeSorters( PT_UINT index );

	PT_SortVector< PT_Event * >	**m_sorter;
	PT_SortVector< PT_Event * >	**m_sorter2;
	PT_SortVector< PT_Event * >	**m_secondarySorter;
	PT_KEY					*m_lookupKeyTable;	// given a key, what is it's index in 
	PTV_OccurrenceComparer	*m_compare;
	PT_UINT					m_maxTypes;
	PT_USHORT				m_currentIndex;
	PT_StandardDataStore *m_pStore;
	PT_Time					*m_maxTime;
	PT_Time					*m_maxTime2;

	// to make us closer to threadsafe, don't delete this sortvector till later..
	PT_SortVector<PT_Event *> *m_tempSorter;

	// merge deletes our sortvector, this can cause some serious 
	// multithreaded sync issues
	bool					m_hackSemaphore;

	//PT_Time					m_currentMax;
	//PT_Time					m_currentStart;
	//PT_Event				***cacheArray;
public:
	jobject					m_lockObject;
	JNIEnv					*m_env;
};


// Custom Sorter used by the Java side to sort. The Java side provides
// handlers for OnRecord and OnEvent... Note that we cache the JNIEnv
// and jobject pointers from our constructor. This means that the sorter
// IS NOT THREADSAFE since if a different thread calls us, it will have
// a different JNIEnv pointer!!!!!!! However, we really have no choice 
// but to cache this stuff because otherwise the overhead of each call
// to onrecordcomplete would be prohibitive
class PTV_JavaSorter : public PT_Sorter
{
public:
	// standard constructor... we need the java object that we are going
	// to call back to.
	PTV_JavaSorter( JNIEnv *env, jobject CustomSorter, PT_Error *Error );
	virtual ~PTV_JavaSorter();

	//void UpdateJNIEnv( JNIEnv *env );

	// just implement OnRecordComplete and OnEventComplete
	virtual void SetImplementBools( );

	// these two methods will just call the JVM
	virtual PT_Error OnRecordComplete( PT_Record *pNewRecord);
	virtual PT_Error OnEventComplete( PT_Event *pNewEvent);

	// this one will call JVM as well
	virtual PT_Error OnTraceComplete( );

	// just return sorter.Length
	virtual PT_UINT Length(){ return m_sorter->Length(); }

	// "Getter method"
	virtual PT_Error GetByIndex( PT_UINT index, int *retVal );
 
	inline jobject GetJavaObject() {return m_pSorter; }

	// call to java to "wait( )" on this thread. allows other threads to do work
	// without us sucking away mips
	void StallThread( long millis );

	// for inserting new indices, using sorting (compare function)
	int InsertHandle( int Handle )
	{ 
		int retVal;
		m_sorter->InsertElement( Handle, &retVal );
		return retVal;
	}

	void InsertHandleAt( int Handle, int pos ){ m_sorter->InsertElementAt( Handle, pos ); }
	

	// for inserting new indices unsorted (you can quicksort it later)...
	inline PT_Error InsertElementNoSort( int Handle ){ return m_sorter->InsertElementNoSort( Handle );}

	PT_Error QuickSortVector( ){ return m_sorter->QuickSortArray( );}

	inline void RemoveAllElements( ){ m_sorter->RemoveAllElements( );}

private:
	PT_SortVector< int > *m_sorter;
	PTV_JavaComparer	*m_compare;

	JNIEnv				*m_pEnv;
	jobject				 m_pSorter;
	jmethodID			 m_RecordMethod;
	jmethodID			 m_EventMethod;
	jmethodID			 m_TraceMethod;

	bool m_SupportsRecordComplete;	// don't make these expensive java calls if we're not required to
	bool m_SupportsEventComplete;	// don't make these expensive java calls if we're not required to
};

// 1) grabs start time and end time of the trace for us
class PTV_EventSorter : public PT_Sorter
{
public:
	// standard constructor... 
	PTV_EventSorter( PT_StandardDataStore *store, PT_Error *Error );

	// just implement OnEventComplete
	virtual void SetImplementBools( );

	virtual PT_Error OnEventComplete( PT_Event *pNewEvent);

	// not implemented yet
	virtual PT_UINT Length(){ return (PT_UINT)-1; }

	// returns first time in this trace
	inline PT_Time	GetMaxTime(){ return m_EndTime;}

	// returns last time in this trace
	inline PT_Time	GetMinTime(){ return m_StartTime;}

private:
	// first time in this trace
	PT_Time				 m_StartTime;
	
	// last time in this trace
	PT_Time				 m_EndTime;

	PT_StandardDataStore *m_pStore;
};


#endif

  




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

#include "PT_Sorters.h"
#include <jni.h>

// for Occurrence sorter
#define SORTER2_TOLERANCE 1000
#define SORTER1_TOLERANCE 10

// cache this value here - it's valid across threads!
JavaVM *g_jvm; 
JNIEnv *g_env;

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_SortVector::~PT_SortVector
// Description: Destructor
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
/*template<class T>
PT_SortVector<T>::~PT_SortVector()
{
	if ( m_pData ) 
	{
		delete m_pData;
	}
}*/


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigRecordSorter::PT_ConfigRecordSorter
// Description: standard constructor
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_ConfigRecordSorter::PT_ConfigRecordSorter( PT_Comparer< PT_Config_Record * > *compare ) : 
	m_sorter( compare, 64 ), m_compare( compare )	// usually not that many record configs
{

}

/////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigRecordSorter::SetImplementBools
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_ConfigRecordSorter::SetImplementBools( )
{
	m_bImplementsRecordConfigComplete = true;
	m_bImplementsRecordComplete = false;
	m_bImplementsEventConfigComplete = false;
	m_bImplementsEventComplete = false;
	m_bImplementsDataConfigComplete = false;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigRecordSorter::OnRecordConfigComplete
// Description: handler method
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ConfigRecordSorter::OnRecordConfigComplete( PT_Config_Record *pNewRecord)
{
	m_sorter.InsertElement( pNewRecord );
	return PT_Error_OK;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigRecordSorter::GetByIndex
// Description: "Getter method". ToDo, return ArrayOutofBounds?
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ConfigRecordSorter::GetByIndex( PT_UINT index, PT_Config_Record **retVal )
{
	*retVal = *(m_sorter[ index ]);

	return PT_Error_OK;
}

// another way to access the RecordArray: thisRecord = MySorter[ index ];
// if you go out of bounds this will return NULL


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigRecordSorter::operator[]
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Config_Record *PT_ConfigRecordSorter::operator[]( PT_UINT index )
{
	PT_Config_Record **rec = m_sorter[ index ];

	if ( rec == NULL ) return NULL;

	return *rec;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigEventSorter::PT_ConfigEventSorter
// Description: standard constructor
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_ConfigEventSorter::PT_ConfigEventSorter( PT_Comparer< PT_Config_Event * > *compare ) : 
	m_compare( compare), m_sorter( compare, 256 )	// usually not that many event configs
{

}

/////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigEventSorter::SetImplementBools
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_ConfigEventSorter::SetImplementBools( )
{
	m_bImplementsRecordConfigComplete = false;
	m_bImplementsRecordComplete = false;
	m_bImplementsEventConfigComplete = true;
	m_bImplementsEventComplete = false;
	m_bImplementsDataConfigComplete = false;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigEventSorter::OnEventConfigComplete
// Description: handler method
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ConfigEventSorter::OnEventConfigComplete( PT_Config_Event *pNewEvent)
{
	m_sorter.InsertElement( pNewEvent );
	return PT_Error_OK;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigEventSorter::GetByIndex
// Description: "Getter method". ToDo, return ArrayOutofBounds?
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ConfigEventSorter::GetByIndex( PT_UINT index, PT_Config_Event **retVal )
{
	*retVal = *(m_sorter[ index ]);

	return PT_Error_OK;
}

// another way to access the EventArray: thisEvent = MySorter[ index ];
// if you go out of bounds this will return NULL


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigEventSorter::operator[]
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Config_Event *PT_ConfigEventSorter::operator[]( PT_UINT index )
{
	PT_Config_Event **event = m_sorter[ index ];

	if ( event == NULL ) return NULL;

	return *event;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigDataSorter::PT_ConfigDataSorter
// Description: standard constructor
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_ConfigDataSorter::PT_ConfigDataSorter( PT_Comparer< PT_Config_Data * > *compare ) : 
	m_compare( compare), m_sorter( compare, 32 )	// usually not that many data configs
{

}

/////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigDataSorter::SetImplementBools
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_ConfigDataSorter::SetImplementBools( )
{
	m_bImplementsRecordConfigComplete = false;
	m_bImplementsRecordComplete = false;
	m_bImplementsEventConfigComplete = false;
	m_bImplementsEventComplete = false;
	m_bImplementsDataConfigComplete = true;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigDataSorter::OnDataConfigComplete
// Description: handler method
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ConfigDataSorter::OnDataConfigComplete( PT_Config_Data *pNewData)
{
	m_sorter.InsertElement( pNewData );
	return PT_Error_OK;
}

// When records are Finished, update all its config datas to reflect
// that they are record data
/*
PT_Error PT_ConfigDataSorter::OnRecordComplete( PT_Record *pNewRecord)
{

	PT_DataList *dataList = pNewRecord->GetDataList( );
	if (dataList == NULL ) return PT_Error_OK;
	
	PT_Error Error;


	for( int i = 0; i < dataList->NumElements( ); i++ )
	{
		PT_KEY type = dataList->GetDefinitionKey( i );
		PT_Config_Data *config;
		
		Error = m_pStore->FindConfigByKey(type, (PT_Config**)(&outConfig));
		if (Error == PT_Error_OK)
		{
			config->m_bRecordData = true;
		}
		else
		{
			return Error;
		}
	}

	return Error;
}
*/

//////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigDataSorter::GetByIndex
// Description: "Getter method". ToDo, return ArrayOutofBounds?
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_ConfigDataSorter::GetByIndex( PT_UINT index, PT_Config_Data **retVal )
{
	*retVal = *(m_sorter[ index ]);

	return PT_Error_OK;
}

// another way to access the DataArray: thisData = MySorter[ index ];
// if you go out of bounds this will return NULL


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_ConfigDataSorter::operator[]
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Config_Data *PT_ConfigDataSorter::operator[]( PT_UINT index )
{
	PT_Config_Data **data = m_sorter[ index ];

	if ( data == NULL ) return NULL;

	return *data;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordIDSorter::PT_RecordIDSorter
// Description: Requiring a DataStore means that you can't pass this sorter into 
// the constructor of a DataStore, you must use AddSorter
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_RecordIDSorter::PT_RecordIDSorter( PT_DataStore *store )
{
	m_Store = store;
	m_numClosed = 0;
	m_length = 0;
	m_maxThread = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordIDSorter::OnRecordComplete
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_RecordIDSorter::OnRecordComplete( PT_Record *pNewRecord )
{
	m_numClosed++;

	PT_ID ID = pNewRecord->GetRecordID( );

	if ( ID + 1 > m_length )
	{
		m_length = ID + 1;
	}

	if (pNewRecord->GetThreadID( ) > m_maxThread)
		m_maxThread = pNewRecord->GetThreadID( );

	return PT_Error_OK;
}

	
////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordIDSorter::SetImplementBools
// Description: just implement OnRecordComplete
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PT_RecordIDSorter::SetImplementBools( )
{
	m_bImplementsRecordConfigComplete = false;
	m_bImplementsRecordComplete = true;
	m_bImplementsEventConfigComplete = false;
	m_bImplementsEventComplete = false;
	m_bImplementsDataConfigComplete = false;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordIDSorter::GetByIndex
// Description: 
// "Getter method" - is actually "GetByID". If a record hasn't been 
// closed (or opened) yet, it sets retVal to NULL
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_RecordIDSorter::GetByIndex( PT_UINT index, PT_Record **retVal )
{
	// there is no record with record ID 0!!!
	if ( index == 0 )
	{
		*retVal = NULL;
		return PT_Error_BadID;
	}

	*retVal = m_Store->GetRecordByID( index );

	if ( (*retVal == NULL) || (!(*retVal)->GetBoolVal( RECORD_CLOSED_BIT )) )
	{
		// doesn't exist or not closed yet
		*retVal = NULL;
		return PT_Error_OK;
	}
	else
	{
		// exists and closed
		return PT_Error_OK;
	}

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordIDSorter::GetByIndex
// Description: 
// "Getter method" - same as GetByIndex except doesn't return null if a record
// exists but is not closed
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PT_RecordIDSorter::GetByIndexRegardless( PT_UINT index, PT_Record **retVal )
{
	// there is no record with record ID 0!!!
	if ( index == 0 )
	{
		*retVal = NULL;
		return PT_Error_BadID;
	}

	*retVal = m_Store->GetRecordByID( index );

	if (*retVal == NULL)
	{
		return PT_Error_OK;
	}
	else
	{
		// exists and closed
		return PT_Error_OK;
	}

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_RecordIDSorter::operator
// Description: 
// index by ID. returns NULL if not open or closed yet
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Record *PT_RecordIDSorter::operator[]( PT_UINT index )
{
	PT_Record *retVal;
	if ( PT_SUCCEEDED(GetByIndexRegardless(index, &retVal) ) )
	{
		return retVal;
	}

	return NULL;
}

PTV_EventSorter::PTV_EventSorter( PT_StandardDataStore *store ,PT_Error *Error )
{
	m_pStore = store;
	m_EndTime = 0;
	m_StartTime = (PT_Time)-1;
}

// just implement OnRecordComplete and OnEventComplete
void PTV_EventSorter::SetImplementBools( )
{
	m_bImplementsRecordConfigComplete = false;
	m_bImplementsRecordComplete = false;
	m_bImplementsEventConfigComplete = false;
	m_bImplementsEventComplete = true;
	m_bImplementsDataConfigComplete = false;
}

PT_Error PTV_EventSorter::OnEventComplete( PT_Event *pNewEvent)
{
	if (( pNewEvent->GetStartTime(m_pStore) < m_StartTime ) || ( m_StartTime == (PT_Time)-1 ))
	{
		m_StartTime = pNewEvent->GetStartTime(m_pStore);
	}
	else if ( pNewEvent->GetEndTime(m_pStore ) > m_EndTime )
	{
		m_EndTime = pNewEvent->GetEndTime(m_pStore );
	}

	return PT_Error_OK;
}


// Comparer for sortvector in our javasorter
class PTV_JavaComparer : public PT_Comparer<int>
{
friend class PTV_JavaSorter;

public:
	PTV_JavaComparer( JNIEnv *env, jobject CustomSorter, PT_Error *Error )
	{
		*Error = PT_Error_OK;

		m_pEnv = NULL;
		m_pSorter = (jobject)NULL;
		m_compareMethod = (jmethodID)NULL;

		jclass cls = env->GetObjectClass( CustomSorter );
		jmethodID mid = env->GetMethodID( cls, "Compare", "(II)I");
		if (mid == 0) 
		{
			*Error = PT_Error_Fail;
			return;
		}

		// ok, everything's cool!
		m_pEnv = env;
		m_pSorter = CustomSorter;
		m_compareMethod = mid;
		m_baseClass = cls;
	}

	virtual int Compare( int _one, int _two )
	{
		PT_ASSERT( m_pEnv && m_pSorter );

		jint i = m_pEnv->CallIntMethod( m_pSorter, m_compareMethod, (jint)_one, (jint)_two );
		return (int) i;
	}


private:
	JNIEnv *m_pEnv;
	jobject m_pSorter;
	jmethodID m_compareMethod;
	jclass m_baseClass;
};

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PTV_JavaSorter::PTV_JavaSorter
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PTV_JavaSorter::PTV_JavaSorter( JNIEnv *env, jobject CustomSorter, PT_Error *Error ) //:
//	m_sorter( compare, 1024 )
{
	*Error = PT_Error_OK;

	m_pEnv = NULL;
	m_pSorter = (jobject)NULL;
	m_RecordMethod = (jmethodID)NULL;
	m_EventMethod = (jmethodID)NULL;
	m_TraceMethod = (jmethodID)NULL;

	jclass cls = env->GetObjectClass( CustomSorter );

	m_SupportsRecordComplete = false;
	m_SupportsEventComplete = false;
	jmethodID supportsRmid = env->GetMethodID( cls, "SupportsRecordComplete", "()Z");
	
	if (supportsRmid != 0) 
	{
		// see if we should notify of onrecordcompletes
		m_SupportsRecordComplete = (bool)env->CallBooleanMethod( CustomSorter, supportsRmid );
	}

	jmethodID supportsEmid = env->GetMethodID( cls, "SupportsEventComplete", "()Z");
	if (supportsEmid != 0) 
	{
		// see if we should notify of onrecordcompletes
		m_SupportsEventComplete = (bool)env->CallBooleanMethod( CustomSorter, supportsEmid );
	}

	jmethodID mid = env->GetMethodID( cls, "OnRecordComplete", "(I)V");
    if (mid == 0) 
	{
		*Error = PT_Error_Fail;
        return;
    }
    jmethodID mid2 = env->GetMethodID( cls, "OnEventComplete", "(I)V");
    if (mid2 == 0) 
	{
		*Error = PT_Error_Fail;
        return;
    }

    jmethodID mid3 = env->GetMethodID( cls, "OnTraceComplete", "()V");
    if (mid3 == 0) 
	{
		*Error = PT_Error_Fail;
        return;
    }

	
	m_compare = new PTV_JavaComparer( env, CustomSorter, Error );
	m_sorter = new PT_SortVector<int>( m_compare, 1024 );

	// ok, everything's cool!
	m_pEnv = env;
	m_pSorter = CustomSorter;
	m_RecordMethod = mid;
	m_EventMethod = mid2;
	m_TraceMethod = mid3;
}


PTV_JavaSorter::~PTV_JavaSorter( )
{
	if (m_sorter != NULL)
	{
		delete m_sorter;
	}
	if (m_compare != NULL)
	{
		delete m_compare;
	}

}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PTV_JavaSorter::OnRecordComplete
// Description: asks the java class to handle this for us
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PTV_JavaSorter::OnRecordComplete( PT_Record *pNewRecord)
{
	PT_ASSERT( m_pEnv && m_pSorter && pNewRecord );

	if ( m_SupportsRecordComplete )
		m_pEnv->CallVoidMethod( m_pSorter, m_RecordMethod, (jint)(pNewRecord->GetRecordID()) );

	return PT_Error_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PTV_JavaSorter::OnEventComplete
// Description: asks the java class to handle this for us
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PT_Error PTV_JavaSorter::OnEventComplete( PT_Event *pNewEvent)
{
	PT_ASSERT( m_pEnv && m_pSorter && pNewEvent );

	// cast the pointer to an int handle for java
	if ( m_SupportsEventComplete )
		m_pEnv->CallVoidMethod( m_pSorter, m_EventMethod, (jint)(pNewEvent) );

	return PT_Error_OK;

}

PT_Error PTV_JavaSorter::OnTraceComplete( )
{
	PT_ASSERT( m_pEnv && m_pSorter);
//cerr<<"trace complete"<<endl;
//    m_TraceMethod = m_pEnv->GetMethodID( m_pEnv->GetObjectClass( m_pSorter ), "OnTraceComplete", "()V");

	// cast the pointer to an int handle for java
//	m_pEnv->CallVoidMethod( m_pSorter, m_TraceMethod );

	return PT_Error_OK;
}

void PTV_JavaSorter::StallThread( long millis )
{
    jmethodID stall = m_pEnv->GetMethodID( m_pEnv->GetObjectClass( m_pSorter ), "wait", "(J)V");

	if (stall != 0)
	{
		// cast the pointer to an int handle for java
		m_pEnv->CallVoidMethod( m_pSorter, stall, millis );
	}
	
}

// "Getter method"
PT_Error PTV_JavaSorter::GetByIndex( PT_UINT index, int *retVal )
{
	int *val = m_sorter->GetElement( index );
	if ( val != NULL )
	{
		*retVal = *val;
		return PT_Error_OK;
	}
	return PT_Error_Fail;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PT_JavaSorter::SetImplementBools
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
void PTV_JavaSorter::SetImplementBools( )
{
	m_bImplementsRecordConfigComplete = false;
	m_bImplementsRecordComplete = m_SupportsRecordComplete;
	m_bImplementsEventConfigComplete = false;
	m_bImplementsEventComplete = m_SupportsEventComplete;
	m_bImplementsDataConfigComplete = false;
}

class PTV_OccurrenceComparer : public PT_Comparer< PT_Event * >
{
public:
	PT_ID cachedID;
	PT_Time cachedTime;
	PT_Event *cachedEvent;

	PTV_OccurrenceComparer( PT_StandardDataStore *store )
	{
		m_pStore = store;
		cachedEvent = 0;
	}
	
	// 1st level comparison is start time, second level compare is recID
	virtual int Compare( PT_Event * _one, PT_Event * _two )
	{
		PT_ID id1;
		PT_Time oTime;

		if (cachedEvent == _one)
		{
			id1 = cachedID;
			oTime = cachedTime;
		}
		else
		{
			id1 = _one->GetRecordID( );
			oTime = _one->GetStartTime( m_pStore->GetRecordByID( id1 ) );
			
			cachedID = id1;
			cachedTime = oTime;
			cachedEvent = _one;
		}

		PT_ID id2 = _two->GetRecordID( );
		PT_Time tTime = _two->GetStartTime( m_pStore->GetRecordByID( id2 ) );
		
		if ( oTime > tTime ) return 1;
		else if ( oTime < tTime ) return -1;
		else
		{	
			if (  id1 > id2 ) return 1;
			else if ( id1 < id2 ) return -1;
			return 0;
		}
	}

	PT_StandardDataStore *m_pStore;
};

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: PTV_OccurrenceSorter::PTV_OccurrenceSorter
// Description: 
// 
// 
// Parameters: 
// Returns: 
//
////////////////////////////////////////////////////////////////////////////////
PTV_OccurrenceSorter::PTV_OccurrenceSorter( PT_StandardDataStore *store, PT_UINT maxNumEventTypes )
{
	m_hackSemaphore = false;
	m_lockObject = NULL;

	m_tempSorter = NULL;
	m_lookupKeyTable = new PT_KEY[ maxNumEventTypes ];
	m_compare = new PTV_OccurrenceComparer( store );
	m_pStore = store;
	m_maxTime = new PT_Time[ maxNumEventTypes ];
	m_maxTime2 = new PT_Time[ maxNumEventTypes ];

	m_sorter = new PT_SortVector<PT_Event *> *[ maxNumEventTypes ];
	m_sorter2 = new PT_SortVector<PT_Event *> *[ maxNumEventTypes ];
	for( PT_UINT i = 0; i < maxNumEventTypes; i++ )
	{
		// slow growth vector
		m_sorter[ i ] = new PT_SortVector<PT_Event *>( m_compare, 4, true );
		m_sorter2[ i ] = new PT_SortVector<PT_Event *>( m_compare, 4, true );
		m_maxTime[ i ] = 0;
		m_maxTime2[ i ] = 0;
	}

	m_maxTypes = maxNumEventTypes;
	m_currentIndex = 0;
}

PTV_OccurrenceSorter::~PTV_OccurrenceSorter(  )
{
	
	for( PT_UINT i = 0; i < m_maxTypes; i++ )
	{
		delete m_sorter[ i ];
	}

	delete []m_sorter;
	
	if ( m_sorter2 != NULL )
	{
		for( PT_UINT i = 0; i < m_maxTypes; i++ )
		{
			delete m_sorter2[ i ];
		}
		
		delete []m_sorter2;
	}

	if ( m_tempSorter != NULL )
	{
		delete m_tempSorter;
	}

	delete m_compare;
	delete m_lookupKeyTable;
	delete m_maxTime;
	delete m_maxTime2;
}

PT_Error PTV_OccurrenceSorter::OnTraceComplete( )
{
	// now we can get rid of the secondary sorters by doing a final merge
	if ( m_sorter2 != NULL )
	{
		for( PT_UINT i = 0; i < m_maxTypes; i++ )
		{
			MergeSorters( i );
			delete m_sorter2[ i ];
		}
		
		delete []m_sorter2;
		m_sorter2 = NULL;
	}	

	return PT_Error_OK;
}
	
PT_Error PTV_OccurrenceSorter::OnEventComplete( PT_Event *pNewEvent )
{
	PT_KEY key = pNewEvent->GetDefinitionKey( );
	PT_UINT index;
	
	if ( key < m_maxTypes ) 
	{
		index = m_lookupKeyTable[ key ];
	}
	else
	{
		PT_WARNING("Invalid Key!!! Can't find event in occ sorter table!");
		return PT_Error_Fail;
	}

	int outPos;

	// order of complexity of below algo...
	// best case, always insertatend of list1 -> O(n)
	// worst case->
	// always insertelement into list2 = 500 * n + n/500 merges
	// merge is an O(n) algo, so worst case is still O(n^2)... 
	// but maybe n^2/500 is good enough here? anyway, it is 500 times faster than we were before

	PT_Time curTime = pNewEvent->GetStartTime( m_pStore );
	if ((curTime > m_maxTime[ index ]) ||  
		((curTime == m_maxTime[ index ]) && (m_sorter[ index ]->IsEmpty() || m_compare->Compare( pNewEvent, *(m_sorter[ index ]->GetLastElement( ))) > 0)))
	{
		m_maxTime[ index ] = curTime;
		m_sorter[ index ]->InsertElementAtEnd( pNewEvent );
	}
	else if ( (curTime > (m_maxTime[ index ] - SORTER1_TOLERANCE) ) )
	{
		// if we are close to the end, then not TOO expensive...
		int outPos;
		m_sorter[ index ]->InsertElementNearEnd( pNewEvent, &outPos );
	}
	else
	{
		// see if we can insert quickly into other list... NEVER do a slow insert to the main list
		// do a merge sort when sorter2 list has grown to a big enough size
		if ((curTime > m_maxTime2[ index ]) ||  
			((curTime == m_maxTime2[ index ]) && (m_sorter2[index]->IsEmpty( ) || m_compare->Compare( pNewEvent, *(m_sorter2[ index ]->GetLastElement( ))) > 0)))
		{
			m_maxTime2[ index ] = curTime;
			m_sorter2[ index ]->InsertElementAtEnd( pNewEvent );
		}
		else
		{
			//insert into list 2
			m_sorter2[ index ]->InsertElement( pNewEvent, &outPos );
		}

		if (m_sorter2[ index ]->Length( ) > SORTER2_TOLERANCE)
		{
			// now, do a merge sort
			MergeSorters( index );
		}
	}

	return PT_Error_OK;
}


void PTV_OccurrenceSorter::SetImplementBools( )
{
	m_bImplementsRecordConfigComplete = false;
	m_bImplementsRecordComplete = false;
	m_bImplementsEventConfigComplete = true;
	m_bImplementsEventComplete = true;
	m_bImplementsDataConfigComplete = false;
}

PT_Error PTV_OccurrenceSorter::OnEventConfigComplete( PT_Config_Event *pNewEvent)
{
	m_lookupKeyTable[pNewEvent->m_key] = m_currentIndex;
	m_currentIndex++;
	
	return PT_Error_OK;
}

jobject GetLockObject( JNIEnv * env )
{
	jclass cls = env->FindClass("jptv/data/JPT_HorizontalOccurrenceSorter");
    if (!cls) return (jobject)NULL;

    jfieldID fidOut = env->GetStaticFieldID( cls, "lockObject", "Ljava/lang/Object;");
    if (!fidOut) return (jobject)NULL;

    jobject objOut = env->GetStaticObjectField(cls, fidOut);
    if (!objOut) return (jobject)NULL;

	return objOut;

}

void PTV_OccurrenceSorter::MergeSorters( PT_UINT index )
{
	JNIEnv *env;
	jobject obj;
//cerr<<"entering!!!"<<endl;
	//if (m_lockObject != NULL)
	{
//cerr<<"entering2!!!"<<g_jvm<<endl;
		//synchronize this section!!!!	
		int ret = g_jvm->AttachCurrentThread((void**)&env, NULL);
//cerr<<"entering2!!!"<<m_lockObject<<" " << 0 << endl;
		
		obj = GetLockObject(env);
		env->NewGlobalRef( obj );
		env->MonitorEnter( obj );
//cerr<<"entering2!!!"<<endl;
    }
//	else
//	{
//cerr<<"not entering2!!!"<<endl;
//	}

	// make the switch to the merged sorter as atomic as possible,
	// so that we are virtually thread safe here!
	/*if (m_tempSorter != NULL)
	{
		delete m_tempSorter;
		m_tempSorter = NULL;
	}
	*/
	PT_SortVector<PT_Event *> *tempSorter = new PT_SortVector<PT_Event *>( m_compare, m_sorter[ index ]->GetAllocatedSize( ), true );

	// merge algo goes here
	PT_UINT len1 = m_sorter[ index ]->Length( );
	PT_UINT len2 = m_sorter2[ index ]->Length( );
	PT_UINT cur1 = 0;
	PT_UINT cur2 = 0;

	while( (cur1 != len1) || (cur2 != len2) )
	{
		PT_ASSERT( cur1 <= len1 && cur2 <= len2 );

		if ((cur1 != len1) && ((cur2 == len2) || m_compare->Compare( *(m_sorter2[ index ]->GetElement( cur2 )), *(m_sorter[ index ]->GetElement( cur1 )) ) >= 0))
		{
			tempSorter->InsertElementAtEnd( *(m_sorter[ index ]->GetElement( cur1 )) );
			cur1++;
		}
		else
		{
			tempSorter->InsertElementAtEnd( *(m_sorter2[ index ]->GetElement( cur2 )) );
			cur2++;
		}
	} 

	m_hackSemaphore = true;
//	PT_SortVector<PT_Event *> *temp2Sorter = m_sorter[index];
	m_sorter[ index ] = tempSorter;
	
	// store this around till next time?
	//delete temp2Sorter;
	m_hackSemaphore = false;

	m_sorter2[ index ]->RemoveAllElements( );

	//if (m_lockObject != NULL)
	{
		env->MonitorExit( obj );
		env->DeleteGlobalRef( obj );

//cerr<<"exiting2!!!"<<endl;
	}
//cerr<<"exiting!!!"<<endl;
}

PT_UINT PTV_OccurrenceSorter::Length( PT_KEY eventIndex )
{
	//while( m_hackSemaphore );

	if ( eventIndex < m_maxTypes ) 
	{
		return m_sorter[ eventIndex ]->Length( );
	}
	else
	{
		return 0;
	}
}

// This is the real "Getter"
PT_Error PTV_OccurrenceSorter::GetByKey( PT_KEY eventKey, PT_UINT index, PT_Event **retVal )
{
	//while( m_hackSemaphore );
	
	int idx;
	if ( eventKey < m_maxTypes ) 
	{
		idx = m_lookupKeyTable[ eventKey ];
	}
	else
	{
		PT_WARNING("Invalid Key!!! Can't find event in occ sorter table!");
		return PT_Error_Fail;
	}

	if (index < m_sorter[ idx ]->Length( ))
	{
		*retVal = *(m_sorter[ idx ]->GetElement( index ));
		return PT_Error_OK;
	}

	PT_WARNING("Index out of bounds!!!");
	return PT_Error_Fail;
}

PT_Error PTV_OccurrenceSorter::GetByIndex( PT_KEY tableIndex, PT_UINT index, PT_Event **retVal )
{
	//while( m_hackSemaphore );

	if (index < m_sorter[ tableIndex ]->Length( ))
	{
		*retVal = *(m_sorter[ tableIndex ]->GetElement( index ));
		return PT_Error_OK;
	}

	PT_WARNING("Index out of bounds!!!");
	return PT_Error_Fail;	
}

PT_UINT PTV_OccurrenceSorter::GetIndexFromKey( PT_KEY key )
{
	//while( m_hackSemaphore );

	int idx;
	if ( key < m_maxTypes ) 
	{
		idx = m_lookupKeyTable[ key ];
		return idx;
	}
	else
	{
		return (PT_UINT)-1;
	}
}


 




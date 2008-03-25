// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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

/**
  * @file  LiveDB.h
  */

#ifndef _LIVEDB_H
#define _LIVEDB_H

// QT Library
#include <qintdict.h>
#include <qptrlist.h>
#include <qmessagebox.h>

// Dreams Library (client)
#include "asim/dralListenerOld.h"

#include "asim/DralDB.h"

// 2Dreams
#include "ZDBase.h"
#include "DBStructs.h"
#include "ConfigDB.h"
#include "CSLEngine.h"
#include "PreferenceMgr.h"

#define MAX_ITEMID_WARNS 50

//extern bool debug_on;

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class LiveDB : public AMemObj, public DRAL_LISTENER_OLD_CLASS, public StatObj
{
    public:
        static LiveDB* getInstance ();
        static void destroy();

    public:
        // ---- AMemObj Interface methods
        INT64  getObjSize() const;
        QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------

    protected:
        LiveDB();
        virtual ~LiveDB();

    // -----------------------------------------------
    // Dral Listener Interface methods
    // -----------------------------------------------
    public:

        void Cycle (
                   UINT64 n
                   );

        void NewItem (
                      UINT32 item_id
                     ) ;

        /**
          * Function description
          */
        inline void SetTagSingleValue (
                     UINT32 item_id,
                     char* tag_name,
                     UINT64 value,
                     UBYTE time_span
                    ) ;

        /**
          * Function description
          */
        inline void SetTagString (
                     UINT32 item_id,
                     char* tag_name,
                     char* str,
                     UBYTE time_span
                    ) ;

        /**
          * Function description
          */
        inline void SetTagSet (
                     UINT32 item_id,
                     char* tag_name,
                     UINT32 nval,
                     UINT64* value,
                     UBYTE time_span
                    ) ;

        /**
          * Function description
          */
        void MoveItems (
                       UINT16 edge_id,
                       UINT32 n,
                       UINT32* item_id
                      ) ;


        /**
          * Function description
          */
        void MoveItemsWithPositions (
                                             UINT16 edge_id,
                                             UINT32 numOfItems,
                                             UINT32 * items,
                                             UINT32 * positions
                                             );

        /**
          * Function description
          */
        inline void EnterNode (
                        UINT16 node_id,
                        UINT32 item_id,
                        UINT32 slot_index
                       ) ;

        /**
          * Function description
          */
        inline void ExitNode (
                       UINT16 node_id,
                       UINT32 slot_index
                      );

        /**
          * Function description
          */
        inline void DeleteItem (
                        UINT32 item_id
                        ) ;

        /**
          * Function description
          */
        void EndSimulation() ;

        /**
          * Function description
          */
        void AddNode (
                             UINT16 node_id,
                             char * node_name,
                             UINT16 parent_id,
                             UINT16 instance
                             );

        /**
          * Function description
          */
        void AddEdge (
                             UINT16 sourceNode,
                             UINT16 destNode,
                             UINT16 edge_id,
                             UINT32 bandwidth,
                             UINT32 latency,
                             char * name
                             );

        /**
          * Function description
          */
        void SetCapacity (
                                 UINT16 node_id,
                                 UINT32 capacity,
                                 UINT32 capacities [],
                                 UINT16 dimensions
                                 );

        /**
          * Function description
          */
        void SetHighWaterMark (
                                      UINT16 node_id,
                                      UINT32 mark
                                      );

        /**
          * Function description
          */
        void Comment (char * comment);

        /**
          * Function description
          */
        void Error (char * error);

        /**
          * Function description
          */
        void NonCriticalError (char * error);

        /**
          * Function description
          */
        inline void Version (UINT16 version);

        inline void NewNode (UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance);

        inline void NewEdge (UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,
             UINT32 bandwidth, UINT32 latency, char * name);

        inline void SetNodeLayout (UINT16 node_id, UINT32 capacity, UINT16 dim, UINT32 capacities []);

        inline void EnterNode (UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position []);
        inline void ExitNode (UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position []);

        inline void SetCycleTag(char tag_name [], UINT64 value);
        inline void SetCycleTagString(char tag_name [], char str []);
        inline void SetCycleTagSet(char tag_name [], UINT32 nval, UINT64 set []);

        inline void SetItemTag(UINT32 item_id, char tag_name [], UINT64 value);
        inline void SetItemTagString(UINT32 item_id, char tag_name [], char str []);
        inline void SetItemTagSet(UINT32 item_id, char tag_name [], UINT32 nval, UINT64 set []);

        inline void SetNodeTag(UINT16 node_id, char tag_name [], UINT64 value,UINT16 level, UINT32 list []);
        inline void SetNodeTagString(UINT16 node_id, char tag_name [], char str [],
             UINT16 level, UINT32 list []);
        inline void SetNodeTagSet(UINT16 node_id, char tag_name [], UINT16 n, UINT64 set [],
             UINT16 level, UINT32 list []);

        void Comment (UINT32 magic_num, char comment []);
        void CommentBin (UINT16 magic_num, char * cont, UINT32 length);

        void StartActivity(UINT64 _firstEffectiveCycle);

        void SetNodeInputBandwidth(UINT16 node_id, UINT32 bandwidth);
        void SetNodeOutputBandwidth(UINT16 node_id, UINT32 bandwidth);
        void SetTagDescription (char tag_name [], char description []);
        void SetNodeClock(UINT16, UINT16) {}
        void NewClock(UINT16, UINT64, UINT16, UINT16, const char []) {}
        void Cycle(UINT16, UINT64, UINT16) {}

        
     // -----------------------------------------------
     // Other LiveDB public methods
     // -----------------------------------------------
     public:
        /**
          * Convenience function ...
          */
        void reset();

        /**
          * Convenience function ...
          */
        //inline bool getLastProcessedEventStatus();
        inline bool getHasFatalError();

        /**
          * Convenience function ...
          */
        inline bool nonCriticalErrors();

        inline bool reachedEOS();
        void Do_EndSimulation();

    // -----------------------------------------------
    // -----------------------------------------------
    protected:
        /**
          * Function description
          */
        //void commitNodeOccupancies();
        void commitNodeTags();

        /**
          * Convenience function ...
          */
        void   computeDeleteItems();
        void computeDeleteItemsLong();
        void computeDeleteItemsLongDlg();
        bool   computeDeleteItem (ItemHandler * hnd,LNewItemListNode2D*);
        inline bool computeDeleteItem (UINT32 item_id);

        void computeNodeRowNumEvents();
        void commitNodeEvents();
        
        /**
          * Convenience function ...
          */
        inline bool incNumCycles(INT32 offset=1);

        /**
          * Function description
          */
        void forceAgePurge();
        inline void resetEdgeLastUsedPos();

    // -----------------------------------------------
    // -----------------------------------------------
    private:

        // basic state
        UINT16 currentTraceVersion;
        UINT32 currentCycle;
        UINT32 firstEffectiveCycle;

        bool   eoSimulation;
        bool   firstCycle;
        //bool   hasNonCriticalErros;
        bool hasFatalError;

        INT32  biggestLat;
        NewItemList2D*    itemList;
        DeleteItemList2D* deleteList;
        //EnterNodeList2D*  enterList;
        //ExitNodeList2D*   exitList;
        NRCEList*         nrceList;


        ZDBase*        myZDB;
        ConfigDB*      myConfigDB;
        CSLEngine*     csle;
        LogMgr*        myLogMgr;
        DralDB*        draldb;
        QString        profcmd;
        PreferenceMgr* pref;

        UINT8* lastUsedPosVector;
        INT32  numConfEdges;
        UINT16 lastUsedPosVectorIndirection[65536];

    private:
       static LiveDB*  _myInstance;
};

bool
LiveDB::incNumCycles(INT32 offset)
{ return myZDB->incNumCycles(offset); }

bool
LiveDB::getHasFatalError()
{ return hasFatalError || draldb->getHasFatalError(); }

bool
LiveDB::reachedEOS()
{ return eoSimulation || draldb->reachedEOS(); }

bool
LiveDB::nonCriticalErrors()
{
    //return hasNonCriticalErros;
    return false;
}

void
LiveDB::EnterNode (UINT16 node_id,UINT32 item_id,UINT32 slot_index)
{
    /*
    // I need to check that a "new item" was launched for item_id
    LNewItemListNode2D* newItemNode = itemList->find((long)item_id);

    if ((newItemNode==NULL) && !(pref->getRelaxedEENode()) )
    {
        return;
    }
    else
    {
        enterList->append((UINT32)node_id);
    }
    */
}

void
LiveDB::ExitNode (UINT16 node_id,UINT32 slot_index)
{
    //exitList->append((UINT32)node_id);
}

void
LiveDB::SetItemTag(UINT32 item_id, char tag_name [], UINT64 value)
{}

void LiveDB::SetItemTagString(UINT32 item_id, char tag_name [], char str [])
{}

void
LiveDB::SetItemTagSet(UINT32 item_id, char tag_name [], UINT32 nval, UINT64 set [])
{}

void
LiveDB::SetTagSingleValue (UINT32 item_id,char* tag_name,UINT64 value,UBYTE time_span)
{}

void
LiveDB::SetTagString (UINT32 item_id,char* tag_name,char* str,UBYTE time_span)
{}

void
LiveDB::SetTagSet (UINT32 item_id,char* tag_name,UINT32 nval,
               UINT64* value,UBYTE time_span)
{}


// ------------------------------------------------------------------
// -- Dral 2.0
// ------------------------------------------------------------------

void
LiveDB::Version (UINT16 version)
{ currentTraceVersion = version; }

void
LiveDB::NewNode (UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance)
{ AddNode (node_id,node_name,parent_id, instance); }

void
LiveDB::NewEdge (UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,
     UINT32 bandwidth, UINT32 latency, char * name)
{ AddEdge(sourceNode,destNode,edge_id,bandwidth,latency,name); }

void
LiveDB::SetNodeLayout (UINT16 node_id, UINT32 capacity, UINT16 dim, UINT32 capacities [])
{ SetCapacity (node_id,capacity,capacities,dim); }

void
LiveDB::EnterNode (UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position [])
{}

void
LiveDB::ExitNode (UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position [])
{}

void
LiveDB::SetCycleTag(char tag_name [], UINT64 value)
{}

void
LiveDB::SetCycleTagString(char tag_name [], char str [])
{}

void
LiveDB::SetCycleTagSet(char tag_name [], UINT32 nval, UINT64 set [])
{}

void
LiveDB::SetNodeTag(UINT16 node_id, char tag_name [], UINT64 value,UINT16 level, UINT32 list [])
{}

void
LiveDB::SetNodeTagString(UINT16 node_id, char tag_name [], char str [],
     UINT16 level, UINT32 list [])
{}

void
LiveDB::SetNodeTagSet(UINT16 node_id, char tag_name [], UINT16 n, UINT64 set [],
     UINT16 level, UINT32 list [])
{}

bool
LiveDB::computeDeleteItem (UINT32 item_id)
{
    // I need to check that a "new item" was launched for item_id
    LNewItemListNode2D* newItemNode = itemList->find((long)item_id);

    // If there is not such a "new item" command...
    if (newItemNode==NULL)
    {
        //{ printf("node not fnd on %d\n",hnd.getItemId()); fflush(stdout); }
        return false;
    }

    //printf("LiveDB::computeDeleteItem(int) called on itemid=%u \n",item_id);
    ItemHandler hnd;
    draldb->lookForItemId(&hnd, (INT32) item_id);
    if (!hnd.isValidItemHandler())
    {
        //printf("invalid handler...\n");
        return false;
    }

    return computeDeleteItem(&hnd,newItemNode);
}

void
LiveDB::resetEdgeLastUsedPos()
{
    bzero((char*)lastUsedPosVector,sizeof(UINT8)*numConfEdges);
}

#endif

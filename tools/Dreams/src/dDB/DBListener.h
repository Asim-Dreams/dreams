// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
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
  * @file  DBListener.h
  */

#ifndef _DRALDB_DBLISTENER_H
#define _DRALDB_DBLISTENER_H

// General includes.
#include <stdlib.h>
#include <vector>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/LogMgr.h"
#include "dDB/ClockDomainMgr.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/StatObj.h"

// Qt includes.
#include <qintdict.h>
#include <qptrlist.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qvaluevector.h>

// Asim includes.
#include <asim/dralListener.h>

// Defines.
#define MAX_TRACKID_WARNS 1000
#define MAX_TAG_COHERENCE 50

extern const bool db_listener_debug_on;

// Class forwarding.
class TagDescVector;
class DBConfig;
class ItemHeap;
class DralGraph;
class StrTable;
class TrackHeap;
class Dict2064;

/*
 * @brief
 * This class stores all the comments that appear in a dral trace.
 *
 * @description
 * As the live db disappeared, an interface to obtain the comments
 * of the trace was needed. This class is used to store the
 * different information embedded as comments in the trace.
 *
 * @author Guillem Sole
 * @date started at 2004-07-26
 * @version 0.1
 */
class DralComment : public AMemObj
{
    public:
        DralComment(char * _comment);
        DralComment(UINT32 _magic_num, char * _comment);
        DralComment(UINT16 _magic_num, char * _comment, UINT32 _length);
        virtual ~DralComment();

        UINT32 getMagicNumber() const;
        UINT32 getCommentLength() const;
        char * getComment() const;

        INT64 getObjSize() const;
        QString getUsageDescription() const;

    private:
        UINT32 magic_num; ///< Magic number of the comment. Useless for dral 1 comments.
        UINT32 length;    ///< Length of the content. Only used for dral binary comments.
        char * comment;   ///< Pointer to the data of the comment.
} ;

/*
 * @typedef DralCommentList
 * @brief
 * This struct stores a list of dral comments.
 */
typedef QPtrList<DralComment> DralCommentList;

/*
 * @typedef DralCommentIterator
 * @brief
 * This struct iterates through a DralCommentList.
 */
typedef QPtrListIterator<DralComment> DralCommentIterator;

typedef enum DBListenerState
{
    DBL_ERROR,
    DBL_VERSION,
    DBL_GRAPH,
    DBL_TRACE,
    DBL_END
} ;

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DBListener : public AMemObj, public StatObj, public DRAL_LISTENER_CLASS
{
    public:
        // ---- AMemObj Interface methods
        INT64  getObjSize() const;
        QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------

        DBListener(TagDescVector * _tgdescvec, StrTable * _strtbl, ItemHeap * _itHeap, DBConfig * _conf, DralGraph * _dbgraph, TrackHeap * _trheap, ClockDomainMgr * _clocks, Dict2064 * _dict);
        virtual ~DBListener();

    // -----------------------------------------------
    // Dral (1) Listener Interface methods
    // -----------------------------------------------
    public:

        void Cycle(UINT64 n);

        void NewItem(UINT32 item_id);
        void SetTagSingleValue(UINT32 item_id, UINT32 tag_idx, UINT64 value, UBYTE time_span);
        void SetTagString(UINT32 item_id, UINT32 tag_idx, UINT32 str_idx, UBYTE time_span);
        void SetTagSet(UINT32 item_id, UINT32 tag_idx, UINT32 nval, UINT64 * value, UBYTE time_span);

        void MoveItems(UINT16 edge_id, UINT32 n, UINT32 * item_id);
        void MoveItemsWithPositions(UINT16 edge_id, UINT32 numOfItems, UINT32 * items, UINT32 * positions);

        void EnterNode(UINT16 node_id, UINT32 item_id, UINT32 slot_index);
        void ExitNode(UINT16 node_id, UINT32 slot_index);

        inline void DeleteItem(UINT32 item_id);

        void EndSimulation();

        void AddNode(UINT16 node_id, char * node_name, UINT16 parent_id, UINT16 instance);
        void AddEdge(UINT16 sourceNode, UINT16 destNode, UINT16 edge_id, UINT32 bandwidth, UINT32 latency, char * name);
        void SetCapacity(UINT16 node_id, UINT32 capacity, UINT32 * capacities, UINT16 dimensions);

        void SetHighWaterMark(UINT16 node_id, UINT32 mark);

        void Comment(char * comment);

        void Error(char * error);

        void NonCriticalError(char * error);

    // -----------------------------------------------
    // Dral (2) Listener Interface methods
    // -----------------------------------------------

        void Version(UINT16 version);

        void NewNode(UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance);
        void NewEdge(UINT16 sourceNode, UINT16 destNode, UINT16 edge_id, UINT32 bandwidth, UINT32 latency, char * name);
        void SetNodeLayout(UINT16 node_id, UINT32 capacity, UINT16 dim, UINT32 * capacities);

        void EnterNode(UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 * position);
        void ExitNode(UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 * position);

        void SetCycleTag(UINT32 tag_idx, UINT64 value);
        void SetCycleTagString(UINT32 tag_idx, UINT32 str_idx);
        void SetCycleTagSet(UINT32 tag_idx, UINT32 nval, UINT64 * set);

        inline void SetItemTag(UINT32 item_id, UINT32 tag_idx, UINT64 value);
        inline void SetItemTagString(UINT32 item_id, UINT32 tag_idx, UINT32 str_idx);
        inline void SetItemTagSet(UINT32 item_id, UINT32 tag_idx, UINT32 nval, UINT64 * set);

        void SetNodeTag(UINT16 node_id, UINT32 tag_idx, UINT64 value,UINT16 level, UINT32 * list);
        void SetNodeTagString(UINT16 node_id, UINT32 tag_idx, UINT32 str_idx, UINT16 level, UINT32 * list);
        void SetNodeTagSet(UINT16 node_id, UINT32 tag_idx, UINT16 n, UINT64 * set, UINT16 level, UINT32 * list);

        void Comment(UINT32 magic_num, char * comment);
        void CommentBin(UINT16 magic_num, char * comment, UINT32 length);

        void StartActivity(UINT64);

        void SetNodeInputBandwidth(UINT16, UINT32);
        void SetNodeOutputBandwidth(UINT16, UINT32);

        void SetTagDescription(UINT32 tag_idx, char * desc);

        // -----------------------------------------------
        // Multi clock methods
        // -----------------------------------------------
        void SetNodeClock(UINT16 nodeId, UINT16 clockId);
        void NewClock(UINT16 clockId, UINT64 freq, UINT16 skew, UINT16 divisions, const char * name);
        void Cycle(UINT16 clockId, UINT64 cycle, UINT16 phase);

        void NewTag(UINT32 tag_idx, const char * tag_name, INT32 tag_name_len);
        void NewString(UINT32 string_idx, const char * str, INT32 str_len);

        // -----------------------------------------------
        // Other public methods
        // -----------------------------------------------
     public:
        void flush();

        inline bool getLastProcessedEventOk() const;
        inline bool getHasFatalError(bool end_sim);

        inline CYCLE getNumCycles(UINT16 clockId) const;
        inline CYCLE getFirstEffectiveCycle(UINT16 clockId) const;
        inline CYCLE getLastEffectiveCycle(UINT16 clockId) const;
        inline CYCLE getFirstCycle() const;
        inline CYCLE getLastCycle() const;

        inline const DralComment * getFirstDral1Comment() const;
        inline const DralComment * getNextDral1Comment() const;
        const DralComment * getDral2Comment(UINT32 magic_number) const;
        inline const DralComment * getFirstDral2Comment() const;
        inline const DralComment * getNextDral2Comment() const;

        inline bool reachedEOS() const;
        inline bool hasDivisions() const;
        void Do_EndSimulation();

        inline INT32 getCurrentTraceVersion() const;
        inline bool isProcessingDralHeader() const;

        // Conf Methods
        void trackItemTags(bool value);

        // exern listeners
        void attachDralListener(DRAL_LISTENER object);

        void setTrackedEdges(INT32 value);
        void addTrackedEdges(UINT16 edgeid);
        void propagateFirstCycle();

        void setIgnoreEdgeBandwidthConstrain();

    protected:
        inline void resetTracksLastUsedLinearSlot();
        void setError();

    private:
        // basic state
        CYCLE * currentCycle;
        CYCLE * firstEffectiveCycle;
        bool * isFirstCycle;
        UINT32 * lastCompressCycle;

        INT16 currentTraceVersion;
        DBListenerState state;     ///< Internal state of the dblistener.

        LogMgr *         myLogMgr;  ///< Pointer to the log manager.
        StrTable *       strtbl;    ///< Pointer to the string table.
        ItemHeap *       itHeap;    ///< Pointer to the item tag heap.
        TagDescVector *  tgdescvec; ///< Pointer to the tag descriptor vector.
        TrackHeap *      trHeap;    ///< Pointer to the track heap.
        DBConfig *       conf;      ///< Pointer to the database configuration.
        DralGraph *      dbGraph;   ///< Pointer to the graph.
        ClockDomainMgr * clocks;    ///< Pointer to the clocks.
        Dict2064       * dict;      ///< Pointer to the dictionary of values.

        bool doTrackItemTags;
        bool traceAchieved;
        bool has_divisions;

        QPtrList<DRAL_LISTENER_CLASS>* externClients;

        INT32 numTrackedEdges;

        UINT16 * lastUsedPosVector;
        UINT16 lastUsedPosVectorIndirection[65536];
        QValueList<UINT16> trackededges;

        INT32 slotitemid; ///< Tag id where the items are allocated.

        CYCLE cycle;      ///< Actual cycle.
        CYCLE firstCycle; ///< Lowest cycle of the simulation.
        CYCLE lastCycle;  ///< Biggest cycle of the simulation.

        DralCommentList * dral1Comments; ///< Dral 1 comments.
        DralCommentList * dral2Comments; ///< Dral 2 comments and binary comments.
        vector<TAG_ID> tag_rename;       ///< Rename table that maps the dral trace tag indexes to dreams tag indexes.
        vector<INT32> str_rename;        ///< Rename table that maps the dral trace string indexes to dreams tag indexes.

        bool do_cycle_callback;
        bool bandwidth_warning[65536];
        bool ignore_bandwidth_constrain;

        UINT64 _itemCnt;
        UINT64 _dumyCnt;
        UINT64 _accLiveCycles;
        UINT64 _accMoveItems;
        UINT64 _accTags;
        UINT64 _accItems;
        UINT64 _cyclesCnt;
};

void
DBListener::SetItemTag(UINT32 item_id, UINT32 tag_idx, UINT64 value)
{
    SetTagSingleValue(item_id, tag_idx,value,0);
}


void DBListener::SetItemTagString(UINT32 item_id, UINT32 tag_idx, UINT32 str_idx)
{
    SetTagString(item_id, tag_idx, str_idx, 0);
}

void
DBListener::SetItemTagSet(UINT32 item_id, UINT32 tag_idx, UINT32 nval, UINT64 * set)
{
    SetTagSet(item_id, tag_idx, nval, set, 0);
}

INT32
DBListener::getCurrentTraceVersion() const
{
    return currentTraceVersion;
}

bool
DBListener::isProcessingDralHeader() const
{
    return ((state == DBL_GRAPH) || (state == DBL_VERSION));
}

bool
DBListener::getLastProcessedEventOk() const
{
    return (state != DBL_ERROR);
}

bool
DBListener::getHasFatalError(bool end_sim)
{
    // Assures that everything is well set.
    if(end_sim)
    {
        Do_EndSimulation();
    }
    return !traceAchieved;
}

bool
DBListener::reachedEOS() const
{
    return (state == DBL_END);
}

bool
DBListener::hasDivisions() const
{
    return has_divisions;
}

void
DBListener::resetTracksLastUsedLinearSlot()
{
    if(lastUsedPosVector != NULL)
    {
        bzero(lastUsedPosVector, sizeof(UINT16) * numTrackedEdges);
    }
}

/*
 * Returns the num of cycles of the requested clock id.
 *
 * @return the num of cycles.
 */
CYCLE
DBListener::getNumCycles(UINT16 clockId) const
{
    CYCLE ret;

    ret = currentCycle[clockId] - firstEffectiveCycle[clockId];
    ret.cycle++;
    return ret;
}

/*
 * Returns the first cycle of the requested clock id.
 *
 * @return the first cycle.
 */
CYCLE
DBListener::getFirstEffectiveCycle(UINT16 clockId) const
{
    return firstEffectiveCycle[clockId];
}

/*
 * Returns the last cycle of the requested clock id.
 *
 * @return the last cycle.
 */
CYCLE
DBListener::getLastEffectiveCycle(UINT16 clockId) const
{
    return currentCycle[clockId];
}

/*
 * Returns the first absolute cycle.
 *
 * @return the first cycle.
 */
CYCLE
DBListener::getFirstCycle() const
{
    return firstCycle;
}

/*
 * Returns the last absolute cycle.
 *
 * @return the last cycle.
 */
CYCLE
DBListener::getLastCycle() const
{
    return lastCycle;
}

/*
 * Returns the first dral 1 comment found in the dral trace.
 *
 * @return the first dral 1 comment.
 */
const DralComment *
DBListener::getFirstDral1Comment() const
{
    return dral1Comments->first();
}

/*
 * Returns the next dral 1 comment found in the dral trace.
 *
 * @return the next dral 1 comment.
 */
const DralComment *
DBListener::getNextDral1Comment() const
{
    return dral1Comments->next();
}

/*
 * Returns the first dral 2 comment found in the dral trace.
 *
 * @return the first dral 2 comment.
 */
const DralComment *
DBListener::getFirstDral2Comment() const
{
    return dral2Comments->first();
}

/*
 * Returns the next dral 2 comment found in the dral trace.
 *
 * @return the next dral 2 comment.
 */
const DralComment *
DBListener::getNextDral2Comment() const
{
    return dral2Comments->next();
}

#endif

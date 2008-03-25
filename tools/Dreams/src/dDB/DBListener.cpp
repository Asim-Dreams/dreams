/*
 * Copyright (C) 2004-2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

/*
 * @file DBListener.cpp
 */

// General includes.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DBListener.h"
#include "dDB/PrimeList.h"
#include "dDB/DBItoa.h"
#include "dDB/TagDescVector.h"
#include "dDB/DBConfig.h"
#include "dDB/StrTable.h"
#include "dDB/dralgraph/DralGraph.h"
#include "dDB/itemheap/ItemHeap.h"
#include "dDB/trackheap/TrackHeap.h"
#include "dDB/Dict2064.h"

// Qt includes.
#include <qprogressdialog.h>
#include <qapplication.h>

const bool db_listener_debug_on = false;

/*#define DBLISTENER_DISPATCH_LISTENERS(cmd) \
{ \
    DRAL_LISTENER listener; \
    for ( listener = externClients->first(); listener!=NULL; listener = externClients->next() ) \
    { \
      cmd \
    } \
} \*/

#define DBLISTENER_DISPATCH_LISTENERS(cmd)

/*
 * Copies all the parameters and creates the necessary objects.
 *
 * @return new object.
 */
DBListener::DBListener(TagDescVector * _tgdescvec, StrTable * _strtbl, ItemHeap * _itHeap, DBConfig * _conf, DralGraph * _dbgraph, TrackHeap * _trheap, ClockDomainMgr * _clocks, Dict2064 * _dict)
{
    myLogMgr  = LogMgr::getInstance();
    tgdescvec = _tgdescvec;
    strtbl    = _strtbl;
    itHeap    = _itHeap;
    conf      = _conf;
    dbGraph   = _dbgraph;
    trHeap    = _trheap;
    clocks    = _clocks;
    dict      = _dict;

    slotitemid = tgdescvec->getSlotItemTagId();

    firstEffectiveCycle = NULL;
    lastCompressCycle = NULL;
    currentCycle = NULL;
    isFirstCycle = NULL;

    externClients = new QPtrList<DRAL_LISTENER_CLASS>();
    lastUsedPosVector = NULL;

    _itemCnt = 0;
    _dumyCnt = 0;
    _accLiveCycles = 0;
    _accMoveItems = 0;
    _accTags = 0;
    _accItems = 0;
    _cyclesCnt = 0;

    state = DBL_VERSION;
    currentTraceVersion = -1;
    doTrackItemTags = true;
    traceAchieved = false;
    has_divisions = false;

    dral1Comments = new DralCommentList();
    dral2Comments = new DralCommentList();

    dral1Comments->setAutoDelete(true);
    dral2Comments->setAutoDelete(true);

    do_cycle_callback = true;
    ignore_bandwidth_constrain = false;
}

/*
 * Destructor of the class. Deletes all the created objects.
 *
 * @return destroys the object.
 */
DBListener::~DBListener()
{
    delete externClients;
    delete dral1Comments;
    delete dral2Comments;

    // Deletes the cycle arrays.
    if(firstEffectiveCycle != NULL)
    {
        delete [] firstEffectiveCycle;
        delete [] currentCycle;
        delete [] isFirstCycle;
        delete [] lastCompressCycle;
    }

    if(lastUsedPosVector != NULL)
    {
        delete [] lastUsedPosVector;
    }
}


// -------------------------------------------------------------------
// DRAL LISTENER methods
// -------------------------------------------------------------------

void
DBListener::Cycle(UINT64 n)
{
    do_cycle_callback = false;
    Cycle(cycle.clock->getId(), n, 0);
    do_cycle_callback = true;

    // extern listeners
    DBLISTENER_DISPATCH_LISTENERS(listener->Cycle(n);)
}

void
DBListener::propagateFirstCycle()
{
    DBLISTENER_DISPATCH_LISTENERS(listener->Cycle(firstEffectiveCycle[cycle.clock->getId()].cycle);)
    resetTracksLastUsedLinearSlot();
}

void
DBListener::setIgnoreEdgeBandwidthConstrain()
{
    ignore_bandwidth_constrain = true;
    for(UINT32 i = 0; i < 65536; i++)
    {
        bandwidth_warning[i] = false;
    }
}

void
DBListener::StartActivity(UINT64 _firstEffectiveCycle)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: StartActivity callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: StartActivity callback can only be done once.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    UINT32 num_clocks; ///< Number of declared clocks.

    clocks->computeRatiosAndLCM();

    // At least allocs space for 1 clock.
    num_clocks = QMAX(clocks->getNumClocks(), 1);

    firstEffectiveCycle = new CYCLE[num_clocks];
    currentCycle = new CYCLE[num_clocks];
    isFirstCycle = new bool[num_clocks];
    lastCompressCycle = new UINT32[num_clocks];

    // Sets the default values.
    for(UINT32 i = 0; i < num_clocks; i++)
    {
        currentCycle[i].clock = clocks->getClockDomain(i);
        currentCycle[i].cycle = -1;
        currentCycle[i].division = 0;
        firstEffectiveCycle[i].clock = clocks->getClockDomain(i);
        firstEffectiveCycle[i].cycle = -1;
        firstEffectiveCycle[i].division = 0;
        isFirstCycle[i] = true;
        lastCompressCycle[i] = 0;
    }

    itHeap->setNumClocks(num_clocks);
    trHeap->setNumClocks(num_clocks);

    // Sets an initial clock state: if phases are used, then the clock will
    // be changed, otherwise, the first clock will be used.
    cycle.clock = clocks->getClockDomain(0);
    cycle.cycle = 0;
    cycle.division = 0;

    state = DBL_TRACE;
    traceAchieved = true;

    dict->startDict(clocks);

    Cycle(0, _firstEffectiveCycle, 0);

    // Sets if the trace has divisions.
    for(CLOCK_ID i = 0; i < clocks->getNumClocks(); i++)
    {
        has_divisions |= (clocks->getClockDomain(i)->getDivisions() > 1);
    }
}

void
DBListener::NewItem(UINT32 item_id)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: NewItem callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: NewItem callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(!itHeap->newItemBlock(item_id, cycle))
    {
        myLogMgr->addLog("Trace Error: the item id is too big. Try to reduce the item id numbering of the trace.");
        setError();
        return;
    }

    _itemCnt++;

    // extern listeners
    DBLISTENER_DISPATCH_LISTENERS(listener->NewItem(item_id);)
}

void
DBListener::SetTagSingleValue(UINT32 item_id, UINT32 tag_idx, UINT64 value, UBYTE time_span)
{
    if(!doTrackItemTags)
    {
        return;
    }

    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: SetTagSingleValue callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: SetTagSingleValue callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, setting item tag before the first cycle callback.");
        setError();
        return;
    }

    // Checks that the type of the tag is correct with previous definitions.
    if(tgdescvec->getTagValueType(tag_rename[tag_idx]) != TAG_INTEGER_TYPE)
    {
        if(tgdescvec->getTagValueType(tag_rename[tag_idx]) == TAG_UNKNOWN_TYPE)
        {
            // If the type was still not set, then is changed to the type of the first set.
            myLogMgr->addLog("Using tag \"" + tgdescvec->getTagName(tag_rename[tag_idx]) + "\" that has no defined type. Setting to integer.");
            tgdescvec->changeTagDescriptor(tag_rename[tag_idx], TAG_INTEGER_TYPE);
        }
        else
        {
            QString err("Trace Error: incoherence detected, setting an integer value into tag " + tgdescvec->getTagName(tag_rename[tag_idx]) + " which is defined as a non-integer.");
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    if(!itHeap->newTag(item_id, tag_rename[tag_idx], value, cycle))
    {
        QString err("Trace Error: incoherence detected, setting a tag after item deletion or before creation on ITEMID ");
        err = err + QString::number(item_id, 10);
        myLogMgr->addLog(err);
        setError();
        return;
    }

    _accTags++;

    // extern listeners
    DBLISTENER_DISPATCH_LISTENERS(listener->SetTagSingleValue(item_id, tag_idx, value, time_span);)
}

void
DBListener::SetTagString(UINT32 item_id, UINT32 tag_idx, UINT32 str_idx, UBYTE time_span)
{
    if(!doTrackItemTags)
    {
        return;
    }

    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: SetTagString callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: SetTagString callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, setting item tag before the first cycle callback.");
        setError();
        return;
    }

    // Checks that the type of the tag is correct with previous definitions.
    if(tgdescvec->getTagValueType(tag_rename[tag_idx]) != TAG_STRING_TYPE)
    {
        if(tgdescvec->getTagValueType(tag_rename[tag_idx]) == TAG_UNKNOWN_TYPE)
        {
            // If the type was still not set, then is changed to the type of the first set.
            myLogMgr->addLog("Using tag \"" + tgdescvec->getTagName(tag_rename[tag_idx]) + "\" that has no defined type. Setting to string.");
            tgdescvec->changeTagDescriptor(tag_rename[tag_idx], TAG_STRING_TYPE);
        }
        else
        {
            QString err("Trace Error: incoherence detected, setting a string value into tag " + tgdescvec->getTagName(tag_rename[tag_idx]) + " which is defined as a non-string.");
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    if(!itHeap->newTag(item_id, tag_rename[tag_idx], str_rename[str_idx], cycle))
    {
        QString err("Trace Error: incoherence detected, setting a tag after item deletion or before creation on ITEMID ");
        err = err + QString::number(item_id, 10);
        myLogMgr->addLog(err);
        setError();
        return;
    }

    _accTags++;

    // extern listeners
    DBLISTENER_DISPATCH_LISTENERS(listener->SetTagString(item_id, tag_idx, str_idx, time_span);)
}

void
DBListener::SetTagSet(UINT32 item_id, UINT32 tag_idx, UINT32 nval, UINT64 * value, UBYTE time_span)
{
    if(!doTrackItemTags)
    {
        return;
    }

    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: SetTagSet callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: SetTagSet callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, setting item tag before the first cycle callback.");
        setError();
        return;
    }

    bool mutant = false;
    SOVList sovl(nval);

    // Checks that the type of the tag is correct with previous definitions.
    if(tgdescvec->getTagValueType(tag_rename[tag_idx]) != TAG_SOV_TYPE)
    {
        if(tgdescvec->getTagValueType(tag_rename[tag_idx]) == TAG_UNKNOWN_TYPE)
        {
            // If the type was still not set, then is changed to the type of the first set.
            myLogMgr->addLog("Using tag \"" + tgdescvec->getTagName(tag_rename[tag_idx]) + "\" that has no defined type. Setting to set of values.");
            tgdescvec->changeTagDescriptor(tag_rename[tag_idx], TAG_SOV_TYPE);
        }
        else
        {
            QString err("Trace Error: incoherence detected, setting a set of values value into tag " + tgdescvec->getTagName(tag_rename[tag_idx]) + " which is defined as a non-SOV.");
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    for(UINT32 i = 0; i < nval; i++)
    {
        sovl.append(value[i]);
    }

    if(!itHeap->newTag(item_id, tag_rename[tag_idx], &sovl, cycle))
    {
        QString err("Trace Error: incoherence detected, setting a tag after item deletion or before creation on ITEMID ");
        err = err + QString::number(item_id, 10);
        myLogMgr->addLog(err);
        setError();
        return;
    }

    _accTags++;

    // extern listeners
    DBLISTENER_DISPATCH_LISTENERS(listener->SetTagSet(item_id, tag_idx, nval, value, time_span);)
}

void
DBListener::MoveItems(UINT16 edge_id, UINT32 n, UINT32 * item_id)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: MoveItems callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: MoveItems callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, move item before the first cycle callback.");
        setError();
        return;
    }

    // Resolve track id.
    TRACK_ID trackId = trHeap->resolveTrackIdForEdge(edge_id);

    if(trackId == TRACK_ID_INVALID)
    {
        DBLISTENER_DISPATCH_LISTENERS(listener->MoveItems(edge_id, n, item_id);)
        return;
    }

    // Check that the number of moved items is less or equal the bw of the edge
    DralGraphEdge * edge = dbGraph->getEdge(edge_id);
    INT32 bw = edge->getBandwidth();

    // Multiple moveitems can be launched to the same edge/cycle:
    UINT16 nextPos = lastUsedPosVector[lastUsedPosVectorIndirection[edge_id]];

    // Checks bandwidth coherence.
    if((nextPos + n) > (UINT32) bw)
    {
        if(ignore_bandwidth_constrain)
        {
            if(!bandwidth_warning[edge_id])
            {
                bandwidth_warning[edge_id] = true;
                QString warning("Trace Warning: edge " + dbGraph->getEdge(edge_id)->getName() + " bandwidth constrain fail ignored.");
                myLogMgr->addLog(warning);
            }
        }
        else
        {
            QString err("Trace Error: incoherence detected, moving more items than edge bandwidth. Edge id=");
            err = err + QString::number(edge_id) + ", " + cycle.toString(CYCLE_MASK_ALL);
            myLogMgr->addLog(err);
            setError();
        }
        return;
    }

    // Adds all the move items.
    for(UINT32 i = 0; i < n; i++)
    {
        if(itHeap->existItemId(item_id[i]))
        {
            if((edge_id == 544) && (cycle.cycle > 100000))
            {
                UINT32 j = 0;
                j++;
            }
            itHeap->newMoveItem(item_id[i], edge_id, nextPos, cycle);
            if(!trHeap->addMoveItem(edge_id, item_id[i], nextPos, cycle))
            {
                char str[256];
                QString err;

                sprintf(str, "Trying to move item %u through edge %s (edge id %u)", item_id[n], edge->getName().ascii(), edge_id);
                err += str;
                sprintf(str, " during a cycle with clock domain %s (clock id %u)", cycle.clock->getName().ascii(), cycle.clock->getId());
                err += str;
                const ClockDomainEntry * tmp_clock = clocks->getClockDomain(edge->getClockId());
                sprintf(str, " and this edge is attached to clock domain %s (clock id %u)\n", tmp_clock->getName().ascii(), tmp_clock->getId());
                err += str;
                sprintf(str, "Error information: %s\n", cycle.toString(CYCLE_MASK_ALL).ascii());
                err += str;
                myLogMgr->addLog(err);
                setError();
            }
            nextPos++;
        }
        else
        {
            QString err ("Trace Error: incoherence detected, moveitem on item after item deletion or before creation on ITEMID ");
            err = err + QString::number(item_id[i], 10);
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    lastUsedPosVector[lastUsedPosVectorIndirection[edge_id]] = nextPos;

    DBLISTENER_DISPATCH_LISTENERS(listener->MoveItems(edge_id, n, item_id);)
}

void
DBListener::MoveItemsWithPositions(UINT16 edge_id, UINT32 n, UINT32 * item_id, UINT32 * positions)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: MoveItems callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: MoveItems callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, move item before the first cycle callback.");
        setError();
        return;
    }

    // Check that the number of moved items is less or equal the bw of the edge
    DralGraphEdge * edge = dbGraph->getEdge(edge_id);
    INT32 bw = edge->getBandwidth();

    // Resolve track id.
    TRACK_ID trackId = trHeap->resolveTrackIdForEdge(edge_id);

    // Checks this edge is being tracked in the database.
    if(trackId == TRACK_ID_INVALID)
    {
        DBLISTENER_DISPATCH_LISTENERS(listener->MoveItemsWithPositions(edge_id, n, item_id, positions);)
        return;
    }

    for(UINT32 i = 0; i < n; i++)
    {
        if(itHeap->existItemId(item_id[i]))
        {
            if(positions[i] < (UINT32) bw)
            {
                itHeap->newMoveItem(item_id[i], edge_id, positions[i], cycle);
                if(!trHeap->addMoveItem(edge_id, item_id[i], positions[i], cycle))
                {
                    char str[256];
                    QString err;

                    sprintf(str, "Trying to move item %u through edge %s (edge id %u)", item_id[n], edge->getName().ascii(), edge_id);
                    err += str;
                    sprintf(str, " during a cycle with clock domain %s (clock id %u)", cycle.clock->getName().ascii(), cycle.clock->getId());
                    err += str;
                    const ClockDomainEntry * tmp_clock = clocks->getClockDomain(edge->getClockId());
                    sprintf(str, " and this edge is attached to clock domain %s (clock id %u)\n", tmp_clock->getName().ascii(), tmp_clock->getId());
                    err += str;
                    sprintf(str, "Error information: %s\n", cycle.toString(CYCLE_MASK_ALL).ascii());
                    err += str;
                    myLogMgr->addLog(err);
                    setError();
                }
            }
            else
            {
                QString err("Trace Error: incoherence detected, moving more items than edge bandwidth. Edge id=");
                err = err + QString::number(edge_id) + ", " + cycle.toString(CYCLE_MASK_ALL);
                myLogMgr->addLog(err);
                setError();
                return;
            }
        }
        else
        {
            QString err ("Trace Error: incoherence detected, moveitem on item after item deletion or before creation on ITEMID ");
            err = err + QString::number(item_id[i], 10);
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->MoveItemsWithPositions(edge_id, n, item_id, positions);)
}

void
DBListener::DeleteItem(UINT32 item_id)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: DeleteItem callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: DeleteItem callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, delete item before the first cycle callback.");
        setError();
        return;
    }

    itHeap->endItemBlock(item_id, cycle);
    DBLISTENER_DISPATCH_LISTENERS(listener->DeleteItem(item_id);)
}

void
DBListener::EndSimulation()
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
            case DBL_END:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: EndSimulation callback recieved before StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    Do_EndSimulation();
    DBLISTENER_DISPATCH_LISTENERS(listener->EndSimulation();)
}

void
DBListener::Do_EndSimulation()
{
    if(state == DBL_END)
    {
        return;
    }

    // Initial values for the bound cycles.
    firstCycle = firstEffectiveCycle[0];
    lastCycle = currentCycle[0];

    // Iterates for all the clocks.
    for(UINT32 i = 0; i < clocks->getNumClocks(); i++)
    {
        // Updates the first and last cycle.
        if(firstCycle > firstEffectiveCycle[i])
        {
            firstCycle = firstEffectiveCycle[i];
        }
        if(lastCycle < currentCycle[i])
        {
            lastCycle = currentCycle[i];
        }

        // Last compression step.
        cycle.cycle += CYCLE_CHUNK_SIZE + 1;
        trHeap = (TrackHeap *) (trHeap->compressYourSelf(cycle, true));
    }
    state = DBL_END;
}

void
DBListener::AddNode(UINT16 node_id, char * node_name, UINT16 parent_id, UINT16 instance)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: AddNode callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: AddNode callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    QString qnodename(node_name);

    if(!dbGraph->addNode(qnodename.stripWhiteSpace(), node_id, parent_id, instance))
    {
        setError();
        return;
    }
    DBLISTENER_DISPATCH_LISTENERS(listener->AddNode(node_id, node_name, parent_id, instance);)
}

void
DBListener::AddEdge(UINT16 sourceNode, UINT16 destNode, UINT16 edge_id, UINT32 bandwidth, UINT32 latency, char * name)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: AddEdge callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: AddEdge callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }
    QString qname(name);

    if(!dbGraph->addEdge(sourceNode, destNode, edge_id, bandwidth, latency, qname.stripWhiteSpace()))
    {
        setError();
        return;
    }
    DBLISTENER_DISPATCH_LISTENERS(listener->AddEdge(sourceNode, destNode, edge_id, bandwidth, latency, name);)
}

void
DBListener::SetCapacity(UINT16 node_id, UINT32 capacity, UINT32 * capacities, UINT16 dimensions)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: SetCapacity callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: SetCapacity callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }
    NodeSlot slot(dimensions, capacities);

    if(!dbGraph->setNodeLayout(node_id, slot))
    {
        setError();
        return;
    }
    DBLISTENER_DISPATCH_LISTENERS(listener->SetCapacity(node_id, capacity, capacities, dimensions);)
}

void
DBListener::SetHighWaterMark(UINT16 node_id, UINT32 mark)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: SetHighWaterMark callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: SetHighWaterMark callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->SetHighWaterMark(node_id, mark);)
}

void
DBListener::Error(char * error)
{
    bool wasOK;

    wasOK = (state == DBL_TRACE);
    if(wasOK)
    {
        Do_EndSimulation();
    }

    myLogMgr->addLog(QString(error));
    setError();
    DBLISTENER_DISPATCH_LISTENERS(listener->Error(error);)
}

void
DBListener::NonCriticalError(char * error)
{
    if(state == DBL_ERROR)
    {
        return;
    }

    myLogMgr->addLog(QString(error));
    DBLISTENER_DISPATCH_LISTENERS(listener->NonCriticalError(error);)
}

// ------------------------------------------------------------------
// -- Dral 2.0
// ------------------------------------------------------------------

void
DBListener::Version(UINT16 version)
{
    if(state == DBL_ERROR)
    {
        return;
    }
    if(state != DBL_VERSION)
    {
        myLogMgr->addLog("Trace Error: the version must be declared just once.");
        setError();
        return;
    }

    state = DBL_GRAPH;

    currentTraceVersion = version;
    if(currentTraceVersion < 2)
    {
        myLogMgr->addLog("Trace Warning: this an old DRAL file and will be processed with backward proopagation policy.");
    }
    itHeap->setDralVersion(version);
    DBLISTENER_DISPATCH_LISTENERS(listener->Version(version);)
}

void
DBListener::NewNode(UINT16 node_id, char * node_name, UINT16 parent_id, UINT16 instance)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: NewNode callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: NewNode callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    AddNode(node_id, node_name, parent_id, instance);
}

void
DBListener::NewEdge(UINT16 sourceNode, UINT16 destNode, UINT16 edge_id, UINT32 bandwidth, UINT32 latency, char * name)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: NewEdge callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: NewEdge callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    AddEdge(sourceNode, destNode, edge_id, bandwidth, latency, name);
}

void
DBListener::SetNodeLayout(UINT16 node_id, UINT32 capacity, UINT16 dim, UINT32 * capacities)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: SetCapacity callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: SetCapacity callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    SetCapacity(node_id, capacity, capacities, dim);
}

void
DBListener::EnterNode(UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 * position)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: EnterNode callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: EnterNode callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, enter node before the first cycle callback.");
        setError();
        return;
    }

    TRACK_ID trackId = trHeap->resolveTrackIdForNode(node_id, dim, position);
    if(trackId >= TRACK_ID_FIRST_INVALID)
    {
        if(trackId == TRACK_ID_INVALID)
        {
            DBLISTENER_DISPATCH_LISTENERS(listener->EnterNode(node_id, item_id, dim, position);)
        }
        else if((trackId == TRACK_ID_LINEAR_NUM_DIM_ERROR) || (trackId == TRACK_ID_LINEAR_OOB_DIM_ERROR))
        {
            DralGraphNode * node = dbGraph->getNode(node_id);
            QString err = "";
            NodeSlot temp(dim, position);
            QString slot_str = temp.toString();

            err = "Trace Error: enternode command invalid.\n";
            err += "Enternode to slot " + slot_str + " of node " + node->getName() + " but the node layout is defined as ";
            err += node->getLayout()->toString();
            myLogMgr->addLog(err);
            setError();
        }
        return;
    }

    if(itHeap->existItemId(item_id))
    {
        DralGraphNode * node = dbGraph->getNode(node_id);
        itHeap->newEnterNode(item_id, node_id, NodeSlot::linearPosition(dim, position, node->getLayout()), cycle);
        if(!trHeap->addTagValue(trackId, slotitemid, cycle, item_id))
        {
            char str[256];
            QString err;

            sprintf(str, "Trying to do an EnterNode to node %s (node id %u)", node->getName().ascii(), node->getNodeId());
            err += str;
            sprintf(str, " during a cycle with clock domain %s (clock id %u)", cycle.clock->getName().ascii(), cycle.clock->getId());
            err += str;
            const ClockDomainEntry * tmp_clock = clocks->getClockDomain(node->getClockId());
            sprintf(str, " and this node is attached to clock domain %s (clock id %u)\n", tmp_clock->getName().ascii(), tmp_clock->getId());
            err += str;
            NodeSlot slot(dim, position);
            sprintf(str, "Error information: %s, item %u, slot: %s\n", cycle.toString(CYCLE_MASK_ALL).ascii(), item_id, slot.toString().ascii());
            err += str;
            myLogMgr->addLog(err);
            setError();
        }
    }
    else
    {
        QString err ("Trace Error: incoherence detected, enternode on item after item deletion or before creation on ITEMID ");
        err = err + QString::number(item_id, 10);
        myLogMgr->addLog(err);
        setError();
        return;
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->EnterNode(node_id, item_id, dim, position);)
}

void
DBListener::EnterNode(UINT16 node_id, UINT32 item_id, UINT32 slot_index)
{
    /// @todo check for bk compatibility on dral 1 traces...
    EnterNode(node_id, item_id, 1, &slot_index);
}

void
DBListener::ExitNode(UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 * position)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: ExitNode callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: ExitNode callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, exit node before the first cycle callback.");
        setError();
        return;
    }

    // resolve track id...
    TRACK_ID trackId = trHeap->resolveTrackIdForNode(node_id, dim, position);
    if(trackId >= TRACK_ID_FIRST_INVALID)
    {
        if(trackId == TRACK_ID_INVALID)
        {
            DBLISTENER_DISPATCH_LISTENERS(listener->ExitNode(node_id, item_id, dim, position);)
        }
        else if((trackId == TRACK_ID_LINEAR_NUM_DIM_ERROR) || (trackId == TRACK_ID_LINEAR_OOB_DIM_ERROR))
        {
            DralGraphNode * node = dbGraph->getNode(node_id);
            QString err = "";
            NodeSlot temp(dim, position);
            QString slot_str = temp.toString();

            err = "Trace Error: enternode command invalid.\n";
            err += "Exitnode to slot " + slot_str + " of node " + node->getName() + " but the node layout is defined as ";
            err += node->getLayout()->toString();
            myLogMgr->addLog(err);
            setError();
        }
        return;
    }

    if(itHeap->existItemId(item_id))
    {
        DralGraphNode * node = dbGraph->getNode(node_id);

        itHeap->newExitNode(item_id, node_id, NodeSlot::linearPosition(dim, position, node->getLayout()), cycle);
        if(!trHeap->undefineTagValue(trackId, slotitemid, cycle))
        {
            char str[256];
            QString err;

            sprintf(str, "Trying to do an ExitNode to node %s (node id %u)", node->getName().ascii(), node->getNodeId());
            err += str;
            sprintf(str, " during a cycle with clock domain %s (clock id %u)", cycle.clock->getName().ascii(), cycle.clock->getId());
            err += str;
            const ClockDomainEntry * tmp_clock = clocks->getClockDomain(node->getClockId());
            sprintf(str, " and this node is attached to clock domain %s (clock id %u)\n", tmp_clock->getName().ascii(), tmp_clock->getId());
            err += str;
            NodeSlot slot(dim, position);
            sprintf(str, "Error information: %s, item %u, slot: %s\n", cycle.toString(CYCLE_MASK_ALL).ascii(), item_id, slot.toString().ascii());
            err += str;
            myLogMgr->addLog(err);
            setError();
        }
    }
    else
    {
        QString err ("Trace Error: incoherence detected, exitnode on item after item deletion or before creation on ITEMID ");
        err = err + QString::number(item_id, 10);
        myLogMgr->addLog(err);
        setError();
        return;
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->ExitNode(node_id, item_id, dim, position);)
}

void
DBListener::ExitNode(UINT16 node_id, UINT32 slot_index)
{
    /// @todo check for bk compatibility on dral 1 traces...
    // TODO: problems with item_id = 0...
    ExitNode(node_id, 0, 1, &slot_index);
}

void
DBListener::SetCycleTag(UINT32 tag_idx, UINT64 value)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: SetCycleTag callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: SetCycleTag callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, cycle tag defined before the first cycle callback.");
        setError();
        return;
    }

    TRACK_ID trackId = trHeap->resolveTrackIdForCycleTag(cycle.clock->getId());
    if(trackId == TRACK_ID_INVALID)
    {
        DBLISTENER_DISPATCH_LISTENERS(listener->SetCycleTag(tag_idx, value);)
        return;
    }

    // Checks that the type of the tag is correct with previous definitions.
    if(tgdescvec->getTagValueType(tag_rename[tag_idx]) != TAG_INTEGER_TYPE)
    {
        if(tgdescvec->getTagValueType(tag_rename[tag_idx]) == TAG_UNKNOWN_TYPE)
        {
            // If the type was still not set, then is changed to the type of the first set.
            myLogMgr->addLog("Using tag \"" + tgdescvec->getTagName(tag_rename[tag_idx]) + "\" that has no defined type. Setting to integer.");
            tgdescvec->changeTagDescriptor(tag_rename[tag_idx], TAG_INTEGER_TYPE);
        }
        else
        {
            QString err("Trace Error: incoherence detected, setting a non-integer value into tag " + tgdescvec->getTagName(tag_rename[tag_idx]) + " which is defined as an integer.");
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    if(!trHeap->addTagValue(trackId, tag_rename[tag_idx], cycle, value))
    {
        setError();
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->SetCycleTag(tag_idx, value);)
}

void
DBListener::SetCycleTagString(UINT32 tag_idx, UINT32 str_idx)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: SetCycleTagString callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: SetCycleTagString callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, cycle tag defined before the first cycle callback.");
        setError();
        return;
    }

    TRACK_ID trackId = trHeap->resolveTrackIdForCycleTag(cycle.clock->getId());
    if(trackId == TRACK_ID_INVALID)
    {
        DBLISTENER_DISPATCH_LISTENERS(listener->SetCycleTagString(tag_idx, str_idx);)
        return;
    }

    // Checks that the type of the tag is correct with previous definitions.
    if(tgdescvec->getTagValueType(tag_rename[tag_idx]) != TAG_STRING_TYPE)
    {
        if(tgdescvec->getTagValueType(tag_rename[tag_idx]) == TAG_UNKNOWN_TYPE)
        {
            // If the type was still not set, then is changed to the type of the first set.
            myLogMgr->addLog("Using tag \"" + tgdescvec->getTagName(tag_rename[tag_idx]) + "\" that has no defined type. Setting to string.");
            tgdescvec->changeTagDescriptor(tag_rename[tag_idx], TAG_STRING_TYPE);
        }
        else
        {
            QString err("Trace Error: incoherence detected, setting a string value into tag " + tgdescvec->getTagName(tag_rename[tag_idx]) + " that is defined with a different type.");
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    if(!trHeap->addTagValue(trackId, tag_rename[tag_idx], cycle, str_rename[str_idx]))
    {
        setError();
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->SetCycleTagString(tag_idx, str_idx);)
}

void
DBListener::SetCycleTagSet(UINT32 tag_idx, UINT32 nval, UINT64 * set)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: SetCycleTagSet callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: SetCycleTagSet callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, cycle tag defined before the first cycle callback.");
        setError();
        return;
    }

    // Checks that the type of the tag is correct with previous definitions.
    if(tgdescvec->getTagValueType(tag_rename[tag_idx]) != TAG_SOV_TYPE)
    {
        if(tgdescvec->getTagValueType(tag_rename[tag_idx]) == TAG_UNKNOWN_TYPE)
        {
            // If the type was still not set, then is changed to the type of the first set.
            myLogMgr->addLog("Using tag \"" + tgdescvec->getTagName(tag_rename[tag_idx]) + "\" that has no defined type. Setting to set of values.");
            tgdescvec->changeTagDescriptor(tag_rename[tag_idx], TAG_SOV_TYPE);
        }
        else
        {
            QString err("Trace Error: incoherence detected, setting a non-string value into tag " + tgdescvec->getTagName(tag_rename[tag_idx]) + " which is defined as a string.");
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->SetCycleTagSet(tag_idx, nval, set);)
}

void
DBListener::SetNodeTag(UINT16 node_id, UINT32 tag_idx, UINT64 value, UINT16 level, UINT32 * list)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: SetNodeTag callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: SetNodeTag callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, node tag defined before the first cycle callback.");
        setError();
        return;
    }

    // resolve track id...
    TRACK_ID trackId = trHeap->resolveTrackIdForNode(node_id, level, list);

    if(trackId >= TRACK_ID_FIRST_INVALID)
    {
        if(trackId == TRACK_ID_INVALID)
        {
            DBLISTENER_DISPATCH_LISTENERS(listener->SetNodeTag(node_id, tag_idx, value, level, list);)
        }
        else if((trackId == TRACK_ID_LINEAR_NUM_DIM_ERROR) || (trackId == TRACK_ID_LINEAR_OOB_DIM_ERROR))
        {
            DralGraphNode * node = dbGraph->getNode(node_id);
            QString err = "";
            NodeSlot temp(level, list);
            QString slot_str = temp.toString();

            err = "Trace Error: enternode command invalid.\n";
            err += "SetNodeTag to slot " + slot_str + " of node " + node->getName() + " but the node layout is defined as ";
            err += node->getLayout()->toString();
            myLogMgr->addLog(err);
            setError();
        }
        return;
    }

    // Checks that the type of the tag is correct with previous definitions.
    if(tgdescvec->getTagValueType(tag_rename[tag_idx]) != TAG_INTEGER_TYPE)
    {
        if(tgdescvec->getTagValueType(tag_rename[tag_idx]) == TAG_UNKNOWN_TYPE)
        {
            // If the type was still not set, then is changed to the type of the first set.
            myLogMgr->addLog("Using tag \"" + tgdescvec->getTagName(tag_rename[tag_idx]) + "\" that has no defined type. Setting to integer.");
            tgdescvec->changeTagDescriptor(tag_rename[tag_idx], TAG_INTEGER_TYPE);
        }
        else
        {
            QString err("Trace Error: incoherence detected, setting a non-integer value into tag " + tgdescvec->getTagName(tag_rename[tag_idx]) + " which is defined as an integer.");
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    if(!trHeap->addTagValue(trackId, tag_rename[tag_idx], cycle, value))
    {
        char str[256];
        QString err;
        DralGraphNode * node = dbGraph->getNode(node_id);

        sprintf(str, "Trying to set a tag to node %s (node id %u)", node->getName().ascii(), node_id);
        err += str;
        sprintf(str, " during a cycle with clock domain %s (clock id %u)", cycle.clock->getName().ascii(), cycle.clock->getId());
        err += str;
        const ClockDomainEntry * tmp_clock = clocks->getClockDomain(node_id);
        sprintf(str, " and this node is attached to clock domain %s (clock id %u)\n", tmp_clock->getName().ascii(), tmp_clock->getId());
        err += str;
        NodeSlot slot(level, list);
        sprintf(str, "Error information: %s, value %llu, slot %s\n", cycle.toString(CYCLE_MASK_ALL).ascii(), value, slot.toString().ascii());
        err += str;
        myLogMgr->addLog(err);
        setError();
        return;
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->SetNodeTag(node_id, tag_idx, value, level, list);)
}

void
DBListener::SetNodeTagString(UINT16 node_id, UINT32 tag_idx, UINT32 str_idx, UINT16 level, UINT32 * list)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: SetNodeTagString callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: SetNodeTagString callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, node tag defined before the first cycle callback.");
        setError();
        return;
    }

    // resolve track id...
    TRACK_ID trackId = trHeap->resolveTrackIdForNode(node_id, level, list);
    if(trackId >= TRACK_ID_FIRST_INVALID)
    {
        if(trackId == TRACK_ID_INVALID)
        {
            DBLISTENER_DISPATCH_LISTENERS(listener->SetNodeTagString(node_id, tag_idx, str_idx, level, list);)
        }
        else if((trackId == TRACK_ID_LINEAR_NUM_DIM_ERROR) || (trackId == TRACK_ID_LINEAR_OOB_DIM_ERROR))
        {
            DralGraphNode * node = dbGraph->getNode(node_id);
            QString err = "";
            NodeSlot temp(level, list);
            QString slot_str = temp.toString();

            err = "Trace Error: enternode command invalid.\n";
            err += "SetNodeTagString to slot " + slot_str + " of node " + node->getName() + " but the node layout is defined as ";
            err += node->getLayout()->toString();
            myLogMgr->addLog(err);
            setError();
        }
        return;
    }

    // Checks that the type of the tag is correct with previous definitions.
    if(tgdescvec->getTagValueType(tag_rename[tag_idx]) != TAG_STRING_TYPE)
    {
        if(tgdescvec->getTagValueType(tag_rename[tag_idx]) == TAG_UNKNOWN_TYPE)
        {
            // If the type was still not set, then is changed to the type of the first set.
            myLogMgr->addLog("Using tag \"" + tgdescvec->getTagName(tag_rename[tag_idx]) + "\" that has no defined type. Setting to string.");
            tgdescvec->changeTagDescriptor(tag_rename[tag_idx], TAG_STRING_TYPE);
        }
        else
        {
            QString err("Trace Error: incoherence detected, setting a non-string value into tag " + tgdescvec->getTagName(tag_rename[tag_idx]) + " which is defined as a string.");
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    if(!trHeap->addTagValue(trackId, tag_rename[tag_idx], cycle, str_rename[str_idx]))
    {
        char str[256];
        QString err;
        DralGraphNode * node = dbGraph->getNode(node_id);

        sprintf(str, "Trying to set a tag to node %s (node id %u)", node->getName().ascii(), node->getNodeId());
        err += str;
        sprintf(str, " during a cycle with clock domain %s (clock id %u)", cycle.clock->getName().ascii(), cycle.clock->getId());
        err += str;
        const ClockDomainEntry * tmp_clock = clocks->getClockDomain(node->getClockId());
        sprintf(str, " and this node is attached to clock domain %s (clock id %u)\n", tmp_clock->getName().ascii(), tmp_clock->getId());
        err += str;
        NodeSlot slot(level, list);
        sprintf(str, "Error information: %s, value %s, slot %s\n", cycle.toString(CYCLE_MASK_ALL).ascii(), strtbl->getString(str_rename[str_idx]).ascii(), slot.toString().ascii());
        err += str;
        myLogMgr->addLog(err);
        setError();
        return;
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->SetNodeTagString(node_id, tag_idx, str_idx, level, list);)
}

void
DBListener::SetNodeTagSet(UINT16 node_id, UINT32 tag_idx, UINT16 n, UINT64 * set, UINT16 level, UINT32 * list)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: SetNodeTagSet callback recieved before StartActivity command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: SetNodeTagSet callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(isFirstCycle[cycle.clock->getId()])
    {
        myLogMgr->addLog("Trace Error: incoherence detected, node tag defined before the first cycle callback.");
        setError();
        return;
    }

    // Checks that the type of the tag is correct with previous definitions.
    if(tgdescvec->getTagValueType(tag_rename[tag_idx]) != TAG_SOV_TYPE)
    {
        if(tgdescvec->getTagValueType(tag_rename[tag_idx]) == TAG_UNKNOWN_TYPE)
        {
            // If the type was still not set, then is changed to the type of the first set.
            myLogMgr->addLog("Using tag \"" + tgdescvec->getTagName(tag_rename[tag_idx]) + "\" that has no defined type. Setting to set of values.");
            tgdescvec->changeTagDescriptor(tag_rename[tag_idx], TAG_SOV_TYPE);
        }
        else
        {
            QString err("Trace Error: incoherence detected, setting a non-set-of-values value into tag " + tgdescvec->getTagName(tag_rename[tag_idx]) + " which is defined as a set of values.");
            myLogMgr->addLog(err);
            setError();
            return;
        }
    }

    DBLISTENER_DISPATCH_LISTENERS(listener->SetNodeTagSet(node_id, tag_idx, n, set, level, list);)
}

void
DBListener::Comment(char * comment)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: Comment callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: Comment callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    DralComment * com; ///< Used to create the dral comment.

    // Creates a new comment.
    com = new DralComment(comment);

    // Adds the Dral 1 comment.
    dral1Comments->append(com);

    // The listeners are called.
    DBLISTENER_DISPATCH_LISTENERS(listener->Comment(comment);)
}

void
DBListener::Comment(UINT32 magic_num, char * comment)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: Comment callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: Comment callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    DralComment * com; ///< Used to create the dral comment.

    // Creates a new comment.
    com = new DralComment(magic_num, comment);

    // Adds the Dral 2 comment.
    dral2Comments->append(com);

    // The listeners are called.
    DBLISTENER_DISPATCH_LISTENERS(listener->Comment(magic_num, comment);)
}

void
DBListener::CommentBin(UINT16 magic_num, char * comment, UINT32 length)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: CommentBin callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: Commentbin callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    DralComment * com; ///< Used to create the dral comment.

    // Creates a new comment.
    com = new DralComment(magic_num, comment, length);

    // Adds the Dral 2 comment.
    dral2Comments->append(com);

    // The listeners are called.
    DBLISTENER_DISPATCH_LISTENERS(listener->CommentBin(magic_num, comment, length);)
}

void 
DBListener::SetNodeInputBandwidth(UINT16, UINT32)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: SetNodeInputBandwidth callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: SetNodeInputBandwidth callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }
}

void 
DBListener::SetNodeOutputBandwidth(UINT16, UINT32)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: SetNodeOutputBandwidth callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: SetNodeOutputBandwidth callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }
}

void 
DBListener::SetTagDescription(UINT32 tag_idx, char * desc)
{
    if(tag_rename[tag_idx] == TAG_ID_INVALID)
    {
        return;
    }
    tgdescvec->setTagDesc(tag_rename[tag_idx], QString(desc));
}

/*
 * Sets the node clock domain to the node id.
 *
 * @return void.
 */
void
DBListener::SetNodeClock(UINT16 nodeId, UINT16 clockId)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: SetNodeClock callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: SetNodeClock callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    // Checks that the clock exists.
    if(!clocks->hasClockDomain(clockId))
    {
        myLogMgr->addLog("Trace Error: incoherence detected, trying to set the clock id " + QString::number(clockId) + " and this clock id doesn't exist.");
        setError();
        return;
    }

    DralGraphNode * node = dbGraph->getNode(nodeId);

    // Checks that the node exists.
    if(node == NULL)
    {
        myLogMgr->addLog("Trace Error: incoherence detected, trying to set a clock id to the node with id " + QString::number(nodeId) + " and this node id doesn't exist.");
        setError();
        return;
    }

    // Sets the clock id.
    node->setClockId(clockId);
}

/*
 * Adds a new clock domain.
 *
 * @return void.
 */
void
DBListener::NewClock(UINT16 clockId, UINT64 freq, UINT16 skew, UINT16 divisions, const char * name)
{
    if(state != DBL_GRAPH)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: NewClock callback recieved before Version command.");
                break;

            case DBL_TRACE:
            case DBL_END:
                myLogMgr->addLog("Trace Error: NewClock callback recieved after StartActivity command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    QString error; ///< Error message.

    // Tries to insert the new clock domain.
    if(!clocks->addNewClockDomain(clockId, freq, divisions, skew, name, error))
    {
        myLogMgr->addLog("Trace Error: " + error + ".");
        setError();
    }
}

/*
 * A new cycle with domain.
 *
 * @return void.
 */
void
DBListener::Cycle(UINT16 clockId, UINT64 cycle, UINT16 phase)
{
    if(state != DBL_TRACE)
    {
        switch(state)
        {
            case DBL_ERROR:
                return;

            case DBL_GRAPH:
                myLogMgr->addLog("Trace Error: Cycle callback recieved before StartActivity command.");
                break;

            case DBL_VERSION:
                myLogMgr->addLog("Trace Error: Cycle callback recieved before Version command.");
                break;

            case DBL_END:
                myLogMgr->addLog("Trace Error: Cycle callback recieved after end command.");
                break;

            default:
                Q_ASSERT(false);
                break;
        }
        setError();
        return;
    }

    if(clockId >= clocks->getNumClocks())
    {
        myLogMgr->addLog("Trace Error: invalid clockid " + QString::number(clockId));
        setError();
        return;
    }

    // sanity check
    if((INT32) cycle < currentCycle[clockId].cycle)
    {
        myLogMgr->addLog("Trace Error: invalid cycle sequence, this is likely due to a corrupted drl file.");
        myLogMgr->addLog("Hitting CTRL+C during the trace generation is likely to produce CRC gzip errors!");
        setError();
        return;
    }

    if((currentCycle[clockId].cycle != -1) && ((cycle - currentCycle[clockId].cycle) > 1))
    {
        myLogMgr->addLog("Trace Warning: a big cycle gap detected. This is likely due to a corrupted drl file.");
        myLogMgr->addLog("Hitting CTRL+C during the trace generation is likely to produce CRC gzip errors!");
    }

    resetTracksLastUsedLinearSlot();

    // Updates the cycle state.
    const ClockDomainEntry * clock;
    clock = clocks->getClockDomain(clockId);

    this->cycle.clock = clock;
    this->cycle.cycle = cycle;
    this->cycle.division = phase;
    currentCycle[clockId] = this->cycle;

    // Phase check.
    if(phase >= clock->getDivisions())
    {
        myLogMgr->addLog("Trace Error: invalid cycle sequence because the specified phase " + QString::number(phase) + " doesn't exist for the clock " + QString::number(clockId) + ".");
        setError();
        return;
    }

    if(isFirstCycle[clockId])
    {
        isFirstCycle[clockId] = false;
        firstEffectiveCycle[clockId] = this->cycle;
        lastCompressCycle[clockId] = cycle;

        itHeap->setFirstEffectiveCycle(this->cycle);
        trHeap->setFirstEffectiveCycle(this->cycle);
    }
    // for for auto-compression mechanisms
    else if((cycle - lastCompressCycle[clockId]) >= CYCLE_CHUNK_SIZE)
    {
        this->cycle.cycle--;
        trHeap = (TrackHeap *) (trHeap->compressYourSelf(this->cycle));
        lastCompressCycle[clockId] = cycle;
        this->cycle.cycle++;
    }

    // Checks limit of cycles used
    if((cycle - firstEffectiveCycle[clockId].cycle) > 1048575)
    {
        myLogMgr->addLog("Trace Error: used more than 1 million cycles.");
        myLogMgr->addLog("\tYou used a range starting at cycle " + QString::number(firstEffectiveCycle[clockId].cycle) + " to " + QString::number(cycle));
        setError();
        return;
    }

    _cyclesCnt++;

    if(do_cycle_callback)
    {
        // extern listeners
        DBLISTENER_DISPATCH_LISTENERS(listener->Cycle(clockId, cycle, phase);)
    }

    // Now we check if the dict needs to alloc more data. Important to do
    // the change of dict when the phase is 0 (if we know that divisions
    // are zero, search is faster).
    if(dict->getNeedsNewDict(clockId) && (phase == 0))
    {
        dict->allocNewDict(cycle - firstEffectiveCycle[clockId].cycle, clockId);
    }
}

void
DBListener::NewTag(UINT32 tag_idx, const char * tag_name, INT32 tag_name_len)
{
    if(state == DBL_ERROR)
    {
        return;
    }

    TAG_ID id;
    QString tag(tag_name);

    id = tgdescvec->getTagId(tag);

    // Checks if the tag is defined.
    if(id == TAG_ID_INVALID)
    {
        myLogMgr->addLog("Trace Warning: the tag " + tag + " appears in the trace but is not used in the ADF.");
        bool ok = tgdescvec->newTagDescriptor(tag, TAG_UNKNOWN_TYPE, TAG_BASE_DECIMAL, TAG_SIZE_64_BITS, true, &id);
        Q_ASSERT(ok);
    }

    // Updates the tag renaming.
    assert(tag_idx == tag_rename.size());
    tag_rename.push_back(id);
}

void
DBListener::NewString(UINT32 string_idx, const char * str, INT32 str_len)
{
    if(state == DBL_ERROR)
    {
        return;
    }

    INT32 id;

    id = strtbl->addString(str);

    // Updates the string renaming.
    assert(string_idx == str_rename.size());
    str_rename.push_back(id);
}

void
DBListener::trackItemTags(bool value)
{
    doTrackItemTags = value;
}

void
DBListener::flush()
{
    if(state != DBL_TRACE)
    {
        return;
    }
    Do_EndSimulation();
}

void
DBListener::attachDralListener(DRAL_LISTENER object)
{
    externClients->append(object);
}

void
DBListener::setTrackedEdges(INT32 value)
{
    //printf ("DBListener::setTrackedEdges with value=%d, tec=%d\n",value,trackededges.count());

    numTrackedEdges = QMAX(value,1);

    lastUsedPosVector = new UINT16[numTrackedEdges];

    // check coherence
    Q_ASSERT ((INT32)(trackededges.count()) == value);
    bzero((char*) lastUsedPosVectorIndirection, sizeof(lastUsedPosVectorIndirection));
    INT32 current = 0;
    for(INT32 i = 0; i < value; i++)
    {
        lastUsedPosVectorIndirection[trackededges[i]] = i;
    }
}

void
DBListener::addTrackedEdges(UINT16 edgeid)
{
    // just put it on a tmp list
    trackededges.append(edgeid);
}

/*
 * Returns the dral 2 comment that has the magic number equal to
 * magic_number.
 *
 * @return the dral 2 comment.
 */
const DralComment *
DBListener::getDral2Comment(UINT32 magic_number) const
{
    const DralComment * com; ///< Variable used as iterator.

    // Searches through all the list.
    for(com = dral2Comments->first(); com != NULL; com = dral2Comments->next())
    {
        // If the actual comment has the same magic number is returned.
        if(com->getMagicNumber() == magic_number)
        {
            return com;
        }
    }

    // Returns NULL if not found.
    return com;
}

void
DBListener::setError()
{
    state = DBL_ERROR;
}

// -------------------------------------------------------------------
// AMemObj methods
// -------------------------------------------------------------------

INT64
DBListener::getObjSize() const
{
    INT64 result;

    result = (trackededges.count() * sizeof(UINT16));

    DralCommentIterator it_com1(* dral1Comments);
    DralComment * com;

    // Adds the DRAL 1 comments.
    while((com = it_com1.current()) != NULL)
    {
        ++it_com1;
        result += com->getObjSize();
    }

    DralCommentIterator it_com2(* dral2Comments);

    // Adds the DRAL 2 comments.
    while((com = it_com2.current()) != NULL)
    {
        ++it_com2;
        result += com->getObjSize();
    }

    return result;
}

QString
DBListener::getUsageDescription() const
{
    QString ret;

    ret = QString("\tDBListener object occupies ") + QString::number(sizeof(DBListener)) + QString(". Other DBListener components:\n");
    ret += QString("\t\tTracked Edges: ") + QString::number(trackededges.count() * sizeof(UINT16)) + QString(" bytes.\n");

    INT32 comments;
    DralCommentIterator it_com1(* dral1Comments);
    DralComment * com;

    comments = 0;

    // Adds the DRAL 1 comments.
    while((com = it_com1.current()) != NULL)
    {
        ++it_com1;
        comments += com->getObjSize();
    }

    DralCommentIterator it_com2(* dral2Comments);

    // Adds the DRAL 2 comments.
    while((com = it_com2.current()) != NULL)
    {
        ++it_com2;
        comments += com->getObjSize();
    }

    ret += QString("\t\tDral Comments: ") + QString::number(comments) + QString(" bytes.\n");
    return ret;
}

QString
DBListener::getStats() const
{
    QString result = "";

    double dummyProp = ((double)DBListener::_dumyCnt/(double)DBListener::_itemCnt)*100.0;
    double movsProps = ((double)DBListener::_accMoveItems/(double)DBListener::_itemCnt);
    double tagsProps = ((double)DBListener::_accTags/(double)DBListener::_itemCnt);
    double lifeMean  = ((double)DBListener::_accLiveCycles/(double)DBListener::_itemCnt);
    double liveMean  = ((double)DBListener::_accItems/(double)DBListener::_cyclesCnt);

    result +="\tProcessed items:\t\t\t"+QString::number((long)(DBListener::_itemCnt))+"\n";
    result +="\tUseless items:\t\t\t"+QString::number(dummyProp)+"%\n";
    result +="\tMoveitems per item:\t\t"+QString::number(movsProps)+"\n";
    result +="\tTags per item:\t\t\t"+QString::number(tagsProps)+"\n";
    result +="\tNumber of cycles items are live:\t"+QString::number(lifeMean)+"\n";
    result +="\tNumber of inflight (live) items:\t"+QString::number(liveMean)+"\n";
    return (result);
}

/*
 * Creator of this class for dral 1 comments.
 *
 * @return new object.
 */
DralComment::DralComment(char * _comment)
{
    magic_num = 0;
    length = strlen(_comment);
    Q_ASSERT(length > 0);
    comment = strdup(_comment);
}

/*
 * Creator of this class for dral 2 comments.
 *
 * @return new object.
 */
DralComment::DralComment(UINT32 _magic_num, char * _comment)
{
    magic_num = _magic_num;
    length = strlen(_comment);
    Q_ASSERT(length > 0);
    comment = strdup(_comment);
}

/*
 * Creator of this class for dral binary comments.
 *
 * @return new object.
 */
DralComment::DralComment(UINT16 _magic_num, char * _comment, UINT32 _length)
{
    magic_num = _magic_num;
    length = _length;
    Q_ASSERT(length > 0);
    comment = strdup(_comment);
}

/*
 * Destructor of this class. Just frees the comment.
 *
 * @return destroys the object.
 */
DralComment::~DralComment()
{
    free(comment);
}

/*
 * Gets the magic number of the entry.
 *
 * @return the magic number.
 */
UINT32
DralComment::getMagicNumber() const
{
    return magic_num;
}

/*
 * Gets the length of the comment.
 *
 * @return the length.
 */
UINT32
DralComment::getCommentLength() const
{
    return length;
}

/*
 * Gets the data of the comment.
 *
 * @return the comment.
 */
char *
DralComment::getComment() const
{
    return comment;
}

/*
 * Returns the size of the object and its components.
 *
 * @return the size of the object.
 */
INT64
DralComment::getObjSize() const
{
    INT64 size;

    size = sizeof(DralComment) + strlen(comment);
    return size;
}

/*
 * Returns a description of the usage of this class.
 *
 * @return the description.
 */
QString
DralComment::getUsageDescription() const
{
    return QString("");
}

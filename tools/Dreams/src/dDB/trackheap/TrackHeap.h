// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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
  * @file  TrackHeap.h
  * @brief
  */

#ifndef _DRALDB_TRACKHEAP_H
#define _DRALDB_TRACKHEAP_H

#include "dDB/DralDBSyntax.h"

// General includes.
#include <list>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/TagDescVector.h"
#include "dDB/aux/ZipObject.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/StatObj.h"
#include "dDB/dralgraph/DralGraph.h"
#include "dDB/itemheap/ItemHeap.h"
#include "dDB/taghandler/TagHandler.h"
#include "dDB/taghandler/TagFlipHandler.h"
#include "dDB/trackheap/TrackHeapDef.h"
#include "dDB/trackheap/TrackVec.h"

// Qt includes.
#include <qdict.h>

/*
 * @typedef struct ClockRangeEntry
 * @brief
 * This struct holds a range of tracks that have the same clock
 * domain.
 */
typedef struct ClockRangeEntry
{
    TRACK_ID first;   ///< First entry of the range.
    TRACK_ID last;    ///< Last entry of the range (not included).
    CLOCK_ID clockId; ///< The clock id of this range.
} ;

/*
 * @brief
 * This class stores the tracking of all the elements.
 *
 * @description
 * When a client wants to track the data of a node or edge of a
 * dral graph, then this heap saves all the values of the tags
 * of this element. Then, the clients can request the data stored
 * to know the values of the tags in any cycle.
 *
 * @author Federico Ardanaz
 * @date started at 2002-07-15
 * @version 0.1
 */
class TrackHeap : public AMemObj, public StatObj, public ZipObject
{
    public:
        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        QString getStats() const;
        ZipObject * compressYourSelf(CYCLE cycle, bool last=false);

        TrackHeap(ItemHeap * _itemdb, DralGraph * _dbgraph, TagDescVector * _tagdescvec, Dict2064 * _dict, StrTable * _strtbl);
        virtual ~TrackHeap();

        TRACK_ID trackEdgeTags(DRAL_ID edge_id);
        TRACK_ID trackNodeTags(DRAL_ID node_id);
        TRACK_ID trackCycleTags(CLOCK_ID clockId);

        inline TRACK_ID resolveTrackIdForNode(DRAL_ID node_id, UINT16 dim, UINT32 * position);
        inline TRACK_ID resolveTrackIdForNode(DRAL_ID node_id);
        inline TRACK_ID resolveTrackIdForEdge(DRAL_ID edge_id, NodeSlot slot);
        inline TRACK_ID resolveTrackIdForEdge(DRAL_ID edge_id);
        inline TRACK_ID resolveTrackIdForCycleTag(CLOCK_ID clockId);

        bool getTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle);
        bool getTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * value, CYCLE * atcycle);
        bool getTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle);
        bool getTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle);
        bool getTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * value, CYCLE * atcycle);
        bool getTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle);
        inline bool getItemInSlot(TRACK_ID trackId, CYCLE cycle, ItemHandler * handler);
        bool getFormatedTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle);
        bool getFormatedTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle);
        inline UINT32 getTagOccupancy(TRACK_ID trackId, TAG_ID tagId) const;

        inline bool addTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 value);
        inline bool addTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString value);
        inline bool addTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList * value);
        inline bool undefineTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle);

        bool addMoveItem(DRAL_ID edgeid, ITEM_ID itemid, UINT16 pos, CYCLE cycle);
        bool initTagHandler(TagHandler ** hnd, TRACK_ID track_id, TAG_ID tag_id, CYCLE cycle, bool start);
        bool initTagHandlerPhase(TagHandler ** hnd, TRACK_ID track_id, TAG_ID tag_id, CYCLE cycle, bool start);
        inline void initTagFlipHandler(TagFlipHandler * hnd, DralDB * draldb, TagFlipHandlerEnv * env);

        void dumpRegression(bool gui_enabled);

        inline void setFirstEffectiveCycle(CYCLE cycle);
        inline void setNumClocks(UINT16 numClocks);

        inline TagIDList * getKnownTagIDs(TRACK_ID trackId) const;
        inline UINT16 getClockId(TRACK_ID trackId) const;

        inline void compressTrackTag(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, bool last);
        inline void purgeAllocatedVectors();

    private:
        void trackNodeTags(DRAL_ID node_id, CLOCK_ID clock_id, TagRenameTable * trt, NodeSlot * base, NodeSlot * act, UINT16 depth);
        void newTrackEntry(CLOCK_ID clockId, TRACK_ID first, TRACK_ID last);

    private:
        TRACK_ID nextTrackID;               ///< Position of the next track.
        TAG_ID slotItemId;                  ///< Tag index used with items.
        INT32 * firstEffectiveCycle;        ///< First cycle of the database.
        TRACK_ID edgeTrackTbl[65536];       ///< Translation between edge id and track id.
        TRACK_ID nodeTrackTbl[65536];       ///< Translation between node id and track id.
        TRACK_ID cycleTrackTbl[65536];      ///< The track id for cycle tags.
        TrackIDVector trackIDVector;        ///< Vector with all the tracking.
        DralGraph * dbgraph;                  ///< Pointer to the database graph.
        ItemHeap * itemdb;               ///< Pointer to the item database.
        Dict2064 * dict;                    ///< Pointer to the dictionary.
        StrTable * strtbl;                  ///< Pointer to the string table.
        TagDescVector * tagdescvec;         ///< Pointer to the tag description pointer.
        QPtrList<TagRenameTable> renames;   ///< A list of the renaming tags created.
        list<ClockRangeEntry> clock_ranges; ///< This list is used to know the clock ranges.
};

/*
 * Gets the item inside the slot of this track.
 *
 * @return void.
 */
bool
TrackHeap::getItemInSlot(TRACK_ID trackId, CYCLE cycle, ItemHandler * handler)
{
    UINT64 value;

    if(!getTagValue(trackId, slotItemId, cycle, &value, NULL))
    {
        handler->invalid();
        return false;
    }
    return itemdb->lookForItemId(handler, (ITEM_ID) value);
}

/*
 * Given a node id and a slot information returns the track id.
 *
 * @return the trackid.
 */
TRACK_ID
TrackHeap::resolveTrackIdForNode(DRAL_ID node_id, UINT16 dim, UINT32 * position)
{
    DralGraphNode * node;
    TRACK_ID track;

    if(dim == 0)
    {
        return nodeTrackTbl[node_id];
    }

    if(nodeTrackTbl[node_id] == TRACK_ID_INVALID)
    {
        return TRACK_ID_INVALID;
    }

    node = dbgraph->getNode(node_id);
    track = NodeSlot::linearPosition(dim, position, node->getLayout());

    if(track >= TRACK_ID_FIRST_INVALID)
    {
        return track;
    }

    track += nodeTrackTbl[node_id] + 1;
    return track;
}

/*
 * Given a node id returns the track id.
 *
 * @return the trackid.
 */
TRACK_ID
TrackHeap::resolveTrackIdForNode(DRAL_ID node_id)
{
    return nodeTrackTbl[node_id];
}

/*
 * Given an edge id and a slot information returns the track id.
 *
 * @return the trackid.
 */
TRACK_ID
TrackHeap::resolveTrackIdForEdge(DRAL_ID edge_id, NodeSlot slot)
{
    DralGraphEdge * edge;

    if((edgeTrackTbl[edge_id] == TRACK_ID_INVALID) || (slot.dimensions != 2))
    {
        return TRACK_ID_INVALID;
    }

    edge = dbgraph->getEdge(edge_id);
    return nodeTrackTbl[edge_id] + edge->getBandwidth() * slot.capacities[0] + slot.capacities[1];
}

/*
 * Given an edge id returns the track id.
 *
 * @return the trackid.
 */
TRACK_ID
TrackHeap::resolveTrackIdForEdge(DRAL_ID edge_id)
{
    return edgeTrackTbl[edge_id];
}

/*
 * Gets the track id for the cycle tags.
 *
 * @return the trackid for cycle tags.
 */
TRACK_ID
TrackHeap::resolveTrackIdForCycleTag(CLOCK_ID clockId)
{
    return cycleTrackTbl[clockId];
}

/*
 * Gets the occupancy for the given track and tag.
 *
 * @return the occupancy.
 */
UINT32
TrackHeap::getTagOccupancy(TRACK_ID trackId, TAG_ID tagId) const
{
    return trackIDVector[trackId].getTagOccupancy(tagId);
}

/**
 * Adds a value in the tag tagId of the track trackId.
 *
 * @return true if the value is be added.
 */
bool
TrackHeap::addTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 value)
{
    CLOCK_ID clockId; ///< Clock id of the cycle

    clockId = cycle.clock->getId();
    Q_ASSERT(trackId < nextTrackID);
    if(trackIDVector[trackId].getClockId() != clockId)
    {
        return false;
    }

    cycle.cycle -= firstEffectiveCycle[clockId];
    return trackIDVector[trackId].addTagValue(tagId, cycle, value);
}

/**
 * Adds a value in the tag tagId of the track trackId.
 *
 * @return true if the value is be added.
 */
bool
TrackHeap::addTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString value)
{
    UINT64 temp;      ///< String to integer conversion.
    CLOCK_ID clockId; ///< Clock id of the cycle

    clockId = cycle.clock->getId();
    Q_ASSERT(trackId < nextTrackID);
    if(trackIDVector[trackId].getClockId() != clockId)
    {
        return false;
    }

    temp = strtbl->addString(value);

    cycle.cycle -= firstEffectiveCycle[clockId];
    return trackIDVector[trackId].addTagValue(tagId, cycle, temp);
}

/*
 * Adds a value in the tag tagId of the track trackId.
 *
 * @return true if the value is added.
 */
bool
TrackHeap::addTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList * value)
{
    CLOCK_ID clockId; ///< Clock id of the cycle

    clockId = cycle.clock->getId();
    Q_ASSERT(trackId < nextTrackID);
    if(trackIDVector[trackId].getClockId() != clockId)
    {
        return false;
    }

    cycle.cycle -= firstEffectiveCycle[clockId];
    return trackIDVector[trackId].addTagValue(tagId, cycle, value);
}

/*
 * Undefines the last value.
 *
 * @return true if the tag is undefined.
 */
bool
TrackHeap::undefineTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle)
{
    CLOCK_ID clockId; ///< Clock id of the cycle

    clockId = cycle.clock->getId();
    Q_ASSERT(trackId < nextTrackID);
    if(trackIDVector[trackId].getClockId() != clockId)
    {
        return false;
    }

    cycle.cycle -= firstEffectiveCycle[clockId];
    return trackIDVector[trackId].undefineTagValue(tagId, cycle);
}

/*
 * Gets the item inside the slot of this track.
 *
 * @return void.
 */
void
TrackHeap::initTagFlipHandler(TagFlipHandler * hnd, DralDB * draldb, TagFlipHandlerEnv * env)
{
    hnd->init(draldb, env);
}

/**
 * Sets the first effective cycle of the database.
 *
 * @return void.
 */
void
TrackHeap::setFirstEffectiveCycle(CYCLE cycle)
{
    firstEffectiveCycle[cycle.clock->getId()] = cycle.cycle;
}

/*
 * Sets the number of clocks.
 *
 * @return void.
 */
void
TrackHeap::setNumClocks(UINT16 numClocks)
{
    Q_ASSERT(firstEffectiveCycle == NULL);
    firstEffectiveCycle = new INT32[numClocks];
}

/*
 * Gets the known tags of the track trackId.
 *
 * @return the tag name list.
 */
TagIDList *
TrackHeap::getKnownTagIDs(TRACK_ID trackId) const
{
    if(trackId >= nextTrackID)
    {
        return NULL;
    }

    return trackIDVector[trackId].getKnownTagIDs();
}

/*
 * Gets the clock id of the track trackId.
 *
 * @return the clock id.
 */
CLOCK_ID
TrackHeap::getClockId(TRACK_ID trackId) const
{
    Q_ASSERT(trackId < nextTrackID);

    return trackIDVector[trackId].getClockId();
}

/*
 * Compresses a tag of a given track.
 *
 * @return the clock id.
 */
void
TrackHeap::compressTrackTag(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, bool last)
{
    // Compresses both high and low values.
    trackIDVector[trackId].compressTag(tagId, cycle, last);
}

/*
 * Purges all the allocated vectors of the track heap
 * that hasn't yet been deleted.
 *
 * @return void.
 */
void
TrackHeap::purgeAllocatedVectors()
{
    TagIdVecNode::purgeAllocatedVectors();
}

#endif

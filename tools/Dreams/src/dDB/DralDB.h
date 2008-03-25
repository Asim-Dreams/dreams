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
  * @file  DralDB.h
  */

#ifndef _DRALDB_DRALDB_H
#define _DRALDB_DRALDB_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/DBListener.h"
#include "dDB/TagDescVector.h"
#include "dDB/DBConfig.h"
#include "dDB/StrTable.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/ClockDomainMgr.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/StatObj.h"
#include "dDB/itemheap/ItemHeap.h"
#include "dDB/trackheap/TrackHeap.h"
#include "dDB/dralgraph/DralGraph.h"

// Qt includes.
#include <qstring.h>
#include <qregexp.h>

// Asim includes.
#include <asim/dralClient.h>

// Class forwarding.
class LogMgr;
class Dict2064;

/**
  * @brief
  * This class implements the database interface with its clients.
  *
  * @description
  * This class uses the 'Facade' design template. All the calls
  * that this class defines are forwarded to the correct object
  * that is the one who resolves the call.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DralDB : public AMemObj, public StatObj
{
    public:
        DralDB();
        virtual ~DralDB();

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        QString getStats() const;

        // -------------------------------------------------------------------
        // -- File & Initialization Methods
        // -------------------------------------------------------------------
        bool openDRLFile(QString filename);
        bool closeDRLFile();
        inline INT32 getNumBytesRead();
        inline INT32 getFileSize();

        // -------------------------------------------------------------------
        // -- DRAL Processing Methods
        // -------------------------------------------------------------------
        inline INT32 processEvents(INT32 commands);
        bool loadDRLHeader();
        inline void startLoading();
        inline bool reachedEOS() const;
        inline bool getLastProcessedEventOk() const;
        inline CYCLE getNumCycles(CLOCK_ID clockId) const;
        inline CYCLE getFirstEffectiveCycle(CLOCK_ID clockId) const;
        inline CYCLE getLastEffectiveCycle(CLOCK_ID clockId) const;
        inline CYCLE getFirstCycle() const;
        inline CYCLE getLastCycle() const;
        inline bool hasDivisions() const;
        inline const DralComment * getFirstDral1Comment() const;
        inline const DralComment * getNextDral1Comment() const;
        inline const DralComment * getDral2Comment(UINT32 magic_number) const;
        inline const DralComment * getFirstDral2Comment() const;
        inline const DralComment * getNextDral2Comment() const;

        // -------------------------------------------------------------------
        // -- Other DRALClient Listeners can be attached
        // -------------------------------------------------------------------
        inline void attachDralListener(DRAL_LISTENER object);

        // -------------------------------------------------------------------
        // -- Tracking Setup Methods
        // -------------------------------------------------------------------

        // track all requests
        inline TRACK_ID trackNodeTags(DRAL_ID node_id);
        inline TRACK_ID trackEdgeTags(DRAL_ID edge_id);
        inline TRACK_ID trackCycleTags(CLOCK_ID clockId);
        inline void  trackItemTags(bool value);

        inline TRACK_ID resolveTrackIdForNode(UINT16 node_id, NodeSlot slot);
        inline TRACK_ID resolveTrackIdForNode(UINT16 node_id);
        inline TRACK_ID resolveTrackIdForEdge(UINT16 edge_id, NodeSlot slot);
        inline TRACK_ID resolveTrackIdForEdge(UINT16 edge_id);
        inline TRACK_ID resolveTrackIdForCycleTag(UINT16 clockId);

        // -------------------------------------------------------------------
        // -- Track-Tag Consult Methods
        // -------------------------------------------------------------------
        inline bool addTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 value);
        inline bool addTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString value);
        inline bool addTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList * value);
        inline bool undefineTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle);

        // -------------------------------------------------------------------
        // -- Track-Tag Consult Methods
        // -------------------------------------------------------------------
        inline bool getTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValue(TRACK_ID trackId, QString tagName, CYCLE cycle, UINT64 *  value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValue(TRACK_ID trackId, QString tagName, CYCLE cycle, QString * value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValue(TRACK_ID trackId, QString tagName, CYCLE cycle, SOVList ** value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValuePhase(TRACK_ID trackId, QString tagName, CYCLE cycle, UINT64 *  value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValuePhase(TRACK_ID trackId, QString tagName, CYCLE cycle, QString * value, CYCLE * atcycle = NULL);
        inline bool getTrackTagValuePhase(TRACK_ID trackId, QString tagName, CYCLE cycle, SOVList ** value, CYCLE * atcycle = NULL);

        inline bool getFormatedTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle = NULL);
        inline bool getFormatedTrackTagValue(TRACK_ID trackId, QString tagName, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle = NULL);
        inline bool getFormatedTrackTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle = NULL);
        inline bool getFormatedTrackTagValuePhase(TRACK_ID trackId, QString tagName, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle = NULL);

        inline UINT32 getTrackTagOccupancy(TRACK_ID trackId, TAG_ID tagId) const;

        inline bool initTagHandler(TagHandler ** hnd, TRACK_ID track_id, TAG_ID tag_id, CYCLE cycle, bool start);
        inline bool initTagHandlerPhase(TagHandler ** hnd, TRACK_ID track_id, TAG_ID tag_id, CYCLE cycle, bool start);
        inline void initTagFlipHandler(TagFlipHandler * hnd, TagFlipHandlerEnv * env);
        inline bool getItemInSlot(TRACK_ID trackId, CYCLE cycle, ItemHandler * handler);

        inline TagIDList * getKnownTagIDs(TRACK_ID trackId) const;

        // -------------------------------------------------------------------
        // -- Item-Handler/Tag Consult Methods
        // -------------------------------------------------------------------
        inline bool getFirstItem(ItemHandler * handler);
        inline bool getLastItem(ItemHandler * handler);

        /**
          * Look for a given value in a given context (tag) starting from a given point.
          * @param target_tagid the tag id we are looking for
          * @param target_value the tag value we are looking for
          * @param staring_point vector entry where to start the scanning
          * @return vector position or -1 if <tag-value> not found
          */
        inline void lookForIntegerValue(ItemHandler * handler, TAG_ID target_tagid, UINT64 target_value, CYCLE cycle, INT32 starting_point = 0);
        inline void lookForStringValue(ItemHandler * handler, TAG_ID target_tagid, QString target_value, bool csensitive, bool exactMatch, CYCLE cycle, INT32 starting_point = 0);
        inline void lookForStringValue(ItemHandler * handler, TAG_ID target_tagid, QRegExp target_value, CYCLE cycle, INT32 starting_point = 0);

        inline bool lookForItemId(ItemHandler * handler, ITEM_ID itemid);

        inline INT32 getItemHeapNumItems();

        // -------------------------------------------------------------------
        // -- Global Configuration Methods
        // -------------------------------------------------------------------
        inline bool getAutoPurge();
        inline bool getIncrementalPurge();
        inline bool getMaxIFIEnabled();
        inline bool getTagBackPropagate();
        inline bool getGUIEnabled();
        inline int  getItemMaxAge();
        inline int  getMaxIFI();
        inline bool getCompressMutable();

        inline void setAutoPurge(bool value);
        inline void setIncrementalPurge(bool value);
        inline void setMaxIFIEnabled(bool value);
        inline void setTagBackPropagate(bool value);
        inline void setGUIEnabled(bool value);
        inline void setItemMaxAge(int value);
        inline void setMaxIFI(int value);
        inline void setCompressMutable(bool value);
        inline void setIgnoreEdgeBandwidthConstrain();

        // -------------------------------------------------------------------
        // -- Tag Descriptor (low level) Methods
        // -------------------------------------------------------------------
        inline TagValueType getTagValueType(TAG_ID id);
        inline TagValueBase getTagValueBase(TAG_ID id);
        inline QString getTagName(TAG_ID id);
        inline TAG_ID getTagId(QString name);
        inline void getFormatedTagValue(QString & buffer, TAG_ID tagId, UINT64 value, INT32 pad = 0);
        inline bool newTagDescriptor(QString tag_name, TagValueType type, TagValueBase base, TagSizeStorage size, bool forwarding, TAG_ID * id);
        inline QStrList getKnownTags();
        inline QString getTagDesc(QString tgName);
        inline void setTagDesc(TAG_ID tag_idx, QString desc);
        inline TAG_ID getItemIdTagId();
        inline TAG_ID getSlotItemTagId();
        inline TAG_ID getNewItemTagId();
        inline TAG_ID getDelItemTagId();
        inline TAG_ID getNumTags();

        //------------------------------------------------------------
        // String table (low level) methods
        //------------------------------------------------------------
        inline QString getString(INT32 id);
        inline INT32 addString(QString);
        inline bool hasString(QString);

        //------------------------------------------------------------
        // Graph access methods
        //------------------------------------------------------------
        QString getGraphDescription();
        void dumpGraphDescription();

        inline QString normalizeNodeName(QString name);
        inline QString slotedNodeName(QString name, NodeSlot nslot);
        inline bool decodeNodeSlot(QString strnodeslot, QString * nodename, NodeSlot * nslot);

        inline DralGraphNode * getNode(DRAL_ID nodeId);
        inline DralGraphNode * getNode(QString name);
        inline DralGraphEdge * getEdge(DRAL_ID edgeId);
        inline DralGraphEdge * findEdgeByNameFromTo(QString name, QString fromstr, QString tostr);
        inline bool findEdgeByName(QString name, DralGraphEdge * &edge);

        inline INT32 getBiggestLatency();

        // -------------------------------------------------------------------
        // -- Misc
        // -------------------------------------------------------------------
        inline QString getFormatedTagValue(TAG_ID tagid, UINT64 tagvalue);
        inline bool getHasFatalError(bool end_sim);

        inline void compressTrackHeap();
        inline void compressTrackTag(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, bool last);
        inline void purgeTrackHeapAllocatedVectors();

        inline void dumpRegression(bool gui_enabled);

        inline CLOCK_ID getNumClocks() const;
        inline const ClockDomainEntry * getClockDomain(CLOCK_ID clockId) const;
        inline ClockDomainFreqIterator getClockDomainFreqIterator() const;
        inline CLOCK_ID getClockId(TRACK_ID trackId) const;

        void clearDict();

        // Access to database structs
        inline TrackHeap * getTrackHeap() const;
        inline ItemHeap * getItemHeap() const;

    private:
        DBListener *     dblistener;      ///< Database listener.
        TrackHeap *      track_heap;      ///< Track heap.
        ItemHeap *       item_heap;       ///< Item tag heap.
        TagDescVector *  tagDescVector;   ///< Tag descriptor.
        DBConfig *       dbConfig;        ///< Database configuration.
        DralGraph *      dbGraph;         ///< Dral graph.
        StrTable *       strtable;        ///< Pointer to the string table.
        LogMgr *         logMgr;          ///< Pointer to the log manager.
        QFile *          eventFile;       ///< File with events.
        Dict2064 *       dict;            ///< Dictionary.
        ClockDomainMgr * clocks;          ///< Clock domains.
        DRAL_CLIENT      dralClient;      ///< Client of the database.
        INT32            numTrackedEdges; ///< Number of edges that are being tracked.
        bool             firstTime;       ///< 
};

// -------------------------------------------------------------------
// -- Initialization Methods
// -------------------------------------------------------------------

/**
 * Gets the number of bytes read of the dral client.
 *
 * @return the number of bytes read.
 */
INT32
DralDB::getNumBytesRead()
{
    return dralClient->GetNumBytesRead();
}

/**
 * Gets the file size of the dral client.
 *
 * @return the file size.
 */
INT32
DralDB::getFileSize()
{
    return dralClient->GetFileSize();
}


// -------------------------------------------------------------------
// -- Tracking Setup Methods
// -------------------------------------------------------------------

/**
 * Tracks the node tags of the whole node.
 *
 * @return the base track id.
 */
TRACK_ID
DralDB::trackNodeTags(DRAL_ID node_id)
{
    return track_heap->trackNodeTags(node_id);
}

/**
 * Tracks the edge edge_id. Forwarded to track heap.
 *
 * @return the base track id.
 */
TRACK_ID
DralDB::trackEdgeTags(DRAL_ID edge_id)
{
    INT32 id = track_heap->trackEdgeTags(edge_id);
    if(id >= 0)
    {
        numTrackedEdges++;
        dblistener->addTrackedEdges(edge_id);
    }
    return id;
}

/**
 * Tracks the cycle tags.
 *
 * @return track id for cycle tags.
 */
TRACK_ID
DralDB::trackCycleTags(CLOCK_ID clockId)
{
    return track_heap->trackCycleTags(clockId);
}

/**
 * Sets the database to track or not the item tags.
 *
 * @return void.
 */
void
DralDB::trackItemTags(bool value)
{
    dblistener->trackItemTags(value);
}

/*
 * Resolves the track id for a given node.
 *
 * @return the track id.
 */
TRACK_ID
DralDB::resolveTrackIdForNode(DRAL_ID node_id, NodeSlot slot)
{
    return track_heap->resolveTrackIdForNode(node_id, slot.dimensions, slot.capacities);
}

/*
 * Resolves the track id for a given node.
 *
 * @return the track id.
 */
TRACK_ID
DralDB::resolveTrackIdForNode(DRAL_ID node_id)
{
    return track_heap->resolveTrackIdForNode(node_id);
}

/*
 * Return the track id of the slot of the given edge.
 *
 * @return the trackid.
 */
TRACK_ID
DralDB::resolveTrackIdForEdge(DRAL_ID edge_id, NodeSlot slot)
{
    return track_heap->resolveTrackIdForEdge(edge_id, slot);
}

/*
 * Return the track id of the slot of the given edge.
 *
 * @return the trackid.
 */
TRACK_ID
DralDB::resolveTrackIdForEdge(DRAL_ID edge_id)
{
    return track_heap->resolveTrackIdForEdge(edge_id);
}

/*
 * Return the track for the cycle tags.
 *
 * @return the trackid.
 */
TRACK_ID
DralDB::resolveTrackIdForCycleTag(CLOCK_ID clockId)
{
    return track_heap->resolveTrackIdForCycleTag(clockId);
}

// -------------------------------------------------------------------
// -- Track-Tag Consult Methods
// -------------------------------------------------------------------

/*
 * Adds a value to the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::addTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 value)
{
    return track_heap->addTagValue(trackId, tagId, cycle, value);
}

/*
 * Adds a value to the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::addTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString value)
{
    return track_heap->addTagValue(trackId, tagId, cycle, value);
}

/*
 * Adds a value to the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::addTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList * value)
{
    return track_heap->addTagValue(trackId, tagId, cycle, value);
}

/*
 * Undefines the tag tagId of the trackId in the cycle cycle.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::undefineTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle)
{
    return track_heap->undefineTagValue(trackId, tagId, cycle);
}

// -------------------------------------------------------------------
// -- Track-Tag Consult Methods
// -------------------------------------------------------------------

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle)
{
    return track_heap->getTagValue(trackId, tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * value, CYCLE * atcycle)
{
    return track_heap->getTagValue(trackId, tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle)
{
    return track_heap->getTagValue(trackId, tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValue(TRACK_ID trackId, QString tagName, CYCLE cycle, UINT64 * value, CYCLE * atcycle)
{
    TAG_ID tagId = tagDescVector->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        return false;
    }
    return track_heap->getTagValue(trackId, (UINT16) tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValue(TRACK_ID trackId, QString tagName, CYCLE cycle, QString * value, CYCLE * atcycle)
{
    TAG_ID tagId = tagDescVector->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        return false;
    }
    return track_heap->getTagValue(trackId, (UINT16) tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValue(TRACK_ID trackId, QString tagName, CYCLE cycle, SOVList ** value, CYCLE * atcycle)
{
    TAG_ID tagId = tagDescVector->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        return false;
    }
    return track_heap->getTagValue(trackId, tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle)
{
    return track_heap->getTagValuePhase(trackId, tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * value, CYCLE * atcycle)
{
    return track_heap->getTagValuePhase(trackId, tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle)
{
    return track_heap->getTagValuePhase(trackId, tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValuePhase(TRACK_ID trackId, QString tagName, CYCLE cycle, UINT64 * value, CYCLE * atcycle)
{
    TAG_ID tagId = tagDescVector->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        return false;
    }
    return track_heap->getTagValuePhase(trackId, (UINT16) tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValuePhase(TRACK_ID trackId, QString tagName, CYCLE cycle, QString * value, CYCLE * atcycle)
{
    TAG_ID tagId = tagDescVector->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        return false;
    }
    return track_heap->getTagValuePhase(trackId, (UINT16) tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getTrackTagValuePhase(TRACK_ID trackId, QString tagName, CYCLE cycle, SOVList ** value, CYCLE * atcycle)
{
    TAG_ID tagId = tagDescVector->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        return false;
    }
    return track_heap->getTagValuePhase(trackId, tagId, cycle, value, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getFormatedTrackTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle)
{
    return track_heap->getFormatedTagValue(trackId, tagId, cycle, fmtvalue, atcycle);
}

/*
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getFormatedTrackTagValue(TRACK_ID trackId, QString tagName, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle)
{
    TAG_ID tagId = tagDescVector->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        return false;
    }
    return track_heap->getFormatedTagValue(trackId, tagId, cycle, fmtvalue, atcycle);
}

/**
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getFormatedTrackTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle)
{
    return track_heap->getFormatedTagValuePhase(trackId, tagId, cycle, fmtvalue, atcycle);
}

/*
 * Gets the value of the tagId of the trackId in the cycle cycle.
 * Forwarded to track heap.
 *
 * @return if everything has gone ok.
 */
bool
DralDB::getFormatedTrackTagValuePhase(TRACK_ID trackId, QString tagName, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle)
{
    TAG_ID tagId = tagDescVector->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        return false;
    }
    return track_heap->getFormatedTagValuePhase(trackId, tagId, cycle, fmtvalue, atcycle);
}

/*
 * Gets the occupancy for the given track and tag.
 *
 * @return the occupancy.
 */
UINT32
DralDB::getTrackTagOccupancy(TRACK_ID trackId, TAG_ID tagId) const
{
    return track_heap->getTagOccupancy(trackId, tagId);
}

/*
 * Gets the item inside the slot with track trackId in the cycle cycle.
 *
 * @return if an item is inside.
 */
bool
DralDB::initTagHandler(TagHandler ** hnd, TRACK_ID track_id, TAG_ID tag_id, CYCLE cycle, bool start)
{
    return track_heap->initTagHandler(hnd, track_id, tag_id, cycle, start);
}

/*
 * Gets the item inside the slot with track trackId in the cycle cycle.
 *
 * @return if an item is inside.
 */
bool
DralDB::initTagHandlerPhase(TagHandler ** hnd, TRACK_ID track_id, TAG_ID tag_id, CYCLE cycle, bool start)
{
    return track_heap->initTagHandlerPhase(hnd, track_id, tag_id, cycle, start);
}

/*
 * Initializes a tag flip handler with the tags inside the tag_list
 * for the slot that has track id track_id.
 *
 * @return void.
 */
void
DralDB::initTagFlipHandler(TagFlipHandler * hnd, TagFlipHandlerEnv * env)
{
    track_heap->initTagFlipHandler(hnd, this, env);
}

/**
 * Gets the item inside the slot with track trackId in the cycle cycle.
 *
 * @return if an item is inside.
 */
bool
DralDB::getItemInSlot(TRACK_ID trackId, CYCLE cycle, ItemHandler * handler)
{
    return track_heap->getItemInSlot(trackId, cycle, handler);
}

/**
 * Gets the known tags of the track trackId. Forwarded to track
 * heap.
 *
 * @return the tag name list.
 */
TagIDList * 
DralDB::getKnownTagIDs(TRACK_ID trackId) const
{
    return track_heap->getKnownTagIDs(trackId);
}

// -------------------------------------------------------------------
// Tag Descriptor methods
// -------------------------------------------------------------------

/**
 * Gets the value type of the tag descriptor id.
 *
 * @return the value type.
 */
TagValueType
DralDB::getTagValueType(TAG_ID id)
{
    return tagDescVector->getTagValueType(id);
}

/**
 * Gets the base of the tag descriptor id.
 *
 * @return the base.
 */
TagValueBase
DralDB::getTagValueBase(TAG_ID id)
{
    return tagDescVector->getTagValueBase(id);
}

/**
 * Gets the name of the tag id.
 *
 * @return the name.
 */
QString
DralDB::getTagName(TAG_ID id)
{
    return tagDescVector->getTagName(id);
}

/**
 * Gets the index of the tag descriptor for name.
 *
 * @return the tag descriptor index.
 */
TAG_ID
DralDB::getTagId(QString name)
{
    return tagDescVector->getTagId(name);
}

/**
 * Adds a new tag descriptor to the vector of tag descriptions.
 *
 * @return the tag descriptor index.
 */
bool
DralDB::newTagDescriptor(QString tag_name, TagValueType type, TagValueBase base, TagSizeStorage size, bool forwarding, TAG_ID * id)
{
    return tagDescVector->newTagDescriptor(tag_name, type, base, size, forwarding, id);
}

/**
 * Gets the known tags of the tag descriptor vector.
 *
 * @return the list of tag names.
 */
QStrList
DralDB::getKnownTags()
{
    return tagDescVector->getKnownTags();
}

/**
 * Gets the long description of the tag with name tgName.
 *
 * @return the description.
 */
QString 
DralDB::getTagDesc(QString tgName)
{
    return tagDescVector->getTagDesc(tgName);
}

/**
 * Sets the description of the tag with index tag_idx.
 *
 * @return void.
 */
void    
DralDB::setTagDesc(TAG_ID tag_idx, QString desc)
{
    tagDescVector->setTagDesc(tag_idx, desc);
} 

/*
 * Returns the requested special tag id.
 *
 * @return the tagid value.
 */
TAG_ID
DralDB::getItemIdTagId()
{
    return tagDescVector->getItemIdTagId();
}

/*
 * Returns the requested special tag id.
 *
 * @return the tagid value.
 */
TAG_ID
DralDB::getSlotItemTagId()
{
    return tagDescVector->getSlotItemTagId();
}

/*
 * Returns the requested special tag id.
 *
 * @return the tagid value.
 */
TAG_ID
DralDB::getNewItemTagId()
{
    return tagDescVector->getNewItemTagId();
}

/*
 * Returns the requested special tag id.
 *
 * @return the tagid value.
 */
TAG_ID
DralDB::getDelItemTagId()
{
    return tagDescVector->getDelItemTagId();
}

/**
 * Return the number of tags used.
 *
 * @return number of tags.
 */
TAG_ID
DralDB::getNumTags()
{
    return tagDescVector->getNumTags();
}

// -------------------------------------------------------------------
// String table methods
// -------------------------------------------------------------------

/**
 * Gets the string for the index id.
 *
 * @return the string.
 */
QString
DralDB::getString(INT32 id)
{
    return strtable->getString(id);
}

/**
 * Adds the string v in the string table.
 *
 * @return the index.
 */
INT32
DralDB::addString(QString v)
{
    return strtable->addString(v);
}

/**
 * Looks up for the string v in the string table.
 *
 * @return if the string is in the table.
 */
bool
DralDB::hasString(QString v)
{
    return strtable->hasString(v);
}

// -------------------------------------------------------------------
// Misc
// -------------------------------------------------------------------

/**
 * Gets the formatted value tagvalue for the tagid format.
 *
 * @return the formatted value.
 */
QString
DralDB::getFormatedTagValue(TAG_ID tagid, UINT64 tagvalue)
{
    return tagDescVector->getFormatedTagValue(tagid, tagvalue);
}

/**
 * Checks if the dblistener has processed all the events without
 * problems.
 *
 * @return if a fatal error has happened.
 */
bool
DralDB::getHasFatalError(bool end_sim)
{
    return dblistener->getHasFatalError(end_sim);
}


// -------------------------------------------------------------------
// -- DRAL Processing Methods
// -------------------------------------------------------------------

/**
 * Process events the events to the dral clients.
 *
 * @return the result of processing the next event.
 */
INT32
DralDB::processEvents(INT32 commands)
{
    if (firstTime)
    {
        // prepare listener
        firstTime=false;
        dblistener->setTrackedEdges(numTrackedEdges);
        dblistener->propagateFirstCycle();
    }
    return dralClient->ProcessNextEvent(true,commands);
}

// -------------------------------------------------------------------
// -- Global Configuration Methods
// -------------------------------------------------------------------

/**
 * Returns the autpurge value.
 *
 * @return autopurge.
 */
bool
DralDB::getAutoPurge()
{
    return dbConfig->getAutoPurge();
}

/**
 * Returns the incrementalPurge value.
 *
 * @return incrementalPurge.
 */
bool
DralDB::getIncrementalPurge()
{
    return dbConfig->getIncrementalPurge();
}

/**
 * Returns the maxIFIEnabled value.
 *
 * @return maxIFIEnabled.
 */
bool
DralDB::getMaxIFIEnabled()
{
    return dbConfig->getMaxIFIEnabled();
}

/**
 * Returns the tagBackPropagate value.
 *
 * @return tagBackPropagate.
 */
bool
DralDB::getTagBackPropagate()
{
    return dbConfig->getTagBackPropagate();
}

/**
 * Returns the guiEnabled value.
 *
 * @return guiEnabled.
 */
bool
DralDB::getGUIEnabled()
{
    return dbConfig->getGUIEnabled();
}

/**
 * Returns the itemMaxAge value.
 *
 * @return itemMaxAge.
 */
int
DralDB::getItemMaxAge()
{
    return dbConfig->getItemMaxAge();
}

/**
 * Sets the maxIFI value.
 *
 * @return maxIFI.
 */
int
DralDB::getMaxIFI()
{
    return dbConfig->getMaxIFI();
}

/**
 * Returns the compressMutable value.
 *
 * @return compressMutable.
 */
bool
DralDB::getCompressMutable()
{
    return dbConfig->getCompressMutable();
}

/**
 * Sets the autoPurge value.
 *
 * @return void.
 */
void
DralDB::setAutoPurge(bool value) 
{
    dbConfig->setAutoPurge(value);
}

/**
 * Sets the incrementalPurge value.
 *
 * @return void.
 */
void 
DralDB::setIncrementalPurge(bool value)
{
    dbConfig->setIncrementalPurge(value);
}

/**
 * Sets the maxIFIEnabled value.
 *
 * @return void.
 */
void 
DralDB::setMaxIFIEnabled(bool value) 
{
    dbConfig->setMaxIFIEnabled(value);
}

/**
 * Sets the tagBackPropagate value.
 *
 * @return void.
 */
void 
DralDB::setTagBackPropagate(bool value) 
{
    dbConfig->setTagBackPropagate(value);
}

/**
 * Sets the guiEnabled value.
 *
 * @return void.
 */
void
DralDB::setGUIEnabled(bool value)
{
    dbConfig->setGUIEnabled(value);
}

/**
 * Sets the compressMutable value.
 *
 * @return void.
 */
void
DralDB::setCompressMutable(bool value)
{
    dbConfig->setCompressMutable(value);
}

/**
 * Sets the itemMaxAge value.
 *
 * @return void.
 */
void
DralDB::setItemMaxAge(int  value)
{
    dbConfig->setItemMaxAge(value);
}

/**
 * Sets the maxIFI value.
 *
 * @return void.
 */
void
DralDB::setMaxIFI(int  value)
{
    dbConfig->setMaxIFI(value);
}

/**
 * Sets the maxIFI value.
 *
 * @return void.
 */
void
DralDB::setIgnoreEdgeBandwidthConstrain()
{
    dblistener->setIgnoreEdgeBandwidthConstrain();
}

/**
 * Attaches a dral listener.
 *
 * @return void.
 */
void
DralDB::attachDralListener(DRAL_LISTENER object)
{
    dblistener->attachDralListener(object);
}

// -------------------------------------------------------------------
// -- Item-Handler Consult Methods
// -------------------------------------------------------------------

/**
 * Tries to find the target_value for the target_tagid.
 *
 * @return void.
 */
void
DralDB::lookForIntegerValue(ItemHandler * handler, TAG_ID target_tagid, UINT64 target_value, CYCLE cycle, INT32 starting_point)
{
    item_heap->lookForIntegerValue(handler, target_tagid, target_value, cycle, starting_point);
}

/**
 * Tries to find the target_value for the target_tagid.
 *
 * @return void.
 */
void
DralDB::lookForStringValue(ItemHandler * handler, TAG_ID target_tagid, QString target_value, bool csensitive, bool exactMatch, CYCLE cycle, INT32 starting_point)
{
    item_heap->lookForStringValue(handler, target_tagid, target_value, csensitive, exactMatch, cycle, starting_point);
}

/**
 * Tries to find the target_value for the target_tagid.
 *
 * @return void.
 */
void
DralDB::lookForStringValue(ItemHandler * handler, TAG_ID target_tagid, QRegExp target_value, CYCLE cycle, INT32 starting_point)
{
    item_heap->lookForStringValue(handler, target_tagid, target_value, cycle, starting_point);
}

/**
 * Sets the ItemHandler to point to the first item in the
 * ItemHeap.
 *
 * @return if at least one item is in the heap.
 */
bool
DralDB::getFirstItem(ItemHandler * handler)
{
    return item_heap->getFirstItem(handler);
}

/**
 * Sets the ItemHandler to point to the last item in the
 * ItemHeap.
 *
 * @return if at least one item is in the heap.
 */
bool
DralDB::getLastItem(ItemHandler * handler)
{
    return item_heap->getLastItem(handler);
}

/**
 * Searches the item itemid in the ItemHeap. The call is
 * forwarded to the ItemHeap.
 *
 * @return if the item is found.
 */
bool
DralDB::lookForItemId(ItemHandler * handler, ITEM_ID itemid)
{
    return item_heap->lookForItemId(handler, itemid);
}

/**
 * Get the number of heap items.
 *
 * @return the number of items in the ItemHeap.
 */
INT32
DralDB::getItemHeapNumItems()
{
    return item_heap->getNumItems();
}

/**
 * Just set the number of tags to the track heap.
 *
 * @return void.
 */
void
DralDB::startLoading()
{
}

/**
 * Gets the end of simulation flag. Forwarded to dblistener.
 *
 * @return if the simulation has ended.
 */
bool
DralDB::reachedEOS() const
{
    return dblistener->reachedEOS();
}

/**
 * Gets if an error has happened.
 *
 * @return if the simulation has ended.
 */
bool
DralDB::getLastProcessedEventOk() const
{
    return dblistener->getLastProcessedEventOk();
}

/**
 * Get the number of effective cycles of the actual drl trace.
 *
 * @return the number of effective cycles.
 */
CYCLE
DralDB::getNumCycles(CLOCK_ID clockId) const
{
    return dblistener->getNumCycles(clockId);
}

/**
 * Get the first effective cycle of the actual drl trace.
 *
 * @return the first effective cycle.
 */
CYCLE
DralDB::getFirstEffectiveCycle(CLOCK_ID clockId) const
{
    return dblistener->getFirstEffectiveCycle(clockId);
}

/*
 * Get the last effective cycle of the actual drl trace.
 *
 * @return the last effective cycle.
 */
CYCLE
DralDB::getLastEffectiveCycle(CLOCK_ID clockId) const
{
    return dblistener->getLastEffectiveCycle(clockId);
}

/*
 * Returns a cycle that is the lowest that can be found in the trace.
 *
 * @return the first cycle.
 */
CYCLE
DralDB::getFirstCycle() const
{
    return dblistener->getFirstCycle();
}

/*
 * Returns a cycle that is the biggest that can be found in the trace.
 *
 * @return the last cycle.
 */
CYCLE
DralDB::getLastCycle() const
{
    return dblistener->getLastCycle();
}

/*
 * Gets if the trace has divisions.
 *
 * @return if the trace has divisions.
 */
bool
DralDB::hasDivisions() const
{
    return dblistener->hasDivisions();
}

/*
 * Get the first dral 1 comment.
 *
 * @return the comment.
 */
const DralComment *
DralDB::getFirstDral1Comment() const
{
    return dblistener->getFirstDral1Comment();
}

/*
 * Get the next dral 1 comment.
 *
 * @return the comment.
 */
const DralComment *
DralDB::getNextDral1Comment() const
{
    return dblistener->getNextDral1Comment();
}

/*
 * Get the dral 2 comment with magic number magic_number.
 *
 * @return the comment.
 */
const DralComment *
DralDB::getDral2Comment(UINT32 magic_number) const
{
    return dblistener->getDral2Comment(magic_number);
}

/*
 * Get the first dral 2 comment.
 *
 * @return the comment.
 */
const DralComment *
DralDB::getFirstDral2Comment() const
{
    return dblistener->getFirstDral2Comment();
}

/*
 * Get the next dral 2 comment.
 *
 * @return the comment.
 */
const DralComment *
DralDB::getNextDral2Comment() const
{
    return dblistener->getNextDral2Comment();
}

/**
 * Dumps to the standard output all the contents of the DataBase.
 *
 * @return void.
 */
void
DralDB::dumpRegression(bool gui_enabled)
{
    track_heap->dumpRegression(gui_enabled);
    item_heap->dumpRegression(gui_enabled);
}

/**
 * Compresses the track heap.
 *
 * @return void.
 */
void
DralDB::compressTrackHeap()
{
    // Compresses all the clock domains.
    for(CLOCK_ID i = 0; i < clocks->getNumClocks(); i++)
    {
        const ClockDomainEntry * clock;
        clock = clocks->getClockDomain(i);
        CYCLE cycle(clock, dblistener->getLastEffectiveCycle(i).cycle + CYCLE_CHUNK_SIZE, 0);

        track_heap = (TrackHeap *) (track_heap->compressYourSelf(cycle, true));
    }
}

/*
 * Compresses a tag for a given track id.
 *
 * @return void.
 */
void
DralDB::compressTrackTag(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, bool last)
{
    track_heap->compressTrackTag(trackId, tagId, cycle, last);
}

/*
 * Purges all the allocated vectors of the track heap
 * that hasn't yet been deleted.
 *
 * @return void.
 */
void
DralDB::purgeTrackHeapAllocatedVectors()
{
    track_heap->purgeAllocatedVectors();
}

/**
 * Method to obtain the edge identifyed by the given number
 *
 * @return a pointer to the edge.
 */
DralGraphEdge *
DralDB::getEdge(DRAL_ID edgeId)
{
    return dbGraph->getEdge(edgeId);
}

/**
 * Function that normalizes a name.
 *
 * @return the normalized name.
 */
QString
DralDB::normalizeNodeName(QString name)
{
    return dbGraph->normalizeNodeName(name);
}

/**
 * Function that expand the name with the slot.
 *
 * @return the sloted name.
 */
QString
DralDB::slotedNodeName(QString name, NodeSlot nslot)
{
    return dbGraph->slotedNodeName(name, nslot);
}

/**
 * Function that decodes a node slot into a name and slot.
 *
 * @return if the name could be decoded.
 */
bool
DralDB::decodeNodeSlot(QString strnodeslot, QString* nodename, NodeSlot* nslot)
{
    return dbGraph->decodeNodeSlot(strnodeslot, nodename, nslot);
}

/**
 * Method to obtain the node identifyed by the given number
 *
 * @return a pointer to the node.
 */
DralGraphNode *
DralDB::getNode(DRAL_ID nodeId)
{
    return dbGraph->getNode(nodeId);
}

/**
 * Method to obtain the node identifyed by the given number
 *
 * @return a pointer to the node.
 */
DralGraphNode *
DralDB::getNode(QString name)
{
    return dbGraph->getNode(name);
}

/**
 * Finds the edge with name name that starts at the node fromstr
 * and ends at the node tostr.
 *
 * @return a pointer to the edge.
 */
DralGraphEdge *
DralDB::findEdgeByNameFromTo(QString name, QString fromstr, QString tostr)
{
    return dbGraph->findEdgeByNameFromTo(name, fromstr, tostr);
}

/** 
 * Method to obtain the edege identifyed by the given name. If there are
 * more than one edge with the same name, no edge is returned.
 *
 * @param name
 * @out edge NULL if no edge is found or if many edges with the same name is found
 * @return TRUE if an edge is found with the given name. FALSE otherwise
 */
bool 
DralDB::findEdgeByName(QString name, DralGraphEdge * &edge)
{
    return dbGraph->findEdgeByName(name, edge);
}

INT32 
DralDB::getBiggestLatency()
{
    return dbGraph->getBiggestLatency();
}

/*
 * Returns the number of clocks that are defined.
 *
 * @return the number of clocks.
 */
CLOCK_ID
DralDB::getNumClocks() const
{
    return clocks->getNumClocks();
}

/*
 * Finds the clock with id clockId.
 *
 * @return the clock domain.
 */
const ClockDomainEntry *
DralDB::getClockDomain(CLOCK_ID  clockId) const
{
    return clocks->getClockDomain(clockId);
}

/*
 * Returns a pointer to a clock domain iterator sorted by frequency.
 *
 * @return the iterator.
 */
ClockDomainFreqIterator
DralDB::getClockDomainFreqIterator() const
{
    return clocks->getFreqIterator();
}

/*
 * Finds the clock domain of the track trackId.
 *
 * @return the clock domain.
 */
CLOCK_ID
DralDB::getClockId(TRACK_ID trackId) const
{
    return track_heap->getClockId(trackId);
}

/*
 * Returns the track heap.
 *
 * @return track heap.
 */
TrackHeap *
DralDB::getTrackHeap() const
{
    return track_heap;
}

/*
 * Returns the item heap.
 *
 * @return item heap.
 */
ItemHeap *
DralDB::getItemHeap() const
{
    return item_heap;
}

#endif

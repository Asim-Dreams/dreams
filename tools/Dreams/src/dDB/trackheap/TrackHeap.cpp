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
  * @file  TrackHeap.cpp
  * @brief
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/trackheap/TrackHeap.h"

// Qt includes.
#include <qprogressdialog.h>

/**
 * Creator of this class. Creates all the hashes needed and copies the
 * parameters.
 *
 * @return new object.
 */
TrackHeap::TrackHeap(ItemHeap * _itemdb, DralGraph * _dbgraph, TagDescVector * _tagdescvec, Dict2064 * _dict, StrTable * _strtbl)
{
    nextTrackID = 0;

    dbgraph = _dbgraph;
    tagdescvec = _tagdescvec;
    slotItemId = tagdescvec->getSlotItemTagId();
    itemdb = _itemdb;
    dict = _dict;
    strtbl = _strtbl;
    renames.setAutoDelete(true);
    firstEffectiveCycle = NULL;
    nextTrackID = 0;
    trackIDVector.clear();
    renames.clear();
    clock_ranges.clear();

    // No track for the edges, nodes and cycles.
    for(INT32 i = 0; i < 65536; i++)
    {
        edgeTrackTbl[i] = TRACK_ID_INVALID;
        nodeTrackTbl[i] = TRACK_ID_INVALID;
        cycleTrackTbl[i] = TRACK_ID_INVALID;
    }
}

/**
 * Destructor of this class. Deletes the hashes.
 *
 * @return destroys the object.
 */
TrackHeap::~TrackHeap()
{
    if(firstEffectiveCycle != NULL)
    {
        delete [] firstEffectiveCycle;
    }
}

/**
 * Tracks the slot nslot of the node node_id.
 *
 * @return the trackid.
 */
TRACK_ID
TrackHeap::trackNodeTags(DRAL_ID node_id)
{
    // get the node
    DralGraphNode * gnode = dbgraph->getNode(node_id);
    if(gnode == NULL)
    {
        return TRACK_ID_INVALID;
    }

    TRACK_ID result = nodeTrackTbl[node_id];

    if(result != TRACK_ID_INVALID)
    {
        return result;
    }

    // Allocates the whole node slots.
    result = nextTrackID++;
    nodeTrackTbl[node_id] = result;

    TagRenameTable * trt;

    trt = new TagRenameTable(2);
    renames.append(trt);

    // Allocates the whole node track.
    trackIDVector.allocateElement(result);
    trackIDVector[result].dict = dict;
    trackIDVector[result].tagdescvec = tagdescvec;
    trackIDVector[result].trt = trt;
    trackIDVector[result].clockId = gnode->getClockId();

    NodeSlot slot(gnode->getDimensions());

    trackNodeTags(node_id, gnode->getClockId(), trt, gnode->getLayout(), &slot, 0);

    // Creates the track list of clock ids.
    newTrackEntry(gnode->getClockId(), result, nextTrackID);

    return result;
}

void
TrackHeap::trackNodeTags(DRAL_ID node_id, CLOCK_ID clock_id, TagRenameTable * trt, NodeSlot * base, NodeSlot * act, UINT16 depth)
{
    if(depth == act->dimensions)
    {
        // Base case, allocates a new slot.
        INT32 result = nextTrackID++;
        trackIDVector.allocateElement(result);
        trackIDVector[result].dict = dict;
        trackIDVector[result].tagdescvec = tagdescvec;
        trackIDVector[result].trt = trt;
        trackIDVector[result].clockId = clock_id;
    }
    else
    {
        // We are not at the lowest level. Recursive calls.
        for(UINT32 i = 0; i < base->capacities[depth]; i++)
        {
            act->capacities[depth] = i;
            trackNodeTags(node_id, clock_id, trt, base, act, depth + 1);
        }
    }
}

/**
 * Tracks the move items in the edge edge_id.
 *
 * @return the trackid assigned to the edge. -1 if any error is
 * found.
 */
TRACK_ID
TrackHeap::trackEdgeTags(DRAL_ID edge_id)
{
    DralGraphEdge * edge = dbgraph->getEdge(edge_id);

    if(edge == NULL)
    {
        return TRACK_ID_INVALID;
    }

    TRACK_ID result;

    // check whether this node_id/slot is already been tracked
    result = edgeTrackTbl[edge_id];
    if(result != TRACK_ID_INVALID)
    {
        return result;
    }

    // get a new track entry
    result = nextTrackID;

    // update the tbl
    edgeTrackTbl[edge_id] = result;
    TagRenameTable * trt;

    trt = new TagRenameTable(2);
    renames.append(trt);

    // Prevents of edges with latencies or bandwidths of 0.
    UINT32 bandwidth = QMAX(edge->getBandwidth(), 1);
    UINT32 latency = QMAX(edge->getLatency(), 1);

    for(UINT32 i = 0; i < bandwidth; i++)
    {
        for(UINT32 j = 0; j < latency; j++)
        {
            trackIDVector.allocateElement(nextTrackID);
            trackIDVector[nextTrackID].dict = dict;
            trackIDVector[nextTrackID].tagdescvec = tagdescvec;
            trackIDVector[nextTrackID].trt = trt;
            trackIDVector[nextTrackID].clockId = edge->getClockId();
            nextTrackID++;
        }
    }

    // Creates the track list of clock ids.
    newTrackEntry(edge->getClockId(), result, nextTrackID);

    return result;
}

/**
 * Tracks the cycle tags.
 *
 * @return the trackid.
 */
TRACK_ID
TrackHeap::trackCycleTags(CLOCK_ID clockId)
{
    TRACK_ID result;

    // check whether this node_id/slot is already been tracked
    result = cycleTrackTbl[clockId];
    if(result != TRACK_ID_INVALID)
    {
        return result;
    }

    result = nextTrackID++;
    trackIDVector.allocateElement(result);
    trackIDVector[result].dict = dict;
    trackIDVector[result].tagdescvec = tagdescvec;
    trackIDVector[result].trt = new TagRenameTable(2);
    trackIDVector[result].clockId = clockId;
    renames.append(trackIDVector[result].trt);
    cycleTrackTbl[clockId] = result;

    // Creates the track list of clock ids.
    newTrackEntry(clockId, result, nextTrackID);

    return result;
}

/*
 * Gets the value of the tag tagId of the track trackId in the
 * cycle cycle. The call is forwarded to the correct trackIdNode.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle)
{
    CLOCK_ID clockId; ///< Clock id of the cycle.
    bool ok;          ///< If a value is found.

    clockId = cycle.clock->getId();
    cycle.cycle -= firstEffectiveCycle[clockId];
    Q_ASSERT(trackId < nextTrackID);
    Q_ASSERT(trackIDVector[trackId].getClockId() == clockId);

    ok = trackIDVector[trackId].getTagValue(tagId, cycle, value, atcycle);

    // Sets the atcycle if exists.
    if(ok && (atcycle != NULL))
    {
        atcycle->cycle += firstEffectiveCycle[clockId];
    }

    return ok;
}

/*
 * Gets the value of the tag tagId of the track trackId in the
 * cycle cycle. The call is forwarded to the correct trackIdNode.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * value, CYCLE * atcycle)
{
    UINT64 temp; ///< Where the value of the internal representation of the string is stored.
    CLOCK_ID clockId; ///< Clock id of the cycle.
    bool ok;          ///< If a value is found.

    clockId = cycle.clock->getId();
    cycle.cycle -= firstEffectiveCycle[clockId];
    Q_ASSERT(trackId < nextTrackID);
    Q_ASSERT(trackIDVector[trackId].getClockId() == clockId);

    ok = trackIDVector[trackId].getTagValue(tagId, cycle, &temp, atcycle);

    * value = strtbl->getString(temp);

    // Sets the atcycle if exists.
    if(ok && (atcycle != NULL))
    {
        atcycle->cycle += firstEffectiveCycle[clockId];
    }

    return ok;
}

/*
 * Gets the value of the tag tagId of the track trackId in the
 * cycle cycle. The call is forwarded to the correct trackIdNode.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle)
{
    CLOCK_ID clockId; ///< Clock id of the cycle.
    bool ok;          ///< If a value is found.

    clockId = cycle.clock->getId();
    cycle.cycle -= firstEffectiveCycle[clockId];
    Q_ASSERT(trackId < nextTrackID);
    Q_ASSERT(trackIDVector[trackId].getClockId() == clockId);

    ok = trackIDVector[trackId].getTagValue(tagId, cycle, value, atcycle);

    // Sets the atcycle if exists.
    if(ok && (atcycle != NULL))
    {
        atcycle->cycle += firstEffectiveCycle[clockId];
    }

    return ok;
}

/*
 * Gets the value of the tag tagId of the track trackId in the
 * cycle cycle. The call is forwarded to the correct trackIdNode.
 * Only looks at the values set in the same phase than cycle.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle)
{
    CLOCK_ID clockId; ///< Clock id of the cycle.
    bool ok;          ///< If a value is found.

    clockId = cycle.clock->getId();
    cycle.cycle -= firstEffectiveCycle[clockId];
    Q_ASSERT(trackId < nextTrackID);
    Q_ASSERT(trackIDVector[trackId].getClockId() == clockId);

    ok = trackIDVector[trackId].getTagValuePhase(tagId, cycle, value, atcycle);

    // Sets the atcycle if exists.
    if(ok && (atcycle != NULL))
    {
        atcycle->cycle += firstEffectiveCycle[clockId];
    }

    return ok;
}

/*
 * Gets the value of the tag tagId of the track trackId in the
 * cycle cycle. The call is forwarded to the correct trackIdNode.
 * Only looks at the values set in the same phase than cycle.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * value, CYCLE * atcycle)
{
    CLOCK_ID clockId; ///< Clock id of the cycle.
    bool ok;          ///< If a value is found.

    clockId = cycle.clock->getId();
    cycle.cycle -= firstEffectiveCycle[clockId];
    Q_ASSERT(trackId < nextTrackID);
    Q_ASSERT(trackIDVector[trackId].getClockId() == clockId);

    UINT64 temp; ///< Where the value of the internal representation of the string is stored.

    ok = trackIDVector[trackId].getTagValuePhase(tagId, cycle, &temp, atcycle);
    * value = strtbl->getString(temp);

    // Sets the atcycle if exists.
    if(ok && (atcycle != NULL))
    {
        atcycle->cycle += firstEffectiveCycle[clockId];
    }

    return ok;
}

/*
 * Gets the value of the tag tagId of the track trackId in the
 * cycle cycle. The call is forwarded to the correct trackIdNode.
 * Only looks at the values set in the same phase than cycle.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle)
{
    CLOCK_ID clockId; ///< Clock id of the cycle.
    bool ok;          ///< If a value is found.

    clockId = cycle.clock->getId();
    cycle.cycle -= firstEffectiveCycle[clockId];
    Q_ASSERT(trackId < nextTrackID);
    Q_ASSERT(trackIDVector[trackId].getClockId() == clockId);

    ok = trackIDVector[trackId].getTagValuePhase(tagId, cycle, value, atcycle);

    // Sets the atcycle if exists.
    if(ok && (atcycle != NULL))
    {
        atcycle->cycle += firstEffectiveCycle[clockId];
    }

    return ok;
}

/*
 * Gets the formatted value of the tag tagId of the track trackId
 * in the cycle cycle. The call is forwarded to the correct
 * trackIdNode.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getFormatedTagValue(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle)
{
    bool qok;
    UINT64 ivalue;

    TagValueType type = tagdescvec->getTagValueType(tagId);
    switch(type)
    {
        case TAG_INTEGER_TYPE:
            qok = getTagValue(trackId, tagId, cycle, &ivalue, atcycle);
            if (!qok)
            {
                return false;
            }
            * fmtvalue = tagdescvec->getFormatedTagValue(tagId, ivalue);
            return true;
            break;

        case TAG_STRING_TYPE:
            return getTagValue(trackId, tagId, cycle, fmtvalue, atcycle);
            break;

        case TAG_SOV_TYPE:
            //SOVList lvalue;
            //qok = getTagValue(trackId, tagId, cycle, lvalue);
            break;

        default:
            return false;
    }
    return false;
}

/*
 * Gets the formatted value of the tag tagId of the track trackId
 * in the cycle cycle. The call is forwarded to the correct
 * trackIdNode.
 *
 * @return true if a value is found.
 */
bool
TrackHeap::getFormatedTagValuePhase(TRACK_ID trackId, TAG_ID tagId, CYCLE cycle, QString * fmtvalue, CYCLE * atcycle)
{
    bool qok;
    UINT64 ivalue;

    TagValueType type = tagdescvec->getTagValueType(tagId);
    switch(type)
    {
        case TAG_INTEGER_TYPE:
            qok = getTagValuePhase(trackId, tagId, cycle, &ivalue, atcycle);
            if (!qok)
            {
                return false;
            }
            * fmtvalue = tagdescvec->getFormatedTagValue(tagId, ivalue);
            return true;
            break;

        case TAG_STRING_TYPE:
            return getTagValuePhase(trackId, tagId, cycle, fmtvalue, atcycle);
            break;

        case TAG_SOV_TYPE:
            //SOVList lvalue;
            //qok = getTagValue(trackId, tagId, cycle, lvalue);
            break;

        default:
            return false;
    }
    return false;
}

/**
 * Adds a move of the item itemIdx in the cycle cycle of the in
 * the position pos of the edge edgeid.
 *
 * @return if a move item is added.
 */
bool
TrackHeap::addMoveItem(DRAL_ID edgeid, ITEM_ID itemid, UINT16 pos, CYCLE cycle)
{
    UINT64 value;
    TRACK_ID trackid = edgeTrackTbl[edgeid];

    // The edge must exist, else the track id would be -1.
    DralGraphEdge * edge = dbgraph->getEdge(edgeid);

    TAG_ID disp;      ///< Displace added to the tagId if the cycle is in the second phase.
    CLOCK_ID clockId; ///< Clock id of the cycle

    clockId = cycle.clock->getId();

    if(trackIDVector[trackid].getClockId() != clockId)
    {
        return false;
    }

    // Adjust by position.
    trackid += pos;

    bool ok;

    cycle.cycle -= firstEffectiveCycle[clockId];
    ok = trackIDVector[trackid].addTagValue(slotItemId, cycle, (UINT64) itemid);
    cycle.cycle++;
    ok = ok && trackIDVector[trackid].undefineTagValue(slotItemId, cycle);
    return ok;
}

/*
 * Initializes a tag handler to iterate along the tag tag_id of
 * the track track_id.
 *
 * @return void.
 */
bool
TrackHeap::initTagHandler(TagHandler ** hnd, TRACK_ID track_id, TAG_ID tag_id, CYCLE cycle, bool start)
{
    TrackIDNode * track; ///< Pointer to the track.

    Q_ASSERT((track_id != TRACK_ID_INVALID) && (track_id < nextTrackID));

    // Gets the track.
    track = trackIDVector.ptr(track_id);

    bool hasHigh;
    bool hasLow;

    if(track->trt->isRenamed(tag_id))
    {
        TagIdVecNode * vec;
        
        vec = track->tgIdVector[track->trt->rename(tag_id)];
        hasHigh = (vec != NULL) && vec->hasHighData();
        hasLow = (vec != NULL) && vec->hasLowData();
    }
    else
    {
        hasHigh = false;
        hasLow = false;
    }

    if(!hasHigh || !hasLow)
    {
        // Values are stored in just one phase or none of them.
        InternalTagHandler * myHnd;

        // Creates a single handler.
        // The phase is 1 if the low phase has values.
        myHnd = new InternalTagHandler(cycle.clock, hasLow);
        * hnd = myHnd;

        // Initializes it.
        myHnd->track_id = track_id;
        myHnd->tag_id = tag_id;

        if(!hasHigh && !hasLow)
        {
            // Creates a void tag handler.
            myHnd->init(NULL, firstEffectiveCycle[track->getClockId()], hasLow);
        }
        else
        {
            // Creates a tag handler with the correct tag vector.
            myHnd->init(track->tgIdVector[track->trt->rename(tag_id)], firstEffectiveCycle[track->getClockId()], hasLow);
        }
    }
    else
    {
        // Values defined in both phases.
        DualTagHandler * myHnd;

        // Creates a dualhandler.
        myHnd = new DualTagHandler(cycle.clock);
        * hnd = myHnd;

        // Initializes the high and low tag handlers.
        myHnd->tagHigh->track_id = track_id;
        myHnd->tagHigh->tag_id = tag_id;
        myHnd->tagLow->track_id = track_id;
        myHnd->tagLow->tag_id = tag_id;
        myHnd->tagHigh->init(track->tgIdVector[track->trt->rename(tag_id)], firstEffectiveCycle[track->getClockId()], 0);
        myHnd->tagLow->init(track->tgIdVector[track->trt->rename(tag_id)], firstEffectiveCycle[track->getClockId()], 1);
    }

    if(start)
    {
        // Points the handler to the cycle 0.
        (* hnd)->rewindToFirstCycle();
    }
    else
    {
        // Points the handler to the desired cycle.
        Q_ASSERT(cycle.clock->getId() == track->getClockId());

        (* hnd)->skipToCycleDefined(cycle);
    }
    return true;
}

/*
 * Initializes a tag handler to iterate along the tag tag_id of
 * the track track_id. The tag handler created just returns values
 * for the same phase than cycle.
 *
 * @return void.
 */
bool
TrackHeap::initTagHandlerPhase(TagHandler ** hnd, TRACK_ID track_id, TAG_ID tag_id, CYCLE cycle, bool start)
{
    TrackIDNode * track; ///< Pointer to the track.

    Q_ASSERT((track_id != TRACK_ID_INVALID) && (track_id < nextTrackID));

    // Gets the track.
    track = trackIDVector.ptr(track_id);

    // Values are stored in just one phase or none of them.
    InternalTagHandler * myHnd;

    // Creates a single handler.
    // The phase is 1 if the low phase has values.
    myHnd = new InternalTagHandler(cycle.clock, cycle.division);
    * hnd = myHnd;

    // Initializes it.
    myHnd->track_id = track_id;
    myHnd->tag_id = tag_id;

    bool found = false;

    if(track->trt->isRenamed(tag_id))
    {
        TagIdVecNode * vec;

        vec = track->tgIdVector[track->trt->rename(tag_id)];
        if(vec != NULL)
        {
            if(cycle.division == 0)
            {
                if(vec->hasHighData())
                {
                    // Creates a tag handler with the high tag vector.
                    myHnd->init(vec, firstEffectiveCycle[track->getClockId()], 0);
                    found = true;
                }
            }
            else
            {
                if(vec->hasLowData())
                {
                    // Creates a tag handler with the high tag vector.
                    myHnd->init(vec, firstEffectiveCycle[track->getClockId()], 1);
                    found = true;
                }
            }
        }
    }

    if(!found)
    {
        // Nothing found, so intialized with void contents.
        myHnd->init(NULL, firstEffectiveCycle[track->getClockId()], 0);
    }

    if(start)
    {
        // Points the handler to the cycle 0.
        myHnd->rewindToFirstCycle();
    }
    else
    {
        // Points the handler to the desired cycle.
        Q_ASSERT(cycle.clock->getId() == track->getClockId());

        myHnd->skipToCycleDefined(cycle);
    }
    return true;
}

/**
 * Dumps the content of the track heap.
 *
 * @return void.
 */
void
TrackHeap::dumpRegression(bool gui_enabled)
{
    char str[256];              ///< Buffer used to store intermadiate dumps.
    QProgressDialog * progress; ///< Progress dialog to show the evolution of the dumping.
    QLabel * label;             ///< Label of the progress dialog.
    UINT32 offset;              ///< Number of cycles between progress dialog updates.

    offset = 0;
    progress = NULL;
    label = NULL;

    if(gui_enabled)
    {
        progress = new QProgressDialog("Dreams is dumping the track database...", 0, nextTrackID, NULL, "commitprogress", TRUE);

        label = new QLabel(QString("\nDreams is dumping the track database...               ") +
                           QString("\nTracks                              : 0"), progress);
        Q_ASSERT(label != NULL);
        label->setMargin(5);
        label->setAlignment(Qt::AlignLeft);
        //label->setFont(QFont("Courier", qApp->font().pointSize(), QFont::Bold));
        progress->setLabel(label);

        offset = nextTrackID / 100;

        // Sanity check.
        if(offset == 0)
        {
            offset++;
        }
    }

    for(TRACK_ID i = 0; i < nextTrackID; i++)
    {
        sprintf(str, FMT32X, i);
        printf("TR=%s\n", str);
        trackIDVector[i].dumpRegression();
        if(gui_enabled)
        {
            if((i % offset) == (offset - 1))
            {
                progress->setProgress(i);
                label->setText(QString("\nDreams is dumping the track database...               ") +
                               QString("\nTracks                              : ") + QString::number(i));
            }
        }
    }

    if(gui_enabled)
    {
        delete progress;
    }
}

/**
 * Computes the size of this object.
 *
 * @return the size of this object.
 */
INT64
TrackHeap::getObjSize() const
{
    INT64 ret;

    ret = sizeof(TrackHeap);

    // Adds all the allocated space by the AEVector.
    ret += trackIDVector.getObjSize() - sizeof(trackIDVector);

    // Adds all the tracks.
    INT32 i = 0;
    while(trackIDVector.hasElement(i))
    {
        ret += trackIDVector[i].getObjSize() - sizeof(TrackIDNode);
        i++;
    }

    QPtrListIterator<TagRenameTable> it_trt(renames);
    TagRenameTable * trt;

    // Adds the renaming tables.
    while((trt = it_trt.current()) != NULL)
    {
        ++it_trt;
        ret += trt->getObjSize();
    }

    // Internal lists.
    return ret;
}

/**
 * Creates a string with the description of this object.
 *
 * @return the description of this object.
 */
QString
TrackHeap::getUsageDescription() const
{
    QString ret;
    INT64 size;

    ret = QString("\tTrackHeap object occupies ") + QString::number(sizeof(TrackHeap)) + QString(" bytes.\n");
    ret += QString("\tTrackIDNode object occupies ") + QString::number(sizeof(TrackIDNode)) + QString(" bytes.\n");
    ret += QString("\tTagIdVecNode object occupies ") + QString::number(sizeof(TagIdVecNode)) + QString(" bytes.\n");
    ret += QString("\tWhole TrackHeap object occupies ") + QString::number(getObjSize()) + QString(" bytes.\n");
    ret += QString("\tDescription of the different tracked elements (") + QString::number(trackIDVector.getNumSegments() * trackIDVector.getSegmentSize()) + QString(" elements allocated with a total size of ");

    // Adds all the tracks.
    INT32 i = 0;
    size = 0;
    while(trackIDVector.hasElement(i))
    {
        size += trackIDVector[i].getObjSize();
        i++;
    }

    ret += QString::number(size) + QString(" bytes).\n");

/*    // Prints the descriptions.
    i = 0;
    while(trackIDVector.hasElement(i))
    {
        ret += QString("\t\t") + QString::number(i) + QString(")\n");
        ret += trackIDVector[i].getUsageDescription();
        i++;
    }*/
    return ret;
}

/**
 * Creates a string with the stats of this object.
 *
 * @return the stats of this object.
 */
QString
TrackHeap::getStats() const
{
    return "";
}

/**
 * Compresses the track id vector.
 *
 * @return itself.
 */
ZipObject *
TrackHeap::compressYourSelf(CYCLE cycle, bool last)
{
    list<ClockRangeEntry>::iterator it_clk; ///< Clock range iterator.
    CLOCK_ID clockId;                       ///< Clock id that is being compressed.
    CYCLE temp = cycle;                     ///< Temporal cycle.

    // Gets the clock id and clock range iterator.
    clockId = cycle.clock->getId();
    it_clk = clock_ranges.begin();

    // Substracts the first effective cycle.
    temp.cycle -= firstEffectiveCycle[clockId];

    // Iterates along all the clock ranges.
    while(it_clk != clock_ranges.end())
    {
        // If this range has the same clock id.
        if(it_clk->clockId == clockId)
        {
            // Compresses all the elements of the range.
            for(TRACK_ID i = it_clk->first; i < it_clk->last; i++)
            {
                trackIDVector[i].compressYourSelf(temp, last);
            }
        }
        it_clk++;
    }

    // Always returns itself.
    return this;
}

/*
 * Adds a clock range. Tries to collapse the last entry.
 *
 * @return void.
 */
void
TrackHeap::newTrackEntry(CLOCK_ID clockId, TRACK_ID first, TRACK_ID last)
{
    list<ClockRangeEntry>::iterator it_clk;

    // Tries to collapse with the last entry.
    if((clock_ranges.size() > 0) && (clock_ranges.back().clockId == clockId))
    {
        clock_ranges.back().last = last;
    }
    else
    {
        ClockRangeEntry entry;

        // Adds a new entry.
        entry.clockId = clockId;
        entry.first = first;
        entry.last = last;
        clock_ranges.push_back(entry);
    }
}

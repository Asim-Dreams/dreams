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
  * @file ItemHeap.cpp
  */

// General includes.
#include <iostream>
#include <set>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/StrTable.h"
#include "dDB/PrimeList.h"
#include "dDB/DBItoa.h"
#include "dDB/LogMgr.h"
#include "dDB/DBConfig.h"
#include "dDB/itemheap/ItemHeap.h"

// Qt includes.
#include <qprogressdialog.h>

/**
 * Creator of this class. Just calls the init function.
 *
 * @return new object.
 */
ItemHeap::ItemHeap(TagDescVector * _tdv, Dict2064 * _dict, DBConfig * _conf, StrTable * _strtbl, ClockDomainMgr * _clocks)
{
    myLogMgr = LogMgr::getInstance();
    tdv = _tdv;
    itemid_tagid = tdv->getItemIdTagId();
    newitem_tagid = tdv->getNewItemTagId();
    delitem_tagid = tdv->getDelItemTagId();

    dralVersion = 9999999;
    dict = _dict;
    conf = _conf;
    strtbl = _strtbl;
    clocks = _clocks;

    firstEffectiveCycle = NULL;
    nextEventVectorEntry = ITEM_UNUSED + 1;
    firstItem = 0x7FFFFFFF;
    lastItem = 0;
    numItems = 0;
    numTags = 0;
    numMoveItems = 0;
    numEnterNodes = 0;
    numExitNodes = 0;
    numGenEvents = 0;
}

/**
 * Destructor of this class. Deletes the created objects.
 *
 * @return destroys the object.
 */
ItemHeap::~ItemHeap()
{
    if(firstEffectiveCycle != NULL)
    {
        delete [] firstEffectiveCycle;
    }
}

/*
 * This function inserts the itemId in the database.
 *
 * @return if the item id is valid.
 */
bool
ItemHeap::newItemBlock(ITEM_ID itemId, CYCLE cycle)
{
    // Checks that the id is within the bounds.
    // TODO: change < to <= ??
    if((itemId >= ITEM_MAX_ENTRIES) || (itemId == ITEM_ID_INVALID))
    {
        return false;
    }

    // Updates the bounds of the item data base.
    lastItem = QMAX(itemId, lastItem);
    firstItem = QMIN(itemId, firstItem);

    // Marks that the item exists.
    itemVector[itemId] = ITEM_OPEN;
    numItems++;

    // Inserts the creation cycle.
    newTag(itemId, newitem_tagid, (UINT64) 0, cycle);

    // Everything is succesful.
    return true;
}

/*
 * Inserts a SOV for the tag tag_id to the item item_id.
 *
 * @return void.
 */
bool
ItemHeap::newTag(ITEM_ID item_id, TAG_ID tag_id, SOVList * list, CYCLE cycle)
{
    ItemHandler handler(false); ///< Used to store the new tags.
    ItemHeapTagNode * tag_node; ///< Pointer used to store the values.
    CLOCK_ID clockId;           ///< Clock id of the cycle.

    Q_ASSERT(tag_id <= MAX_TAG_ID);

    // Gets a handler for the item.
    handler.item_id = item_id;
    newTagLookForItemId(&handler);

    // If the item doesn't exist then we return an error.
    if(!handler.valid_item)
    {
        return false;
    }

    clockId = cycle.clock->getInternalClock();

    // Each value of the set of values is added.
    for(SOVList::iterator it = list->begin(); it != list->end(); it++)
    {
        // Allocates a new slot for the value.
        tag_node = (ItemHeapTagNode *) allocateNewSlot(&handler);

        // Sets the tag.
        tag_node->type = IDB_TAG;
        tag_node->clockId = clockId;
        tag_node->cycle = cycle.cycle - firstEffectiveCycle[clockId];
        tag_node->division = cycle.division;
        tag_node->dkey = dict->getKeyFor(* it, cycle.clock->getId());
        tag_node->tagId = tag_id;
        tag_node->isSOV = 1;
        tag_node->isMutable = 0;

        // Update statistics.
        numTags++;
    }
    return true;
}

/*
 * Inserts a new value for the tag tag_id to the item item_id.
 *
 * @return void.
 */
bool
ItemHeap::newTag(ITEM_ID item_id, TAG_ID tag_id, UINT64 value, CYCLE cycle)
{
    ItemHandler handler(false);  ///< Used to store the new tags.
    ItemHeapTagNode * last_node; ///< Points to the last value of the current tag.
    ItemHeapTagNode * tag_node;  ///< Pointer used to store the values.
    UINT16 entries;              ///< Counts the number of entries for the actual tag. If not zero, then the tag is mutable.
    UINT32 int_cycle;            ///< Internal cycle storage (cycle - firstEffectiveCycle).
    CLOCK_ID clockId;            ///< Clock id of the cycle.

    Q_ASSERT(tag_id <= MAX_TAG_ID);

    // Gets a handler for the item.
    handler.item_id = item_id;
    newTagLookForItemId(&handler);

    // If the item doesn't exist then we return an error.
    if(!handler.valid_item)
    {
        return false;
    }

    clockId = cycle.clock->getInternalClock();

    // Starts with 0 entries for the tag.
    last_node = NULL;
    entries = 0;
    int_cycle = cycle.cycle - firstEffectiveCycle[clockId];

    // Tries to find an old value for the tag.
    // Skips the ITEMID in the first evaluation.
    while(nextTag(&handler))
    {
        // The first time the tag is found the loop ends.
        if((TAG_ID) tagNode(&handler)->tagId == tag_id)
        {
            // Saves the pointer to the last entry of the tag and marks the entry as mutable.
            last_node = tagNode(&handler);
            entries++;
            tagNode(&handler)->isMutable = true;
        }
    }

    // Checks if we can recycle the last entry.
    if(entries == 1)
    {
        // Case that we only have one entry and is mutable tag must be cleared to maintain coherency.
        // First if the value is set in the same cycle, just overwrite it.
        if((int_cycle == last_node->cycle) && (last_node->clockId == clockId))
        {
            last_node->dkey = dict->getKeyFor(value, cycle.clock->getId());
            last_node->isMutable = false;
            return true;
        }
        // If the compress flag is set and the value is the same, just mark the tag as not mutable.
        else if(conf->getCompressMutable() && (dict->getValueByCycle(last_node->dkey, last_node->cycle, clocks->internalToExternalClock(last_node->clockId)) == value))
        {
            last_node->isMutable = false;
            return true;
        }
        // We must create a new mutable entry.
        else
        {
            tag_node = (ItemHeapTagNode *) allocateNewSlot(&handler);
            tag_node->isMutable = true;
        }
    }
    else if(entries > 1)
    {
        // The same as the previous, but the mutable tag is not cleared.
        // First if the value is set in the same cycle, just overwrite it.
        if((int_cycle == last_node->cycle) && (last_node->clockId == clockId))
        {
            last_node->dkey = dict->getKeyFor(value, cycle.clock->getId());
            return true;
        }
        // If the compress flag is set and the value is the same, just left the entry as is.
        else if(conf->getCompressMutable() && (dict->getValueByCycle(last_node->dkey, last_node->cycle, clocks->internalToExternalClock(last_node->clockId)) == value))
        {
            return true;
        }
        // We must create a new mutable entry.
        else
        {
            tag_node = (ItemHeapTagNode *) allocateNewSlot(&handler);
            tag_node->isMutable = true;
        }
    }
    else
    {
        // Non-mutable tag case.
        // Just allocates a new slot.
        tag_node = (ItemHeapTagNode *) allocateNewSlot(&handler);
        tag_node->isMutable = false;
    }

    // Fills the data.
    tag_node->type = IDB_TAG;
    tag_node->dkey = dict->getKeyFor(value, cycle.clock->getId());
    tag_node->clockId = clockId;
    tag_node->cycle = int_cycle;
    tag_node->division = cycle.division;
    tag_node->tagId = tag_id;
    tag_node->isSOV = false;

    // Update statistics.
    numTags++;

    return true;
}

/**
 * Inserts a new move item to the item item_id.
 *
 * @return void.
 */
bool
ItemHeap::newMoveItem(ITEM_ID item_id, DRAL_ID edge_id, UINT32 position, CYCLE cycle)
{
    ItemHandler handler(false);   ///< Used to store the new event.
    ItemHeapMoveNode * move_node; ///< Pointer used to store the values.
    CLOCK_ID clockId;             ///< Clock id of the cycle.

    // TODO: implement escape nodes.
    Q_ASSERT(position <= ITEM_MAX_POSITION);
    Q_ASSERT(edge_id <= NODE_MAX_VALUE);

    // Gets a handler for the item.
    handler.item_id = item_id;
    newEventLookForItemId(&handler);

    // If the item doesn't exist then we return an error.
    if(!handler.valid_item)
    {
        return false;
    }

    // Just allocates a new slot.
    move_node = (ItemHeapMoveNode *) allocateNewSlot(&handler);
    clockId = cycle.clock->getInternalClock();

    // Fills the data.
    move_node->type = IDB_EVENT;
    move_node->eventType = IDB_MOVE_ITEM;
    move_node->clockId = clockId;
    move_node->cycle = cycle.cycle - firstEffectiveCycle[clockId];
    move_node->division = cycle.division;
    move_node->node_edge = edge_id;
    move_node->pos = position;

    // Update statistics.
    numMoveItems++;

    return true;
}

/**
 * Inserts a new enter node to the item item_id.
 *
 * @return void.
 */
bool
ItemHeap::newEnterNode(ITEM_ID item_id, DRAL_ID node_id, UINT32 position, CYCLE cycle)
{
    ItemHandler handler(false);   ///< Used to store the new event.
    ItemHeapMoveNode * move_node; ///< Pointer used to store the values.
    CLOCK_ID clockId;             ///< Clock id of the cycle.

    // TODO: implement escape nodes.
    Q_ASSERT(position <= ITEM_MAX_POSITION);
    Q_ASSERT(node_id <= NODE_MAX_VALUE);

    // Gets a handler for the item.
    handler.item_id = item_id;
    newEventLookForItemId(&handler);

    // If the item doesn't exist then we return an error.
    if(!handler.valid_item)
    {
        return false;
    }

    // Just allocates a new slot.
    move_node = (ItemHeapMoveNode *) allocateNewSlot(&handler);
    clockId = cycle.clock->getInternalClock();

    // Fills the data.
    move_node->type = IDB_EVENT;
    move_node->eventType = IDB_ENTER_NODE;
    move_node->clockId = clockId;
    move_node->cycle = cycle.cycle - firstEffectiveCycle[clockId];
    move_node->division = cycle.division;
    move_node->node_edge = node_id;
    move_node->pos = position;

    // Update statistics.
    numEnterNodes++;

    return true;
}

/**
 * Inserts a new exit node to the item item_id.
 *
 * @return void.
 */
bool
ItemHeap::newExitNode(ITEM_ID item_id, DRAL_ID node_id, UINT32 position, CYCLE cycle)
{
    ItemHandler handler(false);   ///< Used to store the new event.
    ItemHeapMoveNode * move_node; ///< Pointer used to store the values.
    CLOCK_ID clockId;             ///< Clock id of the cycle.

    // TODO: implement escape nodes.
    Q_ASSERT(position <= ITEM_MAX_POSITION);
    Q_ASSERT(node_id <= NODE_MAX_VALUE);

    // Gets a handler for the item.
    handler.item_id = item_id;
    newEventLookForItemId(&handler);

    // If the item doesn't exist then we return an error.
    if(!handler.valid_item)
    {
        return false;
    }

    // Just allocates a new slot.
    move_node = (ItemHeapMoveNode *) allocateNewSlot(&handler);
    clockId = cycle.clock->getInternalClock();

    // Fills the data.
    move_node->type = IDB_EVENT;
    move_node->eventType = IDB_EXIT_NODE;
    move_node->clockId = clockId;
    move_node->cycle = cycle.cycle - firstEffectiveCycle[clockId];
    move_node->division = cycle.division;
    move_node->node_edge = node_id;
    move_node->pos = position;

    // Update statistics.
    numExitNodes++;

    return true;
}

/**
 * Inserts a new generic event to the item item_id.
 *
 * @return void.
 */
bool
ItemHeap::newGenericEvent(ITEM_ID item_id, UINT64 event, CYCLE cycle)
{
    ItemHandler handler(false);     ///< Used to store the new event.
    ItemHeapEventNode * event_node; ///< Pointer used to store the values.
    CLOCK_ID clockId;               ///< Clock id of the cycle.

    // TODO: implement escape nodes.
    Q_ASSERT(event <= ITEM_MAX_EVENT);

    // Gets a handler for the item.
    handler.item_id = item_id;
    newEventLookForItemId(&handler);

    // If the item doesn't exist then we return an error.
    if(!handler.valid_item)
    {
        return false;
    }

    // Just allocates a new slot.
    event_node = (ItemHeapEventNode *) allocateNewSlot(&handler);
    clockId = cycle.clock->getInternalClock();

    // Fills the data.
    event_node->type = IDB_EVENT;
    event_node->eventType = IDB_GEN_EVENT;
    event_node->clockId = clockId;
    event_node->cycle = cycle.cycle - firstEffectiveCycle[clockId];
    event_node->division = cycle.division;
    event_node->event_id = event;

    // Update statistics.
    numGenEvents++;

    return true;
}

/**
 * Just modifies the handler to point the next item.
 *
 * @return if has another item.
 */
bool
ItemHeap::nextItem(ItemHandler * handler)
{
    if(handler->item_id >= lastItem)
    {
        // If we were in the last item then it gets invalid.
        handler->invalid();
        return false;
    }
    else
    {
        // We must find the next valid item.
        bool found = false; ///< Used as an en of loop condition.

        // Loops until the next item.
        while(!found)
        {
            // Increments the item id.
            handler->item_id++;

            Q_ASSERT(handler->item_id < ITEM_MAX_ENTRIES);

            // TODO: fasten up the search by incrementing by chunk size if no element is found!!
            if(itemVector.hasElement(handler->item_id) && (itemVector.at(handler->item_id) != ITEM_UNUSED))
            {
                found = true;
            }
        }
        resetTagState(handler);
        resetEventState(handler);
        return true;
    }
}

/**
 * This function returns the index of the next entry if there's
 * another entry for this item.
 *
 * @return if has another entry.
 */
bool
ItemHeap::nextTag(ItemHandler * handler)
{
    // Special case for ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        // If the item has a first chunk of tags, then some more tags may be left.
        if(itemVector.at(handler->item_id) > ITEM_UNUSED)
        {
            handler->tag_chunk_idx = itemVector.at(handler->item_id);
            handler->tag_idx = 0;

            // Tries to find the next tag in the chunk.
            while(!isLastEntry(handler->tag_chunk_idx, handler->tag_idx) && !isTagNode(handler))
            {
                nextEntry(&handler->tag_chunk_idx, &handler->tag_idx);
            }

            // Checks if we found it.
            handler->valid_tag = isTagNode(handler);
            return handler->valid_tag;
        }
        else
        {
            // No more tags left.
            handler->valid_tag = false;
            return false;
        }
    }

    // If we are already at the end then we can't skip anymore.
    if(isLastEntry(handler->tag_chunk_idx, handler->tag_idx))
    {
        handler->valid_tag = false;
        return false;
    }

    // Skips entries until another tag is found.
    do
    {
        nextEntry(&handler->tag_chunk_idx, &handler->tag_idx);
    } while(!isLastEntry(handler->tag_chunk_idx, handler->tag_idx) && !isTagNode(handler));

    // Check if is pointing a tag.
    handler->valid_tag = isTagNode(handler);
    return handler->valid_tag;
}

/**
 * Moves the handler to the next event. We don't have any special
 * case for index -1.
 *
 * @return if has another event.
 */
bool
ItemHeap::nextEvent(ItemHandler * handler)
{
    // If we are already at the end then we can't skip anymore.
    if(isLastEntry(handler->event_chunk_idx, handler->event_idx))
    {
        handler->valid_event = false;
        return false;
    }

    // Skips entries until another event is found or the end of the events is reached.
    do
    {
        nextEntry(&handler->event_chunk_idx, &handler->event_idx);
    } while(!isLastEntry(handler->event_chunk_idx, handler->event_idx) && !isEventNode(handler));

    // Check if is pointing an event.
    handler->valid_event = isEventNode(handler);
    return handler->valid_event;
}

/**
 * Returns a new handler that points to the item with id itemid.
 * If the item doesn't exists, then the ItemHandler returned is
 * invalid.
 *
 * @return a handler pointing to the item.
 */
bool
ItemHeap::lookForItemId(ItemHandler * handler, ITEM_ID itemid)
{
    // If the element isn't allocated then don't exist.
    if(!itemVector.hasElement(itemid) || (itemVector.at(itemid) == ITEM_UNUSED))
    {
        handler->invalid();
        return false;
    }
    else
    {
        handler->item_id = itemid;
        handler->valid_item = true;
        handler->itemtagheap = this;
        handler->strtbl = strtbl;
        handler->tagdescvec = tdv;
        resetTagState(handler);
        resetEventState(handler);
        return true;
    }
}

bool
ItemHeap::existItemId(ITEM_ID itemid) const
{
    // If the element isn't allocated then don't exist.
    if(!itemVector.hasElement(itemid) || (itemVector.at(itemid) == ITEM_UNUSED))
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * Returns a handler pointing to the first item of the tag heap.
 * If no items have been set, false is returned.
 *
 * @return true if a first item handler is returned.
 */
bool
ItemHeap::getFirstItem(ItemHandler * handler)
{
    if(numItems == 0)
    {
        // If no item has been added then returns false.
        handler->invalid();
        return false;
    }
    else
    {
        lookForItemId(handler, firstItem);
        return true;
    }
}

/**
 * Returns a handler pointing to the last item of the tag heap.
 * If no items have been set, false is returned.
 *
 * @return true if a the handler is valid.
 */
bool
ItemHeap::getLastItem(ItemHandler * handler)
{
    if(numItems == 0)
    {
        // If no item has been added then returns false.
        handler->invalid();
        return false;
    }
    else
    {
        lookForItemId(handler, lastItem);
        return true;
    }
}

/**
 * This function first finds the index where the tag with tag_id
 * rcvTgId of the item in this entry (idx) is defined. Then calls
 * another function to obtain all the information of this tag.
 *
 * @return idx if everything is correct. -1 otherwise.
 */
bool
ItemHeap::getTagById(ItemHandler * handler, TAG_ID rcvTgId, UINT64 * tgvalue, TagValueType * tgvtype, TagValueBase * tgbase, CYCLE * tgwhen, CYCLE cycle)
{
    // Points the handler to the first tag.
    resetTagState(handler);

    // Special case when ITEMID is requested.
    if(rcvTgId == itemid_tagid)
    {
        * tgvtype = tdv->getTagValueType(itemid_tagid);
        * tgbase  = tdv->getTagValueBase(itemid_tagid);
        * tgvalue = handler->item_id;
        tgwhen->clock = clocks->getClockDomain(0);
        tgwhen->cycle = 0;
        tgwhen->division = 0;
        return true;
    }

    INT64 lcm_cycle;        ///< LCM cycle of the requested cycle.
    ItemHandler last;       ///< Points to the last value of the current tag.
    bool found;             ///< Stores if a value for the searched tag is found.
    ItemHeapTagNode * node; ///< Tag node found.

    cycle.cycle -= firstEffectiveCycle[cycle.clock->getInternalClock()];
    lcm_cycle = cycle.toLCMCycles();
    found = false;

    // Skips the ITEMID.
    nextTag(handler);

    // Iterates through the tag entry list until finds the first entry that was stored
    // after the requested cycle.
    while(handler->isValidTagHandler())
    {
        node = tagNode(handler);
        CYCLE cyc(clocks->getClockDomain(clocks->internalToExternalClock(node->clockId)), node->cycle, node->division);

        // If the cycle is greater then we end the loop.
        if(cyc.toLCMCycles() > lcm_cycle)
        {
            break;
        }

        // If the actual entry has a value of the tag we are looking for, then gets the entry.
        if((TAG_ID) node->tagId == rcvTgId)
        {
            UINT32 tempCycle;

            last = * handler;
            found = true;

            // Gets the actual state;
            tempCycle = node->cycle;

            nextTag(handler);

            if(node->isSOV)
            {
                node = tagNode(handler);

                // This skips all the values of the same SOV.
                while(handler->isValidTagHandler() && node->isSOV && (node->cycle == tempCycle) && (node->tagId == rcvTgId))
                {
                    nextTag(handler);
                    node = tagNode(handler);
                }
            }
        }
        else
        {
            nextTag(handler);
        }
    }

    // We haven't found an entry. If backpropagation is enabled maybe we'll find a value.
    if(!found)
    {
        // If the backpropagation is not enabled, a false is returned.
        if(!conf->getTagBackPropagate() && (dralVersion >= 2))
        {
            return false;
        }

        // Loops until the first entry for the searched tag is found.
        while(handler->isValidTagHandler())
        {
            // Checks if we've found the first value.
            if((TAG_ID) tagNode(handler)->tagId == rcvTgId)
            {
                node = tagNode(handler);
                CLOCK_ID clock_id = clocks->internalToExternalClock(node->clockId);

                * tgvtype = tdv->getTagValueType(rcvTgId);
                * tgbase  = tdv->getTagValueBase(rcvTgId);
                * tgvalue = dict->getValueByCycle(node->dkey, node->cycle, clock_id);
                tgwhen->clock = clocks->getClockDomain(clock_id);
                tgwhen->cycle = node->cycle + firstEffectiveCycle[node->clockId];
                tgwhen->division = node->division;
                return true;
            }
            nextTag(handler);
        }

        // There's no entry for the tag so false is returned.
        return false;
    }

    * handler = last;
    node = tagNode(handler);
    CLOCK_ID clock_id = clocks->internalToExternalClock(node->clockId);

    // Just get the last value.
    * tgvtype = tdv->getTagValueType(rcvTgId);
    * tgbase  = tdv->getTagValueBase(rcvTgId);
    * tgvalue = dict->getValueByCycle(node->dkey, node->cycle, clock_id);
    tgwhen->clock = clocks->getClockDomain(clock_id);
    tgwhen->cycle = node->cycle + firstEffectiveCycle[node->clockId];
    tgwhen->division = node->division;
    return true;
}

/*
 * Finds the index where the tag rcvTgId is defined beginning in
 * the index idx.
 *
 * @return the tag index.
 */
bool
ItemHeap::getTagPosition(ItemHandler * handler, TAG_ID rcvTgId)
{
    // Points the handler to the first tag.
    resetTagState(handler);

    // Special case when ITEMID is requested.
    if(rcvTgId == itemid_tagid)
    {
        return true;
    }

    // Tries to find the tag with id rcvTgId.
    // Skips the ITEMID in the first iteration.
    do
    {
        nextTag(handler);
    } while(handler->valid_tag && ((TAG_ID) tagNode(handler)->tagId != rcvTgId));

    // True if is pointing to the tag.
    return handler->valid_tag;
}

/*
 * Finds the index where the cycle is greater or equal than cycle.
 *
 * @return if the handler is pointing to a valid tag.
 */
void
ItemHeap::goToCycle(ItemHandler * handler, CYCLE cycle)
{
    // Points the handler to the first tag.
    // WARNING: at least 1 tag expected (now new_item and del_item always exist).
    handler->tag_chunk_idx = itemVector.at(handler->item_id);
    handler->tag_idx = 0;

    INT64 lcm_cycle = cycle.toLCMCycles();
    CYCLE cyc;
    ItemHeapTagNode * node;

    // Skips entries until the requested cycle.
    do
    {
        nextEntry(&handler->tag_chunk_idx, &handler->tag_idx);
        node = tagNode(handler);
        cyc.clock = clocks->getClockDomain(clocks->internalToExternalClock(node->clockId));
        cyc.cycle = node->cycle;
        cyc.division = node->division;
    } while(!isLastEntry(handler->tag_chunk_idx, handler->tag_idx) && (cyc.toLCMCycles() < lcm_cycle));

    if(isTagNode(handler))
    {
        // Now is pointing to a tag.
        handler->valid_tag = true;

        // Copies the event and tries to find the next event.
        handler->event_chunk_idx = handler->tag_chunk_idx;
        handler->event_idx = handler->tag_idx;
        nextEvent(handler);
    }
    else if(isEventNode(handler))
    {
        // Now is pointing to an event so copies the state.
        handler->valid_event = true;
        handler->event_chunk_idx = handler->tag_chunk_idx;
        handler->event_idx = handler->tag_idx;

        // Tries to find the next tag.
        nextTag(handler);
    }
}

/*
 * Returns a boolean indicating if the current handler is the last
 * tag of an item. At least one tag in the item is supposed.
 *
 * @return true if is the last.
 */
bool
ItemHeap::isLastTag(ItemHandler * handler)
{
    ItemHandler temp; ///< Temporal handler to search through the item tags and events.

    // Special case when is pointing to ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        // No more tags left.
        if(itemVector.at(handler->item_id) <= ITEM_UNUSED)
        {
            return true;
        }

        temp.tag_chunk_idx = itemVector.at(handler->item_id);
        temp.tag_idx = 0;
    }
    else
    {
        // Checks if is the last entry.
        if(isLastEntry(handler->tag_chunk_idx, handler->tag_idx))
        {
            return true;
        }

        temp = * handler;
        // Skips the actual entry.
        nextEntry(&temp.tag_chunk_idx, &temp.tag_idx);
    }

    // Iterates throught the events and tags.
    while(!isLastEntry(temp.tag_chunk_idx, temp.tag_idx))
    {
        // If founds another tag entry then the handler isn't in the last tag.
        if(isTagNode(&temp))
        {
            return false;
        }
        nextEntry(&temp.tag_chunk_idx, &temp.tag_idx);
    }

    // Last check. Can change it to a do..while??
    return !isTagNode(&temp);
}

/**
 * Returns a boolean indicating if the current handler is the last
 * event of an item.
 *
 * @return true if is the last.
 */
bool
ItemHeap::isLastEvent(ItemHandler * handler)
{
    ItemHandler temp; ///< Temporal handler to search through the item tags and events.

    // Special case when is pointing to null event.
    if(handler->event_chunk_idx == -1)
    {
        // No more events left.
        if(itemVector.at(handler->item_id) <= ITEM_UNUSED)
        {
            return true;
        }

        temp.event_chunk_idx = itemVector.at(handler->item_id);
        temp.event_idx = 0;
    }
    else
    {
        // Checks if is the last entry.
        if(isLastEntry(handler->event_chunk_idx, handler->event_idx))
        {
            return true;
        }

        temp = * handler;
        // Skips the actual entry.
        nextEntry(&temp.event_chunk_idx, &temp.event_idx);
    }

    // Iterates throught the events and tags.
    while(!isLastEntry(temp.event_chunk_idx, temp.event_idx))
    {
        // If founds another event entry then the handler isn't in the last event.
        if(isEventNode(&temp))
        {
            return false;
        }
        nextEntry(&temp.event_chunk_idx, &temp.event_idx);
    }

    // Last check. Can change it to a do..while??
    return !isEventNode(&temp);
}

/**
 * Finds where the target_tagid has the value target_value.
 *
 * @return void.
 */
void
ItemHeap::lookForIntegerValue(ItemHandler * handler, TAG_ID target_tagid, UINT64 target_value, CYCLE cycle, ITEM_ID start_item)
{
    // TODO
    Q_ASSERT(false);
/*    INT32 pos = starting_point;
    INT32 itemIdPos = -1;
    UINT16 tagId;
    UINT16 itemId_TagId = TagDescVector::getItemIdTagId();

    INT16  tgbase;
    TagValueType tgvtype;
    UINT64 tgvalue;
    UINT64 tgwhen;

    bool fnd = false;
    while ( (!fnd) && (pos<nextEntry) )
    {
        tagId = aetagvector->at(pos).tagId;
        if (tagId==itemId_TagId)
        {
            itemIdPos=pos;
        }
        if (tagId==target_tagid)
        {
            bool ok = getTag(pos,&tgvalue,&tgvtype,&tgbase,&tgwhen,cycle);
            fnd = ok && (tgvalue == target_value);
        }
        ++pos;
    }

    INT32 result;
    if (fnd)
    {
        result = (itemIdPos);
        *tgPos = pos-1;
    }
    else
    {
        result = -1;
    }
    return (result);*/
}

/**
 * Finds where the target_tagid has the value target_value.
 *
 * @return the item_id of the item that matched the value.
 */
void
ItemHeap::lookForStringValue(ItemHandler * handler, TAG_ID target_tagid, QString target_value, bool csensitive, bool exactMatch, CYCLE cycle, ITEM_ID start_item)
{
    // TODO
    Q_ASSERT(false);
/*    if (starting_point<0)
    {
        return (-1);
    }

    INT32 pos = starting_point;
    INT32 itemIdPos = -1;
    UINT16 tagId;
    UINT16 itemId_TagId = TagDescVector::getItemIdTagId();

    INT16  tgbase;
    TagValueType tgvtype;
    UINT64 tgvalue;
    UINT64 tgwhen;

    if (!csensitive)
    {
        target_value = target_value.upper();
    }

    bool fnd = false;
    while ( (!fnd) && (pos<nextEntry) )
    {
        tagId = aetagvector->at(pos).tagId;
        if (tagId==itemId_TagId)
        {
            itemIdPos=pos;
        }
        if (tagId==target_tagid)
        {
            bool ok = getTag(pos,&tgvalue,&tgvtype,&tgbase,&tgwhen,cycle);
            QString fvalue = strtbl->getString((INT32)tgvalue);
            if (ok && (fvalue!=QString::null))
            {
                if (exactMatch)
                {
                    if (!csensitive)
                    {
                        fvalue = fvalue.upper();
                    }
                    fnd = (fvalue==target_value);
                }
                else
                {
                    fnd = (fvalue.contains(target_value,csensitive)>0);
                }
            }
        }
        ++pos;
    }

    INT32 result;
    if (fnd)
    {
        result = itemIdPos;
        *tgPos = pos-1;
    }
    else
    {
        result = -1;
    }
    return (result);*/
}

/**
 * Finds where the target_tagid matches the regular expression
 * target_value.
 *
 * @return the item_id of the item that matched the value.
 */
void
ItemHeap::lookForStringValue(ItemHandler * handler, TAG_ID target_tagid, QRegExp target_value, CYCLE cycle, ITEM_ID start_item)
{
    // TODO
    Q_ASSERT(false);
/*    if (starting_point<0)
    {
        return (-1);
    }

    UINT16 itemId_TagId = TagDescVector::getItemIdTagId();

    INT32 pos = starting_point;
    INT32 itemIdPos = -1;
    UINT16 tagId;

    INT16  tgbase;
    TagValueType tgvtype;
    UINT64 tgvalue;
    UINT64 tgwhen;

    bool fnd = false;
    while ( (!fnd) && (pos<nextEntry) )
    {
        tagId = aetagvector->at(pos).tagId;
        if (tagId==itemId_TagId)
        {
            itemIdPos=pos;
        }
        if (tagId==target_tagid)
        {
            bool ok = getTag(pos,&tgvalue,&tgvtype,&tgbase,&tgwhen,cycle);
            QString fvalue = strtbl->getString((INT32)tgvalue);
            if (ok && (fvalue!=QString::null))
            {
                fnd = (fvalue.contains(target_value)>0);
            }
        }
        ++pos;
    }

    INT32 result;
    if (fnd)
    {
        result = itemIdPos;
        *tgPos = pos-1;
    }
    else
    {
        result = -1;
    }
    return (result);*/
}

/**
 * Allocates a new tag slot for the item that contains the handler.
 *
 * @return size of the object.
 */
ItemHeapNode *
ItemHeap::allocateNewSlot(ItemHandler * handler)
{
    Q_ASSERT(itemVector.at(handler->item_id) != ITEM_UNUSED);

    // Checks whether the item has the first chunk allocated or not.
    if(itemVector.at(handler->item_id) == ITEM_OPEN)
    {
        UINT32 chunk_idx; ///< Chunk allocated.

        // Must allocate the first chunk.
        chunk_idx = nextEventVectorEntry++;

        // Marks that the item has at least 1 tag.
        itemVector.ref(handler->item_id) = chunk_idx;

        // Allocates the slot.
        eventVector[chunk_idx].setFirstFreeSlot(1);
        eventVector.ref(chunk_idx).setNextChunk(0);

        // Returns the pointer to the tag allocated.
        return &(eventVector.ref(chunk_idx).content[0]);
    }
    else
    {
        UINT32 chunk_idx;     ///< Used to go until the last chunk of the actual item.
        UINT32 new_chunk_idx; ///< Used to allocate a new chunk if necessary.

        chunk_idx = itemVector.at(handler->item_id);

        // Goes until the last chunk.
        while(eventVector.ref(chunk_idx).getNextChunk() != 0)
        {
            chunk_idx += eventVector.ref(chunk_idx).getNextChunk();
        }

        // Looks if the last chunk is full.
        if(eventVector.ref(chunk_idx).getFirstFreeSlot() == 4)
        {
            // We must allocate a new chunk.
            new_chunk_idx = nextEventVectorEntry++;

            // Checks that the offset can be stored.
            Q_ASSERT((new_chunk_idx - chunk_idx) <= ITEM_MAX_CHUNK_OFFSET);

            // Sets the new allocated chunk as the next of the old chunk.
            eventVector.ref(chunk_idx).setNextChunk(new_chunk_idx - chunk_idx);

            // Allocates the slot.
            eventVector[new_chunk_idx].setFirstFreeSlot(1);
            eventVector.ref(new_chunk_idx).setNextChunk(0);

            return &(eventVector.ref(new_chunk_idx).content[0]);
        }
        else
        {
            UINT32 slot; ///< Slot allocated.

            // Just increments the occupancy.
            slot = eventVector.ref(chunk_idx).allocateSlot();
            return &(eventVector.ref(chunk_idx).content[slot]);
        }
    }
}

/**
 * Resets the event state of handler. Points to a the first event
 * position if an event exists.
 *
 * @return void.
 */
void
ItemHeap::resetEventState(ItemHandler * handler)
{
    Q_ASSERT(handler->valid_item);

    // If the item has a first chunk of nodes, then a first event might exist.
    if(itemVector.at(handler->item_id) > ITEM_UNUSED)
    {
        handler->event_chunk_idx = itemVector.at(handler->item_id);
        handler->event_idx = 0;

        // Tries to find the next tag in the chunk.
        while(!isLastEntry(handler->event_chunk_idx, handler->event_idx) && !isEventNode(handler))
        {
            nextEntry(&handler->event_chunk_idx, &handler->event_idx);
        }

        // Checks if we found it.
        handler->valid_event = isEventNode(handler);
    }
    else
    {
        // No more events left.
        handler->valid_event = false;
        handler->event_chunk_idx = -1;
        handler->event_idx = -1;
    }
}

/**
 * Dumps all the tags of all the items to the standard output.
 *
 * @return void.
 */
void
ItemHeap::dumpItemTagHeap()
{
    ItemHandler hnd;

    getFirstItem(&hnd);
    while(hnd.isValidItemHandler())
    {
        hnd.dumpItem();
        hnd.nextItem();
    }
}

/**
 * Dumps all the tags of all the items to the standard output.
 *
 * @return void.
 */
void
ItemHeap::dumpRegression(bool gui_enabled)
{
    ItemHandler hnd;            ///< Handler to iterate through the items.
    INT32 offset;               ///< Number of cycles between progress dialog updates.
    QProgressDialog * progress; ///< Progress dialog to show the evolution of the dumping.
    QLabel * label;             ///< Label of the progress dialog.
    INT32 items;                ///< Used to count the number of items dumped.

    items = 0;
    offset = 0;
    progress = NULL;
    label = NULL;

    if(gui_enabled)
    {
        progress = new QProgressDialog("Dreams is dumping the item database...", 0, getNumItems(), NULL, "commitprogress", TRUE);

        label = new QLabel(QString("\nDreams is dumping the item database...               ") +
                           QString("\nItems                              : 0"), progress);
        Q_ASSERT(label != NULL);
        label->setMargin(5);
        label->setAlignment(Qt::AlignLeft);
        //label->setFont(QFont("Courier", qApp->font().pointSize(), QFont::Bold));
        progress->setLabel(label);

        //progress = new QProgressDialog(NULL, "Dreams is dumping the item database...");
        offset = getNumItems() / 100;

        // Sanity check.
        if(offset == 0)
        {
            offset++;
        }
        // Avoids slow progress bar update.
        else if(offset > 2000)
        {
            // Looks like a random progress :d.
            offset = 1973;
        }
    }

    getFirstItem(&hnd);
    while(hnd.isValidItemHandler())
    {
        items++;
        hnd.dumpRegression();
        hnd.nextItem();
        if(gui_enabled)
        {
            if((items % offset) == (offset - 1))
            {
                // Updates the progress bar.
                progress->setProgress(items);
                label->setText(QString("\nDreams is dumping the item database...               ") +
                               QString("\nItems                              : ") + QString::number(items));
            }
        }
    }

    if(gui_enabled)
    {
        delete progress;
    }
}

/**
 * Returns the size of the tag heap.
 *
 * @return size of the object.
 */
INT64
ItemHeap::getObjSize() const
{
    INT64 alloc;
    alloc = itemVector.getNumSegments() * itemVector.getSegmentSize() * sizeof(UINT32);
    alloc = eventVector.getNumSegments() * eventVector.getSegmentSize() * sizeof(ItemHeapChunk);
    return sizeof(ItemHeap) + alloc;
}

/**
 * Returns a string with a usage description of this class.
 *
 * @return the description.
 */
QString
ItemHeap::getUsageDescription() const
{
    QString result;

    result = QString("\tItemTagHeap object occupies ") + QString::number(getObjSize()) + QString(". Other ItemHeap components:\n");
    result += QString("\t\tItems allocated: ") + QString::number(itemVector.getNumSegments() * itemVector.getSegmentSize()) + QString(" (each one is 4 bytes).\n");
    result += QString("\t\tChunks allocated: ") + QString::number(eventVector.getNumSegments() * eventVector.getSegmentSize()) + QString(" (each one is " + QString::number(sizeof(ItemHeapChunk)) + " bytes).\n");
    return result;
}

/**
 * Creats a string with the stats of this class.
 *
 * @return the stats.
 */
QString
ItemHeap::getStats() const
{
    QString result;
    result = "Total number of items: " + QString::number(numItems);
    result += "Total number of tags (ITEMID not included): " + QString::number(numTags);
    result += "Total number of move items: " + QString::number(numMoveItems);
    result += "Total number of enter nodes: " + QString::number(numEnterNodes);
    result += "Total number of exit nodes: " + QString::number(numExitNodes);
    result += "Total number of generic events: " + QString::number(numGenEvents);
    return result;
}

/**
 * Dumps the content of an ItemHandler.
 *
 * @return void.
 */
void
ItemHandler::dumpItem()
{
    printf ("ItemId: %d\n", item_id);

    bool eol = itemtagheap->isLastTag(this);
    while(!eol)
    {
        itemtagheap->nextTag(this);
        eol = itemtagheap->isLastTag(this);
        printf("TagId = %d (%s), Value=%llu, ClockId=%d, PostCycle=%d, Division=%d, SOV=%d, Mutable=%d, Chunk=%d, Slot=%d\n",
               itemtagheap->getTagId(this),
               tagdescvec->getTagName(itemtagheap->getTagId(this)).latin1(),
               itemtagheap->getTagValue(this),
               itemtagheap->getTagCycle(this).clock->getId(),
               itemtagheap->getTagCycle(this).cycle,
               itemtagheap->getTagCycle(this).division,
               (int) itemtagheap->getTagIsSOV(this),
               (int) itemtagheap->getTagIsMutable(this),
               tag_chunk_idx,
               tag_idx
               );
    }

    eol = itemtagheap->isLastEvent(this);
    while(!eol)
    {
        itemtagheap->nextEvent(this);
        eol = itemtagheap->isLastEvent(this);
        switch(getEventType())
        {
            case IDB_MOVE_ITEM:
                printf("MoveItem Edge=%d, Position=%d, ClockId=%d, Cycle=%d, Division=%d, Chunk=%d, Slot=%d\n",
                    itemtagheap->getEventNodeEdgeId(this),
                    itemtagheap->getEventPosition(this),
                    itemtagheap->getEventCycle(this).clock->getId(),
                    itemtagheap->getEventCycle(this).cycle,
                    itemtagheap->getEventCycle(this).division,
                    event_chunk_idx,
                    event_idx
                );
                break;

            case IDB_ENTER_NODE:
                printf("EnterNode Node=%d, Slot=%d, ClockId=%d, Cycle=%d, Division=%d, Chunk=%d, Slot=%d\n",
                    itemtagheap->getEventNodeEdgeId(this),
                    itemtagheap->getEventPosition(this),
                    itemtagheap->getEventCycle(this).clock->getId(),
                    itemtagheap->getEventCycle(this).cycle,
                    itemtagheap->getEventCycle(this).division,
                    event_chunk_idx,
                    event_idx
                );
                break;

            case IDB_EXIT_NODE:
                printf("ExitNode Node=%d, Slot=%d, ClockId=%d, Cycle=%d, Division=%d, Chunk=%d, Slot=%d\n",
                    itemtagheap->getEventNodeEdgeId(this),
                    itemtagheap->getEventPosition(this),
                    itemtagheap->getEventCycle(this).clock->getId(),
                    itemtagheap->getEventCycle(this).cycle,
                    itemtagheap->getEventCycle(this).division,
                    event_chunk_idx,
                    event_idx
                );
                break;

            case IDB_GEN_EVENT:
                printf("Generic Event Data=%llu, ClockId=%d, Cycle=%d, Division=%d, Chunk=%d, Slot=%d\n",
                    itemtagheap->getEventId(this),
                    itemtagheap->getEventCycle(this).clock->getId(),
                    itemtagheap->getEventCycle(this).cycle,
                    itemtagheap->getEventCycle(this).division,
                    event_chunk_idx,
                    event_idx
                );
                break;

            case IDB_EN_EX_NODE_ESC:
                // TODO:
                Q_ASSERT(false);
                break;

            default:
                Q_ASSERT(false);
                break;
        }
    }
}

/**
 * Dumps the content of an ItemHandler.
 *
 * @return void.
 */
void
ItemHandler::dumpRegression()
{
    char str[256];
    multiset<regressionData, regressionCompare> tags;
    multiset<regressionData, regressionCompare>::iterator it;
    regressionData data;
    UINT16 act_tag;
    UINT16 last_tag;
    QString act_val;
    QString last_val;

    sprintf(str, FMT32X, item_id);
    printf("I=%s\n", str);

    last_tag = 0;
    last_val = QString("");

    bool eol = itemtagheap->isLastTag(this);
    while (!eol)
    {
        nextTag();
        eol = itemtagheap->isLastTag(this);
        data.hnd = * this;
        data.tag = itemtagheap->getTagId(this);
        tags.insert(data);
    }

    for(it = tags.begin(); it != tags.end(); it++)
    {
        act_tag = itemtagheap->getTagId(&(it->hnd));
        act_val = it->hnd.getTagFormatedValue().latin1();

        // Only prints the changes.
        if((act_tag != last_tag) || (act_val != last_val))
        {
            sprintf(str, FMT16X, act_tag);
            printf("T=%s,", tagdescvec->getTagName(act_tag).latin1());
            printf("V=%s,", act_val.latin1());
            sprintf(str, FMT32X, itemtagheap->getTagCycle(&(it->hnd)).clock->getId());
            printf("CId=%s,", str);
            sprintf(str, FMT32X, itemtagheap->getTagCycle(&(it->hnd)).cycle);
            printf("Cyc=%s,", str);
            sprintf(str, FMT32X, itemtagheap->getTagCycle(&(it->hnd)).division);
            printf("Div=%s,", str);
            sprintf(str, FMT16X, (UINT16) itemtagheap->getTagIsSOV(&(it->hnd)));
            printf("S=%s\n", str);
        }
        last_tag = act_tag;
        last_val = act_val;
    }

/*    eol = itemtagheap->isLastEvent(this);
    while(!eol)
    {
        itemtagheap->nextEvent(this);
        eol = itemtagheap->isLastEvent(this);
        switch(getEventType())
        {
            case IDB_MOVE_ITEM:
                sprintf(str, FMT16X, itemtagheap->getEventNodeEdgeId(this));
                printf("MI E=%s,", str);
                sprintf(str, FMT32X, itemtagheap->getEventPosition(this));
                printf("P=%s,", str);
                sprintf(str, FMT32X, itemtagheap->getEventCycle(this));
                printf("C=%s\n", str);
                break;

            case IDB_ENTER_NODE:
                sprintf(str, FMT16X, itemtagheap->getEventNodeEdgeId(this));
                printf("EN N=%s,", str);
                sprintf(str, FMT32X, itemtagheap->getEventPosition(this));
                printf("S=%s,", str);
                sprintf(str, FMT32X, itemtagheap->getEventCycle(this));
                printf("C=%s\n", str);
                break;

            case IDB_EXIT_NODE:
                sprintf(str, FMT16X, itemtagheap->getEventNodeEdgeId(this));
                printf("EE N=%s,", str);
                sprintf(str, FMT32X, itemtagheap->getEventPosition(this));
                printf("S=%s,", str);
                sprintf(str, FMT32X, itemtagheap->getEventCycle(this));
                printf("C=%s\n", str);
                break;

            case IDB_EVENT:
                sprintf(str, FMT64X, itemtagheap->getEventId(this));
                printf("E I=%s,", str);
                sprintf(str, FMT32X, itemtagheap->getEventCycle(this));
                printf("C=%s\n", str);
                break;

            case IDB_EN_EX_NODE_ESC:
                // TODO:
                Q_ASSERT(false);
                break;

            default:
                Q_ASSERT(false);
                break;
        }
    }*/
}

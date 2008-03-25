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
  * @file ItemHeap.h
  */

#ifndef _DRALDB_ITEMTAGHEAP_H
#define _DRALDB_ITEMTAGHEAP_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/DRALTag.h"
#include "dDB/TagDescVector.h"
#include "dDB/Dict2064.h"
#include "dDB/itemheap/ItemHeapDef.h"
#include "dDB/ClockDomainMgr.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/StatObj.h"

// Qt includes.
#include <qvaluelist.h>
#include <qregexp.h>

// Class forwarding.
class ItemHandler;
class LogMgr;
class DBConfig;
class StrTable;

/*
 * @brief
 * This class hold the heap of tags (tag-value) pairs.
 *
 * @description
 * This class holds all the values of all the tags of the items.
 * Each item has all its tags and values ordered by cycle.
 * If a tag is not mutable, just occupies one slot, but if its
 * value is changed along the simulation, each value change occupies
 * a heap slot. This class provides instructions to access and set
 * this information.
 *
 * @version 0.1
 * @date started at 2002-04-01
 * @author Federico Ardanaz
 */
class ItemHeap : public AMemObj, public StatObj
{
    public:
        ItemHeap(TagDescVector * _tdv, Dict2064 * _dict, DBConfig * _conf, StrTable * _strtbl, ClockDomainMgr * _clocks);
        virtual ~ItemHeap();

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        QString getStats() const;

        bool newItemBlock(ITEM_ID itemId, CYCLE cycle);
        inline bool endItemBlock(ITEM_ID itemId, CYCLE cycle);

        bool newTag(ITEM_ID item_id, TAG_ID tag_id, UINT64 value, CYCLE cycle);
        bool newTag(ITEM_ID item_id, TAG_ID tag_id, SOVList * list, CYCLE cycle);
        bool newMoveItem(ITEM_ID item_id, DRAL_ID edge_id, UINT32 position, CYCLE cycle);
        bool newEnterNode(ITEM_ID item_id, DRAL_ID node_id, UINT32 position, CYCLE cycle);
        bool newExitNode(ITEM_ID item_id, DRAL_ID node_id, UINT32 position, CYCLE cycle);
        bool newGenericEvent(ITEM_ID item_id, UINT64 event, CYCLE cycle);

        bool lookForItemId(ItemHandler * handler, ITEM_ID itemid);
        bool existItemId(ITEM_ID itemid) const;

        bool getFirstItem(ItemHandler * handler);
        bool getLastItem(ItemHandler * handler);

        void lookForIntegerValue(ItemHandler * handler, TAG_ID target_tagid, UINT64 target_value, CYCLE cycle, ITEM_ID start_item);
        void lookForStringValue(ItemHandler * handler, TAG_ID target_tagid, QString target_value, bool csensitive, bool exactMatch, CYCLE cycle, ITEM_ID start_item);
        void lookForStringValue(ItemHandler * handler, TAG_ID target_tagid, QRegExp target_value, CYCLE cycle, ITEM_ID start_item);

        inline INT32 getNumItems() const;
        inline void setDralVersion(UINT16 value);
        inline UINT16 getDralVersion() const;
        inline void setFirstEffectiveCycle(CYCLE cycle);
        inline void setNumClocks(UINT16 numClocks);

        void dumpItemTagHeap();
        void dumpRegression(bool gui_enabled);

    protected:

        // Only ItemHandler and TrackHeap can use these functions.
        friend class ItemHandler;

        bool getTagById(ItemHandler * handler, TAG_ID rcvTgId, UINT64 * tgvalue, TagValueType * tgvtype, TagValueBase * tgbase, CYCLE * tgwhen, CYCLE cycle);
        bool getTagPosition(ItemHandler * handler, TAG_ID rcvTgId);
        void goToCycle(ItemHandler * handler, CYCLE cycle);

        bool nextItem(ItemHandler * handler);

        bool nextTag(ItemHandler * handler);
        bool nextEvent(ItemHandler * handler);

        inline void nextEntry(INT32 * chunk_idx, INT32 * entry_idx);
        inline bool isLastEntry(INT32 chunk_idx, INT32 slot_idx) const;

        inline CYCLE getTagCycle(const ItemHandler * handler) const;
        inline UINT64 getTagValue(const ItemHandler * handler) const;
        inline bool getTagDefined(const ItemHandler * handler) const;
        inline TAG_ID getTagId(const ItemHandler * handler) const;
        inline bool getTagIsSOV(const ItemHandler * handler) const;
        inline bool getTagIsMutable(const ItemHandler * handler) const;
        inline TagValueType getTagType(const ItemHandler * handler) const;
        inline INT16 getTagBase(const ItemHandler * handler) const;
        inline void cacheTag(ItemHandler * handler);

        inline ItemHeapEventSubType getEventType(const ItemHandler * handler) const;
        inline CYCLE getEventCycle(const ItemHandler * handler) const;
        inline INT64 getEventId(const ItemHandler * handler) const;
        inline INT16 getEventNodeEdgeId(const ItemHandler * handler) const;
        inline INT32 getEventPosition(const ItemHandler * handler) const;

        bool isLastTag(ItemHandler * handler);
        bool isLastEvent(ItemHandler * handler);

        ItemHeapNode * allocateNewSlot(ItemHandler * handler);

        inline void newTagLookForItemId(ItemHandler * handler);
        inline void newEventLookForItemId(ItemHandler * handler);

        inline ItemHeapTagNode * tagNode(const ItemHandler * handler) const;
        inline ItemHeapNode * eventNode(const ItemHandler * handler) const;

        inline void resetTagState(ItemHandler * handler);
        void resetEventState(ItemHandler * handler);

        inline bool isTagNode(const ItemHandler * handler) const;
        inline bool isEventNode(const ItemHandler * handler) const;

    private:
        INT32 * firstEffectiveCycle;     ///< First cycle.
        ITEM_ID firstItem;               ///< First item that exists (lower id).
        ITEM_ID lastItem;                ///< Last item that exists (bigger id).
        INT32 numItems;                  ///< Number of items.
        INT32 numTags;                   ///< Number of tags.
        INT32 numMoveItems;              ///< Number of move items.
        INT32 numEnterNodes;             ///< Number of enter nodes.
        INT32 numExitNodes;              ///< Number of exit nodes.
        INT32 numGenEvents;              ///< Number of generic events.
        UINT16 dralVersion;              ///< Dral Version. Used for backwards compatibility.
        TAG_ID itemid_tagid;             ///< Value of tag id that represents an ITEMID tag.
        TAG_ID newitem_tagid;            ///< Value of tag id that represents a the creation of an item.
        TAG_ID delitem_tagid;            ///< Value of tag id that represents a the destruction of an item.
        ItemHeapItemVector itemVector;   ///< Vector of item entries.
        ItemHeapEventVector eventVector; ///< Vector of event chunks.
        INT32 nextEventVectorEntry;      ///< Next free event chunk entry.
        TagDescVector * tdv;             ///< Pointer to the tag descriptor vector.
        Dict2064 * dict;                 ///< Pointer to a dictionary.
        StrTable * strtbl;               ///< Pointer to the string table.
        DBConfig * conf;                 ///< Pointer to the database configuration state.
        LogMgr * myLogMgr;               ///< Pointer to the log manager.
        ClockDomainMgr * clocks;         ///< Pointer to the clock domain manager.
};

/*
 * @brief
 * This class is used as interface to access to the item tag heap.
 *
 * @description
 * ItemHandler is used like an iterator to access to the values
 * of the different tags of an item of the heap. All the functions
 * use the different methods defined in the ItemHeap to do all
 * the work. ItemHandler is supposed to be used in an iterator
 * fashion. First, an ItemHandler must be created from another
 * ItemHandler or with the look-up creator (with an item id).
 * Then, it can iterate through all the tags of the item where is
 * pointing until reaches an invalid state. Functions to point to
 * the next item or to skip a tag value are offered too. Also
 * functions to iterate through different events are provided.
 * This events typically are move items and enter or exit nodes.
 *
 * @author Federico Ardanaz
 * @date started at 2002-07-15
 * @version 0.1
 */
class ItemHandler
{
    public:
        inline ItemHandler();

        inline ITEM_ID getItemId() const;

        inline CYCLE getTagCycle() const;
        inline CYCLE getTagCachedCycle() const;
        inline INT64 getTagCachedCycleLCM() const;
        inline UINT64 getTagValue() const;
        inline UINT64 getTagCachedValue() const;
        inline bool getTagDefined() const;
        inline bool getTagCachedDefined() const;
        inline bool getTagIsMutable() const;
        inline bool getTagIsSOV() const;
        inline TagValueType getTagType() const;
        inline INT16 getTagBase() const;
        inline TAG_ID getTagId() const;
        inline TAG_ID getTagCachedId() const;
        inline QString getTagFormatedValue() const;
        inline void cacheTag();

        inline bool getTagByName(QString str, UINT64 * tgvalue, TagValueType * tgvtype, TagValueBase * tgbase, CYCLE * tgwhen, CYCLE cycle);
        inline bool getTagById(TAG_ID tgId, UINT64 * tgvalue, TagValueType * tgvtype, TagValueBase * tgbase, CYCLE * tgwhen, CYCLE cycle);
        inline bool hasTag(TAG_ID tgId);
        inline bool hasTag(QString tgname);

        inline ItemHeapEventSubType getEventType() const;
        inline CYCLE getEventCycle() const;
        inline INT64 getEventId() const;
        inline DRAL_ID getEventNodeId() const;
        inline DRAL_ID getEventEdgeId() const;
        inline INT32 getEventPosition() const;

        inline bool nextItem();

        inline bool nextTag();
        inline void rewindToFirstTag();

        inline bool nextEvent();
        inline void rewindToFirstEvent();

        inline void goToCycle(CYCLE cycle);

        inline bool isValidItemHandler() const;
        inline bool isValidTagHandler() const;
        inline bool isValidEventHandler() const;

        void dumpItem();
        void dumpRegression();

        inline QString getHandlerInformation() const;

        inline bool operator==(const ItemHandler & comp) const;
        inline bool sameTag(const ItemHandler & comp) const;
        inline bool sameEvent(const ItemHandler & comp) const;

        inline INT32 uniqueIndex() const;

        inline void invalid();

    protected:
        inline ItemHandler(bool value);
        inline ItemHandler(ITEM_ID itemId, ItemHeap * _itemtagheap);

        friend class ItemHeap;

    private:
        ITEM_ID item_id;            ///< Item of this handler.
        INT32 tag_chunk_idx;        ///< Index of the chunk where the actual tag is stored.
        INT32 event_chunk_idx;      ///< Index of the chunk where the actual event is stored.
        INT32 tag_idx;              ///< Index within the chunk of the actual tag.
        INT32 event_idx;            ///< Index within the chunk of the actual event.
        bool valid_item;            ///< True if is pointing a valid item.
        bool valid_tag;             ///< True if is pointing a valid tag. Can't be true if valid_item is false.
        bool valid_event;           ///< True if is pointing a valid event. Can't be true if valid_item is false.
        CYCLE cached_tag_cycle;     ///< Cached tag cycle.
        INT64 cached_tag_cycle_lcm; ///< Cached tag cycle.
        UINT64 cached_tag_value;    ///< Cached tag value.
        bool cached_tag_defined;    ///< Cached tag defined.
        TAG_ID cached_tag_id;       ///< Cached tag id.
        ItemHeap * itemtagheap;     ///< Pointer to the item tag heap.
        TagDescVector * tagdescvec; ///< Pointer to the tag description vector.
        StrTable * strtbl;          ///< Pointer to the string table conversion.
};

/** @typdef regressionData
  * @brief
  * Auxiliar struct used to dump tags in order.
  */
typedef struct regressionData
{
    TAG_ID tag;
    mutable ItemHandler hnd;
} ;

/** @struct regressionCompare
  * @brief
  * Used to compare regression data.
  */
struct regressionCompare
{
    bool operator()(const regressionData rd1, const regressionData rd2) const
    {
        return (rd1.tag < rd2.tag);
    }
} ;

/**
 * Creator of this class. Just invalids the handler.
 *
 * @return new object.
 */
ItemHandler::ItemHandler()
{
    invalid();
}

/*
 * Fast creator just for internal use only.
 *
 * @return new object.
 */
ItemHandler::ItemHandler(bool value)
{
}

/**
 * Creator of this class. Creates a new ItemHandler that points
 * to the tags of the item itemId. Forwards the call to the tag
 * heap.
 *
 * @return new object.
 */
ItemHandler::ItemHandler(ITEM_ID itemId, ItemHeap * _itemtagheap)
{
    _itemtagheap->lookForItemId(this, itemId);
}

/**
 * Gets all the information of a tag of the item pointed by the 
 * handler using its name.
 *
 * @return if a value has been found.
 */
bool
ItemHandler::getTagByName(QString str, UINT64 * tgvalue, TagValueType * tgvtype, TagValueBase * tgbase, CYCLE * tgwhen, CYCLE cycle)
{
    Q_ASSERT(valid_item);

    // Get the tag number to avoid lots of string comparisions.
    TAG_ID rcvTgId = tagdescvec->getTagId(str);
    if(rcvTgId == TAG_ID_INVALID)
    {
        return false;
    }
    return itemtagheap->getTagById(this, rcvTgId, tgvalue, tgvtype, tgbase, tgwhen, cycle);
}

/**
 * Gets all the information of a tag  of the item pointed by the
 * handler using its id.
 *
 * @return if a value has been found.
 */
bool
ItemHandler::getTagById(TAG_ID tgId, UINT64 * tgvalue, TagValueType * tgvtype, TagValueBase * tgbase, CYCLE * tgwhen, CYCLE cycle)
{
    Q_ASSERT(valid_item);
    return itemtagheap->getTagById(this, tgId, tgvalue, tgvtype, tgbase, tgwhen, cycle);
}

/**
 * Looks if the item pointed by the handler has a tag defined.
 *
 * @return true if the item has tgId defined.
 */
bool
ItemHandler::hasTag(TAG_ID tgId)
{
    Q_ASSERT(valid_item);
    return itemtagheap->getTagPosition(this, tgId);
}

/**
 * Looks if the item pointed by the handler has a tag defined.
 *
 * @return true if the item has tgId defined.
 */
bool
ItemHandler::hasTag(QString tgname)
{
    Q_ASSERT(valid_item);

    TAG_ID tgId = tagdescvec->getTagId(tgname);
    if(tgId == TAG_ID_INVALID)
    {
        return false;
    }
    return itemtagheap->getTagPosition(this, tgId);
}

/*
 * Gets the itemid of the item of this handler.
 *
 * @return the itemid.
 */
ITEM_ID
ItemHandler::getItemId() const
{
    return item_id;
}

/*
 * Returns the cycle where the actual tag value was set.
 *
 * @return the current cycle.
 */
CYCLE
ItemHandler::getTagCycle() const
{
    Q_ASSERT(valid_tag);
    return itemtagheap->getTagCycle(this);
}

/*
 * Returns the cached cycle.
 *
 * @return the current cycle.
 */
CYCLE
ItemHandler::getTagCachedCycle() const
{
    return cached_tag_cycle;
}

/*
 * Returns the cached cycle.
 *
 * @return the current cycle.
 */
INT64
ItemHandler::getTagCachedCycleLCM() const
{
    return cached_tag_cycle_lcm;
}

/*
 * Gets the value of the current tag of the handler.
 *
 * @return the value.
 */
UINT64
ItemHandler::getTagValue() const
{
    Q_ASSERT(valid_tag);
    return itemtagheap->getTagValue(this);
}

/*
 * Gets the cached value of the current tag.
 *
 * @return the value.
 */
UINT64
ItemHandler::getTagCachedValue() const
{
    return cached_tag_value;
}

/*
 * Gets if the value is defined.
 *
 * @return if the value is defined.
 */
bool
ItemHandler::getTagDefined() const
{
    Q_ASSERT(valid_tag);
    return itemtagheap->getTagDefined(this);
}

/*
 * Gets if the cached value is defined.
 *
 * @return if the value is defined.
 */
bool
ItemHandler::getTagCachedDefined() const
{
    return cached_tag_defined;
}

/**
 * Gets the mutable flag of the actual tag pointed by this handler.
 *
 * @return true if the actual tag is mutable.
 */
bool
ItemHandler::getTagIsMutable() const
{
    Q_ASSERT(valid_tag);
    return itemtagheap->getTagIsMutable(this);
}

/*
 * Gets the SOV flag of the actual tag.
 *
 * @return true if the actual tag is a SOV.
 */
bool
ItemHandler::getTagIsSOV() const
{
    Q_ASSERT(valid_tag);
    return itemtagheap->getTagIsSOV(this);
}

/*
 * Gets the type of tag where the handler is pointing.
 *
 * @return the tag type.
 */
TagValueType
ItemHandler::getTagType() const
{
    Q_ASSERT(valid_tag);
    return itemtagheap->getTagType(this);
}

/*
 * Gets the base of tag where the handler is pointing.
 *
 * @return the tag base.
 */
INT16
ItemHandler::getTagBase() const
{
    Q_ASSERT(valid_tag);
    return itemtagheap->getTagBase(this);
}

/*
 * Gets the tag id of the current tag.
 *
 * @return the tag id.
 */
TAG_ID
ItemHandler::getTagId() const
{
    Q_ASSERT(valid_tag);
    return itemtagheap->getTagId(this);
}

/*
 * Gets the cached tag id.
 *
 * @return the tag id.
 */
TAG_ID
ItemHandler::getTagCachedId() const
{
    return cached_tag_id;
}

/*
 * Returns a string with the formatted value of the actual tag.
 *
 * @return the formatted value.
 */
QString
ItemHandler::getTagFormatedValue() const
{
    UINT64 value;
    TAG_ID tagId;

    Q_ASSERT(valid_tag);

    tagId = itemtagheap->getTagId(this);
    value = itemtagheap->getTagValue(this);
    return tagdescvec->getFormatedTagValue(tagId, value);
}

/*
 * Caches the needed information in the compute algorithm. This
 * avoids several accesses along the internal information, and
 * with just one access all the information is gathered and cached.
 *
 * @return void.
 */
void
ItemHandler::cacheTag()
{
    Q_ASSERT(valid_tag);
    itemtagheap->cacheTag(this);
}

/**
 * Returns the type of event pointed by the handler.
 *
 * @return the type of event.
 */
ItemHeapEventSubType
ItemHandler::getEventType() const
{
    Q_ASSERT(valid_event);
    return itemtagheap->getEventType(this);
}

/**
 * Returns the cycle when the event happened.
 *
 * @return the cycle.
 */
CYCLE
ItemHandler::getEventCycle() const
{
    Q_ASSERT(valid_event);
    return itemtagheap->getEventCycle(this);
}

/*
 * Returns the event id.
 *
 * @return the event id.
 */
INT64
ItemHandler::getEventId() const
{
    Q_ASSERT(valid_event);
    return itemtagheap->getEventId(this);
}

/**
 * Returns the node id if the event is an Enter/Exit node.
 *
 * @return the node id.
 */
DRAL_ID
ItemHandler::getEventNodeId() const
{
    Q_ASSERT(valid_event);
    return itemtagheap->getEventNodeEdgeId(this);
}

/**
 * Returns the edge id of the move item.
 *
 * @return the edge id.
 */
DRAL_ID
ItemHandler::getEventEdgeId() const
{
    Q_ASSERT(valid_event);
    return itemtagheap->getEventNodeEdgeId(this);
}

/**
 * Returns the position of the move item or the enter/exit node.
 *
 * @return the position.
 */
INT32
ItemHandler::getEventPosition() const
{
    Q_ASSERT(valid_event);
    return itemtagheap->getEventPosition(this);
}

/**
 * Uses the itemtagheap to set the handler to point the next item.
 *
 * @return true if another item exists.
 */
bool
ItemHandler::nextItem()
{
    if(!valid_item)
    {
        return false;
    }
    else
    {
        return itemtagheap->nextItem(this);
    }
}

/**
 * Changes the handler state to point to the next tag of the
 * item of the handler.
 *
 * @return true if a next tag exists.
 */
bool
ItemHandler::nextTag()
{
    if(!valid_tag)
    {
        return false;
    }
    else
    {
        return itemtagheap->nextTag(this);
    }
}

/**
 * Changes the handler state to point to the first tag of the
 * item of the handler.
 *
 * @return void.
 */
void
ItemHandler::rewindToFirstTag()
{
    Q_ASSERT(valid_item);
    itemtagheap->resetTagState(this);
}

/**
 * Rewinds the handler to the first value of the current tag.
 *
 * @return void.
 */
bool
ItemHandler::nextEvent()
{
    if(!valid_event)
    {
        return false;
    }
    else
    {
        return itemtagheap->nextEvent(this);
    }
}

/*
 * Rewinds the handler to the first value of the current tag.
 *
 * @return void.
 */
void
ItemHandler::rewindToFirstEvent()
{
    Q_ASSERT(valid_item);
    itemtagheap->resetEventState(this);
}

/*
 * Rewinds the handler to the first value of the current tag.
 *
 * @return void.
 */
void
ItemHandler::goToCycle(CYCLE cycle)
{
    Q_ASSERT(valid_item);
    itemtagheap->goToCycle(this, cycle);
}

/*
 * Just returns the field's value.
 *
 * @return true if it's a valid item handler.
 */
bool
ItemHandler::isValidItemHandler() const
{
    return valid_item;
}

/**
 * Just returns the field's value.
 *
 * @return true if it's a valid tag handler.
 */
bool
ItemHandler::isValidTagHandler() const
{
    return valid_tag;
}

/**
 * Just returns the field's value.
 *
 * @return true if it's a valid event handler.
 */
bool
ItemHandler::isValidEventHandler() const
{
    return valid_event;
}

/**
 * Invalidates the state of the item handler.
 *
 * @return void.
 */
void
ItemHandler::invalid()
{
    valid_item = false;
    valid_tag = false;
    valid_event = false;
    item_id = ITEM_ID_INVALID;
    tag_chunk_idx = -1;
    event_chunk_idx = -1;
    itemtagheap = NULL;
}

/**
 * Compares two handlers and return if they point to the same 
 * entry.
 *
 * @return true if both handler points to the same entry.
 */
bool
ItemHandler::operator==(const ItemHandler & comp) const
{
    return
        (
            (this->item_id == comp.item_id) &&
            (this->tag_chunk_idx == comp.tag_chunk_idx) &&
            (this->tag_idx == comp.tag_idx) &&
            (this->event_chunk_idx == comp.event_chunk_idx) &&
            (this->event_idx == comp.event_idx) &&
            (this->itemtagheap == comp.itemtagheap)
        );
}

/**
 * Compares two handlers and return if they point to the same 
 * tag.
 *
 * @return true if both handler points to the same tag.
 */
bool
ItemHandler::sameTag(const ItemHandler & comp) const
{
    return
        (
            (this->item_id == comp.item_id) &&
            (this->tag_chunk_idx == comp.tag_chunk_idx) &&
            (this->tag_idx == comp.tag_idx) &&
            (this->itemtagheap == comp.itemtagheap)
        );
}

/**
 * Compares two handlers and return if they point to the same 
 * event.
 *
 * @return true if both handler points to the same event.
 */
bool
ItemHandler::sameEvent(const ItemHandler & comp) const
{
    return
        (
            (this->item_id == comp.item_id) &&
            (this->event_chunk_idx == comp.event_chunk_idx) &&
            (this->event_idx == comp.event_idx) &&
            (this->itemtagheap == comp.itemtagheap)
        );
}

/**
 * Returns an integer that is unique for the position where the
 * handler points to.
 *
 * @return a unique index for the handler.
 */
INT32
ItemHandler::uniqueIndex() const
{
    if(tag_chunk_idx == -1)
    {
        return item_id;
    }
    else
    {
        return (ITEM_MAX_ENTRIES + tag_chunk_idx * EVENTS_PER_CHUNK + tag_idx);
    }
}

/**
 * Returns a string that indicates where is pointing the handler.
 * Should only be used to debug purposes.
 *
 * @return the handler information.
 */
QString
ItemHandler::getHandlerInformation() const
{
    QString ret;

    ret = "item = " + QString::number(item_id);
    ret += " tag chunk = " + QString::number(tag_chunk_idx);
    ret += " tag = " + QString::number(tag_idx);
    ret += " event chunk = " + QString::number(event_chunk_idx);
    ret += " event = " + QString::number(event_idx);
    ret += " valid item = " + QString::number(valid_item);
    ret += " valid tag = " + QString::number(valid_tag);
    ret += " valid tag = " + QString::number(valid_event);
    return ret;
}

/*
 * Adds the tag of the destruction of the item.
 *
 * @return true if the item existed.
 */
bool
ItemHeap::endItemBlock(ITEM_ID itemId, CYCLE cycle)
{
    return newTag(itemId, delitem_tagid, (UINT64) 0, cycle);
}

/*
 * Returns the number of items inserted in the database.
 *
 * @return the number of items.
 */
INT32
ItemHeap::getNumItems() const
{
    return numItems;
}

/*
 * Sets the dral version.
 *
 * @return void.
 */
void
ItemHeap::setDralVersion(UINT16 value)
{
    dralVersion = value;
}

/*
 * Returns the dral version.
 *
 * @return dral version.
 */
UINT16
ItemHeap::getDralVersion() const
{
    return dralVersion;
}

/*
 * Sets the first effective cycle. We store the internal
 * clock to avoid some conversions later.
 *
 * @return void.
 */
void
ItemHeap::setFirstEffectiveCycle(CYCLE cycle)
{
    firstEffectiveCycle[cycle.clock->getInternalClock()] = cycle.cycle;
}

/*
 * Sets the number of clocks.
 *
 * @return void.
 */
void
ItemHeap::setNumClocks(UINT16 numClocks)
{
    Q_ASSERT(firstEffectiveCycle == NULL);
    firstEffectiveCycle = new INT32[numClocks];
}

/*
 * Initializes a handler to work with a given item. Only for
 * internal use!!!
 *
 * @return void.
 */
void
ItemHeap::newTagLookForItemId(ItemHandler * handler)
{
    // If the element isn't allocated then don't exist.
    if(!itemVector.hasElement(handler->item_id) || (itemVector.at(handler->item_id) == ITEM_UNUSED))
    {
        handler->valid_item = false;
        return;
    }
    else
    {
        handler->valid_item = true;
        resetTagState(handler);
    }
}

/*
 * Initializes a handler to work with a given item. Only for
 * internal use!!!
 *
 * @return void.
 */
void
ItemHeap::newEventLookForItemId(ItemHandler * handler)
{
    // If the element isn't allocated then don't exist.
    if(!itemVector.hasElement(handler->item_id) || (itemVector.at(handler->item_id) == ITEM_UNUSED))
    {
        handler->valid_item = false;
        return;
    }
    else
    {
        handler->valid_item = true;
    }
}

/**
 * Returns a pointer to the tag node pointed by handler.
 *
 * @return the pointer to the tag.
 */
ItemHeapTagNode *
ItemHeap::tagNode(const ItemHandler * handler) const
{
    return (ItemHeapTagNode *) &eventVector.ref(handler->tag_chunk_idx).content[handler->tag_idx];
}

/**
 * Returns a pointer to the tag node pointed by handler.
 *
 * @return the pointer to the tag.
 */
ItemHeapNode *
ItemHeap::eventNode(const ItemHandler * handler) const
{
    return &eventVector.ref(handler->event_chunk_idx).content[handler->event_idx];
}

/*
 * Resets the tag state of handler. This ItemHandler must be valid
 * and sets the fields tag_chunk_idx, tag_idx and valid_tag of
 * handler.
 *
 * @return void.
 */
void
ItemHeap::resetTagState(ItemHandler * handler)
{
    Q_ASSERT(handler->valid_item);

    // Points it to the first tag, that is always the ITEMID.
    // The physical separation of ITEMID from the other tags needs
    // to be solved with a hack. chunkIdx == -1 means that the handler
    // is actually pointing to this special tag.
    handler->tag_chunk_idx = -1;
    handler->tag_idx = -1;
    handler->valid_tag = true;
}

/*
 * Computes if the given chunk and slot index are the last entry
 * of an item.
 *
 * @return true if is the last.
 */
bool
ItemHeap::isLastEntry(INT32 chunk_idx, INT32 slot_idx) const
{
    if(
        eventVector.ref(chunk_idx).getNextChunk() == 0 &&
        (INT32) eventVector.ref(chunk_idx).getFirstFreeSlot() == slot_idx + 1
    )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * This function advances a chunk and index.
 *
 * @return void.
 */
void
ItemHeap::nextEntry(INT32 * chunk_idx, INT32 * entry_idx)
{
    // Points to the next tag.
    * entry_idx = * entry_idx + 1;

    // If has reached the end of the chunk.
    if(* entry_idx == EVENTS_PER_CHUNK)
    {
        Q_ASSERT(eventVector.ref(* chunk_idx).getNextChunk() > 0);
        // Set the chunk as the next one and the entry_idx is the first one of the chunk.
        * chunk_idx += eventVector.ref(* chunk_idx).getNextChunk();
        * entry_idx = 0;
    }
}

/*
 * Returns the cycle when the actual tag was set.
 *
 * @return the cycle.
 */
CYCLE
ItemHeap::getTagCycle(const ItemHandler * handler) const
{
    if(handler->tag_chunk_idx == -1)
    {
        return CYCLE(clocks->getClockDomain(0), 0, 0);
    }
    else
    {
        ItemHeapTagNode * node = tagNode(handler);

        return CYCLE(clocks->getClockDomain(clocks->internalToExternalClock(node->clockId)), node->cycle + firstEffectiveCycle[node->clockId], node->division);
    }
}

/*
 * Returns the value that is stored in the handler entry of the heap.
 * The dictionary is used to know the value using the key and cycle.
 *
 * @return the value in the handler position.
 */
UINT64
ItemHeap::getTagValue(const ItemHandler * handler) const
{
    // Special case when is pointing to ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        return (UINT64) handler->item_id;
    }
    else
    {
        ItemHeapTagNode * node = tagNode(handler);
        return dict->getValueByCycle(node->dkey, node->cycle, clocks->internalToExternalClock(node->clockId));
    }
}

/*
 * Returns if the value that is stored in the handler entry of the heap
 * is defined.
 *
 * @return if the value is defined.
 */
bool
ItemHeap::getTagDefined(const ItemHandler * handler) const
{
    // Special case when is pointing to ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        return true;
    }
    else
    {
        return true;
    }
}

/**
 * Returns the tag id of the item in the handler.
 *
 * @return the tag id.
 */
TAG_ID
ItemHeap::getTagId(const ItemHandler * handler) const
{
    // Special case when is pointing to ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        return itemid_tagid;
    }
    else
    {
        return (TAG_ID) tagNode(handler)->tagId;
    }
}

/**
 * Returns if the handler is a SOV or not.
 *
 * @return true if is a SOV. False otherwise.
 */
bool
ItemHeap::getTagIsSOV(const ItemHandler * handler) const
{
    // Special case when is pointing to ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        return false;
    }
    else
    {
        return tagNode(handler)->isSOV;
    }
}

/*
 * Returns true if the handler is mutable. False otherwise.
 *
 * @return if the position is mutable.
 */
bool
ItemHeap::getTagIsMutable(const ItemHandler * handler) const
{
    // Special case when is pointing to ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        return false;
    }
    else
    {
        return tagNode(handler)->isMutable;
    }
}

/*
 * Returns the type of tag where the handler is pointing.
 *
 * @return the tag type.
 */
TagValueType
ItemHeap::getTagType(const ItemHandler * handler) const
{
    // Special case when is pointing to ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        return tdv->getTagValueType(itemid_tagid);
    }
    else
    {
        return tdv->getTagValueType(tagNode(handler)->tagId);
    }
}

/*
 * Returns the base of the tag where the handler is pointing.
 *
 * @return the tag base.
 */
INT16
ItemHeap::getTagBase(const ItemHandler * handler) const
{
    // Special case when is pointing to ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        return tdv->getTagValueBase(itemid_tagid);
    }
    else
    {
        return tdv->getTagValueBase(tagNode(handler)->tagId);
    }
}

/*
 * Caches the needed information in the compute algorithm. This
 * avoids several accesses along the internal information, and
 * with just one access all the information is gathered and cached.
 *
 * @return void.
 */
void
ItemHeap::cacheTag(ItemHandler * handler)
{
    // Special case when is pointing to ITEMID.
    if(handler->tag_chunk_idx == -1)
    {
        handler->cached_tag_cycle.clock = clocks->getClockDomain(0);
        handler->cached_tag_cycle.cycle = 0;
        handler->cached_tag_cycle.division = 0;
        handler->cached_tag_cycle_lcm = 0;
        handler->cached_tag_value = (UINT64) handler->item_id;
        handler->cached_tag_defined = true;
        handler->cached_tag_id = itemid_tagid;
    }
    else
    {
        ItemHeapTagNode * node;

        node = tagNode(handler);
        CLOCK_ID clock_id = clocks->internalToExternalClock(node->clockId);
        handler->cached_tag_cycle.cycle = node->cycle + firstEffectiveCycle[node->clockId];
        handler->cached_tag_cycle.clock = clocks->getClockDomain(clock_id);
        handler->cached_tag_cycle.division = node->division;
        handler->cached_tag_cycle_lcm = handler->cached_tag_cycle.toLCMCycles();
        handler->cached_tag_value = dict->getValueByCycle(node->dkey, node->cycle, clock_id);
        handler->cached_tag_defined = true;
        handler->cached_tag_id = node->tagId;
    }
}

/**
 * Returns the event type where is pointing the handler.
 *
 * @return the event type.
 */
ItemHeapEventSubType
ItemHeap::getEventType(const ItemHandler * handler) const
{
    return (ItemHeapEventSubType) eventNode(handler)->event.eventType;
}

/**
 * Returns the event cycle of the event pointed by the handler.
 *
 * @return the cycle.
 */
CYCLE
ItemHeap::getEventCycle(const ItemHandler * handler) const
{
    ItemHeapEventNode * event = (ItemHeapEventNode *) eventNode(handler);

    return CYCLE(clocks->getClockDomain(clocks->internalToExternalClock(event->clockId)), event->cycle + firstEffectiveCycle[event->clockId], event->division);
}

/**
 * Returns the event id of the event pointed by the handler.
 *
 * @return the event id.
 */
INT64
ItemHeap::getEventId(const ItemHandler * handler) const
{
    return eventNode(handler)->event.event_id;
}

/**
 * Returns the event node/edge id of the event pointed by the handler.
 *
 * @return the id.
 */
INT16
ItemHeap::getEventNodeEdgeId(const ItemHandler * handler) const
{
    return (INT16) eventNode(handler)->move.node_edge;
}

/**
 * Returns the event position of the event pointed by the handler.
 *
 * @return the position
 */
INT32
ItemHeap::getEventPosition(const ItemHandler * handler) const
{
    return (INT32) eventNode(handler)->move.pos;
}

/**
 * This function tells if the node pointed by the handler is a
 * tag.
 *
 * @return if is pointing a tag.
 */
bool
ItemHeap::isTagNode(const ItemHandler * handler) const
{
    return (tagNode(handler)->type == IDB_TAG);
}

/**
 * This function tells if the node pointed by the handler is an
 * event.
 *
 * @return if is pointing an event.
 */
bool
ItemHeap::isEventNode(const ItemHandler * handler) const
{
    return (eventNode(handler)->event.type == IDB_EVENT);
}

#endif

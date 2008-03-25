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

/*
 * @file TagFlipHandler.h
 * @brief
 */

#ifndef _DRALDB_TAGFLIPHANDLER_H
#define _DRALDB_TAGFLIPHANDLER_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/taghandler/TagHandler.h"
#include "dDB/itemheap/ItemHeap.h"

// Class forwarding.
class TrackHeap;
class DralDB;

/*
 * @typedef struct TagFlipHandlerEnv
 * Contains all the data that the TagFlipHandler's needs.
 */
typedef struct TagFlipHandlerEnv
{
    TAG_ID numTags;              ///< Number of tags.
    TagValueEntry itemTags;      ///< Pointers to the item tags.
    set<TAG_ID> * usedItemTags;  ///< List of the item tags used in the rules.
    TRACK_ID slotTrackId;        ///< Track id of the slot.
    TagValueEntry slotTags;      ///< Pointers to the slot tags.
    set<TAG_ID> * usedSlotTags;  ///< List of the slot tags used in the rules.
    TRACK_ID nodeTrackId;        ///< Track id of the node.
    TagValueEntry nodeTags;      ///< Pointers to the node tags.
    set<TAG_ID> * usedNodeTags;  ///< List of the node tags used in the rules.
    TRACK_ID cycleTrackId;       ///< Track id of the cycle.
    TagValueEntry cycleTags;     ///< Pointers to the cycle tags.
    set<TAG_ID> * usedCycleTags; ///< List of the cycle tags used in the rules.
} ;

/*
 * @brief
 * This class is used to have a sequential access to the
 * cycles when a tag of a slot changes (and also can know
 * the tag that has changed, it's value and if it's defined).
 *
 * @description
 * This class is used in the computing of the CSL to just compute
 * the colors in the cycles where something has changed in the slot.
 * Doing so, we recycle the result for one cycle if in the following
 * cycles nothing changes. This is critcal in cases where large
 * structures (such caches) are all the time filled with items. In this
 * cases, almost none of the slots changes, but all of them need to
 * be computed.
 * 
 *
 */
class TagFlipHandler
{
    private:
        class InternalTagFlipHandler
        {
            public:
                InternalTagFlipHandler();
                ~InternalTagFlipHandler();

                void init(DralDB * draldb, TRACK_ID trackId, set<TAG_ID> * tagList);

                void skipToNextCycleWithChange();
                void skipToCycle(CYCLE base_cycle);

                inline UINT64 getTagValue();
                inline CYCLE getTagCycle();
                inline TAG_ID getTagId();
                inline bool getTagDefined();

                void dump();

            private:
                TagHandler ** tags; ///< Pointer to the different tag handlers.
                TRACK_ID track_id;  ///< Track Id of the Tag Flip Handler.
                TAG_ID num_tags;    ///< The size of the tag array.
                TAG_ID current_tag; ///< Points to the index of the tag with the lowest cycle change.
        } ;

    public:
        TagFlipHandler();
        ~TagFlipHandler();

        void init(DralDB * draldb, TagFlipHandlerEnv * env);

        bool skipToNextCycleWithChange(CYCLE base_cycle, CYCLE * next_change_cycle);
        void skipToCycle(CYCLE base_cycle);

    private:
        ItemHandler hnd;                  ///< The handler of the item inside the slot.
        set<TAG_ID> usedItemTags;         ///< List of the item tags used.
        TagValueEntry itemTags;           ///< Pointers to the item tags.
        InternalTagFlipHandler slotFlip;  ///< The slot tag flip handler.
        TagValueEntry slotTags;           ///< Pointers to the slot tags.
        InternalTagFlipHandler nodeFlip;  ///< The root node tag flip handler.
        TagValueEntry nodeTags;           ///< Pointers to the node tags.
        InternalTagFlipHandler cycleFlip; ///< The cycle tag flip handler.
        TagValueEntry cycleTags;          ///< Pointers to the cycle tags.
        DralDB * draldb;                  ///< Pointer to the dral database.
        TAG_ID slotItem;                  ///< Tag id where the items are stored inside a slot.
        TAG_ID numTags;                   ///< Number of tags.
} ;

/*
 * Returns the value of the tag that has the change in the lowest
 * cycle.
 *
 * @return returns the value of the current change.
 */
UINT64
TagFlipHandler::InternalTagFlipHandler::getTagValue()
{
    return tags[current_tag]->getTagValue();
}

/*
 * Returns the cycle when the tag that has the change in the lowest
 * cycle.
 *
 * @return returns the cycle of the current change.
 */
CYCLE
TagFlipHandler::InternalTagFlipHandler::getTagCycle()
{
    return tags[current_tag]->getTagCycle();
}

/*
 * Returns the id of the tag that has the change in the lowest
 * cycle.
 *
 * @return the tag id.
 */
TAG_ID
TagFlipHandler::InternalTagFlipHandler::getTagId()
{
    return tags[current_tag]->getTagId();
}

/*
 * Returns if is defined the tag that has the change in the lowest
 * cycle.
 *
 * @return returns if is defined the current change.
 */
bool
TagFlipHandler::InternalTagFlipHandler::getTagDefined()
{
    return tags[current_tag]->getTagDefined();
}

#endif

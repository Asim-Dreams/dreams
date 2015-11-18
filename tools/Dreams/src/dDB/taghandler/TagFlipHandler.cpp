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
 * @file TagFlipHandler.cpp
 * @brief
 */

// General includes.
#include <iostream>
#include <set>
using namespace std;

// Dreams includes.
#include "dDB/DralDB.h"
#include "dDB/taghandler/TagFlipHandler.h"
#include "dDB/trackheap/TrackHeap.h"

/*
 * Creator of the class. Nothing done.
 *
 * @return the new object.
 */
TagFlipHandler::TagFlipHandler()
{
}

/*
 * Destructor of the class.
 *
 * @return deletes the object.
 */
TagFlipHandler::~TagFlipHandler()
{
}

/*
 * Initializes the tag handler with all the environment information.
 *
 * @return void.
 */
void
TagFlipHandler::init(DralDB * _dralDB, TagFlipHandlerEnv * env)
{
    draldb = _dralDB;
    slotItem = draldb->getSlotItemTagId();
    numTags = env->numTags;

    itemTags = env->itemTags;
    slotTags = env->slotTags;
    nodeTags = env->nodeTags;
    cycleTags = env->cycleTags;

    // Sets all the tags as invalid.
    memset(itemTags.valids, 0, numTags);
    memset(slotTags.valids, 0, numTags);
    memset(nodeTags.valids, 0, numTags);
    memset(cycleTags.valids, 0, numTags);

    usedItemTags = * (env->usedItemTags);
    slotFlip.init(draldb, env->slotTrackId, env->usedSlotTags);
    // TODO: implement them when needed...
    // WARNING: possible problems if the track id is invalid, some assert may abort the program...
    //nodeFlip.init(draldb, env->nodeTrackId, env->usedNodeTags);
    //cycleFlip.init(draldb, env->cycleTrackId, env->usedCycleTags);
}

/*
 * Updates all the tags of all the elements and returns the
 * next cycle with change.
 *
 */
bool
TagFlipHandler::skipToNextCycleWithChange(CYCLE base_cycle, CYCLE * next_change_cycle)
{
    TAG_ID tagId;            ///< Stores a tag id.
    INT64 lcm_cycle;         ///< Base cycle multiplied by the LCM ratio.
    bool changes = false;    ///< Is something changed.
    bool defined;            ///< Is the tag defined
    UINT64 value;            ///< Tag value

    // Gets the absolut cycle.
    lcm_cycle = base_cycle.toLCMCycles();

    // Updates the different values of the slot tags.
    // As the node works with the same frequency, no need to work with LCM ratios.
    while(slotFlip.getTagCycle().toLCMCycles() <= lcm_cycle)
    {
        tagId = slotFlip.getTagId();
        defined = slotFlip.getTagDefined();
        value = slotFlip.getTagValue();
        slotTags.valids[tagId] = defined;
        slotTags.values[tagId] = value;

        // Checks if the item has changed.
        if(tagId == slotItem)
        {
            // Clears the valid array.
            memset(itemTags.valids, 0, numTags);

            // Checks if there's a new item inside.
            if(defined)
            {
                // Initializes the item handler.
                draldb->lookForItemId(&hnd, value);
                Q_ASSERT(hnd.isValidItemHandler());

                // Caches the tag.
                // WARNING: we know that at least 1 tag exist: NEW_ITEM, so the assert
                // of the cacheTag function won't be raised.
                hnd.cacheTag();
            }
            else
            {
                hnd.invalid();
            }
        }

        // Something has changed...
        changes = true;

        // Moves to the next change.
        slotFlip.skipToNextCycleWithChange();
    }

    // Gets the next change cycle.
    * next_change_cycle = slotFlip.getTagCycle();

    // Gets the actual state of the item inside.
    while(hnd.isValidTagHandler() && (draldb->getTagBackPropagate() || (hnd.getTagCachedCycleLCM() <= lcm_cycle)))
    {
        tagId = hnd.getTagCachedId();
        // Do not overwrite the tag value with a future value if it was already set (this avoid that a future value with backprogation
        // overwrittes the current value)
        if ((hnd.getTagCachedCycleLCM() <= lcm_cycle) || (! itemTags.valids[tagId]))
        {
            itemTags.valids[tagId] = hnd.getTagCachedDefined();
            itemTags.values[tagId] = hnd.getTagCachedValue();

            // Something has changed...
            changes = true;
        }

        // Moves to the next tag and if exists...
        if(hnd.nextTag())
        {
            // ... caches all the information.
            hnd.cacheTag();
        }
    }

    // If it is a valid tag handler, then gets the lowest change.
    if(hnd.isValidTagHandler())
    {
        INT64 temp;

        temp = hnd.getTagCachedCycleLCM();
        if(temp < next_change_cycle->toLCMCycles())
        {
            next_change_cycle->fromLCMCycles(temp);
        }
    }

    // If different clocks are being used, in some cases an event happens
    // after the base cycle, but when is converted to cycles and phases of
    // the node clock domain, the result cycle is the same as the base cycle.
    // This happens because the LCM cycles are truncated in the conversion,
    // so if this happens, we move to the next phase.
    if((next_change_cycle->cycle == base_cycle.cycle) && (next_change_cycle->division == base_cycle.division))
    {
        * next_change_cycle = base_cycle;
        if(base_cycle.division == 0)
        {
            next_change_cycle->division = 1;
        }
        else
        {
            next_change_cycle->cycle++;
            next_change_cycle->division = 0;
        }
    }
    return changes;
}

/*
 * Gets the next change starting at base cycle.
 *
 * @return if there's a change.
 */
void
TagFlipHandler::skipToCycle(CYCLE base_cycle)
{
    slotFlip.skipToCycle(base_cycle);
}

/*
 * Creator of the class. Just sets to null the tag pointer.
 *
 * @return the new object.
 */
TagFlipHandler::InternalTagFlipHandler::InternalTagFlipHandler()
{
    tags = NULL;
}

/*
 * Destructor of the class. Deletes the array of tags if allocated.
 *
 * @return the new object.
 */
TagFlipHandler::InternalTagFlipHandler::~InternalTagFlipHandler()
{
    if(tags != NULL)
    {
        // Deletes all the tags.
        for(UINT32 i = 0; i < num_tags; i++)
        {
            delete tags[i];
        }

        // Deletes the allocated array.
        delete [] tags;
    }
}

/*
 * Initializes the tag handler with a list of tags.
 *
 * @return void.
 */
void
TagFlipHandler::InternalTagFlipHandler::init(DralDB * draldb, TRACK_ID trackId, set<TAG_ID> * tagList)
{
    CYCLE min_cycle;              ///< Lowest cycle of the tags changes.
    INT32 count;                  ///< Counter to index the array of tags.
    set<TAG_ID>::iterator it_tag; ///< Iterator that iterates along the tags.

    // Allocs 1 extra space.
    num_tags = tagList->size() + 1;

    count = 1;
    tags = new TagHandler *[num_tags];
    current_tag = 0;
    track_id = trackId;

    TrackHeap * track_heap = draldb->getTrackHeap();
    // The first tag is set to infinite. Avoids checking ifs to check if is a tag flip
    // handler with 0 tags.
    // The first tag is initialized with a cycle that is out of bounds, so is like a
    // phantom element, that prevents the cases with 0 tags.
    min_cycle.clock = draldb->getClockDomain(track_heap->getClockId(trackId));
    min_cycle.fromLCMCycles(draldb->getLastCycle().toLCMCycles());
    track_heap->initTagHandler(&tags[0], track_id, 0, min_cycle, true);
    tags[0]->skipToNextCycleWithChange();

    // For each tag.
    for(it_tag = tagList->begin(); it_tag != tagList->end(); it_tag++)
    {
        // Inits the tag handler.
        track_heap->initTagHandler(&tags[count], track_id, * it_tag, min_cycle, true);

        // If the value is undefined, then moves the handler to the next change.
        if(!tags[count]->getTagDefined())
        {
            tags[count]->skipToNextCycleWithChange();
        }

        // Checks if the actual tag is the one with the lowest change.
        if(tags[count]->getTagCycle() < min_cycle)
        {
            // Updates both the minimum cycle and the current tag.
            min_cycle = tags[count]->getTagCycle();
            current_tag = count;
        }

        count++;
    }
}

/*
 * Skips to the next change of the tag with the lowest cycle change
 * (actually the last consulted cycle) and recomputes which is the
 * tag that has the lowest change.
 *
 * @return void.
 */
void
TagFlipHandler::InternalTagFlipHandler::skipToNextCycleWithChange()
{
    CYCLE min_cycle; ///< Lowest cycle of the tags changes.

    // Skips the lowest tag.
    tags[current_tag]->skipToNextCycleWithChange();

    min_cycle = tags[0]->getTagCycle();
    current_tag = 0;

    // Recomputes the youngest.
    for(UINT16 i = 1; i < num_tags; i++)
    {
        // Checks if the actual tag is the one with the lowest change.
        if(tags[i]->getTagCycle() < min_cycle)
        {
            // Updates both the minimum cycle and the current tag.
            min_cycle = tags[i]->getTagCycle();
            current_tag = i;
        }
    }
}

/*
 * Moves the handler to the requested cycle
 *
 * @return void.
 */
void
TagFlipHandler::InternalTagFlipHandler::skipToCycle(CYCLE base_cycle)
{
    current_tag = 0;

    for(UINT32 i = 0; i < num_tags; i++)
    {
        tags[i]->skipToCycle(base_cycle);
    }
}

/*
 * Dumps the content of the different tags.
 *
 * @return void.
 */
void
TagFlipHandler::InternalTagFlipHandler::dump()
{
    cout << "Dumping TagFlipHandler:" << endl;
    for(UINT16 i = 0; i < num_tags; i++)
    {
        cout << "TagID: " << tags[i]->getTagId() << ", Cycle: " << tags[i]->getTagCycle().toString(CYCLE_MASK_ALL) << ", Value: " << tags[i]->getTagValue() << ", Defined: " << tags[i]->getTagDefined() << endl;
    }
    cout << endl;
}

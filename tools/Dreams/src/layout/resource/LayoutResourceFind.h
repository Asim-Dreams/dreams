// ==================================================
// Copyright (C) 2006 Intel Corporation
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
 * @file LayoutResourceFind.h
 */

#ifndef _LAYOUTRESOURCEFIND_H
#define _LAYOUTRESOURCEFIND_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/ClockDomainMgr.h"
#include "dDB/taghandler/TagHandler.h"

// Class forwarding.
class DralDB;

/*
 * @brief
 * Struct that has all the resource find options.
 *
 * @description
 * 
 *
 * @author Guillem Sole
 * @date started at 2005-01-17
 * @version 0.1
 */
class LayoutResourceFindOptions
{
    public:
        LayoutResourceFindOptions();
        ~LayoutResourceFindOptions();

        inline void setFindHigh(bool value);
        inline void setFindLow(bool value);

        inline bool getFindHigh() const;
        inline bool getFindLow() const;

    private:
        bool findHigh; ///< Must search in the items of the high phase.
        bool findLow;  ///< Must search in the items of the low phase.
} ;

/*
 * @brief
 * Struct that allocates all the information to find a value in the
 * resource view.
 *
 * @description
 * 
 *
 * @author Guillem Sole
 * @date started at 2005-01-03
 * @version 0.1
 */
class LayoutResourceFindDataEntry
{
    public:
        LayoutResourceFindDataEntry(UINT32 _row, TRACK_ID _trackId, CYCLE startCycle, UINT64 _drawReceivedDisp, DralDB * draldb);
        ~LayoutResourceFindDataEntry();

        inline void skipToNextItem();

        inline bool isValidDataEntry();
        inline UINT32 getRow() const;
        inline UINT64 getCycle() const;
        inline TRACK_ID getTrackId() const;
        inline ITEM_ID getItemId() const;
        inline CLOCK_ID getClockId() const;

    private:
        inline void computeCycle();

    private:
        UINT32 row;                     ///< Row where this entry is associated.
        TRACK_ID trackId;               ///< The track id of the row.
        UINT64 cycle;                   ///< Actual cycle (in LCM ticks) of the entry.
        UINT64 drawReceivedDisp;        ///< Displacement due that the row where the item is traveling is draw when received.
        TagHandler * items;             ///< Tag handler that runs through the items inside the row.
        const ClockDomainEntry * clock; ///< Pointer to the clock domain of the entry.
} ;

/*
 * @struct ltentry
 *
 * Struct that compares two data entries.
 */
struct ltentry
{
    bool operator()(const LayoutResourceFindDataEntry * entry1, const LayoutResourceFindDataEntry * entry2) const
    {
        if(entry1->getCycle() < entry2->getCycle())
        {
            return true;
        }
        else if(entry1->getCycle() > entry2->getCycle())
        {
            return false;
        }
        else
        {
            return entry1->getRow() < entry2->getRow();
        }
    } ;
} ;

/*
 * @typedef LayoutResourceFindDataSet, LayoutResourceFindDataIterator
 *
 * Structs used to access to a sorted set of find data entries.
 */
typedef set<LayoutResourceFindDataEntry *, ltentry> LayoutResourceFindDataSet;
typedef set<LayoutResourceFindDataEntry *, ltentry>::iterator LayoutResourceFindDataIterator;

/*
 * Advances the tag handler to the next item.
 *
 * @return void.
 */
void
LayoutResourceFindDataEntry::skipToNextItem()
{
    // TODO: the user may specify a rule that sets the event
    // as transparent (implementes some kind of filter), so
    // the entry is pointing to nothing in the resource view.
    // Implement code that checks that a color is defined for
    // the actual cycle. Same for FloorPlan.
    items->skipToNextCycleWithChange();

    if((items->getTagCycle().cycle != BIGGEST_CYCLE) && !items->getTagDefined())
    {
        items->skipToNextCycleWithChange();
    }
    computeCycle();
}

/*
 * This function returns if the entry is valid using the actual
 * cycle of the tag handler.
 *
 * @return if the entry is valid.
 */
bool
LayoutResourceFindDataEntry::isValidDataEntry()
{
    return (items->getTagCycle().cycle != BIGGEST_CYCLE);
}

/*
 * Returns the row of this entry.
 *
 * @return the row of the entry.
 */
UINT32
LayoutResourceFindDataEntry::getRow() const
{
    return row;
}

/*
 * Returns the cycle of this entry.
 *
 * @return the cycle of the entry.
 */
UINT64
LayoutResourceFindDataEntry::getCycle() const
{
    return cycle;
}

/*
 * Returns the track id of this entry.
 *
 * @return the track id of the entry.
 */
TRACK_ID
LayoutResourceFindDataEntry::getTrackId() const
{
    return trackId;
}

/*
 * Returns the item id of this entry. Uses the tag handler to know
 * the actual item id.
 *
 * @return the item id of the entry.
 */
ITEM_ID
LayoutResourceFindDataEntry::getItemId() const
{
    return (ITEM_ID) items->getTagValue();
}

/*
 * Returns the clock id of this entry.
 *
 * @return the clock id of the entry.
 */
CLOCK_ID
LayoutResourceFindDataEntry::getClockId() const
{
    return clock->getId();
}

/*
 * Computes the lcm cycles.
 *
 * @return void.
 */
void
LayoutResourceFindDataEntry::computeCycle()
{
    cycle = items->getTagCycle().toLCMCycles() + drawReceivedDisp;
}

/*
 * Sets if the high phase must be searched.
 *
 * @return void.
 */
void
LayoutResourceFindOptions::setFindHigh(bool value)
{
    findHigh = true;
}

/*
 * Sets if the low phase must be searched.
 *
 * @return void.
 */
void
LayoutResourceFindOptions::setFindLow(bool value)
{
    findLow = true;
}

/*
 * Gets if the high phase must be searched.
 *
 * @return search in the high phase.
 */
bool
LayoutResourceFindOptions::getFindHigh() const
{
    return findHigh;
}

/*
 * Gets if the low phase must be searched.
 *
 * @return search in the low phase.
 */
bool
LayoutResourceFindOptions::getFindLow() const
{
    return findLow;
}

#endif // _LAYOUTRESOURCEFIND_H

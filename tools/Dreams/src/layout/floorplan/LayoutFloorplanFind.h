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
 * @file LayoutFloorplanFind.h
 */

#ifndef _LAYOUTFLOORPLANFIND_H
#define _LAYOUTFLOORPLANFIND_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/ClockDomainMgr.h"
#include "dDB/taghandler/TagHandler.h"

// Class forwarding.
class DralDB;

/*
 * @brief
 * Struct that has all the floorplan find options.
 *
 * @description
 * 
 *
 * @author Guillem Sole
 * @date started at 2005-02-08
 * @version 0.1
 */
class LayoutFloorplanFindOptions
{
    public:
        LayoutFloorplanFindOptions();
        ~LayoutFloorplanFindOptions();

    private:
} ;

/*
 * @brief
 * Struct that allocates all the information to find a value in the
 * Floorplan view.
 *
 * @description
 * 
 *
 * @author Guillem Sole
 * @date started at 2005-01-03
 * @version 0.1
 */
class LayoutFloorplanFindDataEntry
{
    public:
        LayoutFloorplanFindDataEntry(UINT32 _row, UINT32 _col, TRACK_ID _trackId, CYCLE startCycle, DralDB * draldb);
        ~LayoutFloorplanFindDataEntry();

        inline void skipToNextItem();

        inline bool isValidDataEntry();
        inline UINT32 getRow() const;
        inline UINT32 getCol() const;
        inline UINT64 getCycle() const;
        inline TRACK_ID getTrackId() const;
        inline ITEM_ID getItemId() const;
        inline CLOCK_ID getClockId() const;

    private:
        inline void computeCycle();

    private:
        UINT32 row;                     ///< Row where this entry is.
        UINT32 col;                     ///< Col where this entry is.
        TRACK_ID trackId;               ///< The track id of the row.
        UINT64 cycle;                   ///< Actual cycle (in LCM ticks) of the entry.
        TagHandler * items;             ///< Tag handler that runs through the items inside the row.
        const ClockDomainEntry * clock; ///< Pointer to the clock domain of the entry.
} ;

/*
 * @struct ltentry
 *
 * Struct that compares two data entries.
 */
struct ltfloorplanentry
{
    bool operator()(const LayoutFloorplanFindDataEntry * entry1, const LayoutFloorplanFindDataEntry * entry2) const
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
            if(entry1->getRow() < entry2->getRow())
            {
                return true;
            }
            else if(entry1->getRow() > entry2->getRow())
            {
                return false;
            }
            else
            {
                return (entry1->getCol() < entry2->getCol());
            }
        }
    } ;
} ;

/*
 * @typedef LayoutFloorplanFindDataSet
 *
 * Structs used to access to a sorted set of find data entries.
 */
typedef set<LayoutFloorplanFindDataEntry *, ltfloorplanentry> LayoutFloorplanFindDataSet;

/*
 * Advances the tag handler to the next item.
 *
 * @return void.
 */
void
LayoutFloorplanFindDataEntry::skipToNextItem()
{
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
LayoutFloorplanFindDataEntry::isValidDataEntry()
{
    return (items->getTagCycle().cycle != BIGGEST_CYCLE);
}

/*
 * Returns the row of this entry.
 *
 * @return the row of the entry.
 */
UINT32
LayoutFloorplanFindDataEntry::getRow() const
{
    return row;
}

/*
 * Returns the col of this entry.
 *
 * @return the col of the entry.
 */
UINT32
LayoutFloorplanFindDataEntry::getCol() const
{
    return col;
}

/*
 * Returns the cycle of this entry.
 *
 * @return the cycle of the entry.
 */
UINT64
LayoutFloorplanFindDataEntry::getCycle() const
{
    return cycle;
}

/*
 * Returns the track id of this entry.
 *
 * @return the track id of the entry.
 */
TRACK_ID
LayoutFloorplanFindDataEntry::getTrackId() const
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
LayoutFloorplanFindDataEntry::getItemId() const
{
    return (ITEM_ID) items->getTagValue();
}

/*
 * Returns the clock id of this entry.
 *
 * @return the clock id of the entry.
 */
CLOCK_ID
LayoutFloorplanFindDataEntry::getClockId() const
{
    return clock->getId();
}

/*
 * Computes the lcm cycles.
 *
 * @return void.
 */
void
LayoutFloorplanFindDataEntry::computeCycle()
{
    cycle = items->getTagCycle().toLCMCycles();
}

#endif // _LAYOUTFLOORPLANFIND_H

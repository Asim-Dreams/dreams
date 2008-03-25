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
 * @file LayoutFloorplanFind.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "LayoutFloorplanFind.h"

/*
 * Creator of this class. The startCycle is given in the same clock
 * domain of the row. Checks if high or low phase using the division
 * of the clock.
 *
 * @return new object.
 */
LayoutFloorplanFindDataEntry::LayoutFloorplanFindDataEntry(UINT32 _row, UINT32 _col, TRACK_ID _trackId, CYCLE startCycle, DralDB * draldb)
{
    // Gets the row and the clock domain.
    row = _row;
    col = _col;
    trackId = _trackId;
    clock = startCycle.clock;

    // Starts the tag handler.
    draldb->initTagHandler(&items, trackId, draldb->getSlotItemTagId(), startCycle, false);

    computeCycle();
}

/*
 * Destructor of the class.
 *
 * @return destroys the object.
 */
LayoutFloorplanFindDataEntry::~LayoutFloorplanFindDataEntry()
{
    delete items;
}

/*
 * Sets the default values.
 *
 * @return new object.
 */
LayoutFloorplanFindOptions::LayoutFloorplanFindOptions()
{
}

/*
 * Destructor of the class.
 *
 * @return destroys the object.
 */
LayoutFloorplanFindOptions::~LayoutFloorplanFindOptions()
{
}

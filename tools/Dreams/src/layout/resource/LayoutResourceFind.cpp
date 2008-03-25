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
 * @file LayoutResourceFind.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "layout/resource/LayoutResourceFind.h"

/*
 * Creator of this class. The startCycle is given in the same clock
 * domain of the row. Checks if high or low phase using the division
 * of the clock.
 *
 * @return new object.
 */
LayoutResourceFindDataEntry::LayoutResourceFindDataEntry(UINT32 _row, TRACK_ID _trackId, CYCLE startCycle, UINT64 _drawReceivedDisp, DralDB * draldb)
{
    // Gets the row and the clock domain.
    row = _row;
    trackId = _trackId;
    clock = startCycle.clock;
    drawReceivedDisp = _drawReceivedDisp;

    // Starts the tag handler.
    draldb->initTagHandlerPhase(&items, trackId, draldb->getSlotItemTagId(), startCycle, false);

    computeCycle();
}

/*
 * Destructor of the class.
 *
 * @return destroys the object.
 */
LayoutResourceFindDataEntry::~LayoutResourceFindDataEntry()
{
    delete items;
}

/*
 * Sets the default values.
 *
 * @return new object.
 */
LayoutResourceFindOptions::LayoutResourceFindOptions()
{
    setFindHigh(true);
    setFindLow(true);
}

/*
 * Destructor of the class.
 *
 * @return destroys the object.
 */
LayoutResourceFindOptions::~LayoutResourceFindOptions()
{
}

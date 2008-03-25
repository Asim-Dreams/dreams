// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file  EventItem.h
  */

#ifndef _EVENTITEM_H
#define _EVENTITEM_H

#include <stdio.h>
#include <stdlib.h>

#include "asim/AGTTypes.h"
#include "DefaultSizes.h"

/**
  * This class holds the very basic information needed for each event on the event matrix.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class EventItem
{
    public:
        inline EventItem();
        inline EventItem(UINT8 shapecolor, bool nodeEvent);
        inline ~EventItem();

        inline UINT8 getShapeColor() const;
        inline void setShapeColor(UINT8);

    // Our goal is to keep this object as small
    // as possible, currently only 2 bytes
    protected:
        // just a combination of shape and color
        // for the event (index into the row descriptor);
        // 0 is reserver for special cases.
        UINT8 shapeColor;
};


EventItem::EventItem()
{
    shapeColor   = EITEM_NON_INIT_COLORSHAPE;
}

EventItem::EventItem(UINT8 shapecolor, bool nodeEvent)
{
    Q_ASSERT (shapecolor <= EITEM_MAX_COLORSHAPE_INDEX);
    shapeColor = shapecolor;
}

EventItem::~EventItem() {}

UINT8
EventItem::getShapeColor() const
{ return (UINT32) shapeColor; }

void
EventItem::setShapeColor(UINT8 value)
{
    //Q_ASSERT (value <= EITEM_MAX_COLORSHAPE_INDEX);
    shapeColor=value;
    //printf ("##### EventItem:: setting shapecolor to %hu\n",shapeColor);fflush(stdout);
}

#endif


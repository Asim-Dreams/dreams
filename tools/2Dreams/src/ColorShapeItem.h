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
  * @file  ColorShapeItem.h
  */

#ifndef _COLORSHAPEITEM_H
#define _COLORSHAPEITEM_H

#include <stdio.h>
#include <stdlib.h>

#include <qcolor.h>

#include "asim/DralDB.h"

#include "EventShape.h"
#include "AColorPalette.h"

/**
  * This class combines a pallete color and an event shape.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class ColorShapeItem
{
    public:

        ColorShapeItem ();
        ColorShapeItem (EventShape sh, INT32 color);
        ColorShapeItem (EventShape sh, INT32 color, INT32 cycle, INT32 row);
        ~ColorShapeItem () {};

        static ColorShapeItem* clone(ColorShapeItem);
                    
        inline EventShape getShape();
        inline INT32 getColorIdx();
        inline void setShape(EventShape);
        inline void setColorIdx(INT32);

        inline INT32 getCycle();
        inline INT32 getRow();
        inline void setCycle(INT32 cycleValue);
        inline void setRow(INT32 rowValue);

    private:
        // we guess no more than 256 shapes:
        BYTE shape;

        // we index a color pallete
        UINT16 colorIdx ;

    private:
        // only for special events
        UINT32 cycle;
        UINT16 row;         // at most 64k events per cycle ...
};

EventShape 
ColorShapeItem::getShape()
{return (EventShape)shape;}

INT32
ColorShapeItem::getColorIdx()
{ return colorIdx; }

void
ColorShapeItem::setShape(EventShape value)
{shape = value;}

void
ColorShapeItem::setColorIdx(INT32 value)
{colorIdx = value;}

INT32
ColorShapeItem::getCycle()
{return cycle;}

INT32
ColorShapeItem::getRow()
{return (INT32)row;}

void
ColorShapeItem::setCycle(INT32 cycleValue)
{cycle=cycleValue;}

void
ColorShapeItem::setRow(INT32 rowValue)
{
    Q_ASSERT(rowValue<65536);
    row=rowValue;
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// ------------------------------------------------------------------

typedef QPtrList<ColorShapeItem> CSHList;

#endif


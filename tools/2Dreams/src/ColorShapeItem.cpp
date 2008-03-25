/*
 * Copyright (C) 2003-2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

/**
  * @file ColorShapeItem.cpp
  */

#include "ColorShapeItem.h"

ColorShapeItem::ColorShapeItem ()
{
    // default values
    shape = EVENT_SHAPE_RECTANGLE;
    colorIdx = 0;
    cycle = 0;
    row = 0;
}

ColorShapeItem::ColorShapeItem (EventShape sh, INT32 color)
{
    shape=sh;
    colorIdx=(UINT16)color;
    cycle = 0;
    row = 0;
}

ColorShapeItem::ColorShapeItem (EventShape sh, INT32 color, INT32 cy, INT32 rw)
{
    shape=sh;
    colorIdx=(UINT16)color;
    cycle = cy;
    row = rw;
}

ColorShapeItem* 
ColorShapeItem::clone(ColorShapeItem c)
{
    ColorShapeItem* nobj = new ColorShapeItem((EventShape)c.shape,
        (INT32)c.colorIdx, (INT32)c.cycle, (INT32)c.row);

    Q_ASSERT(nobj!=NULL);
    return (nobj);
}



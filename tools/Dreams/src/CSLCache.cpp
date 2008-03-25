/*
 * Copyright (C) 2004-2006 Intel Corporation
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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "CSLCache.h"

ColorShapeItem
CSLCache::getCSH(INT32 cycle, INT32 row, INT32 col) const
{
    // look for a special color-shape combination
    //printf ("getCSH on cycle=%d,row=%d,col=%d \n",cycle,row,col);
    bool fnd = false;
    QPtrListIterator<ColorShapeItem> it_csi(cshlist);
    ColorShapeItem* item=NULL;

    while(!fnd && ((item = it_csi.current()) != NULL))
    {
        fnd =
            (
                (item->getCycle() == cycle) &&
                (item->getRow() == row) &&
                (item->getColumn() == col)
            );

        if(!fnd)
        {
            ++it_csi;
        }
    }

    Q_ASSERT(fnd);
    return (* item);
}

INT32
CSLCache::lookForCachedColorShape(ColorShapeItem csi)
{
    INT32 i = 1;            ///< Position 0 is reserved for non cached ones
    bool fnd = false;

    while ( !fnd && (i<nextColorShapeIdx) )
    {
        fnd = (csitems[i]==csi);    // eq operator redefined to check for relevant fields
        ++i;
    }
    if (fnd)
    {
       return (i-1);
    }
    else
    {
       return (-1);
    }
}


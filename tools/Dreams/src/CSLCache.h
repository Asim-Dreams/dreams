// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
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
  * @file CSLCache.h
  */

#ifndef _CSLCACHE_H_
#define _CSLCACHE_H_

#include "DreamsDefs.h"

#include <qcolor.h>

#include "ColorShapeItem.h"
#include "EventShape.h"

#define EITEM_MAX_COLORSHAPE_INDEX 254              ///< Maximum valid ColorShape value
#define EITEM_NON_INIT_COLORSHAPE  255              ///< ColorShape after creation (without init)
#define EITEM_MAX_COLORSHAPE_ITEMS 256              ///< Number of ColorShape variants by row

/**
  * This class holds the information related to any collection of color/shape/letter
  * contained in a single display element in Dreams. Each of the elements are identifyed
  * by an index. The cache looks for the element and returns the combination of color/shape/letter
  *
  * @version 0.1
  * @date started at 2004-09-10
  * @author Santi Galan
  */
class CSLCache
{
  public:

    CSLCache() : nextColorShapeIdx(1)
    {
    }
    
    /**
     * Method to obtain a color from the cache. The cache can be accessed using two ways:
     * - Cache index. If any index different from 0 is provided, the this method is used.
     * - Space & time coordinates
     */
    inline ColorShapeItem getColorShape(INT32 cycle, INT32 row, INT32 col, INT32 idx) const;

    /**
     * Method to add a new CSL combination to the cache. The index of the entry in the cache is returned.
     * If the combination doesn't fit in the cache the is stored into a *slow* list and a 0 is
     * returned. The cycle/row/col data is used to identify the *slow* cases when getColorShape() is
     * invoked.
     */
    inline INT32 addColorShape(INT32 cycle, INT32 row,  INT32 col, ColorShapeItem csi);

    /**
     * Convenience function to add color/shape combination using the CSLData object
     * produced during the rule evalutation process
     */
    inline INT32 addColorShape(INT32 cycle, INT32 row,  INT32 col, CSLData csldata);
    
  protected:
    /**
     * Return the Color/Shape combination for
     * a given column/row event (look in linked list)
     */
    ColorShapeItem getCSH(INT32 cycle, INT32 row, INT32 col) const;
    INT32 lookForCachedColorShape(ColorShapeItem);

  private:

    /**
     * Normal events try to use some of these "cached" precomputed color/shape
     * combinations [0] is reserved for special combinations
     */
    ColorShapeItem csitems[EITEM_MAX_COLORSHAPE_ITEMS];
    INT32 nextColorShapeIdx;

    /**
     * For rare special cases where more than EITEM_MAX_COLORSHAPE_ITEMS
     * shape/color combinations are requiered, we implement a dynamic linked
     * list of Color/Shape combinations each ColorShapeItem holds the cycle/row
     * of the related event
     */
    QPtrList<ColorShapeItem> cshlist;
};

ColorShapeItem
CSLCache::getColorShape(INT32 cycle, INT32 row, INT32 col, INT32 idx) const
{
    if(idx != 0)
    {
        return csitems[idx];
    }
    else
    {
        return getCSH(cycle, row, col);
    }
}

INT32
CSLCache::addColorShape(INT32 cycle, INT32 row, INT32 col, ColorShapeItem csi)
{
    INT32 resultIdx;
    // 1) check if this comb. is already on the cache
    resultIdx = lookForCachedColorShape(csi);
    if(resultIdx>0) return resultIdx;

    // 2) check whether we have room in the cache for another combination
    if(nextColorShapeIdx<EITEM_MAX_COLORSHAPE_ITEMS)
    {
        resultIdx = nextColorShapeIdx;
        csitems[resultIdx] = csi;
        nextColorShapeIdx++;
        return (resultIdx);
    }

    // 3) create a new 'special' case (non-cached => slow)
    ColorShapeItem* nobj = ColorShapeItem::clone(csi);
    Q_ASSERT(nobj!=NULL);
    nobj->setCycle(cycle);
    nobj->setColumn(col);
    nobj->setRow(row);
    cshlist.append(nobj);
    //printf ("non-csh comb for cycle=%d,row=%d,col=%d \n",cycle,row,col);
    return (0); // reserved for non-cached cases.
}

INT32
CSLCache::addColorShape(INT32 cycle, INT32 row,  INT32 col, CSLData csldata)
{ return addColorShape(cycle,row,col,ColorShapeItem(csldata)); }

#endif


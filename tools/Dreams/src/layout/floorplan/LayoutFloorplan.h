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

/**
 * @file  LayoutFloorplan.h
 */

#ifndef __LAYOUTFLOORPLAN_H__
#define __LAYOUTFLOORPLAN_H__

// Dreams includes.
#include "dDB/aux/AMemObj.h"
#include "layout/floorplan/LayoutFloorplanRectangle.h"
#include "layout/floorplan/LayoutFloorplanTable.h"
#include "DreamsDefs.h"
#include "LayoutFloorplanFind.h"

// Qt includes.
#include <qrect.h>

// Class forwarding.
class DralDB;

/**
 * This class holds the layout definition for the observed elements using the 
 * Floorplan view. 
 *
 * @author Santiago Galan
 * @date started at 2004-06-14
 * @version 0.1
 */
class LayoutFloorplan : public AMemObj
{
  public:
    LayoutFloorplan(DralDB * _draldb);
    virtual ~LayoutFloorplan();

    inline bool getOpened(void) const;
    inline void setRoot(LayoutFloorplanTable * root);

    inline void computeGeometry(void);
    void computeCSL(void);
    void computeCSLFilters(void);

    inline UINT32 getWidth(void) const;
    inline UINT32 getHeight(void) const;
    inline UINT32 getNumCells(void) const;

    inline void getRectanglesAt(UINT32 x1, UINT32 y1, UINT32 x2, UINT32 y2, QPtrList<LayoutFloorplanNode> &listResult);
    inline QString getCellAt(UINT32 x, UINT32 y);
    inline QString getNodeNameAt(UINT32 x, UINT32 y);
    inline INT32 getTrackIdForCell(UINT32 x, UINT32 y);
    inline bool getValidData(INT32 x, INT32 y, CYCLE cycle);

    ItemHandler getCellWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE cycle, INT32 &startX, INT32 &startY);
    ItemHandler getCellWithTagValue(TAG_ID tagId, QString &tagValue, bool cs, bool em, CYCLE cycle, INT32 &startX, INT32 &startY);
    ItemHandler getCellWithTagValue(TAG_ID tagId, QRegExp &tagValue, CYCLE cycle, INT32 &startX, INT32 &startY);

    bool getFirstCellAppearenceWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE & cycle, INT32 &startX, INT32 &startY);

    void getFindData(LayoutFloorplanFindDataSet * entries, CYCLE startCycle, const LayoutFloorplanFindOptions * env) const;

    INT64  getObjSize() const;
    QString getUsageDescription() const;

    void dump(void) const;

  private:
    DralDB * draldb;         ///< The database used to show the shapes and colors.
    LayoutFloorplanTable * pltRoot; ///< Root layout node.
    UINT32 nWidth;           ///< Width of the layout.
    UINT32 nHeight;          ///< Height of the layout.
    QRect rectView;          ///<
    QPoint pointSearch;      ///<
} ;

/*
 * Returns if this object has the layout set.
 *
 * @return if the object has layout.
 */
bool
LayoutFloorplan::getOpened(void) const
{
    return (pltRoot != NULL);
}

/*
 * Sets the root level to the layout.
 *
 * @param root is the root level of the layout.
 *
 * @return void.
 */
void
LayoutFloorplan::setRoot(LayoutFloorplanTable * root)
{
    Q_ASSERT(!pltRoot);
    pltRoot = root;
}

/*
 * Ask the root node the current layout size, if the size is 0, then the tree
 * is scanned in order to recompute the actual size
 *
 * @return void.
 */
void
LayoutFloorplan::computeGeometry(void)
{
    if(pltRoot != NULL)
    {
        nWidth = pltRoot->getWidth();
        nHeight = pltRoot->getHeight();

        // Root node is placed at (0, 0);
        pltRoot->setPosition(0, 0);
    }
}

/*
 * Returns the width of the layout.
 *
 * @return width.
 */
UINT32
LayoutFloorplan::getWidth(void) const
{
    return nWidth;
}

/*
 * Returns the height of the layout.
 *
 * @return height.
 */
UINT32
LayoutFloorplan::getHeight(void) const
{
    return nHeight;
}

/*
 * Returns the num of cells in the layout.
 *
 * @return num of cells.
 */
UINT32
LayoutFloorplan::getNumCells(void) const
{
    UINT32 nCells = 0;

    if(pltRoot != NULL)
    {
        // Forwards the call to the root node.
        nCells = pltRoot->getNumCells();
    }
    return nCells;
}

/*
 * Walks throught the tree selecting those rectangles that intersect with the given coordinates
 *
 * @param x1 Top-left x coordinate
 * @param y1 Top-left y coordinate
 * @param x2 Botton-right x coordinate
 * @param y2 Botton-right y coordinate
 * @param listResult Output list that will hold those rectangles that intersect with the given coordinates
 *                   The method assumes that the list is empty
 */
void
LayoutFloorplan::getRectanglesAt(UINT32 x1, UINT32 y1, UINT32 x2, UINT32 y2, QPtrList<LayoutFloorplanNode> & listResult)
{
    Q_ASSERT(listResult.isEmpty());

    // Sets the coordinates for the viewport rectangle 
    rectView.setLeft(x1);
    rectView.setTop(y1); 
    rectView.setRight(x2);
    rectView.setBottom(y2);

    pltRoot->getRectanglesAt(rectView, listResult);
}

/**
 * TODO
 * This function retrieves the information related to the cell placed in the
 * coordinates given. If there is no cell, then a null string is returnded;
 *
 * @param x X world coordinate
 * @param y X world coordinate
 *
 * @return string with the cell information. This must be revisited.
 */
QString 
LayoutFloorplan::getCellAt(UINT32 x, UINT32 y)
{
    pointSearch.setX(x);
    pointSearch.setY(y);

    return pltRoot->getCellAt(pointSearch);
}

/**
 * TODO
 * This function retrieves the information related to the node placed in the
 * coordinates given. If there is no node, then a null string is returnded;
 *
 * @param x X world coordinate
 * @param y X world coordinate
 *
 * @return string with the node information. This must be revisited.
 */
QString 
LayoutFloorplan::getNodeNameAt(UINT32 x, UINT32 y)
{
    pointSearch.setX(x);
    pointSearch.setY(y);

    return pltRoot->getNodeNameAt(pointSearch);
}

/*
 * TODO
 * This function retrieves the information related to the cell placed in the
 * coordinates given. If there is no cell, then a -1 is returnded;
 *
 * @param x X world coordinate
 * @param y X world coordinate
 *
 * @return the trackid of the cell. This must be revisited.
 */
INT32
LayoutFloorplan::getTrackIdForCell(UINT32 x, UINT32 y)
{
    pointSearch.setX(x);
    pointSearch.setY(y);

    return pltRoot->getTrackIdForCell(pointSearch);
}

/*
 * TODO
 * This function retrieves the information related to the cell placed in the
 * coordinates given. If there is no cell, then a false is returnded;
 *
 * @param x X world coordinate
 * @param y X world coordinate
 *
 * @return if a cell is in the actual coordinate. This must be revisited.
 */
bool
LayoutFloorplan::getValidData(INT32 x, INT32 y, CYCLE cycle)
{
    return (getTrackIdForCell(x, y) >= 0);

    //if(trackId >= 0)
    //{
    //    ItemHandler handler;
    //    draldb->getItemInSlot(trackId, cycle, &handler);
    //    bValid = handler.isValidItemHandler();
    //}
}

#endif // __LAYOUTFLOORPLAN_H__.

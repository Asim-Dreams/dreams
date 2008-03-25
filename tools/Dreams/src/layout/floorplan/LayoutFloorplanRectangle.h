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
 * @file  LayoutFloorplanRectangle.h
 */

#ifndef _LAYOUTRECTANGLE_H
#define _LAYOUTRECTANGLE_H

#include <iostream>

#include "asim/AScrollView.h"

#include "DreamsDefs.h"
#include "layout/floorplan/LayoutFloorplanNode.h"
#include "expressions/Expression.h"
#include "CSLCache.h"
#include "FilterHandler.h"
#include "LayoutFloorplanFind.h"

using namespace std;

/**
 * Class that represents a rectangle (set of cells) inside the layout repesented in the Floorplan
 */
class LayoutFloorplanRectangle : public LayoutFloorplanNode
{
  public:
    enum TitlePos
    {
        NoTitle,
        North,
        East,
        South,
        West
    };

  private:

    enum Type
    {
        Uninitialized,
        Edge,
        Node
    };

    static const UINT16 TITLE_HEIGHT = CELL_HEIGHT + 5;

    TitlePos tpTitle;
    bool bTitleOnTop;
    UINT32 nMargin;
    bool bBorder;
    bool bSlotBorder;

    bool bBGColorDefined;
    QColor bgColor;

    Type    rectType;
    UINT16  dralId;

    UINT16 dimensions;
    UINT32 *capacities;

    /** Array that holds accCapacities[i] = Prod(capacities[0..i]) */
    UINT32 *accCapacities;

    UINT32 totalCapacity;

    TRACK_ID trackIdBase;

    UINT32 *dimensionHeight;
    UINT32 *dimensionWidth;

    /** Array of layouts. */
    UINT32 (*pnLayoutFloorplan)[2];

    /** Temporary array to store the partial coordinates computed by the getTrackIdForCell method */
    UINT32 *coordinates;

    // Size of each cell
    UINT32 nCellHeight;
    UINT32 nCellWidth;
    
    ExpressionList qlistRules; ///< Pointers to Rules for this environment

    CSLCache cslCache;                  ///< Cache of color/shape/letter combinations for curent node (Row/Group)

    UINT32 currentTagIdColor;
    UINT32 nTagIdColor;                 ///< Number of scheme colors computed by the rectangle
    INT32 tagIdColor[COLOR_SCHEMES];    ///< Tag Identifyer that represents the tag that holds the color for the current
                                        ///  view element. We can have many color schemes defined for the current rectangle.
    FILTER_HANDLER filter;
    UINT32 nTagIdFilter;

    set<TAG_ID> item_tags;  ///< List of item tags that are used in the rules of this rectangle.
    set<TAG_ID> slot_tags;  ///< List of slot tags that are used in the rules of this rectangle.
    set<TAG_ID> node_tags;  ///< List of node tags that are used in the rules of this rectangle.
    set<TAG_ID> cycle_tags; ///< List of cycle tags that are used in the rules of this rectangle.

    void registerDimensionSlots(INT16 currentDimension, UINT32 *currentCoordinates, UINT32 &elemsCreated);

    void computeGeometry(void);

    void computePosition(UINT32 parentX, UINT32 parentY) { }

  public:

    LayoutFloorplanRectangle(DralDB *draldb, TitlePos title = NoTitle, bool bOnTop = true, bool margin = true, bool border = true) 
      : LayoutFloorplanNode(draldb), tpTitle(title), bTitleOnTop(bOnTop), nMargin(CELL_MARGIN), bBorder(false), bSlotBorder(border), bBGColorDefined(false),
        dimensions(0), capacities(NULL), accCapacities(NULL), totalCapacity(0), trackIdBase(TRACK_ID_INVALID), 
        dimensionHeight(NULL), dimensionWidth(NULL), pnLayoutFloorplan(NULL), coordinates(NULL),
        nCellHeight(CELL_HEIGHT), nCellWidth(CUBE_SIZE), nTagIdColor(0), filter(NULL)
    { };

    ~LayoutFloorplanRectangle() 
    { 
        if(capacities) delete capacities; 
        if(accCapacities) delete accCapacities; 
        if(dimensionWidth) delete dimensionWidth; 
        if(dimensionHeight) delete dimensionHeight; 
        if(pnLayoutFloorplan) delete pnLayoutFloorplan; 
        if(coordinates) delete coordinates; 
    }

    void addSon(LayoutFloorplanNode *node)     { Q_ASSERT(false); }

    void setNode(UINT16 id )
    {
        rectType = Node;
        dralId = id;
    }

    void setEdge(UINT16 id )
    {
        rectType = Edge;
        dralId = id;
    }

    void setCapacity(UINT16 dimensions, UINT32 *capacities);

    inline UINT32 getCapacity(UINT16 dimension) const;
    inline UINT32 getDimensions(void) const;

    inline bool setLayout(UINT16 dimension, UINT32 nRow, UINT32 nCol);

    void setSlotBorder(bool border) { bSlotBorder = border; }
    bool getSlotBorder(void) const  { return bSlotBorder; }
    void setBorder(bool border)     { bBorder = border; }
    bool getBorder(void) const      { return bBorder; }

    void setMargin(UINT32 margin)   { nMargin = margin; }
    UINT32 getMargin(void) const    { return nMargin; }

    void setBGColor(QColor &color)  { bgColor = color; bBGColorDefined = true; }

    void setTitleOnTop(bool onTop)  { bTitleOnTop = onTop; }
    bool getTitleOnTop(void) const  { return bTitleOnTop; }

    void setTitle(TitlePos title)   { tpTitle = title; }
    TitlePos getTitle(void) const   { return tpTitle; }

    void setCellHeigh(UINT32 heigh) { nCellHeight = heigh; }
    void setCellWidth(UINT32 width) { nCellWidth = width; }

    UINT32 getCellHeight(void) const { return nCellHeight; }
    UINT32 getCellWidth(void) const  { return nCellWidth; }

    inline UINT32 getNumCells(void) const;

    QString getName(void) const;

    void addDefaultRules(ExpressionList &rules, INT32 level);
    void addOverrideRules(ExpressionList &rules, INT32 level);
    void computeCSL(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell);

    void setTFilter(INT32 tagIdFilter, UINT32 nTaps);

    void computeCSLFilters(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell);
    void initRuleEvalStructs();
    void freeRuleEvalStructs();

    inline void getRectanglesAt(QRect &rectReference, QPtrList<LayoutFloorplanNode> &listResult);
    inline TRACK_ID getTrackIdForCell(UINT32 x, UINT32 y) const;
    inline TRACK_ID getTrackIdForCell(QPoint &point) const;
    inline TRACK_ID getTrackIdForCell(UINT32 cellId) const;
    INT32 getCellIdAt(UINT32 refX, UINT32 refY) const;
    QString getCellAt(QPoint &point);
    QString getNodeNameAt(QPoint &point);

    ItemHandler getCellWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE cycle, INT32 & startX, INT32 & startY);
    ItemHandler getCellWithTagValue(TAG_ID tagId, QString & tagValue, bool cs, bool em, CYCLE cycle, INT32 & startX, INT32 & startY);
    ItemHandler getCellWithTagValue(TAG_ID tagId, QRegExp & tagValue, CYCLE cycle, INT32 & startX, INT32 & startY);

    void getCellIdCoordinates(INT32 cellId, INT32 &startX, INT32 &startY) const;

    inline void setTagIdColor(TAG_ID tagId);
    void drawTitle(QPainter * painter);
    void draw(QPainter * painter, HighLightMgrFloorplan * hlmgr, CYCLE cycle, AScrollView * asv);

    void getFindData(LayoutFloorplanFindDataSet * entries, CYCLE startCycle, const LayoutFloorplanFindOptions * env) const;

    INT64 getObjSize() const;
    QString getUsageDescription() const;

    /** Debug method to display the tree structure */
    inline void dump(void);

    friend class RuleEngine;
};

UINT32 
LayoutFloorplanRectangle::getNumCells(void) const
{
    return totalCapacity;
}

UINT32 
LayoutFloorplanRectangle::getCapacity(UINT16 dimension) const
{
    Q_ASSERT(capacities);
    Q_ASSERT(dimension < dimensions);

    return capacities[dimension];
}

UINT32 
LayoutFloorplanRectangle::getDimensions(void) const
{
    return dimensions;
}

bool 
LayoutFloorplanRectangle::setLayout(UINT16 dimension, UINT32 nRow, UINT32 nCol)
{
    if(pnLayoutFloorplan == NULL) pnLayoutFloorplan = new UINT32[dimensions][2];

    if((dimension < dimensions) && ((nRow * nCol) > 0) && ((nRow * nCol) >= capacities[dimension]))
    {
        pnLayoutFloorplan[dimension][0] = nCol; 
        pnLayoutFloorplan[dimension][1] = nRow; 
        return true;
    }

    return false;
}

/**
 * Computes given the reference point, which cell is pointed. We assume that the first element
 * is placed at top-left position. For that reason we walk the dimensions from lef to right.
 * XXX
 * Add support to allow user specifyed order. Basically we will change the computation of the
 * current element based on the point position.
 *
 * @param rectReference QRect that holds the reference coordinates to compare to
 * @param listResult Output list that holds the rectangles that should be displayed
 */
void 
LayoutFloorplanRectangle::getRectanglesAt(QRect &rectReference, QPtrList<LayoutFloorplanNode> &listResult)
{
    rectNode.setLeft(x);  rectNode.setTop(y); 
    rectNode.setRight(x + nWidth); rectNode.setBottom(y + nHeight);

    if(rectNode.intersects(rectReference)) listResult.append(this);
}

TRACK_ID
LayoutFloorplanRectangle::getTrackIdForCell(QPoint &point) const
{
    return getTrackIdForCell(point.x(), point.y());
}

/**
 * Method used to obtain the trackId related to one cell in the rectangle. 
 * To identify the cell its position inside the list of cells is used
 *
 * @param cellId Number to access the cell
 * @return Slot trackId. -1 if not found
 */
TRACK_ID 
LayoutFloorplanRectangle::getTrackIdForCell(UINT32 cellId) const
{
    TRACK_ID trId = TRACK_ID_INVALID;
    if(cellId < totalCapacity)
    {
        trId = trackIdBase + cellId;
    }

    return trId;
}

/**
 * Scans the list of sons in order to search the cell located under the point given
 * Returns the slot trakId located in the position (x, y). 
 *
 * @param x X coordinates that holds the reference coordinates to compare to
 * @param y Y coordinates that holds the reference coordinates to compare to
 * @return Slot trackId. -1 if not found
 */
TRACK_ID
LayoutFloorplanRectangle::getTrackIdForCell(UINT32 refX, UINT32 refY) const
{
    TRACK_ID trackId = TRACK_ID_INVALID;
    INT32 cellId = getCellIdAt(refX, refY);
    if(cellId != -1)
    {
        trackId = trackIdBase + cellId;
    }
    return trackId;
}

void 
LayoutFloorplanRectangle::setTagIdColor(TAG_ID tagId)
{
    Q_ASSERT(nTagIdColor < COLOR_SCHEMES);
    tagIdColor[nTagIdColor] = tagId;
    nTagIdColor++;
}

void 
LayoutFloorplanRectangle::dump(void)
{
    cerr << "Rectangle nHeight = " << getHeight() << " nWidth = " << getWidth() << " x = " << getX() << " y = " << getY() << " nMargin = " << nMargin << " bSlotBorder = " << bSlotBorder << " sNode = " << getName() << " nCellHeight = " << nCellHeight << " nCellWidth = " << nCellWidth << " dimensions = " << dimensions << " capacities";
    for(UINT16 i = 0; i < dimensions; i++)
        cerr << "[" << capacities[i] << "]";
    cerr << " ";
    for(UINT16 i = 0; i < dimensions; i++)
    {
        cerr << "pnLayoutFloorplan[" << i << "]" << "[0] = " << pnLayoutFloorplan[i][0];
        cerr << "pnLayoutFloorplan[" << i << "]" << "[1] = " << pnLayoutFloorplan[i][1];
    }
    cerr << endl;
}
#endif /* _LAYOUTRECTANGLE_H */



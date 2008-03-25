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
 * @file  LayoutFloorplanNode.h
 */

#ifndef _LAYOUTNODE_H
#define _LAYOUTNODE_H

// Qt includes.
#include <qptrlist.h>
#include <qrect.h>
#include <qprogressdialog.h>

// ASIM
#include "asim/AScrollView.h"

// Dreams includes.
#include "DreamsDefs.h"
#include "dDB/DralDB.h"
#include "dDB/itemheap/ItemHeap.h"
#include "DreamsDefs.h"
#include "expressions/Expression.h"
#include "LayoutFloorplanFind.h"

// Class forwarding.
class RuleEngine;
class HighLightMgrFloorplan;
class LayoutFloorplanRectangle;

/**
 * This class defines the layout node. This class contains the size, the
 * geometry, borders, etc...  of one element to be displayed in the Floorplan
 * view.
 * Is a basic implementation of a n-tree, where each node has a pointer to 
 * a list of nodes, and a pointer to its first son.
 *
 * @author Santiago Galan
 * @date started at 2004-06-14
 * @version 0.1
 */
class LayoutFloorplanNode : public AMemObj
{
  protected:

    static QRect rectNode;
    
    DralDB *draldb;

    UINT32 x;
    UINT32 y;

    UINT32 nHeight;
    UINT32 nWidth;

    // Pointers to the tree components
    QPtrList<LayoutFloorplanNode> *pqlistSon;

    UINT16 clockId;

  public:

    static const UINT32 CELL_WIDTH = CUBE_SIZE;
    static const UINT32 CELL_HEIGHT = CUBE_SIZE;
    static const UINT32 CELL_BORDER = CUBE_BORDER;
    static const UINT32 CELL_MARGIN = CUBE_BORDER;

    static const UINT32 COLOR_SCHEMES = 8;

    LayoutFloorplanNode(DralDB *db) : draldb(db), x(0), y(0), nHeight(0), nWidth(0), clockId(0) { };
    virtual ~LayoutFloorplanNode() { };

    virtual void addSon(LayoutFloorplanNode *node) { Q_ASSERT(pqlistSon); pqlistSon->append(node); }
    LayoutFloorplanNode *getFirstSon(void)         { Q_ASSERT(pqlistSon); return pqlistSon->first(); };
    LayoutFloorplanNode *getNextSon(void)          { Q_ASSERT(pqlistSon); return pqlistSon->next();; };

    virtual void computeGeometry() = 0;

    virtual void addDefaultRules(ExpressionList &rules, INT32 level);
    virtual void addOverrideRules(ExpressionList &rules, INT32 level);
    virtual void computeCSL(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell) = 0;
    virtual void computeCSLFilters(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell) = 0;
    virtual void initRuleEvalStructs() = 0;
    virtual void freeRuleEvalStructs() = 0;

    void setHeight(UINT32 height) { nHeight = height; }
    inline UINT32 getHeight(void);

    void setWidth(UINT32 width) { nWidth = width; }
    inline UINT32 getWidth(void);

    virtual UINT32 getNumCells(void) const;

    /**
     * Virtual function that computes the sons positions based on parents.
     *
     * @param parentX Parent's x position in the layout
     * @param parentY Parent's y position in the layout
     */
    virtual void computePosition(UINT32 parentX, UINT32 parentY) = 0; 
    void setPosition(UINT32 newX, UINT32 newY)
    {
        x = newX;
        y = newY;
        if(pqlistSon != NULL) computePosition(x, y); 
    }

    UINT32 getX(void) const { return x; }
    UINT32 getY(void) const { return y; }

    virtual void getRectanglesAt(QRect &rectReference, QPtrList<LayoutFloorplanNode> &listResult) = 0;
    virtual QString getCellAt(QPoint &point) = 0;
    virtual QString getNodeNameAt(QPoint &point) = 0;
    virtual ItemHandler getCellWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE cycle, INT32 &startX, INT32 &startY);
    virtual ItemHandler getCellWithTagValue(TAG_ID tagId, QString &tagValue, bool cs, bool em, CYCLE cycle, INT32 &startX, INT32 &startY);
    virtual ItemHandler getCellWithTagValue(TAG_ID tagId, QRegExp &tagValue, CYCLE cycle, INT32 &startX, INT32 &startY);
    virtual TRACK_ID getTrackIdForCell(QPoint &point) const;

    virtual void getFindData(LayoutFloorplanFindDataSet * entries, CYCLE startCycle, const LayoutFloorplanFindOptions * env) const;

    virtual void draw(QPainter * painter, HighLightMgrFloorplan * hlmgr, CYCLE cycle, AScrollView * asv) = 0;

    /** Debug method to display the tree structure */
    virtual void dump(void)
    {
        Q_ASSERT(pqlistSon);
        for(LayoutFloorplanNode *plnCurrent = pqlistSon->first(); plnCurrent; plnCurrent = pqlistSon->next())
            plnCurrent->dump();
    }

    friend class RuleEngine;

  protected:

};

UINT32 
LayoutFloorplanNode::getHeight(void)
{ 
    if(nHeight == 0) computeGeometry(); 
    return nHeight; 
}

UINT32 
LayoutFloorplanNode::getWidth(void)
{
    if(nWidth == 0) computeGeometry(); 
    return nWidth; 
}
#endif /* _LAYOUTNODE_H */


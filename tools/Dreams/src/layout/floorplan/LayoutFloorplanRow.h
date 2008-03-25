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
 * @file  LayoutFloorplanRow.h
 */

#ifndef _LAYOUTROW_H
#define _LAYOUTROW_H

#include "DreamsDefs.h"
#include <qvaluevector.h>

#include "layout/floorplan/LayoutFloorplanNode.h"
#include "layout/floorplan/LayoutFloorplanColumn.h"

/**
 * Class that represents a Row inside the layout repesented in the Floorplan
 */
class LayoutFloorplanRow : public LayoutFloorplanNode
{
  private:

  public:
    LayoutFloorplanRow(DralDB *db) : LayoutFloorplanNode(db)
    { 
        pqlistSon = new QPtrList<LayoutFloorplanNode>();
        pqlistSon->setAutoDelete(true);
    }

    ~LayoutFloorplanRow() { delete pqlistSon; }

    // Only accept as son, some LayoutFloorplanNode types
    void addSon(LayoutFloorplanNode *node)   { Q_ASSERT(false); }
    void addSon(LayoutFloorplanColumn *node) { LayoutFloorplanNode::addSon(node); }

    void initRuleEvalStructs();
    void freeRuleEvalStructs();

    void computeGeometry(void);

    /**
     * Method used to compute the global width of each column that participates
     * in all the tables defined in the layout.
     */
    void computeColumnWidth(QValueVector<UINT32> &colWidth);
    void setColumnWidth(QValueVector<UINT32> &colWidth);

    void computePosition(UINT32 parentX, UINT32 parentY); 
    void computeCSL(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell);
    void computeCSLFilters(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell);

    void getRectanglesAt(QRect &rectReference, QPtrList<LayoutFloorplanNode> &listResult);
    QString getCellAt(QPoint &point);
    QString getNodeNameAt(QPoint &point);

    void draw(QPainter * painter, HighLightMgrFloorplan * hlmgr, CYCLE cycle, AScrollView * asv) { }; 

    INT64 getObjSize() const;
    QString getUsageDescription() const;

    /** Debug method to display the tree structure */
    void dump(void)
    {
        cerr << "Row nHeight = " << getHeight() << " nWidth = " << getWidth() << " x = " << getX() << " y = " << getY() << endl;
        LayoutFloorplanNode::dump();
    }
};

#endif /* _LAYOUTROW_H */




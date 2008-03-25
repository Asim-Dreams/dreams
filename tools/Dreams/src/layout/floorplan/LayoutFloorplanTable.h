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
 * @file  LayoutFloorplanTable.h
 */

#ifndef _LAYOUTTABLE_H
#define _LAYOUTTABLE_H

#include "DreamsDefs.h"
#include "layout/floorplan/LayoutFloorplanNode.h"
#include "layout/floorplan/LayoutFloorplanRow.h"

/**
 * Class that represents a Table inside the layout repesented in the Floorplan
 */
class LayoutFloorplanTable : public LayoutFloorplanNode
{
  private:

    bool bBorder;

    bool bBGColorDefined;
    QColor bgColor;

  public:
    LayoutFloorplanTable(DralDB *db) : LayoutFloorplanNode(db), bBorder(false), bBGColorDefined(false)
    { 
        pqlistSon = new QPtrList<LayoutFloorplanNode>();
        pqlistSon->setAutoDelete(true);
    }

    ~LayoutFloorplanTable() { delete pqlistSon; }

    void addSon(LayoutFloorplanNode *node) { Q_ASSERT(false); }
    void addSon(LayoutFloorplanRow *node)  { LayoutFloorplanNode::addSon(node); }

    void setBorder(bool border)     { bBorder = border; }
    bool getBorder(void)            { return bBorder; }
    void setBGColor(QColor &color)  { bgColor = color; bBGColorDefined = true; }

    void computeGeometry(void);
    void computeCSL(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell);
    void computeCSLFilters(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell);
    void initRuleEvalStructs();
    void freeRuleEvalStructs();
    void computePosition(UINT32 parentX, UINT32 parentY); 

    void getRectanglesAt(QRect &rectReference, QPtrList<LayoutFloorplanNode> &listResult);
    QString getCellAt(QPoint &point);
    QString getNodeNameAt(QPoint &point);

    void draw(QPainter * painter, HighLightMgrFloorplan * hlmgr, CYCLE cycle, AScrollView * asv);

    INT64  getObjSize() const;
    QString getUsageDescription() const;

    /** Debug method to display the tree structure */
    void dump(void)
    {
        cerr << "Table nHeight = " << getHeight() << " nWidth = " << getWidth() << " x = " << getX() << " y = " << getY() << endl;
        cerr << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
        LayoutFloorplanNode::dump();
        cerr << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
    }
};

#endif /* _LAYOUTTABLE_H */



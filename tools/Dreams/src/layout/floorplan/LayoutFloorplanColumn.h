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
 * @file LayoutFloorplanColumn.h
 */

#ifndef _LAYOUTCOLUMN_H
#define _LAYOUTCOLUMN_H

#include "DreamsDefs.h"
#include "layout/floorplan/LayoutFloorplanRectangle.h"

// Forward declaration of LayoutFloorplanTable to avoid cross include...
class LayoutFloorplanTable;

/**
 * Class that represents a column inside the layout repesented in the Floorplan
 */
class LayoutFloorplanColumn : public LayoutFloorplanNode
{
  private:
    QString sName;
    bool bMargin;

    bool bBorder;
    QColor borderColor;
    bool bFillColor;
    QColor fillColor;

  public:
    LayoutFloorplanColumn(DralDB *db, bool margin = true) : LayoutFloorplanNode(db), bMargin(margin)
    { 
        pqlistSon = new QPtrList<LayoutFloorplanNode>();
        pqlistSon->setAutoDelete(true);
    }

    ~LayoutFloorplanColumn() { delete pqlistSon; }

    void setName(QString &name) { sName = name; }
    const QString getName(void) { return sName; }

    void setMargin(bool margin) { bMargin = margin; }
    bool getMargin(void)        { return bMargin; }

    // Only accept as son, some LayoutFloorplanNode types
    void addSon(LayoutFloorplanNode *node)      { Q_ASSERT(false); }
    void addSon(LayoutFloorplanTable *node);
    void addSon(LayoutFloorplanRectangle *node) { LayoutFloorplanNode::addSon(node); }

    void computeGeometry(void);
    void computeCSL(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell);
    void computeCSLFilters(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell);
    void initRuleEvalStructs();
    void freeRuleEvalStructs();
    void computePosition(UINT32 parentX, UINT32 parentY);

    void getRectanglesAt(QRect &rectReference, QPtrList<LayoutFloorplanNode> &listResult);
    QString getCellAt(QPoint &point);
    QString getNodeNameAt(QPoint &point);

    void draw(QPainter * painter, HighLightMgrFloorplan * hlmgr, CYCLE cycle, AScrollView * asv) { };

    INT64 getObjSize() const;
    QString getUsageDescription() const;

    /** Debug method to display the tree structure */
    void dump(void)
    {
        cerr << "Column nHeight = " << getHeight() << " nWidth = " << getWidth() << " x = " << getX() << " y = " << getY() << " sName = " << (sName.isNull() ? "null" : sName) << " bMargin = " << bMargin << endl;
        LayoutFloorplanNode::dump();
    }
};

#endif /* _LAYOUTCOLUMN_H */


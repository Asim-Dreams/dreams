/*
 * Copyright (C) 2006 Intel Corporation
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
#include "DrawingTools.h"
#include "layout/floorplan/LayoutFloorplanTable.h"

void 
LayoutFloorplanTable::computeGeometry(void)
{ 
    //cerr << "LayoutFloorplanTable::computeGeometry" << endl;
    // We assume that all the inner nodes are displayed in a vertical way
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanRow * node;

    // We computte all the geometry of all the elements that compose the
    // table (<tr>). Aftewards we will compute the maxmimum width of the columns
    // and set them
    QValueVector<UINT32> colWidth;
    while((node = dynamic_cast<LayoutFloorplanRow *>(it_node.current())) != NULL)
    {
        ++it_node;
        node->computeGeometry();
        node->computeColumnWidth(colWidth);
    }

    // After computing the maximum of each column in the rows, we set the width 
    // to each of them
    nWidth = 0;
    nHeight = 0;
    it_node.toFirst();
    while((node = dynamic_cast<LayoutFloorplanRow *>(it_node.current())) != NULL)
    {
        ++it_node;
        node->setColumnWidth(colWidth);
        nHeight += node->getHeight();
        nWidth  = QMAX(nWidth, node->getWidth());
    }
}

void 
LayoutFloorplanTable::computeCSL(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell)
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->computeCSL(progress, label, totalCells, currentCell);
    }
}

void 
LayoutFloorplanTable::computeCSLFilters(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell)
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->computeCSLFilters(progress, label, totalCells, currentCell);
    }
}

void
LayoutFloorplanTable::initRuleEvalStructs()
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->initRuleEvalStructs();
    }
}

void
LayoutFloorplanTable::freeRuleEvalStructs()
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->freeRuleEvalStructs();
    }
}

/**
 * The only possible sons are the row, so all of them have the same x
 * but different y
 */
void 
LayoutFloorplanTable::computePosition(UINT32 parentX, UINT32 parentY)
{
    QPtrListIterator<LayoutFloorplanNode> it_node(*pqlistSon);
    LayoutFloorplanNode * node;
    UINT32 currentY = parentY;

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->setPosition(parentX, currentY);

        currentY += node->getHeight();
    }
}

/**
 * Scans the list of sons in order to build the list of rectangles that instersect with the given
 * QRect.
 *
 * @param rectReference QRect that holds the reference coordinates to compare to
 * @param listResult Output list that holds the rectangles that should be displayed
 */
void 
LayoutFloorplanTable::getRectanglesAt(QRect &rectReference, QPtrList<LayoutFloorplanNode> &listResult)
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;

    rectNode.setLeft(x); rectNode.setTop(y); 
    rectNode.setRight(x + nWidth); rectNode.setBottom(y + nHeight);

    if(!rectNode.intersects(rectReference))
    {
        return;
    }

    if(bBorder || bBGColorDefined)
        listResult.append(this);

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->getRectanglesAt(rectReference, listResult);
    }
}

/**
 * Scans the list of sons in order to search the cell located under the point given
 *
 * @param point QPoint that holds the reference coordinates to compare to
 */
QString 
LayoutFloorplanTable::getCellAt(QPoint &point)
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    QString qsResult = QString::null;

    rectNode.setLeft(x);  rectNode.setTop(y); 
    rectNode.setRight(x + nWidth); rectNode.setBottom(y + nHeight);

    if(rectNode.contains(point))
    {
        while((node = it_node.current()) != NULL && qsResult.isNull())
        {
            ++it_node;
            qsResult = node->getCellAt(point);
        }
    }

    return qsResult;
}

/**
 * Scans the list of sons in order to search the node namelocated under the point given
 *
 * @param point QPoint that holds the reference coordinates to compare to
 */
QString 
LayoutFloorplanTable::getNodeNameAt(QPoint &point)
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    QString qsResult = QString::null;

    rectNode.setLeft(x);  rectNode.setTop(y); 
    rectNode.setRight(x + nWidth); rectNode.setBottom(y + nHeight);

    if(rectNode.contains(point))
    {
        while((node = it_node.current()) != NULL && qsResult.isNull())
        {
            ++it_node;
            qsResult = node->getNodeNameAt(point);
        }
    }

    return qsResult;
}

void 
LayoutFloorplanTable::draw(QPainter * painter, HighLightMgrFloorplan * hlmgr, CYCLE cycle, AScrollView * asv)
{
    if(bBorder || bBGColorDefined)
    {
        painter->setBrush((bBGColorDefined ? QBrush(bgColor) : Qt::NoBrush));
        painter->setPen((bBorder ? QPen(Qt::black) : Qt::NoPen));
        DrawingTools::drawRectangle(getX(), getY(), painter, getWidth(), getHeight());
    }
}

INT64
LayoutFloorplanTable::getObjSize() const
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    INT64 result = sizeof(LayoutFloorplanTable);

    Q_ASSERT(pqlistSon);
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        result += node->getObjSize();
    }

    return result;
}

QString
LayoutFloorplanTable::getUsageDescription() const
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    QString result = "\nLayoutFloorplanTable size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n";

    result += "Detailed memory usage for each component:\n";

    Q_ASSERT(pqlistSon);

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        result += node->getUsageDescription();
    }

    return result;
}

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
#include "layout/floorplan/LayoutFloorplanColumn.h"
#include "layout/floorplan/LayoutFloorplanTable.h"

void 
LayoutFloorplanColumn::addSon(LayoutFloorplanTable *node)
{
    LayoutFloorplanNode::addSon(node);
}

void 
LayoutFloorplanColumn::computeGeometry()
{ 
    // We assume that all the inner nodes are displayed in a vertial way
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;

    nWidth = 0;
    nHeight = 0;

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        nHeight += node->getHeight() + (bMargin ? CELL_MARGIN : 0);
        nWidth  = QMAX(nWidth, node->getWidth() + (bMargin ? 2 * CELL_MARGIN : 0));
    }
    nHeight += (bMargin ? CELL_MARGIN : 0);
}

void
LayoutFloorplanColumn::computeCSL(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell)
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
LayoutFloorplanColumn::computeCSLFilters(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell)
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
LayoutFloorplanColumn::initRuleEvalStructs()
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
LayoutFloorplanColumn::freeRuleEvalStructs()
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
 * The only possible sons are the rectangles, so all of them have the same x
 * but different y. We must add the up-left margins.
 */
void 
LayoutFloorplanColumn::computePosition(UINT32 parentX, UINT32 parentY)
{
    //cerr << "LayoutFloorplanRow::computePosition: parentX = " << parentX << " parentY = " << parentY << endl;

    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    UINT32 currentY = parentY + (bMargin ? CELL_MARGIN : 0);

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->setPosition(parentX + (bMargin ? CELL_MARGIN : 0), currentY);
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
LayoutFloorplanColumn::getRectanglesAt(QRect &rectReference, QPtrList<LayoutFloorplanNode> &listResult)
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;

    rectNode.setLeft(x);  rectNode.setTop(y); 
    rectNode.setRight(x + nWidth); rectNode.setBottom(y + nHeight);

    if(!rectNode.intersects(rectReference))
    {
        return;
    }

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
LayoutFloorplanColumn::getCellAt(QPoint &point)
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    QString qsResult = QString::null;

    rectNode.setLeft(x); rectNode.setTop(y); 
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
 * Scans the list of sons in order to search the node name located under the point given
 *
 * @param point QPoint that holds the reference coordinates to compare to
 */
QString 
LayoutFloorplanColumn::getNodeNameAt(QPoint &point)
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    QString qsResult = QString::null;

    rectNode.setLeft(x); rectNode.setTop(y); 
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

INT64
LayoutFloorplanColumn::getObjSize() const
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    INT64 result = sizeof(LayoutFloorplanColumn);

    Q_ASSERT(pqlistSon);

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        result += node->getObjSize();
    }

    return result;
}

QString
LayoutFloorplanColumn::getUsageDescription() const
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    QString result = "\nLayoutFloorplanColumn size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n";

    result += "Detailed memory usage for each component:\n";

    Q_ASSERT(pqlistSon);

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        result += node->getUsageDescription();
    }

    return result;
}

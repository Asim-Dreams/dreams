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
#include "layout/floorplan/LayoutFloorplanRow.h"
#include "layout/floorplan/LayoutFloorplanColumn.h"

void
LayoutFloorplanRow::computeGeometry(void)
{ 
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;

    nWidth = 0;
    nHeight = 0;
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        nHeight = QMAX(nHeight, node->getHeight());
        nWidth += node->getWidth();
    }
}

/**
 * Method used for compute the width of each column that conforms a table
 * This method is used in the process of computing the position of each <td>
 * inside the <table>
 *
 * @param vector that hold the maximum withd of the column "n" in all the table
 */
void
LayoutFloorplanRow::computeColumnWidth(QValueVector<UINT32> &colWidth)
{ 
    //cerr << "LayoutFloorplanRow::computeColumnWidth: Compute the maximum width of each column" << endl;

    QPtrListIterator<LayoutFloorplanNode> it_node(*pqlistSon);
    LayoutFloorplanNode * node;

    UINT32 nColumn = 0;
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        if(nColumn < colWidth.size())
        {
            colWidth[nColumn] = QMAX(node->getWidth(), colWidth[nColumn]);
        }
        else
        {
            colWidth.push_back(node->getWidth());
        }

        //cerr << "\tColumn[" << nColumn << "] = " << colWidth[nColumn] << endl;

        nColumn++;
    }
}

void 
LayoutFloorplanRow::setColumnWidth(QValueVector<UINT32> &colWidth)
{

    //cerr << "LayoutFloorplanRow::setColumnWidth" << endl;
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;

    UINT32 nColumn = 0;
    nWidth = 0;
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        // We change the width of each column to the maximum
        // in order to match the painting positions
        //cerr << "\tsetting width: Column[" << nColumn << "] = " << colWidth[nColumn] << endl;
        node->setWidth(colWidth[nColumn]);
        nWidth += colWidth[nColumn];
        nColumn++;
    }
}

void 
LayoutFloorplanRow::computeCSL(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell)
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
LayoutFloorplanRow::computeCSLFilters(QProgressDialog * progress, QLabel * label, UINT32 totalCells, UINT32 * currentCell)
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
LayoutFloorplanRow::initRuleEvalStructs()
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
LayoutFloorplanRow::freeRuleEvalStructs()
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
 * The only possible sons are cols, so all of them have the same y
 * but different x
 * This is the old version of computing the position of all the elements
 * inside a rule.
 */
void 
LayoutFloorplanRow::computePosition(UINT32 parentX, UINT32 parentY)
{
    //cerr << "LayoutFloorplanRow::computePosition: parentX = " << parentX << " parentY = " << parentY << endl;
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    UINT32 currentX = parentX;

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        node->setPosition(currentX, parentY);
        currentX += node->getWidth();
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
LayoutFloorplanRow::getRectanglesAt(QRect &rectReference, QPtrList<LayoutFloorplanNode> &listResult)
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
LayoutFloorplanRow::getCellAt(QPoint &point)
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
LayoutFloorplanRow::getNodeNameAt(QPoint &point)
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
LayoutFloorplanRow::getObjSize() const
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    INT64 result = sizeof(LayoutFloorplanRow);

    Q_ASSERT(pqlistSon);

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        result += node->getObjSize();
    }

    return result;
}

QString
LayoutFloorplanRow::getUsageDescription() const
{
    QPtrListIterator<LayoutFloorplanNode> it_node(* pqlistSon);
    LayoutFloorplanNode * node;
    QString result = "\nLayoutFloorplanRow size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n";

    result += "Detailed memory usage for each component:\n";

    Q_ASSERT(pqlistSon);

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        result += node->getUsageDescription();
    }

    return result;
}

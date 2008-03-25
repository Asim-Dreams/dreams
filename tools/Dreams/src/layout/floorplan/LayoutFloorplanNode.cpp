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
#include "layout/floorplan/LayoutFloorplanNode.h"
#include "expressions/RuleEngine.h"

QRect LayoutFloorplanNode::rectNode;

UINT32 
LayoutFloorplanNode::getNumCells(void) const
{
    UINT32 nCells = 0;
    Q_ASSERT(pqlistSon);

    // Accumulates the cells of all its sons.
    for(LayoutFloorplanNode *plnCurrent = pqlistSon->first(); plnCurrent; plnCurrent = pqlistSon->next())
    {
        nCells += plnCurrent->getNumCells();
    }

    return nCells;
}
/**
 * Scans the list of sons in order to search the cell located under the point given
 *
 * @param point QPoint that holds the reference coordinates to compare to
 */
TRACK_ID
LayoutFloorplanNode::getTrackIdForCell(QPoint &point) const
{
    TRACK_ID tridResult = TRACK_ID_INVALID;

    rectNode.setLeft(x); rectNode.setTop(y); 
    rectNode.setRight(x + nWidth); rectNode.setBottom(y + nHeight);

    if(rectNode.contains(point))
    {
        for(LayoutFloorplanNode *plnCurrent = pqlistSon->first(); plnCurrent && (tridResult == TRACK_ID_INVALID); plnCurrent = pqlistSon->next())
        {
            tridResult = plnCurrent->getTrackIdForCell(point);
        }
    }

    return tridResult;
}

/*
 * Gets a set of tag handlers of all the slots of the layout.
 *
 * @return void.
 */
void
LayoutFloorplanNode::getFindData(LayoutFloorplanFindDataSet * entries, CYCLE startCycle, const LayoutFloorplanFindOptions * env) const
{
    for(LayoutFloorplanNode *plnCurrent = pqlistSon->first(); plnCurrent; plnCurrent = pqlistSon->next())
    {
        plnCurrent->getFindData(entries, startCycle, env);
    }
}

/**
 * Scans the list of sons in order to search the cell that contains any Item 
 * that matches the search criteria
 *
 * @param tagId Tag identifyer that describes the criteria
 * @param tagValue Value to look for.  
 * @param cycle Current searching cycle 
 *
 * @out startX X coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @out startY Y coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @return Item handler located at the returned cell in the given cycle
 */
ItemHandler 
LayoutFloorplanNode::getCellWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE cycle, INT32 &startX, INT32 &startY)
{
    ItemHandler itemResult;
    INT32 localX = (startX == static_cast<INT32>(getX()) ? -1 : startX);
    INT32 localY = (startY == static_cast<INT32>(getY()) ? -1 : startY);

    rectNode.setLeft(x);
    rectNode.setTop(y); 
    rectNode.setRight(x + nWidth);
    rectNode.setBottom(y + nHeight);

    //cerr << "LayoutFloorplanNode::getCellWithTagValue: startX = " << startX << " startY = " << startY << " x = " << x << " y = " << y << endl;
    if((localX == -1) || (localY == -1) || rectNode.contains(startX, startY)) 
    {
        //cerr << "\tPoint contained" << endl;

        UINT32 i = 0;
        LayoutFloorplanNode *plnCurrent = NULL;
        for(LayoutFloorplanNode *plnCurrent = pqlistSon->first(); plnCurrent && !itemResult.isValidItemHandler(); plnCurrent = pqlistSon->next())
        {
            //cerr << "\tElement " << i++ << ": Looking at starting point: startX = " << startX << " startY = " << startY << endl;
            itemResult = plnCurrent->getCellWithTagValue(tagId, tagValue, cycle, startX, startY);
        }

        // If the condition is true, means that a valid item has been found,
        // but it is the last item of the structure
        if(itemResult.isValidItemHandler() && (startX == -1) && (startY == -1) && plnCurrent)
        {
            startX = plnCurrent->getX();
            startY = plnCurrent->getY();
        }
    }

    return itemResult;
}

/**
 * Scans the list of sons in order to search the cell that contains any Item 
 * that matches the search criteria
 *
 * @param tagId Tag identifyer that describes the criteria
 * @param tagValue Value to look for.  
 * @param cycle Current searching cycle 
 *
 * @out startX X coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @out startY Y coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @return Item handler located at the returned cell in the given cycle
 */
ItemHandler 
LayoutFloorplanNode::getCellWithTagValue(TAG_ID tagId, QString &tagValue, bool cs, bool em, CYCLE cycle, INT32 &startX, INT32 &startY)
{
    ItemHandler itemResult;
    INT32 localX = (startX == static_cast<INT32>(getX()) ? -1 : startX);
    INT32 localY = (startY == static_cast<INT32>(getY()) ? -1 : startY);

    rectNode.setLeft(x);
    rectNode.setTop(y); 
    rectNode.setRight(x + nWidth);
    rectNode.setBottom(y + nHeight);

    //cerr << "LayoutFloorplanNode::getCellWithTagValue: startX = " << startX << " startY = " << startY << " x = " << x << " y = " << y << endl;
    if((localX == -1) || (localY == -1) || rectNode.contains(startX, startY)) 
    {
        //cerr << "\tPoint contained" << endl;
        
        UINT32 i = 0;
        LayoutFloorplanNode *plnCurrent = NULL;
        for(LayoutFloorplanNode *plnCurrent = pqlistSon->first(); plnCurrent && !itemResult.isValidItemHandler(); plnCurrent = pqlistSon->next())
        {
            //cerr << "\tElement " << i++ << ": Looking at starting point: startX = " << startX << " startY = " << startY << endl;
            itemResult = plnCurrent->getCellWithTagValue(tagId, tagValue, cs, em, cycle, startX, startY);
        }

        // If the condition is true, means that a valid item has been found,
        // but it is the last item of the structure
        if(itemResult.isValidItemHandler() && (startX == -1) && (startY == -1) && plnCurrent)
        {
            startX = plnCurrent->getX();
            startY = plnCurrent->getY();
        }
    }

    return itemResult;
}

/**
 * Scans the list of sons in order to search the cell that contains any Item 
 * that matches the search criteria
 *
 * @param tagId Tag identifyer that describes the criteria
 * @param tagValue Value to look for.  
 * @param cycle Current searching cycle 
 *
 * @out startX X coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @out startY Y coordinates of the point where to start the search. Is also 
 *             updated with the X coordinate of the cell where to start the 
 *             next search. If the returned value is -1, means that the last
 *             in the given cycle is the one that matches the criteria
 *
 * @return Item handler located at the returned cell in the given cycle
 */
ItemHandler 
LayoutFloorplanNode::getCellWithTagValue(TAG_ID tagId, QRegExp &tagValue, CYCLE cycle, INT32 &startX, INT32 &startY)
{
    ItemHandler itemResult;
    INT32 localX = (startX == static_cast<INT32>(getX()) ? -1 : startX);
    INT32 localY = (startY == static_cast<INT32>(getY()) ? -1 : startY);

    rectNode.setLeft(x);
    rectNode.setTop(y);
    rectNode.setRight(x + nWidth);
    rectNode.setBottom(y + nHeight);

    //cerr << "LayoutFloorplanNode::getCellWithTagValue: startX = " << startX << " startY = " << startY << " x = " << x << " y = " << y << endl;
    if((localX == -1) || (localY == -1) || rectNode.contains(startX, startY)) 
    {
        //cerr << "\tPoint contained" << endl;
        
        UINT32 i = 0;
        LayoutFloorplanNode *plnCurrent = NULL;
        for(LayoutFloorplanNode *plnCurrent = pqlistSon->first(); plnCurrent && !itemResult.isValidItemHandler(); plnCurrent = pqlistSon->next())
        {
            //cerr << "\tElement " << i++ << ": Looking at starting point: startX = " << startX << " startY = " << startY << endl;
            itemResult = plnCurrent->getCellWithTagValue(tagId, tagValue, cycle, startX, startY);
        }

        // If the condition is true, means that a valid item has been found,
        // but it is the last item of the structure
        if(itemResult.isValidItemHandler() && (startX == -1) && (startY == -1) && plnCurrent)
        {
            startX = plnCurrent->getX();
            startY = plnCurrent->getY();
        }
    }

    return itemResult;
}

void
LayoutFloorplanNode::addDefaultRules(ExpressionList &rules, INT32 level)
{
    RuleEngine::addDefaultRules(rules, this, level);
}

void
LayoutFloorplanNode::addOverrideRules(ExpressionList &rules, INT32 level)
{
    RuleEngine::addOverrideRules(rules, this, level);
}

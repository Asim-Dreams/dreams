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
#include "layout/resource/LayoutResourceGroup.h"
#include "layout/resource/LayoutResource.h"
#include "layout/resource/LayoutResourceRow.h"
#include "layout/resource/LayoutResourceMultiRow.h"
#include "DrawingTools.h"
#include "PreferenceMgr.h"

// Qt includes.
#include <qregexp.h>

/*
 * Forwards the creation to its super class and copies the root
 * group.
 *
 * @return new object.
 */
LayoutResourceGroup::LayoutResourceGroup(LayoutResourceGroup * root, QString _name, UINT32 _groupId, DralDB * draldb, UINT32 row, TRACK_ID trackId, TAG_ID tagIdColor)
    : LayoutResourceNode(draldb, row, (DRAL_ID) -1, trackId, tagIdColor, GroupRow)
{
    rootGroup = root;
    name = _name;
    groupId = _groupId;
    clockId = 0;
}

/*
 * Deletes all the nodes that have been attached to this group.
 *
 * @return destroys the object.
 */
LayoutResourceGroup::~LayoutResourceGroup()
{
    // Deletes all the nodes of the group.
    for(UINT32 i = 0; i < nodeList.size(); i++)
    {
        delete nodeList[i];
    }
}

/*
 * Updates the state of the mapping for the group. Allocates a row
 * for itself and if is expanded, forwards the call to all of its
 * nodes.
 *
 * @return void.
 */
void
LayoutResourceGroup::computeLayoutCache(LayoutResourceCache * cache, UINT32 numGroups, UINT32 lastRow, bool isLast)
{
    UINT32 row = cache->numVirtualRows;

    // Sets its conversion.
    cache->virToPhy[row].setNode(this);
    cache->virToPhy[row].setDisp(0);
    cache->phyToVir[physicalPos] = row;

    // Updates the row counter.
    cache->numVirtualRows = row + 1;

    // If the group is expanded.
    if(cache->groupExpansion[groupId])
    {

        // Shows an unfold symbol.
        cache->foldCache[row] = ResourceViewFoldUnfold;

        if(nodeList.size() > 0)
        {
            UINT32 i;

            // Forwards the call to its nodes with one more group.
            for(i = 0; i < nodeList.size() - 1; i++)
            {
                nodeList[i]->computeLayoutCache(cache, numGroups + 1, lastRow, false);
            }
            nodeList[i]->computeLayoutCache(cache, numGroups + 1, lastRow + 1, true);
        }
    }
    else
    {
        // Shows a fold symbol.
        cache->foldCache[row] = ResourceViewFoldFold;
    }
}

/*
 * Adds the rules to all the nodes attached to it.
 *
 * @return void.
 */
void
LayoutResourceGroup::addNodeRules(ExpressionList * rules, layoutResourcePhase phase)
{
    // Adds the rule to all the nodes.
    for(UINT32 i = 0; i < nodeList.size(); i++)
    {
        nodeList[i]->addNodeRules(rules, phase);
    }
}

/*
 * Adds the rule to all the nodes attached to it.
 *
 * @return void.
 */
void
LayoutResourceGroup::addNodeRules(Expression * rule, layoutResourcePhase phase)
{
    // Adds the rule to all the nodes.
    for(UINT32 i = 0; i < nodeList.size(); i++)
    {
        nodeList[i]->addNodeRules(rule, phase);
    }
}

/*
 * Returns the row where that has the dralId dralId.
 *
 * @return the row with dralId.
 */
UINT32
LayoutResourceGroup::getRowWithDralId(DRAL_ID dralId, RowType type) const
{
    UINT32 row = (UINT32) -1;
    UINT32 i = 0;

    // Runs through the list until finds the row.
    while((i < nodeList.size()) && (row == (UINT32) -1))
    {
        row = nodeList[i]->getRowWithDralId(dralId, type);
        i++;
    }

    return row;
}

/*
 * Fills the data set with the information of all its nodes.
 *
 * @return void.
 */
void
LayoutResourceGroup::getFindData(LayoutResourceFindDataSet * entries, CYCLE startCycle, const LayoutResourceFindOptions * env) const
{
    // Adds the find data of all its nodes.
    for(UINT32 i = 0; i < nodeList.size(); i++)
    {
        nodeList[i]->getFindData(entries, startCycle, env);
    }
}

/*
 * Method to compute the colors and shape of the events of a given
 * chunk of cycles. Just forwards the call to all the resource nodes.
 *
 * @return void.
 */
void
LayoutResourceGroup::computeCSL(QProgressDialog * progress, QLabel * label)
{
    // Computes the color for all the nodes of the group.
    for(UINT32 i = 0; i < nodeList.size(); i++)
    {
        nodeList[i]->computeCSL(progress, label);
    }
}

/*
 * Method to paint all the node events as points
 *
 * @param currentRow Current virtual row. Used to differ between the real 
 *                   screen row that must be painted and the position in the 
 *                   layout
 * @param x1 Starting horizontal coordinate. This param is constant
 * @param x2 Ending horizontal coordinate. This param is constant
 * @param y1 Starting vertical coordinate. This param is modifyed by each
 *           node after the painting process. 
 * @param y2 Ending vertical coordinate. This param is constant
 * @param p Painter where to paint
 */
void
LayoutResourceGroup::drawSubSamplingScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, INT32 stepx, INT32 stepy, QPainter * p, LayoutResourceCache * cache, bool paint_events)
{
    if(background != QColor(255, 255, 255))
    {
        p->setBrush(QBrush(background, Qt::Dense5Pattern));
        p->setPen(Qt::NoPen);
        DrawingTools::drawRectangle((col1 * CUBE_SIDE) - 1, (* row1 * CUBE_SIDE) - 1, p, (CUBE_SIDE * (col2 - col1)) + 2, CUBE_SIDE + 2);
        p->setPen(QPen(QColor(0, 0, 0), CUBE_BORDER));
    }
    * row1 = getVirtualRow(cache) + 1;
}

/*
 * Method to paint all the node events with their color and without their border
 *
 * @param x1 Starting horizontal coordinate. This param is constant
 * @param x2 Ending horizontal coordinate. This param is constant
 * @param y1 Starting vertical coordinate. This param is modifyed by each
 *           node after the painting process. 
 * @param y2 Ending vertical coordinate. This param is constant
 * @param p Painter where to paint
 */
void
LayoutResourceGroup::drawNoFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, LayoutResourceCache * cache, bool paint_events)
{
    if(background != QColor(255, 255, 255))
    {
        p->setBrush(QBrush(background, Qt::Dense5Pattern));
        p->setPen(Qt::NoPen);
        DrawingTools::drawRectangle((col1 * CUBE_SIDE) - 1, (* row1 * CUBE_SIDE) - 1, p, (CUBE_SIDE * (col2 - col1)) + 2, CUBE_SIDE + 2);
        p->setPen(QPen(QColor(0, 0, 0), CUBE_BORDER));
    }
    * row1 = getVirtualRow(cache) + 1;
}

/*
 * Method to paint all the node events with their borders and colors
 *
 * @param x1 Starting horizontal coordinate. This param is constant
 * @param x2 Ending horizontal coordinate. This param is constant
 * @param y1 Starting vertical coordinate. This param is modifyed by each
 *           node after the painting process. 
 * @param y2 Ending vertical coordinate. This param is constant
 * @param p Painter where to paint
 */
void
LayoutResourceGroup::drawFullScale(bool useDivisions, INT32 col1, INT32 col2, INT32 * row1, INT32 row2, QPainter * p, AScrollView * asv, LayoutResourceCache * cache, bool paint_events)
{
    if(background != QColor(255, 255, 255))
    {
        p->setBrush(QBrush(background, Qt::Dense5Pattern));
        p->setPen(Qt::NoPen);
        DrawingTools::drawRectangle((col1 * CUBE_SIDE) - 1, (* row1 * CUBE_SIDE) - 1, p, (CUBE_SIDE * (col2 - col1)) + 2, CUBE_SIDE + 2);
        p->setPen(QPen(QColor(0, 0, 0), CUBE_BORDER));
    }
    * row1 = getVirtualRow(cache) + 1;
}

void
LayoutResourceGroup::dumpRegression()
{
    // Dumps all the nodes of the group.
    for(UINT32 i = 0; i < nodeList.size(); i++)
    {
        nodeList[i]->dumpRegression();
    }
}

QString
LayoutResourceGroup::getRowComposedName() const
{
    QString ret;

    if(getRootGroup() != NULL)
    {
        ret = getRootGroup()->getRowComposedName() + " -> ";
    }
    ret += name;
    return ret;
}

/*
 * Returns a description of the group.
 *
 * @return a description of the group.
 */
QString
LayoutResourceGroup::getRowDescription(UINT32 row, LayoutResourceCache * cache, UINT32 mask) const
{
    QString rowDesc;

    if(mask & RV_ROW_DESC_NUM)
    {
        rowDesc = "Row " + QString::number(getVirtualRow(cache) + row) + " ";
    }
    if(mask & RV_ROW_DESC_TYPE)
    {
        rowDesc += "Group ";
    }
    if(mask & RV_ROW_DESC_NAME)
    {
        rowDesc += name;
    }

    return rowDesc;
}

void
LayoutResourceGroup::showRow(UINT32 row, LayoutResourceCache * cache)
{
    // This doesn't mean that a group is invalid if nothing is
    // defined inside, but if nothing is inside, this function
    // shouldn't be called with this object because it can't
    // show nothing.
    Q_ASSERT(nodeList.size() > 0);

    // First of all, changes the state of this group to unfolded.
    cache->groupExpansion[groupId] = true;

    LayoutResourceNode * node = NULL;

    // First finds the node where the row is.
    for(UINT32 i = 0; i < nodeList.size(); i++)
    {
        node = nodeList[i];
        if(node->getPhysicalRow() > row)
        {
            // The actual node is beyond the requested row, so we
            // must go to the previous node.
            node = nodeList[i - 1];
            break;
        }
    }

    // Now node points to the node of the row.
    // Two options: ...
    if(dynamic_cast<LayoutResourceRow *>(node) || dynamic_cast<LayoutResourceMultiRow *>(node))
    {
        // ... the node is a row, so we have finished.
        return;
    }
    else
    {
        // ... the node is a group, we continue the search.
        LayoutResourceGroup * group;

        group = dynamic_cast<LayoutResourceGroup *>(node);
        Q_ASSERT(group != NULL);
        group->showRow(row, cache);
    }
}

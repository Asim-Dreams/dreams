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

/**
  * @file LayoutResourceNode.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "layout/resource/LayoutResourceNode.h"
#include "layout/resource/LayoutResourceCache.h"

/*
 * Creator of the class.
 *
 * @param row position inside the layout.
 * @param type type of row (default is SlotRow).
 *
 * @return the new object.
 */
LayoutResourceNode::LayoutResourceNode(DralDB * _draldb, UINT32 row, DRAL_ID _dralId, TRACK_ID _trackId, TAG_ID _tagIdColor, RowType _type, bool _isDrawOnMove)
{
    draldb = _draldb;
    physicalPos = row;
    dralId = _dralId;
    trackId = _trackId;
    tagIdColor = _tagIdColor;
    type = _type;
    isDrawOnMove = _isDrawOnMove;
    showHigh = false;
    showLow = false;
    showExpanded = false;
}

/*
 * Destructor of the class that just frees the allocated resources.
 *
 * @return destroys the object.
 */
LayoutResourceNode::~LayoutResourceNode()
{
}

/*
 * Adds the rules to the node.
 *
 * @return void.
 */
void
LayoutResourceNode::addNodeRules(ExpressionList * rules, layoutResourcePhase phase)
{
    ExpressionIterator it_rules(* rules);
    Expression * rule;

    if(phase == LAYOUTRESOURCE_PHASE_HIGH)
    {
        // First adds the rules to the already globally defined rules.
        while((rule = it_rules.current()) != NULL)
        {
            ++it_rules;
            rulesHigh.append(rule);
        }
    }
    else
    {
        // First adds the rules to the already globally defined rules.
        while((rule = it_rules.current()) != NULL)
        {
            ++it_rules;
            rulesLow.append(rule);
        }
    }
}

/*
 * Adds the rule to the node.
 *
 * @return void.
 */
void
LayoutResourceNode::addNodeRules(Expression * rule, layoutResourcePhase phase)
{
    if(phase == LAYOUTRESOURCE_PHASE_HIGH)
    {
        rulesHigh.append(rule);
    }
    else
    {
        rulesLow.append(rule);
    }
}

/*
 * Method to obtain a contained row position.
 *
 * @return position in the virtual layout.
 */
UINT32
LayoutResourceNode::getVirtualRow(LayoutResourceCache * cache) const
{
    return cache->phyToVir[physicalPos];
}

/*
 * Adds the rule to the node.
 *
 * @return void.
 */
INT64  
LayoutResourceNode::getObjSize() const
{
    INT64 result = sizeof(LayoutResourceNode);

    return result;
}

/*
 * Returns the usage description of the object.
 *
 * @return an usage description.
 */
QString 
LayoutResourceNode::getUsageDescription() const
{
    QString result = "\nLayoutResourceNode size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n";
    result += "Detailed memory usage for each component:\n";
    result += "1) cslCache:\t" + QString::number(sizeof(cslCache)) + "\n";

    return result;
}

/*
 * Returns the stats of the object.
 *
 * @return the stats.
 */
QString 
LayoutResourceNode::getStats() const
{
    return "";
}

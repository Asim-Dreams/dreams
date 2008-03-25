// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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

/*
 * @file DralGraphNode.cpp
 */

// Dreams includes.
#include "dDB/dralgraph/DralGraphNode.h"
#include "dDB/dralgraph/DralGraph.h"

/*
 * Creator of this class. Just assigns all the parameters to the
 * fields of this class.
 *
 * @return new object.
 */
DralGraphNode::DralGraphNode(QString pname, UINT16 pnode_id, UINT16 pparent_id, UINT16 pinstance, DralGraph * dbgraph)
{
    this->name = pname;
    this->node_id = pnode_id;
    this->parent_id = pparent_id;
    this->instance = pinstance;
    this->dbgraph = dbgraph;
    clock_id = 0;
    clock_set = false;
    inputbw = 0;
    outputbw = 0;
}

/*
 * Destructor of this class.
 *
 * @return destroys the object.
 */
DralGraphNode::~DralGraphNode()
{
}

/*
 * Returns the id of the clock domain of the node.
 *
 * @return the clock id.
 */
CLOCK_ID
DralGraphNode::getClockId() const
{
    // Checks if this node has the clock set.
    if(!clock_set)
    {
        // If not, ask it to its parent.
        // All the root nodes must have a clock defined.
        Q_ASSERT(node_id != parent_id);

        DralGraphNode * node = dbgraph->getNode(parent_id);
        Q_ASSERT(node != NULL);

        return node->getClockId();
    }
    else
    {
        // If has the clock set, just returns it.
        return clock_id;
    }
}

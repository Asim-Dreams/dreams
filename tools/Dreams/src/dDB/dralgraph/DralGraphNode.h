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
 * @file DralGraphNode.h
 */

#ifndef _DRALDB_DBGRAPHNODE_H
#define _DRALDB_DBGRAPHNODE_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/DRALTag.h"

// Qt includes.
#include <qstring.h>
#include <qvaluelist.h>
#include <qdict.h>
#include <qintdict.h>

// Class forwarding.
class DralGraph;

/*
 * @brief
 * This class represents a DRAL graph node.
 *
 * @description
 * A DRAL graph node is used to store all the information that
 * a DRAL trace gives us about this node.
 *
 * @author Federico Ardanaz
 * @date started at 2002-07-15
 * @version 0.1
 */
class DralGraphNode
{
    public:
        DralGraphNode(QString name, UINT16 node_id, UINT16 parent_id, UINT16 instance, DralGraph * dbgraph);
        ~DralGraphNode();

        inline QString getName() const;
        inline DRAL_ID getNodeId() const;
        inline DRAL_ID getParentNodeId() const;
        inline UINT16 getInstance() const;
        CLOCK_ID getClockId() const;
        inline UINT16 getInputBW() const;
        inline UINT16 getOutputBW() const;
        inline UINT32 getCapacity() const;
        inline UINT16 getDimensions() const;
        inline UINT32 * getCapacities() const;
        inline NodeSlot * getLayout();

        inline void setLayout(NodeSlot _layout);
        inline void setClockId(UINT16 _clock_id);

    private:
        QString name;      ///< Name of the node. This name is a unique string for identifying the node : node_name[instance]{node_slot}.  
        DRAL_ID node_id;   ///< Holds the id of the node.
        DRAL_ID parent_id; ///< Contains the id of this node's parent.
        UINT16 instance;   ///< Instance of the node. Needed due the existence of different nodes with the same name.
        CLOCK_ID clock_id; ///< Identifier of the clock domain of the node.
        bool clock_set;    ///< True if the clock is set for this node.
        NodeSlot layout;   ///< Contains the layout information of the node.
        UINT16 inputbw;    ///< Contains the input bandwidth of the node. Not used.
        UINT16 outputbw;   ///< Contains the output bandwidth of the node. Not used.
        DralGraph * dbgraph; ///< Pointer to the db graph.
};

/*
 * @typedef DGNList
 * @brief
 * This struct is a dictionary of nodes indexed by integers.
 */
typedef QIntDict<DralGraphNode> DBGNList;

/*
 * @typedef DGNListByName
 * @brief
 * This struct is a dictionary of nodes indexed by name (field name of this class).
 */
typedef QDict<DralGraphNode> DBGNListByName;

/*
 * Returns the name of the node. 
 *
 * @return the name.
 */
QString
DralGraphNode::getName() const
{
    return name;
}

/*
 * Returns the id of the node. 
 *
 * @return the id.
 */
DRAL_ID
DralGraphNode::getNodeId() const
{
    return node_id;
}

/*
 * Returns the id of the parent. 
 *
 * @return the id.
 */
DRAL_ID
DralGraphNode::getParentNodeId() const
{
    return parent_id;
}

/*
 * Returns the instance of the node. 
 *
 * @return the instance.
 */
UINT16
DralGraphNode::getInstance() const
{
    return instance;
}

/*
 * Returns the input bandwidth of the node. 
 *
 * @return the input bandwidth.
 */
UINT16 
DralGraphNode::getInputBW() const
{
    return inputbw;
}

/**
 * Returns the output bandwidth of the node. 
 *
 * @return the output bandwidth.
 */
UINT16
DralGraphNode::getOutputBW() const
{
    return outputbw;
}

/**
 * Returns the capacity of the node. 
 *
 * @return the capacity.
 */
UINT32
DralGraphNode::getCapacity() const
{
    return layout.totalCapacity;
}

/**
 * Returns the dimensions of the node. 
 *
 * @return the dimensions.
 */
UINT16
DralGraphNode::getDimensions() const
{
    return layout.dimensions;
}

/**
 * Returns the capacities of the node. 
 *
 * @return the capacities.
 */
UINT32 *
DralGraphNode::getCapacities() const
{
    return layout.capacities;
}

/**
 * Returns the layout of the node. 
 *
 * @return the layout.
 */
NodeSlot *
DralGraphNode::getLayout()
{
    return &layout;
}

/*
 * Sets the layout of the node. Each node have a different layout
 * used to represent different types of hardware structures.
 *
 * @return void.
 */
void
DralGraphNode::setLayout(NodeSlot _layout)
{
    Q_ASSERT(_layout.dimensions >= 1);
    Q_ASSERT(_layout.capacities != NULL);

    layout = _layout;
}

/*
 * Sets the node clock.
 *
 * @return void.
 */
void
DralGraphNode::setClockId(UINT16 _clock_id)
{
    Q_ASSERT(!clock_set);
    clock_id = _clock_id;
    clock_set = true;
}

#endif

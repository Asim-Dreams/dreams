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

/**
  * @file DralGraphEdge.h
  */

#ifndef _DRALDB_DBGRAPHEDGE_H
#define _DRALDB_DBGRAPHEDGE_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/DRALTag.h"
#include "dDB/dralgraph/DralGraphNode.h"

// Qt includes.
#include <qstring.h>
#include <qintdict.h>

/*
 * @brief
 * This class represents a DRAL graph edge.
 *
 * @description
 * A DRAL graph edge is used to store all the information of  
 *
 * @author Federico Ardanaz
 * @date started at 2002-07-15
 * @version 0.1
 */
class DralGraphEdge
{
    public:
        DralGraphEdge(DralGraphNode * source_node, DralGraphNode * destination_node, UINT16 edge_id, UINT32 bandwidth, UINT32 latency, QString name);

        ~DralGraphEdge();

        inline DralGraphNode * getSourceNode() const;
        inline DralGraphNode * getDestinationNode() const;
        inline QString getSourceNodeName() const;
        inline QString getDestinationNodeName() const;
        inline DRAL_ID getEdgeId() const;
        inline CLOCK_ID getClockId() const;
        inline UINT32 getBandwidth() const;
        inline bool setBandwidth(UINT32 bw);
        inline UINT32 getLatency() const;
        inline QString getName() const;
        inline QString getCompleteName() const;

    private:
        UINT32 bandwidth;                 ///< Maximum bandwidth of the egdes (items per cycle).
        UINT32 latency;                   ///< Latency of the edge.
        DralGraphNode * source_node;      ///< Source node of the edge.
        DralGraphNode * destination_node; ///< Destination node of the edge.
        DRAL_ID edge_id;                  ///< Holds the id of the edge. Unique value.
        QString edgeName;                 ///< The name of the edge.
        bool bw_set;                      ///< Has the bandwidth been manually set?
};

/**
  * @typedef DBGEList
  * @brief
  * This struct is a dictionary of edges indexed by integers.
  */
typedef QIntDict<DralGraphEdge> DBGEList;

/**
 * Returns the source node of the edge.
 *
 * @return the source node.
 */
DralGraphNode*
DralGraphEdge::getSourceNode() const
{
    return source_node;
}

/**
 * Returns the destination node of the edge.
 *
 * @return the destination node.
 */
DralGraphNode*
DralGraphEdge::getDestinationNode() const
{
    return destination_node;
}

/**
 * Returns the source node name of the edge.
 *
 * @return the source node name.
 */
QString
DralGraphEdge::getSourceNodeName() const
{
    return (source_node->getName());
}

/**
 * Returns the destination node name of the edge.
 *
 * @return the destination node name.
 */
QString
DralGraphEdge::getDestinationNodeName() const
{
    return (destination_node->getName());
}

/*
 * Returns the id of the edge.
 *
 * @return the id.
 */
DRAL_ID
DralGraphEdge::getEdgeId() const
{
    return edge_id;
}

/*
 * Returns the id of the clock domain of the edge. The edge domain
 * is specified by the node reader domain.
 *
 * @return the clock id.
 */
CLOCK_ID
DralGraphEdge::getClockId() const
{
    return destination_node->getClockId();
}

/*
 * Returns the bandwidth of the edge.
 *
 * @return the bandwidth.
 */
UINT32
DralGraphEdge::getBandwidth() const
{
    Q_ASSERT(bandwidth != (UINT32) -1);
    return bandwidth;
}

/*
 * Sets the bandwidth
 *
 * @return the bandwidth.
 */
bool
DralGraphEdge::setBandwidth(UINT32 bw)
{
    if(bw_set)
    {
        return false;
    }
    else
    {
        bw_set = true;
        bandwidth = bw;
    }
    return true;
}

/**
 * Returns the latency of the edge.
 *
 * @return the latency.
 */
UINT32
DralGraphEdge::getLatency() const
{
    return latency;
}

/**
 * Returns the name of the edge.
 *
 * @return the name.
 */
QString
DralGraphEdge::getName() const
{
    return edgeName;
}

/**
 * Returns the name of the edge followed by the name of the source
 * node plus the name of the destination node.
 *
 * @return the complete name.
 */
QString
DralGraphEdge::getCompleteName() const
{
    return edgeName + ";" + source_node->getName() + ";" + destination_node->getName();
}

#endif

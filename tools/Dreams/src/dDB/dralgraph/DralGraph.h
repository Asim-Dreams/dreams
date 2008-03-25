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
  * @file  DralGraph.h
  */

#ifndef _DRALDB_DBGRAPH_H
#define _DRALDB_DBGRAPH_H

// General includes.
#include <vector>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/dralgraph/DralGraphEdge.h"
#include "dDB/dralgraph/DralGraphNode.h"

// Class forwarding.
class LogMgr;

/**
  * @brief
  * This class stores all the nodes and edges of the DRAL graph.
  *
  * @description
  * This class is used to store all the nodes and edges that
  * are used in the DRAL trace. Both nodes and edges can be
  * found using their ids and the nodes can be indexed using
  * their name too.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DralGraph
{
    public:
        DralGraph();
        ~DralGraph();

        bool addNode(QString name, UINT16 node_id, UINT16 parent_id, UINT16 instance);
        bool addEdge(UINT16 source_node, UINT16 destination_node, UINT16 edge_id, UINT32 bandwidth, UINT32 latency, QString  name);

        inline INT32 getNumEdges() const;
        inline INT32 getNumNodes() const;

        inline DralGraphNode * getNode(UINT16 node_id);
        inline DralGraphNode * getNode(QString name);
        inline INT32 getNodeCapacity(UINT16 node_id) const;
        inline INT32 getNodeInputBW(UINT16 node_id) const;
        inline INT32 getNodeOutputBW(UINT16 node_id) const;
        bool setNodeLayout(UINT16 node_id, NodeSlot layout);

        inline DralGraphEdge * getEdge(UINT16 edge_id);
        DralGraphEdge * findEdgeByNameFromTo(QString name,QString fromstr,QString tostr);
        bool findEdgeByName(QString name, DralGraphEdge* &edge);
        inline INT32 getEdgeBandwidth(UINT16 edge_id) const;
        inline INT32 getEdgeLatency(UINT16 edge_id) const;

        void setDefaultClock();

        UINT32 getBiggestLatency() const;

        QString getGraphDescription() const;

        QString normalizeNodeName(QString name) const;
        QString slotedNodeName(QString name, NodeSlot nslot) const;
        bool decodeNodeSlot(QString strnodeslot, QString * nodename, NodeSlot * nslot) const;

    private:
        DBGNList * dbgnList;             ///< List used to obtain the nodes indexing with their ids.
        DBGNListByName * dbgnListByName; ///< List used to obtain the nodes indexing with their names.
        vector<DralGraphEdge *> edges;     ///< Vector with all the edges.
        INT32 max_edge;                  ///< Max edge id assigned.
        INT32 num_edges;                 ///< Number of edges.
        LogMgr * myLogMgr;               ///< Log file.
};

/*
 * Searches the node with id node_id using the node list indexed
 * with integers.
 *
 * @return the node with id node_id.
 */
DralGraphNode *
DralGraph::getNode(UINT16 node_id)
{
    return dbgnList->find((long) node_id);
}

/*
 * Searches the node with name name using the node list indexed
 * with names.
 *
 * @return the node with name name.
 */
DralGraphNode *
DralGraph::getNode(QString name)
{
    return dbgnListByName->find(name);
}

/*
 * Searches the node with id node_id using the node list indexed
 * with integers. Then returns the capacity of this node.
 *
 * @return the node capcity if the node exists. -1 otherwise.
 */
INT32
DralGraph::getNodeCapacity(UINT16 node_id) const
{
    DralGraphNode * node = dbgnList->find((long) node_id);
    if(node == NULL)
    {
        return -1;
    }
    return node->getCapacity();
}

/*
 * Searches the node with id node_id using the node list indexed
 * with integers. Then returns the input bandwidth of this node.
 *
 * @return the node input bandwidth if the node exists. -1 otherwise.
 */
INT32
DralGraph::getNodeInputBW(UINT16 node_id) const
{
    DralGraphNode* node = dbgnList->find((long) node_id);
    if(node == NULL)
    {
        return -1;
    }
    return node->getInputBW();
}

/**
 * Searches the node with id node_id using the node list indexed
 * with integers. Then returns the output bandwidth of this node.
 *
 * @return the node output bandwidth if the node exists. -1 otherwise.
 */
INT32
DralGraph::getNodeOutputBW(UINT16 node_id) const
{
    DralGraphNode * node = dbgnList->find((long) node_id);
    if(node == NULL)
    {
        return -1;
    }
    return node->getOutputBW();
}

/**
 * Searches the edge with id edge_id using the edge list indexed
 * with integers.
 *
 * @return the edge with id edge_id.
 */
DralGraphEdge *
DralGraph::getEdge(UINT16 edge_id)
{
    return edges[edge_id];
}


/**
 * Searches the edge with id edge_id using the edge list indexed
 * with integers. Then returns the bandwidth of this edge.
 *
 * @return the edge bandwidth if the edge exists. -1 otherwise.
 */
INT32
DralGraph::getEdgeBandwidth(UINT16 edge_id) const
{
    if(edges[edge_id] == NULL)
    {
        return -1;
    }
    else
    {
        return (INT32) edges[edge_id]->getBandwidth();
    }
}

/**
 * Searches the edge with id edge_id using the edge list indexed
 * with integers. Then returns the latency of this edge.
 *
 * @return the edge latency if the edge exists. -1 otherwise.
 */
INT32
DralGraph::getEdgeLatency(UINT16 edge_id) const
{
    if(edges[edge_id] == NULL)
    {
        return -1;
    }
    else
    {
        return (INT32) edges[edge_id]->getLatency();
    }
}

#endif

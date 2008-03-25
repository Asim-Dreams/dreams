// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file DralGraphNode.h
  */

#ifndef _DRALGRAPHEDGE_H
#define _DRALGRAPHEDGE_H

// Qt library
#include <qstring.h>
#include <qcolor.h>
#include <qintdict.h>

#include "asim/DralDB.h"

// 2Dreams
#include "ADFDefinitions.h"
#include "EventShape.h"
#include "RowRules.h"
#include "DralGraphNode.h"
/**
  * @def DEFAULT_DGE_SIZE
  * @brief This is default size used for hashed list of Dral Edges.
  * Bear in mind that this MUST be a prime number.
  * To look for other prime numbers: http://www.mathematical.com/primes1to100k.html
  */
#define DEFAULT_DGE_SIZE 2111

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DralGraphEdge
{
    public:
        DralGraphEdge (
                      DralGraphNode* source_node,
                      DralGraphNode* destination_node,
                      UINT16 edge_id,
                      UINT32 bandwidth,
                      UINT32 latency,
                      QString name
                      );

        ~DralGraphEdge();

        inline DralGraphNode* getSourceNode() { return source_node; }
        inline DralGraphNode* getDestinationNode() { return destination_node; }
        inline QString getSourceNodeName() { return (source_node->getName()); }
        inline QString getDestinationNodeName() { return (destination_node->getName()); }
        inline UINT16 getEdgeId() { return edge_id; }
        inline UINT32 getBandwidth() { return bandwidth; }
        inline UINT32 getLatency() { return latency; }
        inline QString  getName() { return edgeName; }

        inline bool configured() { return isConfigured; }
        inline void setConfigured(bool v) { isConfigured=v; }

        //inline void setDfs(INT32 v) { dfsNum=v; }
        //inline INT32 getDfs() { return dfsNum; }

        inline bool      addRow(INT16 row);
        inline RowRules* getRow(INT16 row);
        inline RRList*   getRowList();

    private:

        // DRAL edge attributes
        UINT32 bandwidth;
        UINT32 latency;
        DralGraphNode* source_node;
        DralGraphNode* destination_node;
        UINT16 edge_id;
        QString edgeName;

        bool  isConfigured;
        //INT32 dfsNum;

        RRList* rowlist;
};

bool
DralGraphEdge::addRow(INT16 row)
{
    RowRules* rule = rowlist->find((long)row);
    if (rule!=NULL) return false;
    rule = new RowRules(edge_id,row,SimpleEdgeRow);
    rowlist->insert((long)row,rule);
    return true;
}

RowRules*
DralGraphEdge::getRow(INT16 row)
{ return rowlist->find((long)row); }

RRList*
DralGraphEdge::getRowList()
{ return rowlist; }

/**
  * @typedef DGEList
  * @brief descrip.
  */
typedef QIntDict<DralGraphEdge> DGEList;


#endif


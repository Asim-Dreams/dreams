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

#ifndef _DRALGRAPHNODE_H
#define _DRALGRAPHNODE_H

// Qt library
#include <qstring.h>
#include <qcolor.h>
#include <qvaluelist.h>
#include <qdict.h>
#include <qintdict.h>

#include "asim/DralDB.h"

// 2Dreams
#include "ADFDefinitions.h"
#include "RowRules.h"

/**
  * @def DEFAULT_DGN_SIZE
  * @brief This is default size used for hashed list of Dral Nodes.
  * Bear in mind that this MUST be a prime number.
  * To look for other prime numbers: http://www.mathematical.com/primes1to100k.html
  */
#define DEFAULT_DGN_SIZE 211


/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DralGraphNode
{
    public:
        DralGraphNode (
                      QString name,
                      UINT16 node_id,
                      UINT16 parent_id,
                      UINT16 instance
                      );

        ~DralGraphNode();

        inline QString getName() { return name; }
        inline UINT16 getNodeId() { return node_id; }
        inline UINT32 getCapacity() { return capacity; }
        inline void setCapacity(UINT32 val) { capacity=val; }

        inline bool configured() { return isConfigured; }
        inline void setConfigured(bool v) { isConfigured=v; }

        inline bool addSimpleNodeRow(INT16 row);
        inline bool addSlotedNodeRow(INT16 row,NodeSlot slot);

        inline RowRules* getRow(INT16 row);
        inline RRList*   getRowList();

    private:
        QString name;
        UINT32 node_id;
        UINT32 capacity;
        UINT16 parent_id;
        UINT16 instance;

        bool   isConfigured;
        RRList* rowlist;
};

bool
DralGraphNode::addSimpleNodeRow(INT16 row)
{
    RowRules* rule = rowlist->find((long)row);
    if (rule!=NULL) { return false; }
    rule = new RowRules(node_id,row,SimpleNodeRow);
    rowlist->insert((long)row,rule);
    //printf("DralGraphNode::addSimpleNodeRow on row %d \n",(int)row);
    return true;
}

bool
DralGraphNode::addSlotedNodeRow(INT16 row,NodeSlot slot)
{
    RowRules* rule = rowlist->find((long)row);
    if (rule!=NULL) { return false; }
    rule = new RowRules(node_id,row,SlotNodeRow);
    rowlist->insert((long)row,rule);
    // set slot...
    return true;
}

RowRules*
DralGraphNode::getRow(INT16 row)
{ return rowlist->find((long)row); }

RRList*
DralGraphNode::getRowList()
{ return rowlist; }


/**
  * @typedef DGNList
  * @brief descrip.
  */
typedef QIntDict<DralGraphNode> DGNList;

/**
  * @typedef DGNListByName
  * @brief descrip.
  */
typedef QDict<DralGraphNode> DGNListByName;

#endif


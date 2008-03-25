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
  * @file DralGraphEdge.cpp
  */

// Dreams includes.
#include "dDB/dralgraph/DralGraphEdge.h"

/**
 * Creator of this class. Just assigns all the parameters to the
 * fields.
 *
 * @return new object.
 */
DralGraphEdge::DralGraphEdge(DralGraphNode * psource_node, DralGraphNode * pdestination_node, UINT16 pedge_id, UINT32 pbandwidth, UINT32 platency, QString pname)
{
    this->source_node = psource_node;
    this->destination_node = pdestination_node;
    this->edge_id = pedge_id;
    this->bandwidth = pbandwidth;
    this->latency = platency;
    this->edgeName = pname;
    bw_set = false;
}

/**
 * Destructor of this class. Nothing is done.
 *
 * @return destroys the object.
 */
DralGraphEdge::~DralGraphEdge()
{
}

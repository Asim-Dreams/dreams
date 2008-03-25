// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
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
    @file   node.cpp
    @author Federico Ardanaz
 */


#include "vcgdNode.h"

NodeDB::NodeDB()
{ }

NodeDB::~NodeDB()
{ }

string
NodeDB::getNode(UINT16 id)
{
    string result = "_unknownNode";
    NODE_MAP_ID::iterator it = nidmap.find(id);
    if (it!=nidmap.end())
    { result=(*it).second.nodeName; }

    return result;
}
      
NODE_MAP_ID::iterator 
NodeDB::findNode(UINT16 id)
{ return nidmap.find(id); }

      
NODE_REGISTRY 
NodeDB::getNodeRegisrty(UINT16 id) 
{
    NODE_REGISTRY result;    
    result.valid=0;

    NODE_MAP_ID::iterator it = nidmap.find(id);
    if (it!=nidmap.end())
    {
        result=(*it).second; 
        result.valid=1;
    }
    return result;
}

void
NodeDB::addNode(UINT16 id, string name, UINT16 parent, vcgNode* nodeT, vcgNode* nodeC)
{
    pair<UINT16,NODE_REGISTRY> element;
    element.first = id;
    element.second.nodeName=name;
    element.second.id=id;
    element.second.parent=parent;
    element.second.nodeT = nodeT;
    element.second.nodeC = nodeC;
    nidmap.insert(element);    
}

NodeDB*
NodeDB::getInstance()
{
    if (!_instance) { _instance = new NodeDB(); }
    return _instance;
}

NodeDB* NodeDB::_instance=NULL;

NODE_MAP_ID::iterator 
NodeDB::getFirstEntry()
{
    return nidmap.begin();
}

NODE_MAP_ID::iterator 
NodeDB::getEndmark()
{
    return nidmap.end();
}



/*****************************************************************************
 *
 * @author Oscar Rosell
 *
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
 *****************************************************************************/
#include "node_db.h"
#include <iostream>

using namespace std;

CLOCK_DB_CLASS::CLOCK_DOMAIN::CLOCK_DOMAIN()
  : currentCycle_(0)
{
}

CLOCK_DB_CLASS::CLOCK_DOMAIN::CLOCK_DOMAIN(UINT16 clock_id, UINT64 freq, UINT16 skew, const char* const name)
  : clockId_(clock_id),
    freq_(freq),
    skew_(skew),
    name_(name),
    currentCycle_(0),
    cyc2ns_factor_(100.0/freq)
{
}

void 
CLOCK_DB_CLASS::addClock(UINT16 clock_id, UINT64 freq, UINT16 skew, const char* const name )
{
    cid2Info_[clock_id] = CLOCK_DOMAIN(clock_id, freq, skew, name);
}

UINT64
CLOCK_DB_CLASS::getCycle(UINT16 clock_id) const
{
    map<UINT16, CLOCK_DOMAIN>::const_iterator it = cid2Info_.find(clock_id);
    assert(it != cid2Info_.end());
    return it->second.currentCycle_;
}

string
CLOCK_DB_CLASS::getName(UINT16 clock_id) const
{
    map<UINT16, CLOCK_DOMAIN>::const_iterator it = cid2Info_.find(clock_id);
    assert(it != cid2Info_.end());
    return it->second.name_;
}

void
CLOCK_DB_CLASS::setCycle(UINT16 clock_id, UINT64 cycle)
{
    assert(cid2Info_.find(clock_id) != cid2Info_.end());
    cid2Info_[clock_id].currentCycle_ = cycle;
}

double
CLOCK_DB_CLASS::cyclesToNanoseconds(UINT16 clock_id, UINT64 cycles) const
{
    map<UINT16, CLOCK_DOMAIN>::const_iterator it = cid2Info_.find(clock_id);
    if(it == cid2Info_.end()) {
        return(-1);
    } else {
        return cycles * it->second.cyc2ns_factor_; 
    }
    assert(it != cid2Info_.end());
}

map<UINT32, NODE_DB_CLASS::NODE_INFO> NODE_DB_CLASS::nid2Info_;

NODE_DB_CLASS::NODE_INFO::NODE_INFO()
{
    clockId_ = NULL_CLOCK_ID;
}

NODE_DB_CLASS::NODE_INFO::NODE_INFO(string name, UINT16 instance)
{
    DEBUG(std::cout << "NODE: " << name << " INSTANCE: " << instance << std::endl);
    name_ = name;
    instance_ = instance;
    clockId_ = NULL_CLOCK_ID;
    active_ = true;
};

void 
NODE_DB_CLASS::addNode(UINT32 id, const string name, UINT16 instance)
{
    nid2Info_[id] = NODE_INFO(name,instance);
};

string
NODE_DB_CLASS::getNodeName(UINT32 node_id)
{
    return getNodeInfo(node_id).name_;
};

UINT32
NODE_DB_CLASS::getNodeId(string name, UINT16 instance)
{
    for (std::map<UINT32, NODE_INFO>::const_iterator it = nid2Info_.begin(); it != nid2Info_.end(); ++it)
    {
        if (it->second.name_ == name && it->second.instance_ == instance)
        {
            return it->first;
        }
    }
    return NULL_NODE_ID;
}

UINT16 
NODE_DB_CLASS::getNodeInstance(UINT32 node_id)
{
    return getNodeInfo(node_id).instance_;
};

NODE_DB_CLASS::NODE_INFO 
NODE_DB_CLASS::getNodeInfo(UINT32 node_id) 
{
    assert(nid2Info_.find(node_id)!=nid2Info_.end());
    return nid2Info_[node_id];
};

UINT16
NODE_DB_CLASS::getNodeClock(UINT16 node_id)
{
    return getNodeInfo(node_id).clockId_;
}

UINT64 
NODE_DB_CLASS::getNodeCycle(UINT16 node_id)
{
    return THE_CLOCK_DB::Instance().getCycle(getNodeClock(node_id));
}

void
NODE_DB_CLASS::setNodeClock(UINT16 node_id, UINT16 clock_id)
{
    assert(nid2Info_.find(node_id)!=nid2Info_.end());
    nid2Info_[node_id].clockId_ = clock_id;
}

void
NODE_DB_CLASS::expandClock()
{
    for (map<UINT32, NODE_INFO>::const_iterator it = nid2Info_.begin(); it != nid2Info_.end(); ++it)
    {
        expandClock(it->first);
    }
}

void
NODE_DB_CLASS::expandClock(UINT32 node_id)
{
    if (node_id == NULL_NODE_ID) 
        return;

    if (nid2Info_[node_id].clockId_ == NODE_INFO::NULL_CLOCK_ID)
    {   
        const UINT16 parent_id = nid2Info_[node_id].parentId_;
        if (parent_id != node_id)
        {
            expandClock(parent_id);
            nid2Info_[node_id].clockId_ = nid2Info_[parent_id].clockId_;
        }
    }
}

void
NODE_DB_CLASS::setParent(UINT16 node_id, UINT16 parent_id)
{
    assert(nid2Info_.find(node_id)!=nid2Info_.end());
    nid2Info_[node_id].parentId_ = parent_id;
}

void
NODE_DB_CLASS::setActivationStatus(UINT16 node_id, bool active)
{
    assert(nid2Info_.find(node_id)!=nid2Info_.end());
    nid2Info_[node_id].active_ = active;
}

void
NODE_DB_CLASS::setActivationStatus(string name, bool active)
{
    for (std::map<UINT32, NODE_INFO>::iterator it = nid2Info_.begin(); it != nid2Info_.end(); ++it)
    {
        if (it->second.name_.find(name) != string::npos)
        {
            // Found
            it->second.active_ = active;
        }
    }
}

bool
NODE_DB_CLASS::isActive(UINT16 node_id)
{
    assert(nid2Info_.find(node_id)!=nid2Info_.end());
    return nid2Info_[node_id].active_;
}

double
NODE_DB_CLASS::cyclesToNanoseconds(UINT16 node_id, UINT64 cycles)
{
    if(nid2Info_.find(node_id)==nid2Info_.end()) {
        return(-1);
    } else {
        return THE_CLOCK_DB::Instance().cyclesToNanoseconds(nid2Info_[node_id].clockId_, cycles);
    }
    assert(nid2Info_.find(node_id)!=nid2Info_.end());
}

void
NODE_DB_CLASS::clear()
{
    nid2Info_.clear();
};


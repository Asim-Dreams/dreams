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
#ifndef NODE_DB_H
#define NODE_DB_H
#include <asim/dralListener.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <Singleton.h>

#define NDEBUG 

#ifdef NDEBUG
#define DEBUG(x) 
#else
#define DEBUG(x) x 
#endif

class CLOCK_DB_CLASS
{
  public:
    struct CLOCK_DOMAIN
    {
        CLOCK_DOMAIN();
        CLOCK_DOMAIN(UINT16 clock_id, UINT64 freq, UINT16 skew, const char* const name);

        UINT16 clockId_;
        UINT64 freq_;
        UINT16 skew_;
        std::string name_;
        UINT64 currentCycle_;
        double cyc2ns_factor_;
    };

    void
    addClock(UINT16 clock_id, UINT64 freq, UINT16 skew, const char* const name);

    UINT64
    getCycle(UINT16 clock_id) const;

    std::string 
    getName(UINT16 clock_id) const;

    void
    setCycle(UINT16 clock_id, UINT64 cycle);

    double cyclesToNanoseconds(UINT16 clock_id, UINT64 cycles) const;

  private: 
    std::map<UINT16, CLOCK_DOMAIN> cid2Info_;
};

typedef Loki::SingletonHolder<CLOCK_DB_CLASS> THE_CLOCK_DB;

class NODE_DB_CLASS
{
  public:
    struct NODE_INFO
    {
        enum {NULL_CLOCK_ID = UINT16(-1)};
        NODE_INFO();
        NODE_INFO(std::string name, UINT16 instance);

        std::string name_;
        UINT16 instance_;
        UINT16 clockId_;
        UINT32 parentId_;
        bool active_;
    };

    static const UINT32 NULL_NODE_ID = UINT32(-1);
    
    static void 
    addNode(UINT32 id, const std::string name, UINT16 instance);

    static std::string 
    getNodeName(UINT32 node_id); 

    static UINT32
    getNodeId(std::string name, UINT16 instance);

    static UINT16
    getNodeInstance(UINT32 node_id);
    
    static NODE_INFO
    getNodeInfo(UINT32 node_id);

    static UINT16
    getNodeClock(UINT16 node_id);

    static UINT64
    getNodeCycle(UINT16 node_id);

    static void
    setNodeClock(UINT16 node_id, UINT16 clock_id);

    static void
    expandClock();

    static void
    expandClock(UINT32 node_id);

    static void
    setParent(UINT16 node_id, UINT16 parent_id);

    static double
    cyclesToNanoseconds(UINT16 node_id, UINT64 cycle);

    static void
    setActivationStatus(UINT16 node_id, bool active);

    static void 
    setActivationStatus(std::string name, bool active);

    static bool
    isActive(UINT16 node_id);

    static void 
    clear();
        
  private: 
    static std::map<UINT32, NODE_INFO> nid2Info_;
};

#endif 

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
    @file   listener.h
    @author Federico Ardanaz
 */

#ifndef _VCGDREAMS_LISTENER_H
#define _VCGDREAMS_LISTENER_H

#include <string>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <vector>

#include <asim/dralListener.h>
#include "vcgGraph.h"
#include "vcgdNode.h"

class LISTENER_CLASS : public DRAL_LISTENER_CLASS
{
  public:
#ifdef VCGDREAMS_DEBUG_MODE      
      static const bool _debug_mode = true;
#else
      static const bool _debug_mode = false;
#endif

  public:

    LISTENER_CLASS(string name,int nedge, bool selfedges, int minLat);
    virtual ~LISTENER_CLASS();

    vcgGraph* GetConnectionGraph();
    vcgGraph* GetHierarchyGraph();

    bool GraphReady() { return graphReady; }
    bool DralOk()     { return !someError; }
        
  public:  
    // -----------------------------------------------------------------------
    // DRAL_LISTENER_CLASS virtual methods
    // -----------------------------------------------------------------------
    // we only care about a few callbacks

    virtual void NewNode (UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance);
    virtual void AddNode (UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance);

    virtual void NewEdge (UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,UINT32 bandwidth, UINT32 latency, char * name);
    virtual void AddEdge (UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,UINT32 bandwidth, UINT32 latency, char * name);

    virtual void Error (char * error);
    virtual void EndSimulation (void);
    virtual void StartActivity (UINT64 start_activity_cycle);

    // ignored callbacks
    virtual void Cycle (UINT64 cycle) {}; 
    virtual void NewItem (UINT32 item_id) {};
    virtual void MoveItems (UINT16 edge_id, UINT32 numOfItems, UINT32 * items) {};
    virtual void MoveItemsWithPositions (UINT16 edge_id, UINT32 numOfItems,UINT32 * items, UINT32 * positions){};
    virtual void DeleteItem (UINT32 item_id){};
    virtual void NonCriticalError (char * error){};
    virtual void Version (UINT16 version){};
    virtual void SetNodeLayout (UINT16 node_id, UINT32 capacity, UINT16 dim, UINT32 capacities []){};
    virtual void EnterNode (UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position []){};
    virtual void ExitNode (UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position []){};
    virtual void Comment (UINT32 magic_num, char comment []){};
    virtual void CommentBin (UINT16 magic_num, char * cont, UINT32 length){};
    virtual void SetNodeInputBandwidth(UINT16 node_id, UINT32 bandwidth){};
    virtual void SetNodeOutputBandwidth(UINT16 node_id, UINT32 bandwidth){};
    virtual void SetTagDescription (UINT32 tag_idx, char description []){};
    virtual void EnterNode ( UINT16 node_id, UINT32 item_id, UINT32 slot){};
    virtual void ExitNode (UINT16 node_id, UINT32 slot){};
    virtual void SetCapacity ( UINT16 node_id, UINT32 capacity, UINT32 capacities [], UINT16 dimensions){};
    virtual void SetHighWaterMark (UINT16 node_id, UINT32 mark){};
    virtual void Comment (char * comment){};
    virtual void SetNodeClock(UINT16 node_id, UINT16 clock_id){};
    virtual void NewClock ( UINT16 clockId, UINT64 freq, UINT16 skew, UINT16 divisions, const char name []){}; 
    virtual void Cycle(UINT16 clock_id, UINT64 cycle,UINT16 phase){};
    virtual void SetCycleTag(UINT32 tag_idx, UINT64 value){};
    virtual void SetCycleTagString(UINT32 tag_idx, UINT32 str_idx){};
    virtual void SetCycleTagSet(UINT32 tag_idx, UINT32 nval, UINT64 set []){};
    virtual void SetItemTag( UINT32 item_id, UINT32 tag_idx, UINT64 value){};
    virtual void SetItemTagString( UINT32 item_id, UINT32 tag_idx, UINT32 str_idx){};
    virtual void SetItemTagSet( UINT32 item_id, UINT32 tag_idx, UINT32 nval, UINT64 set []){};
    virtual void SetNodeTag( UINT16 node_id, UINT32 tag_idx, UINT64 value, UINT16 level, UINT32 list []){};
    virtual void SetNodeTagString( UINT16 node_id, UINT32 tag_idx, UINT32 str_idx, UINT16 level, UINT32 list []){} 
    virtual void SetNodeTagSet( UINT16 node_id, UINT32 tag_idx, UINT16 n, UINT64 set [], UINT16 level, UINT32 list []){};
    virtual void NewTag (UINT32 tag_idx, const char * tag_name, INT32 tag_name_len){};
    virtual void NewString (UINT32 string_idx, const char * str, INT32 str_len){};
    virtual void SetTagSingleValue ( UINT32 item_id, UINT32 tag_idx, UINT64 value, UBYTE time_span_flags){};
    virtual void SetTagString ( UINT32 item_id, UINT32 tag_idx, UINT32 str_idx, UBYTE time_span_flags){};
    virtual void SetTagSet ( UINT32 item_id, UINT32 tag_idx, UINT32 set_size, UINT64 * set, UBYTE time_span_flags){};
    
  private:    
    string NormalizeNodeName(string node_name, UINT16 instance);
    void ProduceTreeEdges();
    void PutConnectionLevels();
    void CloseGraphs();

    vcgGraph*   myConnectionGraph;
    vcgGraph*   myHierarchyGraph;
    bool        graphReady;   
    bool        someError;

    int         nearEdgeThreshold;
    bool        selfEdges;
    int         minLatency;

};


#endif


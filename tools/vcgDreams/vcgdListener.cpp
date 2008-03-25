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
    @file   listener.cpp
    @author Federico Ardanaz
 */


#include "vcgdListener.h"

using namespace std;

LISTENER_CLASS::LISTENER_CLASS(string baseTitle, int nedge, bool selfedges, int minLat)
{
    myConnectionGraph = new vcgGraph();
    myHierarchyGraph = new vcgGraph();

    ostringstream ctitle;
    ctitle << baseTitle << " - " << "Connection Graph";
    const char* cctitle = ctitle.str().c_str();
    myConnectionGraph->setTitle((char*)cctitle);
    myConnectionGraph->setLayoutAlgorithm(vcgLayoutAlgorithm_MAXDEPTHSLOW);
    myConnectionGraph->setXspace(100);
    myConnectionGraph->setYspace(150);
    myConnectionGraph->setXlspace(60);
    myConnectionGraph->setDisplayEdgeLabels(vcgYesNo_YES);
    myConnectionGraph->setDirtyEdgeLabels(vcgYesNo_YES);
    myConnectionGraph->setPriorityPhase(vcgYesNo_YES);
    myConnectionGraph->setStraightPhase(vcgYesNo_YES);

    ostringstream htitle;
    htitle << baseTitle << " - " << "Node Hierarchy Tree";
    const char* chtitle = htitle.str().c_str();
    myHierarchyGraph->setTitle((char*)chtitle);
    myHierarchyGraph->setLayoutAlgorithm(vcgLayoutAlgorithm_MAXDEPTHSLOW);
    myHierarchyGraph->setXspace(50);
    myHierarchyGraph->setYspace(80);
    myHierarchyGraph->setXlspace(30);

    graphReady = false;
    someError  = false;
    nearEdgeThreshold=nedge;
    selfEdges=selfedges;
    minLatency=minLat;
        
}

LISTENER_CLASS::~LISTENER_CLASS()
{
    delete myConnectionGraph;
    delete myHierarchyGraph;
}
    

void
LISTENER_CLASS::NewNode(UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance)
{
    if (_debug_mode)
        cout << "NewNode #" << node_id << " " << node_name << "{" << instance << "}" << "; parent " << parent_id << endl;

    string nodeName = NormalizeNodeName(node_name,instance);
    
    vcgNode* nodeT = new vcgNode((char*)(nodeName.c_str()));
    nodeT->setHorizontalOrder(instance*1000);
    nodeT->setBorderWidth(1);
    
    vcgNode* nodeC = new vcgNode((char*)(nodeName.c_str()));
    nodeC->setHorizontalOrder(instance*1500);
    nodeC->setBorderWidth(1);
    
    myConnectionGraph->addComponent(nodeC);
    myHierarchyGraph->addComponent(nodeT);

    // update the node db
    NodeDB::getInstance()->addNode(node_id,nodeName,parent_id,nodeT,nodeC);
}

void 
LISTENER_CLASS::AddNode (UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance)
{ NewNode(node_id,node_name,parent_id,instance); }

void 
LISTENER_CLASS::NewEdge (UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,UINT32 bandwidth, UINT32 latency, char * name)
{
    NodeDB* nodedb = NodeDB::getInstance();
    
    if (_debug_mode)
    {
        cout << "NewEdge #" << edge_id << " " << name << "<" << sourceNode << "," << destNode << ">" << " lat="<< latency;
        cout << " bw=" << bandwidth << endl;
    }
    NODE_MAP_ID::iterator nodeFromIt =  nodedb->findNode(sourceNode);
    NODE_MAP_ID::iterator nodeToIt   =  nodedb->findNode(destNode);
    NODE_MAP_ID::iterator end        =  nodedb->getEndmark();

    // check edge coherency
    if ((nodeFromIt==end)||(nodeToIt==end)) return;

    // check self-edges
    if (!selfEdges && (nodeFromIt==nodeToIt)) return;

    // check minLat
    if (latency<minLatency) return;

    string nodeFrom = (nodeFromIt->second).nodeName; 
    string nodeTo   = (nodeToIt->second).nodeName; 

    ostringstream sname;
    sname << name << "<"<<latency<<","<<bandwidth<<">";
    
    vcgEdge* edge   = new vcgEdge((char*)(nodeFrom.c_str()),
            (char*)(nodeTo.c_str()),(char*)((sname.str()).c_str()));
            
    edge->setThickness(1);

    // near edge? not working that well... disabled
    /*
    bool near=false;
    if (latency < nearEdgeThreshold)
    {
        if (nodeTo.compare(nodeFrom))
        {
            int fromCnt=(nodeFromIt->second).nearCnt;
            int toCnt=(nodeToIt->second).nearCnt;
            near = (fromCnt<2 && toCnt<2);
            if (near)
            {
                (nodeFromIt->second).nearCnt++;
                (nodeToIt->second).nearCnt++;
            }
        }
    }
    edge->setNear(near);
    */

    myConnectionGraph->addComponent(edge);   
}

void 
LISTENER_CLASS::AddEdge (UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,UINT32 bandwidth, UINT32 latency, char * name)
{ NewEdge(sourceNode,destNode,edge_id,bandwidth,latency,name); }
    
void 
LISTENER_CLASS::Error (char * error)
{
    someError=true;
    cerr << "Dral IO error: " << error << endl;
}

void 
LISTENER_CLASS::StartActivity(UINT64 start_activity_cycle)
{ CloseGraphs(); }

void 
LISTENER_CLASS::EndSimulation()
{ CloseGraphs(); }

void
LISTENER_CLASS::CloseGraphs()
{
    graphReady=true;
    ProduceTreeEdges();
    PutConnectionLevels();
}

void
LISTENER_CLASS::ProduceTreeEdges()
{
    NodeDB* nodedb = NodeDB::getInstance();
    
    NODE_MAP_ID::iterator it = nodedb->getFirstEntry();
    NODE_MAP_ID::iterator end = nodedb->getEndmark();
    while (it!=end)
    {
        string nodeTo = (*it).second.nodeName;
        UINT16 pa =     (*it).second.parent;
        string nodeFrom = nodedb->getNode(pa);
        vcgEdge* edge   = new vcgEdge((char*)(nodeFrom.c_str()),(char*)(nodeTo.c_str()));
        myHierarchyGraph->addComponent(edge);

        // now build level sets
        if (!pa)
        {
            (it->second).treeVLevel=0;
            (it->second).nodeT->setVerticalOrder(0);
        }
        else
        {
            NODE_MAP_ID::iterator itparent = nodedb->findNode(pa);
            if (itparent!=end)
            {
                (it->second).treeVLevel= (itparent->second).treeVLevel + 1;
                (it->second).nodeT->setVerticalOrder((it->second).treeVLevel);
            }
            else
            { 
                (it->second).treeVLevel=0; 
                (it->second).nodeT->setVerticalOrder(0);
            }
        }
        
        ++it;
    }
}

void
LISTENER_CLASS::PutConnectionLevels()
{
}

vcgGraph* 
LISTENER_CLASS::GetConnectionGraph()
{
    return (graphReady ? myConnectionGraph : NULL);
}

vcgGraph* 
LISTENER_CLASS::GetHierarchyGraph()
{
    return (graphReady ? myHierarchyGraph : NULL);
}

string
LISTENER_CLASS::NormalizeNodeName(string node_name, UINT16 instance)
{
    ostringstream nname;
    nname << node_name << "{" << instance << "}";
    return nname.str();
}


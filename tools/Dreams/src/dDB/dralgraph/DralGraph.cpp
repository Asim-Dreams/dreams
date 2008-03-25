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
  * @file DralGraph.cpp
  */

// Dreams includes.
#include "dDB/LogMgr.h"
#include "dDB/dralgraph/DralGraph.h"

// Qt includes.
#include <qvaluelist.h>
#include <qregexp.h>

/**
 * Creator of this class.
 *
 * @return new object.
 */
DralGraph::DralGraph() : edges(65536)
{
    // Allocates the lists.
    dbgnList       = new DBGNList(DEFAULT_DBGN_SIZE);
    dbgnListByName = new DBGNListByName(DEFAULT_DBGN_SIZE);

    max_edge = 0;
    num_edges = 0;

    Q_ASSERT(dbgnList!=NULL);
    Q_ASSERT(dbgnListByName!=NULL);

    for(UINT32 i = 0; i < 65536; i++)
    {
        edges[i] = NULL;
    }

    // Sets true autodelete the lists.
    dbgnList->setAutoDelete(true);

    // Nodes objects are the same than in dbgeList!
    dbgnListByName->setAutoDelete(false);

    myLogMgr = LogMgr::getInstance();
}

/**
 * Destructor of this class.
 *
 * @return destroys the object.
 */
DralGraph::~DralGraph()
{
    delete dbgnListByName;
    delete dbgnList;

    for(INT32 i = 0; i < max_edge; i++)
    {
        if(edges[i] == NULL)
        {
            delete edges[i];
        }
    }
}

/**
 * Gets the edge that has the name name and the name of its
 * source node is fromstr and the name of its destination node
 * is tostr.
 *
 * @return the requested edge.
 */
DralGraphEdge *
DralGraph::findEdgeByNameFromTo(QString name, QString fromstr, QString tostr)
{
    bool fnd = false;
    INT32 it = 0;
    DralGraphEdge * edge;

    for(INT32 i = 0; i < max_edge; i++)
    {
        edge = edges[i];
        if(
            (edge != NULL) &&
            (name == edge->getName()) &&
            (fromstr == edge->getSourceNodeName()) &&
            (tostr == edge->getDestinationNodeName())
        )
        {
            return edge;
        }
    }
    return NULL;
}

/** 
 * Gets the edge that has the name name. If more than one edge
 * with the given is found, no edge is returned
 *
 * @param name Name of the searched edge
 * @out Edge found
 * @return TRUE if any edge with the given name is found.
 */
bool
DralGraph::findEdgeByName(QString name, DralGraphEdge* &edge)
{
    bool fnd = false;
    DralGraphEdge *current = NULL;
    edge = NULL;
    for(INT32 i = 0; i < max_edge; i++)
    {
        current = edges[i];
        if(name == current->getName()) 
        {
            // More than one edge with the same name has been found
            if(edge != NULL) 
            {
                edge = NULL;
                return true;
            }

            fnd = true;
            edge = current;
        }
    }
    return fnd;
}

UINT32
DralGraph::getBiggestLatency() const
{
    UINT32 result = 0;
    DralGraphEdge * edge;

    for(INT32 i = 0; i < max_edge; i++)
    {
        edge = edges[i];
        result = QMAX(result, edge->getLatency());
    }

    return result;
}

/**
 * Adds a new node to the DRAL graph. A new node is created and is
 * added in the two node lists. If the node_id or the name are
 * repeated the node is not added.
 *
 * @return true if the node can be added false otherwise.
 */
bool
DralGraph::addNode(QString name, UINT16 node_id, UINT16 parent_id, UINT16 instance)
{
    // compute internal name
    QString nodeName = name+"{"+QString::number(instance)+"}";

    // 1) check that the ID is not repeated
    /** @todo we must relay on "long" qt type, review this! */
    DralGraphNode* dgnode = dbgnList->find((long)node_id);
    if (dgnode!=NULL)
    {
        myLogMgr->addLog ("DralDB: Repeated DRAL node detected (ID "+QString::number(node_id)+
        ") in DRAL file, please report this problem.");
        return false;
    }

    // 2) check if the name is repeated
    dgnode = dbgnListByName->find(nodeName);
    if (dgnode!=NULL)
    {
        myLogMgr->addLog ("DralDB: Repeated DRAL node detected (Name " + name +
        ", Instance="+QString::number(instance) + ") in DRAL file, please report this problem.");
        return false;
    }

    // Creates the node and inserts it in the name and node_id lists.
    dgnode = new DralGraphNode(nodeName, node_id, parent_id, instance, this);
    Q_ASSERT(dgnode!=NULL);

    dbgnList->insert((long) node_id, dgnode);
    dbgnListByName->insert(nodeName, dgnode);

    return true;
}

/**
 * Adds a new edge to the DRAL graph. A new edge is created and is
 * added in the edge list. If the edge_id is repeated or the source
 * or destination node doesn't exist the edge is not added.
 *
 * @return true if the edge can be added false otherwise.
 */
bool 
DralGraph::addEdge(UINT16 source_node, UINT16 destination_node, UINT16 edge_id, UINT32 bandwidth, UINT32 latency, QString name)
{
    // check that the ID is not repeated
    if(edges[edge_id] != NULL)
    {
        QString err = "DralDB: Repeated DRAL edge detected (ID " + QString::number(edge_id) + ") in DRAL file, please report this problem.";
        myLogMgr->addLog(err);
        return false;
    }

    max_edge = QMAX(max_edge, edge_id + 1);
    num_edges++;

    // Check that both source and destination nodes exists.
    DralGraphNode* srcNode = dbgnList->find((long)source_node);
    DralGraphNode* dstNode = dbgnList->find((long)destination_node);
    if((srcNode == NULL) || (dstNode == NULL))
    {
        QString err = "DralDB: Adding DRAL edge over unknown nodes(from ID " + QString::number(source_node) + " to ID " + QString::number(destination_node) + "), please report this problem.";
        myLogMgr->addLog(err);
        return false;
    }

    // Creates the edge and inserts it in the node_id list.
    edges[edge_id] = new DralGraphEdge(srcNode, dstNode, edge_id, bandwidth, latency, name);

    return true;
}

/*
 * Sets a clock id 0 to all the parent nodes. Used for backward compatibility
 * with the none clock server traces.
 *
 * @return void.
 */
void
DralGraph::setDefaultClock()
{
    QIntDictIterator<DralGraphNode> it_node(* dbgnList); ///< Node iterator.
    DralGraphNode * node;                                ///< Node.

    // Runs through all the nodes.
    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        // If the node is a root node, then sets the clock id to 0.
        if(node->getNodeId() == node->getParentNodeId())
        {
            node->setClockId(0);
        }
    }
}

/**
 * Method to unify the name of the nodes that conforms the DralGraph It also
 * keeps the compatibility with the previous versions of ADF & Dral
 *
 * @param name Node name to normalize
 * @result normalized node name
 */
QString
DralGraph::normalizeNodeName(QString name) const
{
    QString result=name;
    QRegExp rexp();
    bool ok = false;

    // Backwards compatibility check.
    if(name[name.length() - 1] == ']')
    {
        INT32 i = name.length() - 2;

        // Skips the instance.
        while(i >= 0 && name[i] >= '0' && name[i] <= '9')
        {
            i--;
        }

        // Changes claudators for keys.
        if(i >= 0 && name[i] == '[')
        {
            result[i] = '{';
            result[name.length() - 1] = '}';
            ok = true;
        }
    }
    if(!ok)
    {
        int pos = name.find(QChar('{'));
        if (pos<0)
        {
            result += "{0}";
        }
    }

    return result;
}

QString
DralGraph::slotedNodeName(QString name, NodeSlot slot) const
{
    QString nname = DralGraph::normalizeNodeName(name);
    if(slot.dimensions == 0)
    {
        return nname;
    }
    QString slotstr = "{";
    for(UINT16 i = 0; i < slot.dimensions; i++)
    {
        slotstr += QString::number(slot.capacities[i]);
        if(i < (slot.dimensions - 1))
        {
            slotstr += ",";
        }
    }
    slotstr += "}";
    return nname + slotstr;
}

bool
DralGraph::decodeNodeSlot(QString strnodeslot, QString * rnodename, NodeSlot * rnslot) const
{
    //printf (">>decodeNodeSlot called with=%s\n",strnodeslot.latin1());fflush(stdout);
    // separate node name from slot specification

    QString nodename=QString::null;
    QString slotspec=QString::null;
    int pos = strnodeslot.find(QChar(';'));
    if (pos<0)
    {
        nodename=strnodeslot;
    }
    else
    {
        nodename = strnodeslot.left(pos);
        slotspec = strnodeslot.right(strnodeslot.length()-pos-1);
    }
    // set the result node name
    *rnodename = DralGraph::normalizeNodeName(nodename);

    // if slot spec -> root & we are done
    if (slotspec==QString::null)
    {
        rnslot->dimensions=0;
        rnslot->capacities=NULL;
        return true;
    }

    // decode the node slot from x,y,z,d to a NodeSlot struct
    QRegExp rx( "\\d+" );
    QValueList<UINT32> tmplist;
    int count = 0;
    pos = 0;
    while ( pos >= 0 )
    {
        pos = rx.search( slotspec, pos );
        if (pos>=0)
        {
            QString dimspec = slotspec.mid(pos,rx.matchedLength());
            bool pok;
            uint dim = dimspec.toUInt (&pok);
            if (!pok) return false;
            tmplist.append((UINT32) dim);
            count++;
        }
        pos += rx.matchedLength();
    }

    if (count==0) // nothing after ;
    {
        rnslot->dimensions = 0;
        rnslot->capacities = NULL;
        return true;
    }
    else
    {
        UINT32* vec = new UINT32[count];
        QValueList<UINT32>::Iterator it = tmplist.begin();
        int idx=0;
        while (it != tmplist.end())
        {
            vec[idx]= *it;
            ++it;
            ++idx;
        }
        rnslot->dimensions = count;
        rnslot->capacities = vec;
        return true;
    }
}

/**
 * Sets the layout to the node with id node_id.
 *
 * @return true if the node exists. False otherwise.
 */
bool
DralGraph::setNodeLayout(UINT16 node_id, NodeSlot layout)
{
    // look for the node
    DralGraphNode* node = dbgnList->find((long) node_id);
    if (node==NULL)
    {
        myLogMgr->addLog("DralDB: Setting layout over an unknown node, (ID "+
        QString::number((int)node_id)+") on the DRAL file, please report this problem.");
        return false;
    }
    node->setLayout(layout);
    return (true);
}

/**
 * Creates a description of the graph using the contents of the
 * nodes and edges lists.
 *
 * @return the description of the graph.
 */
QString
DralGraph::getGraphDescription() const
{
    QString result = "DRAL-Graph has ";
    result += QString::number(dbgnList->count());
    result += " nodes and ";
    result += QString::number(num_edges);
    result += " edges\n\n";

    result += "Node List:\n\n";
    QIntDictIterator<DralGraphNode> it_node(* dbgnList);
    DralGraphNode * node;

    while((node = it_node.current()) != NULL)
    {
        ++it_node;
        result += "Node Id=" + QString::number((int) node->getNodeId());
        result += ", name="+node->getName(); 
        result += ", capacity="+node->getCapacity(); 
        result += "\n";
    }

    result += "Edge List:\n\n";
    DralGraphEdge * edge;

    for(INT32 i = 0; i < max_edge; i++)
    {
        edge = edges[i];
        if(edge != NULL)
        {
            result += "Edge Id="+QString::number((int) edge->getEdgeId());
            result += " name=" + edge->getName();
            result += " from " + edge->getSourceNodeName();
            result += " to " + edge->getDestinationNodeName();
            result += " bandwidth=" + QString::number((int)edge->getBandwidth());
            result += " latency=" + QString::number((int) edge->getLatency()); 
            result += "\n";
        }
    }
    return result;
}

/*
bool
DralGraph::decodeEdgeSlot(QString stredgeslot,QString* ename,QString* from, QString *to, INT32 *pos)
{
    //printf (">>decodeEdgeSlot called with=%s\n",stredgeslot.latin1());fflush(stdout);

    // get the edge name
    int pos1 = stredgeslot.find(QChar(';'));
    if (pos1<0) return false;
    *ename = stredgeslot.left((uint)pos1);
    //printf ("ename=%s\n",(*ename).latin1());

    // get the from/to node names
    int pos2 = stredgeslot.find(QChar(';'),pos1+1);
    if (pos2<0) return false;
    int pos3 = stredgeslot.find(QChar(';'),pos2+1);
    if (pos3<0) return false;

    *from = stredgeslot.mid(pos1+1,pos2-pos1-1);
    *to   = stredgeslot.mid(pos2+1,pos3-pos2-1);

    //printf ("from=%s\n",(*from).latin1());
    //printf ("to=%s\n",(*to).latin1());

    QString spos = stredgeslot.right((uint)(stredgeslot.length()-pos3-1));
    //printf ("spos=%s\n",spos.latin1());

    bool pok;
    int rpos = spos.toInt(&pok);
    if (!pok) return false;
    *pos = rpos;
    return true;
}

QString
DralGraph::slotedEdgeName(DralGraphEdge* edge, INT32 pos)
{
    QString ename = edge->getName();
    QString sname = edge->getSourceNodeName();
    QString dname = edge->getDestinationNodeName();
    return ename+";"+sname+";"+dname+";"+QString::number(pos);
}
*/

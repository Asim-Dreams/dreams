/*
 * Copyright (C) 2003-2006 Intel Corporation
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
 */

/**
  * @file ConfigDB.cpp
  */

#include "ConfigDB.h"

ConfigDB* ConfigDB::_myInstance = NULL;

ConfigDB*
ConfigDB::getInstance()
{
    if (_myInstance==NULL)
        _myInstance = new ConfigDB();

    Q_ASSERT(_myInstance!=NULL);
    return _myInstance;
}

void
ConfigDB::destroy()
{
    if (_myInstance!=NULL) { delete _myInstance; }
}

ConfigDB::ConfigDB()
{
    crlist = NULL;
    srlist = NULL;
    lrlist = NULL;
    shrtblset = NULL;
    iwfrlist = NULL;
    brlist= NULL;
    erlist= NULL;
    nrlist= NULL;
    entrlist= NULL;
    extrlist= NULL;
    tabspeclist = NULL;
    ruleDesc = NULL;

    // create all the internal list and hash tables
    dgnList       = new DGNList(DEFAULT_DGN_SIZE);Q_ASSERT(dgnList!=NULL);
    dgnListByName = new DGNListByName(DEFAULT_DGN_SIZE);Q_ASSERT(dgnListByName!=NULL);
    dgeList       = new DGEList(DEFAULT_DGE_SIZE);Q_ASSERT(dgeList!=NULL);

    myZDB = ZDBase::getInstance();
    myLogMgr = LogMgr::getInstance();

    clearRules();
}

ConfigDB::~ConfigDB()
{
    if (dgnList!=NULL) { delete dgnList; }
    if (dgnListByName!=NULL) { delete  dgnListByName; }
    if (dgeList!=NULL) { delete  dgeList; }
    if (crlist!=NULL) { delete  crlist; }
    if (srlist!=NULL) { delete  srlist; }
    if (lrlist!=NULL) { delete  lrlist; }
    if (shrtblset!=NULL) { delete  shrtblset; }
    if (tabspeclist!=NULL) { delete tabspeclist; }
    if (iwfrlist!=NULL) { delete iwfrlist; }
    if (brlist!= NULL) { delete brlist ; }
    if (erlist!= NULL) { delete erlist ; }
    if (nrlist!= NULL) { delete nrlist ; }
    if (entrlist!= NULL) { delete entrlist ; }
    if (extrlist!= NULL) { delete extrlist ; }
    if (tabspeclist != NULL) { delete tabspeclist  ; }
}

void
ConfigDB::clearRules()
{
    // ADF stuff
    defaultColor = QColor(255,255,255);
    defaultShape = EVENT_SHAPE_RECTANGLE;
    defaultLetter= '\0';

    defaultItemWinFont.italic=false;
    defaultItemWinFont.bold=false;
    defaultItemWinFont.underline=false;
    defaultItemWinFont.color=QColor(0,0,0);

    if (brlist==NULL)
    {
        brlist = new BlankRowList(PrimeList::nearPrime(300));
        Q_ASSERT(brlist!=NULL);
        brlist->setAutoDelete(true);
    }
    else
    {
        brlist->clear();
    }

    if (erlist==NULL)
    {
        erlist = new EdgeRowList(PrimeList::nearPrime(3000));
        Q_ASSERT(erlist!=NULL);
        erlist->setAutoDelete(true);
    }
    else
    {
        erlist->clear();
    }

    if (nrlist== NULL)
    {
        nrlist = new NodeRowList(PrimeList::nearPrime(1000));
        Q_ASSERT(nrlist!=NULL);
        nrlist->setAutoDelete(true);
    }
    else
    {
        nrlist->clear();
    }

    if (entrlist== NULL)
    {
        entrlist = new EnterNodeRowList(PrimeList::nearPrime(500));
        Q_ASSERT(entrlist!=NULL);
        entrlist->setAutoDelete(true);
    }
    else
    {
        entrlist->clear();
    }

    if (extrlist== NULL)
    {
        extrlist = new ExitNodeRowList(PrimeList::nearPrime(500));
        Q_ASSERT(extrlist!=NULL);
        extrlist->setAutoDelete(true);
    }
    else
    {
        extrlist->clear();
    }

    if (crlist==NULL)
    {
        crlist = new ColorRuleList(32);
        Q_ASSERT(crlist!=NULL);
    }
    else
    {
        crlist->reset();
    }

    if (srlist==NULL)
    {
        srlist = new ShapeRuleList(32);
        Q_ASSERT(srlist!=NULL);
    }
    else
    {
        srlist->reset();
    }

    if (lrlist==NULL)
    {
        lrlist = new LetterRuleList(32);
        Q_ASSERT(lrlist!=NULL);
    }
    else
    {
        lrlist->reset();
    }

    if (shrtblset==NULL)
    {
        shrtblset = new ShadeRuleTblSet();
        Q_ASSERT(shrtblset!=NULL);
        shrtblset->setAutoDelete(true);
    }
    else
    {
        shrtblset->clear();
    }

    if (iwfrlist==NULL)
    {
        iwfrlist = new ItemWinFontRuleList(32);
        Q_ASSERT(iwfrlist!=NULL);
    }
    else
    {
        iwfrlist->reset();
    }

    if (tabspeclist==NULL)
    {
        tabspeclist = new TabSpecList();
        Q_ASSERT(tabspeclist!=NULL);
        tabspeclist->setAutoDelete(true);
    }
    else
    {
        tabspeclist->clear();
    }

    if (ruleDesc==NULL)
    {
        ruleDesc = new RuleDescriptorHash(PrimeList::nearPrime(1000));
        Q_ASSERT(ruleDesc!=NULL);
        ruleDesc->setAutoDelete(true);
    }
    else
    {
        ruleDesc->clear();
    }
    
    defaultColorDFSNum =0;
    defaultShapeDFSNum =0;
    defaultLetterDFSNum=0;
    defaultItemWinFontDFSNum=0;

    // ordering
    nextRow=0;
    nextUsgRow=0;

    fakeEdgeId=65535;
    fakeNodeId=65535;
}

bool
ConfigDB::addRuleDescriptor(INT32 dfs,QString desc)
{
    //printf ("dbg: ConfigDB::addRuleDescriptor called on dfs=%d,desc=%s\n",dfs,desc.latin1());
    
    // repeated dfs not allowed
    QString* str = ruleDesc->find((long)dfs);
    if (str!=NULL) return false;
    ruleDesc->insert((long)dfs, new QString(desc));
    return true;
}

QString 
ConfigDB::getRuleDescriptor(INT32 dfs)
{
    QString* result = (ruleDesc->find((long)dfs));
    if (result!=NULL)
    {
        return *result;
    }
    else
    {
        return QString::null;
    }
}

bool
ConfigDB::hasOccupancy(ShadeRuleTbl* shrtbl,INT32 occupancy)
{
    bool fnd = false;
    ShadeRuleTbl::iterator it = shrtbl->begin();

    while (!fnd && (it != shrtbl->end()) )
    {
        ShadeRule rule = *it;
        fnd = (rule.occupancy==occupancy);
        ++it;
    }
    return fnd;
}


void
ConfigDB::addBlankRow(INT32 row,QColor color,bool hasColor)
{
    //printf ("adding blank row at %d, colored=%d\n",(int)row,(int)hasColor);fflush(stdout);
    BlankRow* br = new BlankRow;
    br->colored=hasColor;
    br->color=color;
    br->row=row;
    brlist->insert((long)row,br);
}


DralGraphEdge*
ConfigDB::findEdgeByNameFromTo(QString tgnamestr,QString fromstr,QString tostr)
{
    bool fnd = false;
    QIntDictIterator<DralGraphEdge> it(*dgeList);
    DralGraphEdge* result = it.current();

    while (!fnd && (result!=NULL) )
    {
        fnd = ( ( tgnamestr == result->getName() ) &&
                ( fromstr   == result->getSourceNodeName()) &&
                ( tostr     == result->getDestinationNodeName())
              );
        if (!fnd)
        {
            ++it;
            result = it.current();
        }
    }
    return (result);
}

bool
ConfigDB::addNode (QString name,UINT16 node_id, UINT16 parent_id,UINT16 instance)
{
    //printf ("addNode called on %s id=%d, parend_id=%d,instance=%d\n",name.latin1(),(int)node_id,(int)parent_id,(int)instance);

    // compute internal name
    QString nodeName = name+"["+QString::number((int)instance)+"]";

    // 1) check that the ID is not repeated
    /** @todo we must relay on "long" qt type, review this! */
    DralGraphNode* dgnode = dgnList->find((long)node_id);
    if (dgnode!=NULL)
    {
        myLogMgr->addLog ("2Dreams: Repeated DRAL node detected (ID "+QString::number((int)node_id)+
        ") in DRAL file, please report this problem.");
        return (false);
    }

    // 2) check if the name is repeated
    dgnode = dgnListByName->find(nodeName);
    if (dgnode!=NULL)
    {
        myLogMgr->addLog ("2Dreams: Repeated DRAL node detected (Name "+name+
        ", Instance="+QString::number((int)instance)+") in DRAL file, please report this problem.");
        return (false);
    }

    dgnode = new DralGraphNode(nodeName,node_id,parent_id,instance);
    Q_ASSERT(dgnode!=NULL);
    dgnList->insert((long)node_id, dgnode);
    dgnListByName->insert(nodeName, dgnode);

    return (true);
}

bool
ConfigDB::addEdge (UINT16 source_node,UINT16 destination_node,
                          UINT16 edge_id, UINT32 bandwidth,UINT32 latency,
                          QString name)
{
    //printf ("ConfigDB::addEdge called on src=%d, dst=%d, name=%s, id=%d, bw=%d, lat=%d\n",
    //(int)source_node,(int)destination_node,name.latin1(),(int)edge_id,(int)bandwidth, (int)latency);


    // check that the ID is not repeated

    /** @todo we must relay on "long" qt type, review this! */
    DralGraphEdge* dgedge = dgeList->find((long)edge_id);
    if (dgedge!=NULL)
    {
        QString err = "2Dreams: Repeated DRAL edge detected (ID "+QString::number((int)edge_id)+
        ") in DRAL file, please report this problem.";
        myLogMgr->addLog (err);
        return (false);
    }

    /** @todo we must relay on "long" qt type, review this! */
    DralGraphNode* srcNode = dgnList->find((long)source_node);
    DralGraphNode* dstNode = dgnList->find((long)destination_node);
    if ( (srcNode==NULL)||(dstNode==NULL) )
    {
        myLogMgr->addLog ("2Dreams: Adding DRAL edge over unknown nodes(from ID "+
        QString::number((int)source_node)+" to ID "+QString::number((int)destination_node)+
        "), please report this problem.");
        return (false);
    }

    dgedge = new DralGraphEdge(srcNode,dstNode,edge_id,bandwidth,latency,name);
    Q_ASSERT(dgedge!=NULL);
    dgeList->insert((long)edge_id, dgedge);

    return (true);
}

bool
ConfigDB::setCapacity (UINT16 node_id,UINT32 capacity)
{
    // look for the node
    DralGraphNode* node = dgnList->find((long)node_id);
    if (node==NULL)
    {
        myLogMgr->addLog("2Dreams: Setting capacity over an unknown node, (ID "+
        QString::number((int)node_id)+") on the DRAL file, please report this problem.");
        return false;
    }
    node->setCapacity(capacity);

    return (true);
}

void
ConfigDB::reset()
{
    clearRules();
    dgnList->clear();
    dgnListByName->clear();
    dgeList->clear();
}

void
ConfigDB::applyRowConfig()
{
    applyEdgeRowConfig();
    applyNodeRowConfig();
    applyBlankRowConfig();
}

void
ConfigDB::applyEdgeRowConfig()
{
    // walk through the edges
    QIntDictIterator<DralGraphEdge> it(*dgeList);
    DralGraphEdge* edge = it.current();
    while (edge!=NULL)
    {
        if (edge->configured())
        {
            // walk through the rows
            RRList* rrlist = edge->getRowList();
            QIntDictIterator<RowRules> itr( *rrlist );
            QString desc(edge->getName());
            desc = "Edge " + desc + " from " + edge->getSourceNodeName() +
            " to " + edge->getDestinationNodeName();
            INT32 bw = edge->getBandwidth();
            for ( ; itr.current(); ++itr )
            {
                INT32 basepos = itr.currentKey();
                for (INT32 i=0;i<bw;i++)
                {
                    QString comp = "Row " + QString::number(basepos+i) + " : "+desc;
                    if (bw>1) { comp = comp + " ("+QString::number(i)+")"; }
                    myZDB->setRowType(basepos+i,SimpleEdgeRow);
                    myZDB->setRowDescription(basepos+i,comp);
                    myZDB->setRowEdgeId(basepos+i,edge->getEdgeId());
                    myZDB->setRowEdgePos(basepos+i,i);
                }
            }
        }
        ++it;
        edge = it.current();
    }
}

void
ConfigDB::applyBlankRowConfig()
{

    // walk through the edges
    QIntDictIterator<BlankRow> it(*brlist);
    BlankRow* brow = it.current();
    while (brow!=NULL)
    {
        myZDB->setRowType(brow->row,SimpleBlankRow);
        ++it;
        brow = it.current();
    }
}

void
ConfigDB::applyNodeRowConfig()
{
    // walk through the nodes
    QIntDictIterator<DralGraphNode> it(*dgnList);
    DralGraphNode* node = it.current();
    while (node!=NULL)
    {
        if (node->configured())
        {
            // walk through the rows
            RRList* rrlist = node->getRowList();
            QIntDictIterator<RowRules> itr( *rrlist );
            for ( ; itr.current(); ++itr )
            {
                RowRules* rr  = itr.current();
                RowType rtype = rr->getType();
                INT32 rowpos = rr->getRowPos();
                QString desc;
                switch(rtype)
                {
                    case SimpleNodeRow:
                    desc = "Node " + node->getName() + " (root level)";
                    desc = "Row " + QString::number(rowpos) + " : "+desc;
                    myZDB->setRowType(rowpos,SimpleNodeRow);
                    myZDB->setRowDescription(rowpos,desc);
                    myZDB->setRowNodeId(rowpos,node->getNodeId());
                    break;

                    case InputNodeRow:
                    break;

                    case OutputNodeRow:
                    break;

                    case SlotNodeRow:
                    break;

                    default:
                    Q_ASSERT(false);
                };
            }
        }
        ++it;
        node = it.current();
    }
}

/*
bool
ConfigDB::applyUsageColorTables()
{
    INT32 i;
    // iterate over nodes
    QIntDictIterator<DralGraphNode> it(*dgnList);
    DralGraphNode* node = it.current();
    while (node!=NULL)
    {
        if (node->configured())
        {
            INT32 usgrow = node->getUsageRowPos();
            // set the specified capacity
            myZDB->setCapacity(usgrow+this->nextRow,node->getCapacity());

            // create color table
            for (i=0;i<UDV_COLOR_TBL_SIZE;i++)
            {
                QColor upperRefColor,lowerRefColor;
                INT32  upperRefIdx;
                INT32  lowerRefIdx;
                bool ok = getNearestSamples(node,i,upperRefColor,lowerRefColor,upperRefIdx,lowerRefIdx);
                if (!ok)
                {
                    myLogMgr->addLog("2Dreams: ADF error, missing some shading rule.");
                    return (false);
                }

                double prop = (double)(upperRefIdx-i)/(double)(upperRefIdx-lowerRefIdx+1);

                // simple interpolation
                INT32 red = (INT32)((double)upperRefColor.red()*(1.0-prop) +
                                    (double)lowerRefColor.red()*prop);

                INT32 green = (INT32)((double)upperRefColor.green()*(1.0-prop) +
                                    (double)lowerRefColor.green()*prop);

                INT32 blue = (INT32)((double)upperRefColor.blue()*(1.0-prop) +
                                    (double)lowerRefColor.blue()*prop);

                //myLogMgr->addLog("interpolated color " + QColor(red,green,blue).name());
                myZDB->setOccupancyColor(usgrow+this->nextRow,i,QColor(red,green,blue));
            }

        }
        ++it;
        node = it.current();
    }
    return (true);
}


ShadeRuleTbl*
ConfigDB::getShadeRuleTable(ShadeRuleTblSet* tblset,INT32 maxdfs)
{

    ShadeRuleTbl* resultList = NULL;
    INT32 lastUsedDfs=-1;

    QIntDictIterator<ShadeRuleTbl> it (*tblset);

    // look for the higher dfs found
    for (; it.current()!=NULL; ++it)
    {
        INT32 currentDfs = (INT32)it.currentKey();
        if (currentDfs<maxdfs)
        {
            if (currentDfs>lastUsedDfs)
            {
                resultList=it.current();
                lastUsedDfs=currentDfs;
            }
        }
    }
    return resultList;
}
*/

DralGraphEdge*
ConfigDB::createFakeEdge(QString name, QString from, QString to)
{
    DralGraphEdge* dgedge;
    dgedge = findEdgeByNameFromTo(name,from,to);
    if (dgedge==NULL)
    {
        DralGraphNode* fromNode = dgnListByName->find(from);
        DralGraphNode* toNode   = dgnListByName->find(to);
        if (fromNode==NULL)
        {
            fromNode = createFakeNode(from);
        }

        if (toNode==NULL)
        {
            toNode = createFakeNode(to);
        }

        dgedge = new DralGraphEdge(fromNode,toNode,fakeEdgeId,1,1,name);
        Q_ASSERT(dgedge!=NULL);
        dgeList->insert((long)fakeEdgeId, dgedge);
        --fakeEdgeId;
    }

    return (dgedge);
}

DralGraphNode*
ConfigDB::createFakeNode(QString name)
{

    DralGraphNode* dgnode;
    dgnode = dgnListByName->find(name);
    if (dgnode==NULL)
    {
        dgnode = new DralGraphNode(name,fakeNodeId,0,0);
        Q_ASSERT(dgnode!=NULL);
        dgnList->insert((long)fakeNodeId, dgnode);
        dgnListByName->insert(name, dgnode);
        --fakeNodeId;
    }
    return (dgnode);
}

INT32
ConfigDB::getBiggestLatency()
{
    QIntDictIterator<DralGraphEdge> it (*dgeList);
    UINT32 result = 0;
    for ( ; it.current(); ++it )
    {
        DralGraphEdge* edge= it.current();
        if (edge->configured())
           result = QMAX(result,edge->getLatency());
    }
    return result;
}

void
ConfigDB::clearNonConfiguredEdges()
{
    QIntDictIterator<DralGraphEdge> it (*dgeList);
    while (it.current())
    {
        DralGraphEdge* edge= it.current();
        if (! edge->configured() )
        {
            dgeList->remove(it.currentKey());
        }
        else
        {
            ++it;
        }
    }
}

void
ConfigDB::clearNonConfiguredNodes()
{
    QIntDictIterator<DralGraphNode> it (*dgnList);
    while (it.current())
    {
        DralGraphNode* node = it.current();
        if (! node->configured() )
        {
            dgnList->remove(it.currentKey());
        }
        else
        {
            ++it;
        }
    }

    QDictIterator<DralGraphNode> itn (*dgnListByName);
    while (itn.current())
    {
        DralGraphNode* node = itn.current();
        if (! node->configured() )
        {
            dgnListByName->remove(itn.currentKey());
        }
        else
        {
            ++itn;
        }
    }
}

// ---- AMemObj Interface methods
INT64
ConfigDB::getObjSize() const
{
    INT64 result = sizeof(ConfigDB);

    // aprox.
    result += dgnList->count()*(sizeof(DralGraphNode)+ 2*sizeof(void*));
    result += dgnListByName->count()*(sizeof(DralGraphNode)+ 2*sizeof(void*));
    result += dgeList->count()*(sizeof(DralGraphEdge)+ 2*sizeof(void*));
    result += crlist->capacity()*(sizeof(TagValueColor)+ 2*sizeof(void*));
    result += srlist->count()*(sizeof(TagValueShape)+ 2*sizeof(void*));
    result += lrlist->count()*(sizeof(TagValueLetter)+ 2*sizeof(void*));
    result += shrtblset->count()*(sizeof(ShadeRuleTblSet)+ 2*sizeof(void*));

    QDictIterator<ShadeRuleTbl> it( *shrtblset );
    for ( ; it.current(); ++it )
    {
        ShadeRuleTbl* tbl = it.current();
        result += tbl->count()*(sizeof(ShadeRule)+ 2*sizeof(void*));
    }

    result += iwfrlist->count()*(sizeof(TagValueItemWinFont)+ 2*sizeof(void*));
    result += brlist->count()*(sizeof(BlankRow)+ 2*sizeof(void*));
    result += erlist->count()*(sizeof(EdgeRow)+ 2*sizeof(void*));
    result += tabspeclist->count()*(sizeof(TabSpec)+ 3*sizeof(void*));

    return result;
}

QString
ConfigDB::getUsageDescription() const
{
    INT64 graph=0;
    INT64 rules=0;

    graph += dgnList->count()*(sizeof(DralGraphNode)+ 2*sizeof(void*));
    graph += dgnListByName->count()*(sizeof(DralGraphNode)+ 2*sizeof(void*));
    graph += dgeList->count()*(sizeof(DralGraphEdge)+ 2*sizeof(void*));

    QDictIterator<ShadeRuleTbl> it( *shrtblset );
    for ( ; it.current(); ++it )
    {
        ShadeRuleTbl* tbl = it.current();
        rules += tbl->count()*(sizeof(ShadeRule)+ 2*sizeof(void*));
    }

    rules += iwfrlist->count()*(sizeof(TagValueItemWinFont)+ 2*sizeof(void*));
    rules += brlist->count()*(sizeof(BlankRow)+ 2*sizeof(void*));
    rules += erlist->count()*(sizeof(EdgeRow)+ 2*sizeof(void*));
    rules += tabspeclist->count()*(sizeof(TabSpec)+ 3*sizeof(void*));

    QString result = "\nObject ConfigDB contains:\n";
    result += "1) DRAL Graph descriptors: " + QString::number((long)graph) + " bytes\n";
    result += "2) ADF Rules: " + QString::number((long)rules) + " bytes\n";

    return result;
}

QString
ConfigDB::getStats() const
{
    QString result = "";

    int cntCGN = 0;
    QIntDictIterator<DralGraphNode> it1( *dgnList );
    for ( ; it1.current(); ++it1 )
    {
    	DralGraphNode* node = it1.current();
        if (node->configured()) ++cntCGN;
    }

    int cntCGE = 0;
    QIntDictIterator<DralGraphEdge> it2( *dgeList );
    for ( ; it2.current(); ++it2 )
    {
    	DralGraphEdge* edge = it2.current();
        if (edge->configured()) ++cntCGE;
    }

    result += "\tNumber of DRAL-Nodes:\t\t"+QString::number(dgnList->count())+"\n";
    result += "\tNumber of Visible DRAL-Nodes:\t"+QString::number(cntCGN)+"\n";
    result += "\tNumber of DRAL-Edges:\t\t"+QString::number(dgeList->count())+"\n";
    result += "\tNumber of VisibleDRAL-Edges:\t"+QString::number(cntCGE)+"\n";

    int grules = crlist->capacity();
    grules += srlist->count();
    grules += lrlist->count();
    grules += iwfrlist->count();
    result += "\tGlobal Scope Rules:\t"+QString::number(grules)+"\n";

    return (result);
}


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
  * @file  ConfigDB.h
  */

#ifndef _CONFIGDB_H
#define _CONFIGDB_H

// QT library
#include <qcolor.h>
#include <qstring.h>

#include "asim/DralDB.h"

// 2Dreams
#include "ADFDefinitions.h"
#include "TabSpec.h"
#include "DralGraphNode.h"
#include "DralGraphEdge.h"
#include "ZDBase.h"

// fwd reference
class CSLEngine;

typedef QIntDict<QString> RuleDescriptorHash;
 
/**
  * This class holds DRAL Graph and configuration information.
  *
  * @author Federico Ardanaz
  * @date started at 2002-09-05
  * @version 0.1
  */
class ConfigDB : public AMemObj, public StatObj
{
    public:
        // ---- AMemObj Interface methods
        INT64  getObjSize() const;
        QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------

    public:
        TabSpecList* getTabSpecList() { return tabspeclist; }

        inline bool isBlankRow (INT32 row,bool &isColored, QColor &color);
        inline bool getRowDrawOnMove(INT16 row);

        inline INT32 getNumEdges();
        inline INT32 getNumNodes();
        inline INT32 getNumRows();
        inline INT32 getNumUsageRows();

        inline bool hasEdge (UINT16 edge_id);
        inline DralGraphEdge* getEdge(UINT16 edge_id);

        inline bool hasNode (UINT16 node_id);
        inline DralGraphNode* getNode(UINT16 node_id);
        inline INT32 getTrackIdForRow(INT32 row);

        inline bool hasConfiguredEdge (UINT16 edge_id);
        inline bool hasConfiguredNode (UINT16 node_id);

        inline INT32 getEdgeBandwidth (UINT16 edge_id);
        inline INT32 getEdgeLatency   (UINT16 edge_id);

        inline INT32 getNodeCapacity  (UINT16 node_id);

        INT32 getBiggestLatency();

        /**
          * Function description
          */
        void applyRowConfig();

        /**
          * Function description
          */
        void reset();

        /**
          * Function description
          */
        bool addNode (
                     QString name,
                     UINT16 node_id,
                     UINT16 parent_id,
                     UINT16 instance
                     );

        /**
          * Function description
          */
        bool addEdge (
                     UINT16 source_node,
                     UINT16 destination_node,
                     UINT16 edge_id,
                     UINT32 bandwidth,
                     UINT32 latency,
                     QString  name
                     ) ;

        /**
          * Function description
          */
        bool setCapacity (UINT16 node_id,UINT32 capacity);


        bool hasOccupancy(ShadeRuleTbl* shrtbl,INT32 occupancy);

        /**
          *
          */
        //void commitNodeTags(UINT64 currentCycle);

        void clearNonConfiguredEdges();
        void clearNonConfiguredNodes();

        inline ShadeRuleTbl* getShadeRuleTable(QString name);
        
        bool addRuleDescriptor(INT32 dfs,QString desc);
        QString getRuleDescriptor(INT32 dfs);
        
        /**
          * We make TabSpec friend to ease some operations
          */
        friend class TabSpec;
        friend class ADFParser1;
        friend class ADFParser2;
        // review this
        friend class CSLEngine;

    public:
        static ConfigDB* getInstance ();
        static void destroy();

    protected:
        ConfigDB();
        virtual ~ConfigDB();

    protected:
        void applyEdgeRowConfig();
        void applyNodeRowConfig();
        void applyBlankRowConfig();
        void clearRules();

        //bool getNearestSamples(DralGraphNode* node,INT32 idx,QColor& urc,QColor& lrc,INT32& uri,INT32& lri);
        //ShadeRuleTbl* getShadeRuleTable(ShadeRuleTblSet* set,INT32 maxdfs);

        DralGraphNode* createFakeNode(QString name);
        DralGraphEdge* createFakeEdge(QString name, QString from, QString to);

        inline QString normalizeNodeName(QString name);

        DralGraphEdge* findEdgeByNameFromTo(QString tgnamestr,QString fromstr,QString tostr);
        void addBlankRow(INT32 row,QColor color,bool hasColor);

        inline RowRules* getRowRules(INT16 row);
        inline RowRules* getEdgeRowRules(INT16 row,EdgeRow* er);
        inline RowRules* getNodeRowRules(INT16 row,NodeRow* nr);
        inline RowRules* getEnterNodeRowRules(INT16 row,EnterNodeRow* entr);
        inline RowRules* getExitNodeRowRules(INT16 row,ExitNodeRow* extr);

    private:
        // ref to compressed dbase
        ZDBase *myZDB;

        // Graph node list
        DGNList*       dgnList;
        DGNListByName* dgnListByName;

        // Graph edges list
        DGEList* dgeList;

        // List of globally defined value specifications
        QColor      defaultColor;
        EventShape  defaultShape;
        char        defaultLetter;
        ItemWinFont defaultItemWinFont;

        // List of globally defined  tag-value specifications
        ColorRuleList*       crlist;
        ShapeRuleList*       srlist;
        LetterRuleList*      lrlist;
        ItemWinFontRuleList* iwfrlist;

        // shading tbl structures
        ShadeRuleTblSet* shrtblset;

        // Booleans to indicate whether the simple or <tag,value> attribute
        // value takes precedence.
        INT32 defaultColorDFSNum;      ///< keeps DFS Num of default global default color
        INT32 defaultShapeDFSNum;
        INT32 defaultLetterDFSNum;
        INT32 defaultItemWinFontDFSNum;

        // for row counting
        INT32 nextRow;
        INT32 nextUsgRow;

        // log file
        LogMgr* myLogMgr;

        // for fake edges and nodes
        UINT16 fakeEdgeId;
        UINT16 fakeNodeId;

        BlankRowList* brlist;               ///< for blank-row support
        EdgeRowList*  erlist;               ///< for egde-row support
        NodeRowList*  nrlist;               ///< for node-row support
        EnterNodeRowList* entrlist;         ///< for enternode-row support
        ExitNodeRowList*  extrlist;         ///< for exitnode-row support

        TabSpecList*  tabspeclist;          ///< for tabbed disassembly window support

        RuleDescriptorHash* ruleDesc;
        
    private:
       static ConfigDB* _myInstance;
};


// ---------------------------------------------------------------------------
// ConfigDB methods
// ---------------------------------------------------------------------------
/*
INT32
ConfigDB::getBasePosition(UINT16 edge_id)
{
    DralGraphEdge* edge = dgeList->find((long)edge_id);
    Q_ASSERT(edge!=NULL);
    return edge->getRowPos();
}
*/

INT32
ConfigDB::getNumEdges()
{ return dgeList->count(); }

INT32
ConfigDB::getNumNodes()
{ return dgnList->count(); }

INT32
ConfigDB::getNumRows()
{ return nextRow; }

INT32
ConfigDB::getNumUsageRows()
{ return nextUsgRow; }


bool
ConfigDB::hasEdge (UINT16 edge_id)
{
    return (dgeList->find((long)edge_id)!=NULL);
}

DralGraphEdge* 
ConfigDB::getEdge(UINT16 edge_id)
{
    return dgeList->find((long)edge_id);
}

bool
ConfigDB::hasNode (UINT16 node_id)
{
    return (dgnList->find((long)node_id)!=NULL);
}

DralGraphNode*
ConfigDB::getNode(UINT16 node_id)
{
    return dgnList->find((long)node_id);
}

bool
ConfigDB::hasConfiguredEdge (UINT16 edge_id)
{
    DralGraphEdge* edge = dgeList->find((long)edge_id);
    if (edge==NULL) { return false; }
    return edge->configured();

}

bool
ConfigDB::hasConfiguredNode (UINT16 node_id)
{
    DralGraphNode* node = dgnList->find((long)node_id);
    if (node==NULL) { return false; }
    return node->configured();
}

INT32
ConfigDB::getEdgeBandwidth (UINT16 edge_id)
{
    DralGraphEdge* edge = dgeList->find((long)edge_id);
    if (edge==NULL) { return -1; }
    return (INT32)edge->getBandwidth();
}

INT32
ConfigDB::getEdgeLatency (UINT16 edge_id)
{
    DralGraphEdge* edge = dgeList->find((long)edge_id);
    if (edge==NULL) { return -1; }
    return (INT32)edge->getLatency();
}

INT32
ConfigDB::getNodeCapacity  (UINT16 node_id)
{
    DralGraphNode* node = dgnList->find((long)node_id);
    if (node==NULL) { return -1; }
    return node->getCapacity();
}

bool
ConfigDB::getRowDrawOnMove(INT16 row)
{
    RowRules* rr = getRowRules(row);
    return rr->getDrawOnMove();
}

bool
ConfigDB::isBlankRow (INT32 row,bool &isColored, QColor &color)
{
    BlankRow* br;
    br = brlist->find((long)row);
    if (br==NULL)
    {
        return false;
    }
    isColored=br->colored;
    color=br->color;
    return (true);
}

QString
ConfigDB::normalizeNodeName(QString name)
{
    QString result=name;
    QRegExp rexp();
    int pos = name.find(QChar('['));
    if (pos<0)
    {
        result += "[0]";
    }
    return (result);
}

RowRules*
ConfigDB::getRowRules(INT16 row)
{
    EdgeRow* er = erlist->find(row);
    NodeRow* nr = nrlist->find(row);
    EnterNodeRow* entr = entrlist->find(row);
    ExitNodeRow*  extr = extrlist->find(row);
    if (er!=NULL)
    { return getEdgeRowRules(row,er); }
    else if (nr!=NULL)
    { return getNodeRowRules(row,nr); }
    else if (entr!=NULL)
    { return getEnterNodeRowRules(row,entr); }
    else if (extr!=NULL)
    { return getExitNodeRowRules(row,extr); }
    else
    {
        QString err = "ADF Error: reference to non-configured row " + QString::number(row);
        myLogMgr->addLog(err);
        return NULL;
    }
}

INT32
ConfigDB::getTrackIdForRow(INT32 row)
{
    NodeRow* nr = nrlist->find(row);
    EnterNodeRow* entr = entrlist->find(row);
    ExitNodeRow*  extr = extrlist->find(row);

    if (nr!=NULL)
    { return nr->trackId; }
    else if (entr!=NULL)
    { return entr->trackId; }
    else if (extr!=NULL)
    { return extr->trackId; }
    else
    {return -1;}
}


RowRules*
ConfigDB::getEdgeRowRules(INT16 row,EdgeRow* er)
{
    DralGraphEdge* edge = dgeList->find(er->edge_id);
    if (edge==NULL)
    {
        QString err = "ADF Error: reference to non-configured edge id " + QString::number(er->edge_id);
        myLogMgr->addLog(err);
        return NULL;
    }
    return edge->getRow(row);
}

RowRules*
ConfigDB::getNodeRowRules(INT16 row,NodeRow* nr)
{
    DralGraphNode* node = dgnList->find(nr->node_id);
    if (node==NULL)
    {
        QString err = "ADF Error: reference to non-configured node id " + QString::number(nr->node_id);
        myLogMgr->addLog(err);
        return NULL;
    }
    return node->getRow(row);
}

RowRules*
ConfigDB::getEnterNodeRowRules(INT16 row,EnterNodeRow* entr)
{
    DralGraphNode* node = dgnList->find(entr->node_id);
    if (node==NULL)
    {
        QString err = "ADF Error: reference to non-configured node id " + QString::number(entr->node_id);
        myLogMgr->addLog(err);
        return NULL;
    }
    return node->getRow(row);
}

RowRules*
ConfigDB::getExitNodeRowRules(INT16 row,ExitNodeRow* extr)
{
    DralGraphNode* node = dgnList->find(extr->node_id);
    if (node==NULL)
    {
        QString err = "ADF Error: reference to non-configured node id " + QString::number(extr->node_id);
        myLogMgr->addLog(err);
        return NULL;
    }
    return node->getRow(row);
}

ShadeRuleTbl*
ConfigDB::getShadeRuleTable(QString name)
{ return shrtblset->find(name); }

#endif


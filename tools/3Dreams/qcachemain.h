/*****************************************************************************
*
* @brief Main program object. Header File.
*
* @author Oscar Rosell
*
*  Copyright (C) 2003-2006 Intel Corporation
*  
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*  
*
*****************************************************************************/

#ifndef QCACHE_MAIN_H
#define QCACHE_MAIN_H
/** \file */
// QCache includes
#include "tagDictionary.h"
#include "visualizations.h"
#include "qnode.h"
#include "qedge.h"
#include "dbase.h"
// Lib includes
#include <Main.h>
#include <dralClient.h>
#include <dralListener.h>
// STL includes
#include <stl/hash_map>
#include <deque>
#include <vector>
// QT includes
#include <qaccel.h>
// General includes
#include <iostream.h>

class AScrollView;
class QCacheView;
class PaintEvent;

using qcdb::ItemDefinition;

/** A timer for driving the simulation */
class Timer:public QTimer
{
  Q_OBJECT
  public:
    enum {INITIAL_VELOCITY = 8};
    Timer()
    {
        velocity_=INITIAL_VELOCITY;
    }
    using QTimer::start;
    void start()
    {
        start(velocity_);
    }

  public slots:
    void decreaseVelocity()
    {
        UINT16 tmp = velocity_;
        velocity_ = (velocity_ ? velocity_<< 1: 1);
        velocity_ = (velocity_ ? velocity_: tmp);
        changeInterval(velocity_);
    }
    void increaseVelocity()
    {
        velocity_ = velocity_ >> 1;
        changeInterval(velocity_);
    }
  public:
    UINT16 velocity_;
};

class QCacheMain : public Main, DRAL_LISTENER_CLASS
{
  Q_OBJECT
  public:
    /** Default constructor */
    QCacheMain(QWidget *parent=0, const char *name=0, WFlags f=0);
    /** Destructor */
    ~QCacheMain();

    void do_genericOpen();
    void do_genericPrint();
    void do_genericSave();
    AScrollView* getAScrollViewObject();
    char* getApplicationCaption();

    void showItems();

    void initApplication();
    void initMenus();
    void initMiscellaneous();

    /* DRAL_LISTENER virtual functions*/

    virtual void Cycle (UINT64 cycle);
    virtual void NewItem (UINT32 item_id);
    virtual void SetTagSingleValue (
        UINT32 item_id, char * tag_name,
        UINT64 value, unsigned char time_span_flags);
    virtual void SetTagString (
        UINT32 item_id, char * tag_name,
        char * str, unsigned char time_span_flags);
    virtual void SetTagSet (
        UINT32 item_id, char * tag_name, UINT32 set_size,
        UINT64 * set, unsigned char time_span_flags);
    virtual void MoveItems (
        UINT16 edge_id, UINT32 numOfItems, UINT32 * items);
    virtual void MoveItemsWithPositions (
        UINT16 edge_id, UINT32 numOfItems,
        UINT32 * items, UINT32 * positions);
    virtual void EnterNode (
        UINT16 node_id, UINT32 item_id, UINT32 slot);
    virtual void ExitNode (UINT16 node_id, UINT32 slot);
    virtual void DeleteItem (UINT32 item_id);
    virtual void EndSimulation ();
    virtual void AddEdge (
        UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,
        UINT32 bandwidth, UINT32 latency, char * name);
    virtual void SetCapacity (
        UINT16 node_id, UINT32 capacity,
        UINT32 capacities [], UINT16 dimensions);
    virtual void SetHighWaterMark (UINT16 node_id, UINT32 mark);
    virtual void Comment (char * comment);
    virtual void AddNode (
        UINT16 node_id, char * node_name, UINT16 parent_id, UINT16 instance);
    virtual void Error (char * error);
    virtual void NonCriticalError (char * error);

    ItemDefinition* itemToInformation(UINT32 item_id);

  public slots: 
    void run();
    void gotoCycle();
    void selectedNodesEntry(int node_id);
    void undoOperation();
    void redoOperation();
    void timeout();
    void dumpEdges();
    void dumpNodes();

  private:

    QCacheView* scrollview_;
    QPopupMenu* menuItems_;
    QPopupMenu* menuDB_;
    QPopupMenu* menuDump_;

    QAccel accel_;

    Timer timer_; 

    DRAL_CLIENT dralReader_;

    bool runFlag_;
    int runMenuEntry_;
    UINT64 cycle_;

    bool readConfiguration(const QString filename);
    bool readConfigurationElement(const QDomElement& elem);
    bool readConfigurationNodeElement(const QDomElement& elem);
    bool readConfigurationSetElement(const QDomElement& elem, PaintEvent* pe);

    bool addTrackedNode(const QDomElement& elem);
    QCacheNode* createNode(const QCTrackedNode* node_info);

    typedef QCacheNode::NodeId NodeId;

    typedef hash_map<NodeId,QCacheNode *> Nodes;
    typedef Nodes::iterator NodeIt;
    typedef Nodes::const_iterator NodeConstIt;

    Nodes nodes_;

    typedef hash_map<NodeId, QCTrackedNode *> TrackedNodes;
    typedef TrackedNodes::iterator TrackedNodeIt;
    typedef TrackedNodes::const_iterator TrackedNodeConstIt;

    TrackedNodes trackedNodes_;
    
    typedef qcdb::EdgeId EdgeId;

    typedef hash_map<EdgeId,QCacheEdge *> Edges;
    typedef Edges::iterator EdgeIt;
    typedef Edges::const_iterator EdgeConstIt;

    Edges edges_;

    qcdb::DBase db_;
};

#endif

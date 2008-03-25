/*****************************************************************************
*
* @brief Main program object. Source File.
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

#include "qcacheview.h"
#include "tagDictionary.h"
#include "qcachemain.h"
#include "HistoryList.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream.h>
#include <qfiledialog.h>

void 
QCacheMain::initApplication()
{
    Main::initApplication();
    
    initMenus();
    initMiscellaneous();

    // Testing, load always the same file
    QString config("prova.xml");

    if (!readConfiguration(config))
    {
        QMessageBox::information(0,"An error ocurred",
        "The configuration file could not be processed correctly");
    }
}

void 
QCacheMain::initMenus()
{
    // Run menu entry -- we store it to change it in the future
    runMenuEntry_ = file->insertItem("Run", this, SLOT(run()), Key_P+CTRL);
    // Disabled until a correct DRAL file is loaded
    file->setItemEnabled(runMenuEntry_,false);

    file->insertItem("Goto cycle...", this, SLOT(gotoCycle()));
    edit->insertItem("Undo", this, SLOT(undoOperation()), Key_Z+CTRL);
    edit->insertItem("Redo", this, SLOT(redoOperation()), Key_R+CTRL);

    // QCache specific menu -- node finder
    menuItems_ = new QPopupMenu(this);
    menu->insertItem("Nodes",menuItems_);
    connect(menuItems_,SIGNAL(activated(int)),this,SLOT(selectedNodesEntry(int)));

    menuDB_ = new QPopupMenu(this);
    menu->insertItem("DB",menuDB_);
    menuDump_ = new QPopupMenu(this);
    menuDB_->insertItem("Dump", menuDump_);
    menuDump_->insertItem("Edges", this, SLOT(dumpEdges()));
    menuDump_->insertItem("Nodes", this, SLOT(dumpNodes()));
}

void 
QCacheMain::initMiscellaneous()
{
    // Connect the key accel
    accel_.connectItem( accel_.insertItem(Key_F+CTRL), 
                        &timer_,                  
                        SLOT(increaseVelocity()));  
    accel_.connectItem( accel_.insertItem(Key_G+CTRL), 
                        &timer_,                  
                        SLOT(decreaseVelocity()));  
}

void 
QCacheMain::do_genericOpen()
{
    QString filename = QFileDialog::getOpenFileName();

    if (!filename.isEmpty())
    {
        int fd = open(filename,O_RDONLY);
        if (!fd)
        {
            QMessageBox::information(0,"An error ocurred",
                                    "The file cannot be opened\nPlease check that you have read permissions");
            return;
        }

        if (dralReader_)
            delete dralReader_;

        dralReader_ = new DRAL_CLIENT_CLASS(fd,this);
        if (dralReader_->GetFileSize()!=-1)
        {
            // The dral file seems to be correct -> enable the run option
            file->setItemEnabled(runMenuEntry_,true);
        }
    }
}

QCacheMain::QCacheMain(QWidget *parent, const char *name, WFlags f):
	Main(parent,name,f)
	,accel_(this)
    ,dralReader_(0)
    ,runFlag_(false)
{
    scrollview_ = new QCacheView(this,25,25);
    QCacheEdge* edge = new QCacheEdge(scrollview_,0,0);
    edge->setBandwidth(1);
    edge->setLatency(1);
    edges_[0] = edge;
}

QCacheMain::~QCacheMain()
{
    // Free dynamically allocated memory
    // Delete Nodes
    for (NodeIt it=nodes_.begin(); it!=nodes_.end(); ++it)
    {
        delete it->second;
    }
    delete dralReader_;
    delete scrollview_;
}

void 
QCacheMain::do_genericPrint()
{
    // Not implemented
}

void 
QCacheMain::do_genericSave()
{
    // Not implemented
}

AScrollView* 
QCacheMain::getAScrollViewObject()
{
    return scrollview_;
}

char* 
QCacheMain::getApplicationCaption()
{
    return("NDreams");
}

void 
QCacheMain::showItems()
{
}

void 
QCacheMain::Cycle (UINT64 cycle)
{
    cycle_ = cycle;
    scrollview_->Cycle(cycle);
    for (EdgeIt it = edges_.begin(); it != edges_.end(); ++it)
    {
        it->second->clock();
    }
}

void 
QCacheMain::NewItem (UINT32 item_id)
{
    db_.registerNewItem(item_id);
}

void 
QCacheMain::SetTagSingleValue (
        UINT32 item_id, char * tag_name,
        UINT64 value, unsigned char time_span_flags)
{
    db_.registerSetTagSingleValue(item_id,tag_name,value);
}

void 
QCacheMain::SetTagString (
        UINT32 item_id, char * tag_name,
        char * str, UBYTE time_span_flags)
{
    db_.registerSetTagString(item_id,tag_name,str);
}
void 
QCacheMain::SetTagSet (
        UINT32 item_id, char * tag_name, UINT32 set_size,
        UINT64 * set, unsigned char time_span_flags)
{
    db_.registerSetTagSet(item_id,tag_name,set_size,set);
}

void 
QCacheMain::MoveItems (
        UINT16 edge_id, UINT32 numOfItems, UINT32 * items)
{
    EdgeIt it = edges_.find(edge_id);
    if (it!=edges_.end())
    {
        QCacheEdge* edge = it->second;
        for (UINT32 i=0; i<numOfItems; ++i)
        {
            edge->enterItem(db_.getItemDefinition(items[i]));
        }
    }
}

void 
QCacheMain::MoveItemsWithPositions(
        UINT16 edge_id, UINT32 numOfItems, UINT32* items, UINT32* positions)
{
}

void 
QCacheMain::Error (char * error)
{
    qFatal(error);
}

void 
QCacheMain::NonCriticalError (char * error)
{
    qWarning(error);
}

void 
QCacheMain::EnterNode (
        UINT16 node_id, UINT32 item_id, UINT32 slot)
{
    NodeIt it = nodes_.find(node_id);
    if (it != nodes_.end())
    {
        it->second->enterItem(item_id, slot);
    }
}

void 
QCacheMain::ExitNode(UINT16 node_id, UINT32 slot)
{    
    NodeIt it = nodes_.find(node_id);
    if (it != nodes_.end())
    {
        it->second->exitItem(slot);
    }
}

void 
QCacheMain::DeleteItem (UINT32 item_id)
{
    db_.registerDeleteItem(item_id);
}

void 
QCacheMain::EndSimulation ()
{
    runFlag_ = false;
    timer_.stop();
    QMessageBox::information( this, "NDreams",
                        "Simulation ended.");
}

void QCacheMain::AddNode (
        UINT16 node_id, char * node_name, UINT16 parent_id, UINT16 instance)
{
    db_.registerNewNode(node_id, node_name, parent_id, instance);

    NodeIt it = nodes_.find(node_id);

    if (it!=nodes_.end())
    {
        QCacheNode* node = it->second;
        node->setName(node_name);
        node->setParentNode(parent_id);
        menuItems_->insertItem(node_name, node_id);
    }
}

void 
QCacheMain::AddEdge (
        UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,
        UINT32 bandwidth, UINT32 latency, char* name)
{
    // Just put it in the database
    db_.registerNewEdge(edge_id, sourceNode, destNode, bandwidth, latency, name);
}

QCacheNode* 
QCacheMain::createNode(const QCTrackedNode* node_info)
{
    QCacheNode* new_node = new QCacheNode(this, node_info->id());

    if (!(new_node->readConfiguration(node_info->config())));
    {
        delete new_node;
        cout << "Configuration for node " << node_info->id() << " is not correct." << endl;
        new_node = 0;
    }
    return new_node;
}


void 
QCacheMain::SetCapacity (
        UINT16 node_id, UINT32 capacity, UINT32 capacities [], UINT16 dimensions)
{
    cout << "Node " << node_id << " Capacity " << capacity << " V0 " << capacities[0] << " Dims " << dimensions << endl;
    NodeIt it = nodes_.find(node_id);

    if (it!=nodes_.end())
    {
        QCacheNode* node = it->second;
        node->setCapacity(capacities,dimensions);
        node->initialize(scrollview_);
    }

    TrackedNodeIt tracked_it = trackedNodes_.find(node_id);

    if (tracked_it != trackedNodes_.end())
    {    
        // createNode(tracked_it->second);
        trackedNodes_.erase(tracked_it);
    }
}

void 
QCacheMain::SetHighWaterMark (UINT16 node_id, UINT32 mark)
{}

void 
QCacheMain::Comment (char* comment)
{}

void 
QCacheMain::gotoCycle()
{
    if (dralReader_)
    {
        bool oldrf = runFlag_;
        if (runFlag_)
        {
            run();
        }
        // Ask user input
        int cycle = QInputDialog::getInteger(tr("Enter the destination cycle\n"),tr("Enter the destination cycle\n"),cycle_,cycle_);
        UINT32 cycle_togo = cycle;
        if (cycle_togo > cycle_)
        {
            scrollview_->hide();
    
            int i=0;
            while (cycle_togo > cycle_)
            {
                dralReader_->ProcessNextEvent(true,1);
                if (i%100==0) 
                    qApp->processEvents(); // Read external events
                i++;
            }
            scrollview_->show();
        }
        if (oldrf)
        {
            run();
        }
    }
}

void 
QCacheMain::timeout()
{
    for (int i=0;i<10;++i)
    {
        if (dralReader_->ProcessNextEvent(true,30) == false)
        // No more events
            return;
    }
}

void 
QCacheMain::run()
{
#ifdef TIMER
    if (dralReader_)
    {
        runFlag_=!runFlag_;

        if (runFlag_==false)
        {
            file->changeItem(runMenuEntry_,"Run");
            timer_.stop();
        }
        else
        {
            connect(&timer_,SIGNAL(timeout()),this,SLOT(timeout()));
            timer_.start();
            file->changeItem(runMenuEntry_,"Pause");
        }
    }
#else
    if (dralReader_) // The user opened a file
    {
        // Change the flag and the menu entry
        runFlag_=!runFlag_;

        if (runFlag_==false)
        {
            file->changeItem(runMenuEntry_,"Run");
        }
        else
        {
            file->changeItem(runMenuEntry_,"Pause");
        }
        int i = 1;
        while (runFlag_) // Final version
        {
            dralReader_->ProcessNextEvent(true,1);
            if (i%100==0) 
                qApp->processEvents(); // Read external events
            i++;
        }
    }
    else
    {
        QMessageBox::information( this, "NDreams",
                        "A trace file must be opened first.");

    }
#endif
}

bool 
QCacheMain::readConfiguration(const QString filename)
{
    QDomDocument doc( "mydocument" );

    QFile f( filename );
    if ( !f.open( IO_ReadOnly ) )
    {
        QMessageBox::information(0,"An error ocurred",
        "The configuration file couldn't be opened. Verify you have permissions on the file.");
        return false;
    }
    if ( !doc.setContent( &f ) ) 
    {
        QMessageBox::information(0,"An error ocurred",
        "The configuration file doesn't contain a valid XML file.");
        f.close();
        return false;
    }

    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();

    while( !n.isNull() ) 
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if( !e.isNull() ) 
        { // the node was really an element.
            if (!readConfigurationElement(e))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        n = n.nextSibling();
    }

    f.close();
    return true;
}

bool 
QCacheMain::readConfigurationElement(const QDomElement& elem)
{
    QString tag_name(elem.tagName());

    if (tag_name=="node")
    {
        bool ok = addTrackedNode(elem);

        cout << "Configuring node" << endl;
        return (ok && readConfigurationNodeElement(elem));
    }
    if (tag_name=="set")
    {
        cout << "Configuring set" << endl;
        PaintEvent* pe = NULL;
        return readConfigurationSetElement(elem,pe);
    }
    if (tag_name=="if")
    {    
        return true;
    }

    return true;
}

bool 
QCacheMain::addTrackedNode(const QDomElement& elem)
{
    QString id(elem.attribute("id","NOT FOUND"));
    if (id=="NOT FOUND")
        return false;

    bool ok = false;
    UINT16 node_id = id.toUInt(&ok);
    if (!ok)
        return false;
        
    QCTrackedNode* trackedNode = new QCTrackedNode(node_id);
    trackedNode->setConfig(elem);
    trackedNodes_[node_id] = trackedNode;
    return true;
}

bool 
QCacheMain::readConfigurationNodeElement(const QDomElement& elem)
{
    QString id(elem.attribute("id","NOT FOUND"));
    if (id=="NOT FOUND")
        return false;

    bool ok = false;
    UINT16 node_id = id.toUInt(&ok);
    if (!ok)
        return false;

    QCacheNode* new_node = new QCacheNode(this, node_id);
    nodes_[node_id] = new_node;

    return new_node->readConfiguration(elem);
}

bool 
QCacheMain::readConfigurationSetElement(const QDomElement& elem, PaintEvent* pe)
{
    QString color(elem.attribute("color","NOT FOUND"));
    if (color=="NOT FOUND")
        return false;
    cout << "Set color:" << color.latin1() << endl;
    PaintEventFill* pef = new PaintEventFill();
    pef->setColor(QColor(color));
    pe = pef;

    return true;
}

ItemDefinition* 
QCacheMain::itemToInformation(UINT32 id)
{
    return db_.getItemDefinition(id);
}

void 
QCacheMain::selectedNodesEntry(int node_id)
{
    scrollview_->centerNode(node_id);
}

void 
QCacheMain::dumpEdges()
{
    return db_.dumpEdges();
}

void 
QCacheMain::dumpNodes()
{
    return db_.dumpNodes();
}

void 
QCacheMain::undoOperation()
{
    HistoryList hl;
    hl.undo();
}

void QCacheMain::redoOperation()
{
    HistoryList hl;
    hl.redo();
}

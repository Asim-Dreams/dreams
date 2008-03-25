/*****************************************************************************
*
* @brief Node representation in qcache. Source File.
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

#include "qnode.h"
#include "qcachemain.h"
#include "qcacheview.h"
#include "visualizations.h"
#include "nodeDialog.h"
#include <assert.h>


QCTrackedNode::QCTrackedNode(NodeId id):
    id_(id),
    configDOM_(0)
{
}

QCTrackedNode::~QCTrackedNode()
{
    delete configDOM_;
}

void
QCTrackedNode::setConfig(const QDomElement& dom)
{
    delete configDOM_;
    configDOM_ = new QDomElement(dom);
}

const QDomElement&
QCTrackedNode::config() const
{
    return *configDOM_;
}


QCacheNode::QCacheNode(QCacheMain* parent, NodeId id):
    QObject(parent),
    parent_(parent),
    id_(id),
    num_items_(0)
{
};

QCacheNode::~QCacheNode()
{
};

/// Sets the structure of the node. That is number of levels and elements inside each one.
void 
QCacheNode::setStructure(const NodeDefinition& def)
{
    UINT32 elements = 1;

    cout << "Setting structure" << endl;

    // Calculate total number of elements
    for (vector<LevelDefinition>::const_iterator it=def.begin();it!=def.end();++it)
    {
        elements=elements*(it->elements);
    }
    items_.resize(elements);

    for (UINT32 i=0; i<elements; ++i)
    {
        items_[i]=qcdb::NULL_ITEM_ID;
    }
    structure_ = def;

    // Pass structure information to the visualizations that require it
    for (VisuIterator it=visualizations_.begin();it!=visualizations_.end();++it)
    {
        (*it)->setStructure(def);
    }
};



void 
QCacheNode::setCapacity(UINT32 capacities [], UINT16 dimensions)
{
    NodeDefinition def;

    cout << "Setting capacity" << endl;

    LevelDefinition level;
    level.arrangement = LevelDefinition::LEVEL_HORIZONTAL;
    level.margin = 5;

    for (UINT16 i=0; i<dimensions; ++i)
    {
        cout << "Dimensions: " << dimensions << endl;
        level.elements = capacities[i];
        def.push_back(level);
    }
    setStructure(def);

    cout << "Capacity was set for node: " << id_ << endl;
};

void 
QCacheNode::setName(QString name)
{
    name_ = name;
};

QString 
QCacheNode::name()
{
    return name_;
};

void 
QCacheNode::setParentNode(NodeId parent)
{
    parentIds_.push_back(parent);
};

void 
QCacheNode::enterItem(UINT32 item_id, UINT32 slot)
{
    cerr << "Enter Item: " << item_id << " Slot: " << slot << " Node: " << id_ << endl;
    assert(slot < items_.size());
    // Store items position
    items_[slot]=item_id;
    // Get item information
    ItemDefinition* item_info = getItemDefinition(item_id);
    // Notify the current visualizations
    for (VisuIterator it=visualizations_.begin();it!=visualizations_.end();++it)
    {
        (*it)->enterItem(item_info,slot);
    }
    ++num_items_;
};

void 
QCacheNode::exitItem(UINT32 slot)
{
    cerr << "Exit Item from Slot: " << slot << endl;
    assert(slot < items_.size());
    // Nullify the slot info
    items_[slot]=qcdb::NULL_ITEM_ID;
    // Notify visualizations
    for (VisuIterator it=visualizations_.begin();it!=visualizations_.end();++it)
    {
        (*it)->exitItem(slot);
    }
    --num_items_;
};

void 
QCacheNode::deleteItem(UINT32 item_id)
{
    cerr << "Deleting Item: " << item_id << endl;
    exitItem(itemToSlot(item_id));
};

void 
QCacheNode::cycle(UINT64 cycle)
{
};

void 
QCacheNode::setTag()
{
};

UINT32 
QCacheNode::itemToSlot(UINT32 item_id) const
{
    // Not implemented yet
    return 0;
};

UINT32 
QCacheNode::totalCapacity() const
{
    return items_.size();
}

UINT32 
QCacheNode::numItems() const
{
    return num_items_;
}

UINT32 
QCacheNode::slotToItem(UINT32 slot) const
{
   return items_[slot];
};

void 
QCacheNode::initialize(QCacheView* view)
{
    assert(view!=NULL);
    cout << "Initialize node." << endl;

    for (VisuIterator it = visualizations_.begin(); it != visualizations_.end(); ++it)
    {
        cout << "\tSetting scroll view" << endl;
        (*it)->setScrollView(view);
    }
};


ItemDefinition* 
QCacheNode::getItemDefinition(UINT32 item_id) const
{
    return parent_->itemToInformation(item_id);
}

bool 
QCacheNode::readConfiguration(const QDomElement& config)
{
    QString tag_name(config.tagName());

    assert(tag_name=="node");
    cout << "QCacheNode reading configuration" << endl;

    bool ok = true;

    QDomNode n = config.firstChild();
    while( !n.isNull() ) 
    {
        QDomElement elem = n.toElement(); // try to convert the node to an element.
        if( !elem.isNull() ) 
        { // the node was really an element.
            if (elem.tagName()=="visualization")
            {
                QCacheVisualization* vis;
                QString type = elem.attribute("type", "NOT FOUND");
                if (type=="NOT FOUND")
                    return false;
                else
                {
                    // Specific visualization calls
                    if (type=="basic_cache")
                    {
                        vis = VisualizationFactory::createBasicCache(elem, NULL, this);
                    }
                    if (type=="tracker")
                    {
                        vis = VisualizationFactory::createTracker(elem, NULL, this);
                    }
                    if (type=="pumping")
                    {
                        vis = VisualizationFactory::createPumping(elem, NULL, this);
                    }
                }

                if (vis)
                {
                    cout << "Visualization correctly added\n";
                    visualizations_.push_back(vis);
                }
                ok = ok && vis;
            }
        }
        else 
        {
            return false;
        }
        n = n.nextSibling();
    }

    return true;
};

QPopupMenu* 
QCacheNode::contextualMenu() const
{
    QPopupMenu* menu = new QPopupMenu(parent_);

    menu->insertItem(name_,this,SLOT(showDialog()));
    return menu;
};

void 
QCacheNode::showDialog()
{
    NodeDialog nd;
 
    nd.setNItems(numItems());
    nd.setCapacity(totalCapacity());
    nd.setNodeName(name());

    for (VisuIterator it=visualizations_.begin();it!=visualizations_.end();++it)
    {
        nd.addVisualization((*it));
    }
    
    nd.exec(); 
}


/*****************************************************************************
*
* @brief Edge representation in qcache. Source File.
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

#include "qedge.h"

QCTrackedEdge::QCTrackedEdge(EdgeId id):
    id_(id),
    configDOM_(0)
{
}

QCTrackedEdge::~QCTrackedEdge()
{
    delete configDOM_;
}

void 
QCTrackedEdge::setConfig(const QDomElement& dom)
{
    delete configDOM_;
    configDOM_ = new QDomElement(dom);
}

const QDomElement&
QCTrackedEdge::config() const
{
    return *configDOM_;
}
 

QCacheEdge::QCacheEdge(QCacheView* parent, NodeId orig_id, NodeId dst_id):
    QCacheViewItem(parent, 20, 20),
    orig_id_(orig_id),
    dst_id_(dst_id),
    first_(0),
    last_(0),
    num_items_(0)
{
    cout << "Creating edge..." << endl;
}

QCacheEdge::EdgeId 
QCacheEdge::id() const
{
    return id_;
}

void 
QCacheEdge::setId(EdgeId id)
{
    id_ = id;
}

void 
QCacheEdge::SetNodeOrigin(QCacheNode* node)
{
    orig_ = node;
}
void  
QCacheEdge::SetNodeDestination(QCacheNode* node)
{
    dst_ = node;
}

void 
QCacheEdge::setName(QString name)
{
    name_ = name;
};

QString
QCacheEdge::name() const 
{
    return name_;
};

void
QCacheEdge::setBandwidth(UINT16 bandwidth)
{
    bandwidth_=bandwidth;
    item_queue_.resize(bandwidth_ * latency_);
    last_ = bandwidth_ * latency_;
};
    
UINT16
QCacheEdge::bandwidth() const
{
    return bandwidth_;
};
    
void
QCacheEdge::setLatency(UINT16 latency)
{
    latency_=latency;
    item_queue_.resize(bandwidth_ * latency_);
    last_ = bandwidth_ * latency_;
};
    
UINT16 
QCacheEdge::latency() const
{
    return latency_;
};

void
QCacheEdge::drawItemCoordinatesContents(QPainter* p, int cx, int cy, int cw, int ch)
{
    cout << "Drawing edge..." << endl;

    if (item_queue_[first_])
    {
        p->setBrush("red");
    }
    DrawingTools::drawCircle(0,0, p, 20);
    DrawingTools::drawArrow(width(),0,0,height(), p);
}

void 
QCacheEdge::configure()
{
};

void
QCacheEdge::enterItem(UINT32 item_id)
{
    cout << "Item " << item_id << " entered the edge " << id_ << endl;
}

void
QCacheEdge::enterItem(ItemDefinition* item_def)
{
    cout << "Item " << item_def->id() << " entered the edge " << id_ << endl;
    for (UINT16 i = last_; i<bandwidth_; ++i)
    {
        if (item_queue_[i]==0)
        {
            item_queue_[i]=item_def;        
        }
    }
    ++num_items_;
}

void
QCacheEdge::clock()
{
    cout << "Edge being clocked." << " Band: " << bandwidth_ << " Lat: " << latency_ << endl;
    redraw();

    for (UINT16 i=0; i < bandwidth_; ++i)
    {
        if (item_queue_[first_]!=0)
        {
            cout << "Item leaving edge " << id() << endl;
        }
    }
    memset(&item_queue_[first_], 0, bandwidth_ * sizeof(ItemDefinition *));
    last_ = advanceCircularIndex(last_);
    first_ = advanceCircularIndex(first_);
}

UINT16
QCacheEdge::advanceCircularIndex(UINT16 idx) const
{
    return (idx + bandwidth_) % item_queue_.size();
}

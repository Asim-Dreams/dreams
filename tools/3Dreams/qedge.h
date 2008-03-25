/*****************************************************************************
*
* @brief Edge representation in qcache. Header File.
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

#ifndef QEDGE_H
#define QEDGE_H

#// QCache includes
#include "dbase.h"
#include "qnode.h"
#include "QCacheViewItem.h"
// Lib includes
#include "libagt/AGTTypes.h"
// STL includes
#include <vector>
// QT includes
#include <qstring.h>
#include <qpopupmenu.h>

class QCTrackedEdge
{ 
    typedef qcdb::EdgeId EdgeId;
  public:
    QCTrackedEdge(EdgeId id);
    ~QCTrackedEdge();

    void setConfig(const QDomElement& dom);
    const QDomElement& config() const;
  
  private:
    const EdgeId id_;
    QDomElement* configDOM_;
};

class QCacheEdge:public QCacheViewItem
{
    typedef qcdb::NodeId NodeId;
    typedef qcdb::EdgeId EdgeId;
    typedef qcdb::ItemDefinition ItemDefinition;

  public:
    QCacheEdge(QCacheView* parent, NodeId orig_id, NodeId dst_id);

    NodeId origin(){return orig_id_;};
    NodeId destination(){return dst_id_;};

    void SetNodeOrigin(QCacheNode* node);
    void SetNodeDestination(QCacheNode* node);

    void setId(EdgeId id);
    EdgeId id() const;

    void setName(QString name);
    QString name() const;
    void setBandwidth(UINT16 bandwidth);
    UINT16 bandwidth() const;
    void setLatency(UINT16 latency);
    UINT16 latency() const;

    virtual void configure();

    void enterItem(UINT32 item_id);
    void enterItem(ItemDefinition* item_id);
    void clock();

  protected:
    virtual void    
    drawItemCoordinatesContents (
                                    QPainter* p,     ///< Painter where to draw on
                                    int cx,          ///< left X coordinate (Item Coordinates System)
                                    int cy,          ///< upper Y coordinate (Item Coordinates System)
                                    int cw,          ///< redrawing area width (Item Coordinates System)
                                    int ch           ///< redrawing are height (Iten Coordinates System)
                                );

  private:
    QCacheView* view_;

    NodeId orig_id_;
    NodeId dst_id_;

    QCacheNode* orig_;
    QCacheNode* dst_;

    QString name_;
    EdgeId id_;

    UINT16 bandwidth_;
    UINT16 latency_;

    vector<ItemDefinition *> item_queue_;
    UINT16 first_;
    UINT16 last_;
    UINT16 num_items_;

    UINT16 advanceCircularIndex(UINT16 idx) const;
};

#endif

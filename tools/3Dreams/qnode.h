/*****************************************************************************
*
* @brief Node representation in qcache. Header File.
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

#ifndef QNODE_H
#define QNODE_H

// QCache includes
#include "dbase.h"
// Lib includes
#include "libagt/AGTTypes.h"
// STL includes
#include <vector>
// QT includes
#include <qstring.h>
#include <qpopupmenu.h>

class QCacheMain;
class QCacheVisualization;
class QCacheView;

struct LevelDefinition
{
    UINT32  elements;
    UINT32  margin;
    typedef enum LevelArrangement 
    {
        LEVEL_HORIZONTAL = 1,   ///< A single row
        LEVEL_VERTICAL,         ///< A single column
        LEVEL_DISTRIBUTED       ///< Elements distributed in as many columns and rows as possible
    } LevelArrangement;

    LevelArrangement arrangement;

    enum OrderingMode
    {
        ORDER_MANUAL = 1,        ///< The ordering is given from outside
        ORDER_QUEUE              ///< The elements are ordered in a FIFO queue
    } order;
};

typedef vector<LevelDefinition> NodeDefinition;

class QCTrackedNode
{
    typedef qcdb::NodeId NodeId;
  public:
    QCTrackedNode(NodeId id);
    ~QCTrackedNode();

    void setConfig(const QDomElement& dom);
    const QDomElement& config() const;

    NodeId id() const
    {
        return id_;
    }
  
  private:
    const NodeId id_;
    QDomElement* configDOM_;

};

/** Node representation*/
class QCacheNode : public QObject
{
    Q_OBJECT
  public:
    typedef qcdb::NodeId NodeId;

    QCacheNode(QCacheMain* parent, NodeId id);
    ~QCacheNode();

    void setStructure(const NodeDefinition& def);
    void setCapacity(UINT32 capacities[], UINT16 dimensions);
    void setName(QString name);
    QString name();
    void setParentNode(NodeId parent_id);

    void enterItem(UINT32 item_id, UINT32 slot);
    void exitItem(UINT32 slot);
    void deleteItem(UINT32 item_id);
    void cycle(UINT64 cycle);
    void setTag();

    UINT32 itemToSlot(UINT32 item_id) const;
    UINT32 slotToItem(UINT32 slot) const;

    void initialize(QCacheView* view);

    UINT32 totalCapacity() const;
    UINT32 numItems() const;

    // XML configuration read
    bool readConfiguration(const QDomElement& config);
    
    QPopupMenu* contextualMenu() const;

    qcdb::ItemDefinition* getItemDefinition(UINT32 item_id) const;
  

  private:
    
    typedef vector<QCacheVisualization*> Visualizations;
    typedef Visualizations::iterator VisuIterator;
    typedef Visualizations::const_iterator VisuConstIterator;
    Visualizations visualizations_;

    QCacheMain* parent_;
    
    typedef vector<UINT32> Items;
    typedef Items::iterator ItemIterator;
    typedef Items::const_iterator ItemConstIterator;
    Items items_;

    NodeDefinition structure_;

    NodeId id_;
    vector<NodeId> parentIds_;
    QString name_;

    UINT32 num_items_;

  private slots:
    void showDialog();

};

#endif

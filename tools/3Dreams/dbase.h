/*****************************************************************************
*
* @brief QCache DataBase. Header File.
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

#ifndef QCACHE_DBASE
#define QCACHE_DBASE

#include "libagt/AGTTypes.h"
#include "tagDictionary.h"
#include <qlistview.h>
#include <stl/hash_map>
/// QCache database definitions workspace
namespace qcdb
{


/** Class that represents a tag associated with an item. It contains the tag name and the value (or values) associated. */
class TagElement
{
  public:
    /** Constructor. Creates a tag element where the value is a string.*/
    TagElement(INT32 cyc,UINT32 p_item_id,
                        unsigned char tag_index,char* p_str,UBYTE p_time_span);
    /** Constructor. Creates a tag element where the value is a number.*/
    TagElement(INT32 cyc,UINT32 p_item_id,
                        unsigned char tag_index,UINT64 p_value,UBYTE p_time_span);
    /** Constructor. Creates a tag element where the value is a list of numbers.*/
    TagElement(INT32 cyc,UINT32 p_item_id,
                        unsigned char tag_index,UINT32 p_nval,
                        UINT64* p_value_array,UBYTE p_time_span);
    /** Destructor */
    ~TagElement();

// REDO
    /** Transforms the element into a QListViewItem, that stores in two columns the tag name and the value. If the tag has more
        than one value associated, then a tree of QListViewItem is created.*/
    QListViewItem* toListViewItem(QListView* parent=0) const
    {
        QListViewItem* ptrLVI = 0;
        QListViewItem* ptrLVItemp = 0;

        if (values_.size()==1)
        {
            ptrLVI = new QListViewItem(parent,tagName(),QString::number(static_cast<INT32>(tagIntValue())));
            return(ptrLVI);
        }
        if (values_.size()>1)
        {
            vector<UINT64>::const_iterator it;

            ptrLVI = new QListViewItem(parent,tagName(),"");

            for (it=values_.begin();it!=values_.end();it++)
            {
                ptrLVItemp = new QListViewItem(ptrLVI,"",QString::number(static_cast<UINT32>(*it)));
            }
            return(ptrLVI);
        }
        ptrLVI = new QListViewItem(parent,tagName(),tagStrValue());
        return (ptrLVI);
    };

    QListViewItem* toListViewItem(QListViewItem* parent)
    {
        return (0);
    };

    inline QString tagName() const
    {
        TagDescriptor td = TagDictionary::getEntry(tag_index_);
        return (td.name());
    }
    inline unsigned char tagIndex() const
    {
        return tag_index_;
    }
    inline void setTagStrValue(const QString value)
    {
        str_ = value;
    }
    inline QString tagStrValue() const
    {
        return (str_);
    }   
    inline UINT64 tagIntValue() const
    {
        return (values_[0]);
    }   
    const vector<UINT64>& tagIntValues() const;
    inline int type() const
    {
        TagDescriptor td = TagDictionary::getEntry(tag_index_);
        return (td.type());
    }
    enum TagType
    { 
        TYPE_TAG_STRING=0,
        TYPE_TAG_INT,
        TYPE_TAG_LIST,
        NUM_TYPE_TAGS,
        TYPE_TAG_NULL
    };
  private:
    QString str_;
    unsigned char tag_index_;
    vector<UINT64> values_;
};

typedef UINT32 ItemId;
typedef deque<TagElement*> TagList;

const ItemId NULL_ITEM_ID = static_cast<UINT32>(-1);

class ItemDefinition
{
  public:
    explicit ItemDefinition(ItemId id);
    ~ItemDefinition();

    ItemId id() const
    {
        return id_;
    }
    UINT32 slot()
    {
        return slot_;
    }
    void setSlot(const UINT32 slot)
    {
        slot_ = slot;
    }
    UINT16 node() const
    {
        return node_;
    }
    void setNode(const UINT16 node)
    {
        node_ = node;
    }
    QDialog* getDialog() const;

    void addTag(TagElement* tag);
    TagElement* findTag(const QString& tag_name) const;
    TagElement* findTag(const unsigned char tag_index) const;

    TagList tags_;
  private:
    const ItemId id_; 
    UINT32 slot_;
    UINT16 node_;
}; 

typedef UINT16 NodeId;
class NodeDefinition
{ 
  public:
    NodeDefinition(NodeId id, char* name, NodeId parent, UINT16 instance);
    ~NodeDefinition();

    NodeId id() const
    {
        return id_;
    };

    NodeId parent() const
    {
        return parent_;
    };

    QString name() const
    {
        return name_;
    };
    
    UINT16 instance() const
    {
        return instance_;
    };

    void dumpTrace() const;

  private:
    const NodeId id_; 
    const NodeId parent_;
    const QString name_;
    const UINT16 instance_;
};

typedef UINT16 EdgeId;

class EdgeDefinition
{
  public:
    EdgeDefinition(EdgeId id, UINT16 source, UINT16 dest, UINT16 bandwidth, UINT16 latency, char* name);
    ~EdgeDefinition();

    EdgeId id() const
    {
        return id_;
    };

    UINT16 source() const
    {
        return source_;
    };

    UINT16 destination() const
    {
        return dest_;
    };

    UINT32 bandwidth() const
    {
        return bandwidth_;
    };

    UINT32 latency() const
    {
        return latency_;
    };

    QString name() const
    {
        return name_;
    };

    void dumpTrace() const;

  private:
    const EdgeId id_;
    const UINT16 source_;
    const UINT16 dest_;
    const UINT16 bandwidth_;
    const UINT16 latency_;
    const QString name_;
};

class DBase
{
  public:
    DBase();
    ~DBase();

    void registerNewItem(UINT32 item_id);
    void registerDeleteItem(UINT32 item_id);
    void registerSetTagSingleValue(UINT32 item_id,char tag_name[8], UINT64 value);
    void registerSetTagString(UINT32 item_id,char tag_name[8],char* value);
    void registerSetTagSet(UINT32 item_id,char tag_name[8],UINT32 nval,UINT64* value);

    void registerNewEdge(UINT16 edge_id, UINT16 source, UINT16 destination,
                         UINT32 bandwidth, UINT32 latency, char* name);
    void registerNewNode(UINT16 node_id, char* node_name, UINT16 parent_id, UINT16 instance);
                         
    ItemDefinition* getItemDefinition(UINT32 item_id) const;
    EdgeDefinition* getEdgeDefinition(UINT16 id) const;
    vector<EdgeDefinition*> getEdgeDefinitions(UINT16 source, UINT16 destination) const;
    TagElement* getItemTag(UINT32 item_id, const QString& tag_name) const;

    void dumpEdges() const;
    void dumpNodes() const;

  private:
    typedef hash_map<ItemId,ItemDefinition*> Items;
    typedef Items::iterator ItemIt;
    typedef Items::const_iterator ItemConstIt;
    Items items_;
    typedef hash_map<EdgeId,EdgeDefinition*> Edges;
    typedef Edges::iterator EdgeIt;
    typedef Edges::const_iterator EdgeConstIt;
    Edges edges_;
    typedef hash_map<NodeId,NodeDefinition*> Nodes;
    typedef Nodes::iterator NodeIt;
    typedef Nodes::const_iterator NodeConstIt;
    Nodes nodes_;

};
}

#endif

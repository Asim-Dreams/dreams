/*****************************************************************************
*
* @brief QCache DataBase. Source File.
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

#include "dbase.h"
#include "tagDictionary.h"
#include "tagDialog.h"
#include <qlistview.h>
#include <qnamespace.h>

using namespace qcdb;

ItemDefinition::ItemDefinition(ItemId id):
    id_(id),
    slot_(0),
    node_(0)
{}

ItemDefinition::~ItemDefinition()
{ 
    TagList::iterator it;

    for (it=tags_.begin();it!=tags_.end();it++)
    {
        delete (*it);
    }
}

QDialog* 
ItemDefinition::getDialog() const
{
    ItemDialog* dialog = new ItemDialog(0,0,false,Qt::WDestructiveClose);
    dialog->setItemId(id_);

    QListView* lv = dialog->getListView();
    // Add Columns
    lv->addColumn("Tag");
    lv->addColumn("Value");
    lv->setRootIsDecorated(true);

    // Add tag values
    for (TagList::const_iterator it=tags_.begin();it!=tags_.end();++it)
    {
        // This allocates memory; but QT handles it when the dialog is destroyed
        (*it)->toListViewItem(lv);
    }
//    dialog->exec();
    return (dialog);
}

void 
ItemDefinition::addTag(TagElement* tag)
{
    TagList::iterator it;

    for (it=tags_.begin();it!=tags_.end();++it)
    {
        if ((*it)->tagIndex()==tag->tagIndex())
        {
            delete (*it); // free space
            (*it)=tag;
            return;
        }
    }
    tags_.push_back(tag);
}

TagElement* 
ItemDefinition::findTag(const unsigned char tag_index) const
{
    TagList::const_iterator it;

    for (it=tags_.begin();it!=tags_.end();++it)
    {
          if ((*it)->tagIndex() == tag_index)
            return ((*it));
    }
    return NULL;
} 

TagElement* 
ItemDefinition::findTag(const QString& tag_name) const
{
    TagList::const_iterator it;
    bool found = false;

    unsigned char idx = TagDictionary::getEntryIndex(tag_name, &found);

    if (!found)
    {
        // cerr << __FILE__ << ": Tag " << tag_name << " was not found in dictionary" << endl;
        return NULL;
    }

    for (it=tags_.begin();it!=tags_.end();++it)
    {
          if ((*it)->tagIndex() == idx)
            return ((*it));
    }
    return NULL;
}

EdgeDefinition::EdgeDefinition(EdgeId id, UINT16 source, UINT16 dest, UINT16 bandwidth, UINT16 latency, char* name):
    id_(id),
    source_(source),
    dest_(dest),
    bandwidth_(bandwidth),
    latency_(latency),
    name_(name)
{
}

void
EdgeDefinition::dumpTrace() const
{
    cout << "\tEdge: " << name_ << " ID: " << id_ << " From: " << source_ << " To: " << dest_ << " Band: " << bandwidth_
        << " Lat: " << latency_ << endl;
}

NodeDefinition::NodeDefinition(NodeId id, char* name, NodeId parent, UINT16 instance):
    id_(id),
    parent_(parent),
    name_(name),
    instance_(instance)
{    
}

void
NodeDefinition::dumpTrace() const
{
    cout << "\tNode: " << name_ << " ID: " << id_ << " Parent: " << parent_ << " Instance: " << instance_ << endl;
}

TagElement::TagElement(INT32 cyc,UINT32 p_item_id,
                    unsigned char tag_index,char* p_str,UBYTE p_time_span)
{
    tag_index_ = tag_index;
    str_ = strdup(p_str);
}


TagElement::TagElement(INT32 cyc,UINT32 p_item_id,
                        unsigned char tag_index,UINT64 p_value,UBYTE p_time_span)
{
    tag_index_ = tag_index;
    values_.push_back(p_value);
}

TagElement::TagElement(INT32 cyc,UINT32 p_item_id,
                        unsigned char tag_index,UINT32 p_nval,
                        UINT64* p_value_array,UBYTE p_time_span)
{
    tag_index_ = tag_index;
    while (p_nval > 0)
    {
        values_.push_back((*(p_value_array++)));
        --p_nval;
    }
}

TagElement::~TagElement()
{
}

const vector<UINT64>& 
TagElement::tagIntValues() const
{
    return values_;
}

DBase::DBase()
{
}

DBase::~DBase()
{
    Items::iterator it;

    for (it=items_.begin();it!=items_.end();++it)
    {
        delete it->second;
    }
}

void 
DBase::registerNewItem(UINT32 item_id)
{
    assert(items_.find(item_id)==items_.end());
    items_[item_id] = new ItemDefinition(item_id);
}

void 
DBase::registerDeleteItem(UINT32 item_id)
{
    Items::iterator it = items_.find(item_id);
    if (it!=items_.end()) // Found
    {
        ItemDefinition* item = (items_.find(item_id))->second;
//        ExitNode(item->node(),item->slot());
        item->setNode(0);
        items_.erase(item_id);
        delete item;
    }
}

void 
DBase::registerSetTagSingleValue(UINT32 item_id,char tag_name[8], UINT64 value)
{
    TagElement* tag;

    assert(items_[item_id]!=NULL);

    TagDescriptor td(QString(tag_name).stripWhiteSpace(),TagDescriptor::TYPE_TAG_INTEGER);
    // cout << __FILE__ << ": Registering tag " << tag_name << endl; 
    unsigned char index = TagDictionary::addEntry(td);
    
    tag = new TagElement(0,item_id, index, value, 0);
    items_[item_id]->addTag(tag);
}

void 
DBase::registerSetTagString(UINT32 item_id,char tag_name[8],char* value)
{
    TagElement* tag;

    assert(items_[item_id]!=NULL);

    TagDescriptor td(QString(tag_name).stripWhiteSpace(),TagDescriptor::TYPE_TAG_STRING);
    unsigned char index = TagDictionary::addEntry(td);

    tag = new TagElement(0, item_id, index, value, 0);
    items_[item_id]->addTag(tag);
}

void 
DBase::registerSetTagSet(UINT32 item_id,char tag_name[8],UINT32 nval,UINT64* value)
{
    TagElement* tag;

    assert(items_[item_id]!=NULL);

    TagDescriptor td(QString(tag_name).stripWhiteSpace(),TagDescriptor::TYPE_TAG_SET);
    char index = TagDictionary::addEntry(td);

    tag = new TagElement(0,item_id, index, nval, value, 0);
    items_[item_id]->addTag(tag);
}

void 
DBase::registerNewEdge(UINT16 edge_id, UINT16 source, UINT16 destination,
                         UINT32 bandwidth, UINT32 latency, char* name)
{ 
    cout << "Registering edge: " << edge_id << endl;
    EdgeDefinition* edge = new EdgeDefinition(edge_id, source, destination, bandwidth, latency, name);
    edges_[edge_id] = edge;
}

void 
DBase::registerNewNode(UINT16 node_id, char* node_name, UINT16 parent_id, UINT16 instance)
{ 
    cout << "Registering node: " << node_id << endl;
    NodeDefinition* node = new NodeDefinition(node_id, node_name, parent_id, instance);
    nodes_[node_id] = node;
}


ItemDefinition* 
DBase::getItemDefinition(UINT32 item_id) const
{
    ItemConstIt it = items_.find(item_id);
    assert(it != items_.end());
    return (it->second);
}

EdgeDefinition* 
DBase::getEdgeDefinition(UINT16 edge_id) const
{
    EdgeConstIt it = edges_.find(edge_id);
    assert(it != edges_.end());
    return (it->second);
}

vector<EdgeDefinition*>
DBase::getEdgeDefinitions(UINT16 source, UINT16 destination) const
{
    vector<EdgeDefinition*> selected_edges;

    for (EdgeConstIt it=edges_.begin(); it!=edges_.end();++it)
    {
        EdgeDefinition* edge = it->second;
        if (edge->source()==source && edge->destination()==destination)
        {
            selected_edges.push_back(edge);
        }
    }

    return selected_edges;
}

TagElement* 
DBase::getItemTag(UINT32 item_id, const QString& tag_name) const
{
    ItemConstIt it = items_.find(item_id);
    assert(it != items_.end());
    return ((it->second)->findTag(tag_name));
}

void
DBase::dumpEdges() const
{
    for (EdgeConstIt it=edges_.begin(); it!=edges_.end();++it)
    {
        EdgeDefinition* edge = it->second;
        edge->dumpTrace();
    }
}

void
DBase::dumpNodes() const
{
    for (NodeConstIt it=nodes_.begin(); it!=nodes_.end();++it)
    {
        NodeDefinition* node = it->second;
        node->dumpTrace();
    }
}

/*****************************************************************************
 *
 * @author Oscar Rosell
 *
 * Copyright (C) 2006 Intel Corporation
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
 *****************************************************************************/
#include "selector.h"
#include "node_db.h"
#include "listener.h"
#include "selector_factory.h"
#include <iostream>

using namespace std;


const char* const ENTER_NODE_SELECTOR::theId_ = "enters";
const char* const FIRST_STAGE_SELECTOR::theId_ = "starts";
const char* const LAST_STAGE_SELECTOR::theId_ = "ends";
const char* const ITEM_TAG_SELECTOR::theId_ = "itemtag";
const char* const ALL_SELECTOR::theId_ = "all";

NODE_INFO::NODE_INFO():
    instance_(NULL_NODE_INSTANCE),
    node_id_(NODE_DB_CLASS::NULL_NODE_ID)
{
}

bool
NODE_INFO::readConfig(xmlNode* config)
{
    xmlChar* node = xmlGetProp(config, BAD_CAST "nodename");
    if (node)
    {
        setNode(reinterpret_cast<const char*>(node));
        xmlChar* instance = xmlGetProp(config, BAD_CAST "instance");
        if (instance)
        {
            const char* inst = reinterpret_cast<const char*>(instance);
            char* end;
            UINT32 inst_num = strtol(inst, &end, 0);
            if (*end == '\0')
            {
                setInstance(inst_num);
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

void
NODE_INFO::setNode(std::string node)
{
    node_name_ = node;
}

void
NODE_INFO::setInstance(UINT32 instance)
{
    instance_ = instance;
    node_id_ = NODE_DB_CLASS::getNodeId(node_name_, instance_);
}

UINT32
NODE_INFO::getNodeId() const
{
    return node_id_;
}

ENTER_NODE_SELECTOR*
ENTER_NODE_SELECTOR::Clone() const
{
    ENTER_NODE_SELECTOR* clone = new ENTER_NODE_SELECTOR;
    clone->setNode(node_name_);
    clone->setInstance(instance_);
    return clone;
}

bool
ENTER_NODE_SELECTOR::isValid(LIFETIME_CLASS* lifetime) const
{
    const UINT32 node_id = getNodeId();
    if (node_id == NODE_DB_CLASS::NULL_NODE_ID)
    {
        return (lifetime->hasEnteredPartialMatch(node_name_));
    }
    else
    {
        return (lifetime->hasEntered(node_id));
    }
}

bool
ENTER_NODE_SELECTOR::readConfig(xmlNode* config)
{
    assert(!xmlStrcmp(config->name, BAD_CAST "enters"));
    return NODE_INFO::readConfig(config);
}

FIRST_STAGE_SELECTOR*
FIRST_STAGE_SELECTOR::Clone() const
{
    FIRST_STAGE_SELECTOR* clone = new FIRST_STAGE_SELECTOR;
    clone->setNode(node_name_);
    clone->setInstance(instance_);
    return clone;
}

bool
FIRST_STAGE_SELECTOR::isValid(LIFETIME_CLASS* lifetime) const
{
    if (lifetime->numStages() == 0)
    {
        return false;
    }
    const string first_stage = lifetime->stageName(0);
    return (first_stage == node_name_) ? true : false;
}

bool 
FIRST_STAGE_SELECTOR::readConfig(xmlNode* config)
{
    assert(!xmlStrcmp(config->name, BAD_CAST "starts"));
    return NODE_INFO::readConfig(config);
}

LAST_STAGE_SELECTOR*
LAST_STAGE_SELECTOR::Clone() const
{
    LAST_STAGE_SELECTOR* clone = new LAST_STAGE_SELECTOR;
    clone->setNode(node_name_);
    clone->setInstance(instance_);
    return clone;
}

bool
LAST_STAGE_SELECTOR::isValid(LIFETIME_CLASS* lifetime) const
{
    const UINT32 num_stages = lifetime->numStages();
    if (num_stages == 0)
    {
        return false;
    }
    string last_stage = lifetime->stageName(num_stages-1);
    return (last_stage == node_name_) ? true : false;
}

bool
LAST_STAGE_SELECTOR::readConfig(xmlNode* config)
{
    assert(!xmlStrcmp(config->name, BAD_CAST "ends"));
    return NODE_INFO::readConfig(config);
}

ITEM_TAG_SELECTOR*
ITEM_TAG_SELECTOR::Clone() const
{
    ITEM_TAG_SELECTOR* clone = new ITEM_TAG_SELECTOR;
    clone->name_ = name_;
    clone->value_ = value_;
    clone->num_value_ = num_value_;
    return clone;
}

bool
ITEM_TAG_SELECTOR::isValid(LIFETIME_CLASS* lifetime) const
{
    set<UINT32> items = lifetime->getItems();

    for (set<UINT32>::const_iterator  it = items.begin(); it != items.end(); ++it)
    {
        const ITEM_TAGS* tags = THE_ITEM_TAGS_DB::Instance().getTags(*it);
        if (tags && tags->hasValue(name_, value_))
        {
            return true;
        }
    }
    return false;
}

bool
ITEM_TAG_SELECTOR::readConfig(xmlNode* config)
{
    assert(!xmlStrcmp(config->name, BAD_CAST "itemtag"));
    xmlChar* tag_name = xmlGetProp(config, BAD_CAST "name");
    xmlChar* tag_value = xmlGetProp(config, BAD_CAST "value");
    if (tag_name && tag_value)
    {
        name_ = reinterpret_cast<const char*>(tag_name);
        value_ = reinterpret_cast<const char*>(tag_value);
        THE_ITEM_TAGS_DB::Instance().registerTag(name_);
        return true;
    }
    else
    {
        return false;
    }
}

ALL_SELECTOR* 
ALL_SELECTOR::Clone() const
{   
    ALL_SELECTOR* clone = new ALL_SELECTOR;
    return clone;
}


AND_SELECTOR*
AND_SELECTOR::Clone() const
{
    SELECTOR* clone_child_a = selector_a_->Clone();
    SELECTOR* clone_child_b = selector_b_->Clone();
    AND_SELECTOR* clone = new AND_SELECTOR(clone_child_a, clone_child_b);
    return clone;
}

bool 
AND_SELECTOR::isValid(LIFETIME_CLASS* lifetime) const
{
    return (selector_a_->isValid(lifetime) && selector_b_->isValid(lifetime));
}

COMPOSITE_SELECTOR::~COMPOSITE_SELECTOR()
{
    for (vector<SELECTOR*>::iterator it = selectors_.begin(); it != selectors_.end(); ++it)
    {
        delete (*it);
    }
}

COMPOSITE_SELECTOR*
COMPOSITE_SELECTOR::Clone() const
{
    COMPOSITE_SELECTOR* clone = new COMPOSITE_SELECTOR;
    for (vector<SELECTOR*>::const_iterator it = selectors_.begin(); it != selectors_.end(); ++it)
    {
        clone->selectors_.push_back((*it)->Clone());
    }
    return clone;
}

bool
COMPOSITE_SELECTOR::isValid(LIFETIME_CLASS* lifetime) const
{
    for (UINT32 i=0; i<selectors_.size(); ++i)
    {
        if (selectors_[i]->isValid(lifetime) == false)
        {
            return false;
        }
    }
    return true;
}


bool 
COMPOSITE_SELECTOR::readConfig(xmlNode* config)
{
    assert(!xmlStrcmp(config->name, BAD_CAST "selector"));

    xmlNode* child_node;
    xmlChar* name = xmlGetProp(config, BAD_CAST "name");

    name_ = reinterpret_cast<const char*>(name);

    for (child_node = config->children; child_node != NULL; child_node = child_node->next)
    {
        if (child_node->type == XML_ELEMENT_NODE)
        {
            if (!xmlStrcmp(child_node->name, BAD_CAST "info"))
            {
                if (!addInfo(child_node)) return false;
            }
            else if (!xmlStrcmp(child_node->name, BAD_CAST "not"))
            {
                string selector_name = "not-";

                xmlNode* base_selector = 0;
                xmlNode* child_selector = 0;
                for (child_selector = child_node->children; child_selector != NULL; child_selector = child_selector->next)
                {
                    if (child_selector->type == XML_ELEMENT_NODE)
                    {
                        base_selector = child_selector;
                        selector_name += (reinterpret_cast<const char*>(child_selector->name));
                    }
                }
                SELECTOR* new_selector = THE_SELECTOR_FACTORY::Instance().CreateObject(selector_name);
                if (!new_selector->readConfig(base_selector)) 
                {
                    delete new_selector;
                    return false;
                }
                selectors_.push_back(new_selector);
            }
            else if (!xmlStrcmp(child_node->name, BAD_CAST "selected-by"))
            {
            }
            else
            {
                string selector_name = (reinterpret_cast<const char*>(child_node->name));
                SELECTOR* new_selector = 0;
                try {
                    new_selector = THE_SELECTOR_FACTORY::Instance().CreateObject(selector_name);
                }
                catch (...) {
                    delete new_selector;
                    return false;
                }

                if (!new_selector->readConfig(child_node)) 
                {
                    delete new_selector;
                    return false;
                }

                selectors_.push_back(new_selector);
            }
        }
    }
    selfRegister();
    return true;
}

bool
COMPOSITE_SELECTOR::addInfo(xmlNode* info_node)
{
    bool ok = true;

    xmlChar* text = xmlNodeGetContent(info_node);
    info_ = string(reinterpret_cast<char *>(text));
    xmlFree(text);

    return ok;
}

void
COMPOSITE_SELECTOR::selfRegister()
{
    THE_SELECTOR_FACTORY::Instance().Register(name_, *this, info_);
}

void 
RegisterDefaultSelectors()
{
    ENTER_NODE_SELECTOR ens;
    THE_SELECTOR_FACTORY::Instance().Register("enters", ens, "The item enters a given node.");
    NOT_SELECTOR<ENTER_NODE_SELECTOR> not_ens;
    THE_SELECTOR_FACTORY::Instance().Register("not-enters", not_ens, "The item doesn't enter a given node.");
    FIRST_STAGE_SELECTOR fss;
    THE_SELECTOR_FACTORY::Instance().Register("starts", fss, "The item first stage is the given one.");
    NOT_SELECTOR<ENTER_NODE_SELECTOR> not_fss;
    THE_SELECTOR_FACTORY::Instance().Register("not-starts", not_fss, "The item first stage is not the given one.");
    LAST_STAGE_SELECTOR lss;
    THE_SELECTOR_FACTORY::Instance().Register("ends", lss, "The item last stage is the given one.");
    NOT_SELECTOR<LAST_STAGE_SELECTOR> not_lss;
    THE_SELECTOR_FACTORY::Instance().Register("not-ends", not_lss, "The item last stage is not the given one.");
    ITEM_TAG_SELECTOR its;
    THE_SELECTOR_FACTORY::Instance().Register(its.id(), its, "The item has an associated tag value.");
    ALL_SELECTOR alls;
    THE_SELECTOR_FACTORY::Instance().Register("all", alls, "Any transaction.");
}


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
#ifndef ITEM_TAGS_H
#define ITEM_TAGS_H

#include <asim/syntax.h>
#include <string>
#include <map>
#include <deque>
#include <set>
#include <list>
#include <utility>
#include <iostream>
#include <assert.h>
#include <Singleton.h>
#include "node_db.h"

class STRING_TO_ID_DICT
{
  public:
    typedef std::string KEY;
    typedef UINT32 VALUE;

    STRING_TO_ID_DICT():
        counter_(0)
    {}

    VALUE getValue(KEY key)
    {
        std::map<KEY, VALUE>::iterator it = dictionary_.find(key);
        if (it!=dictionary_.end()) // FOUND
        {
            return it->second;
        }
        else 
        {
            // NEW ENTRY
            reverse_dictionary_[counter_] = key;
            dictionary_[key] = counter_++;
            return dictionary_[key];
        }
    }

    KEY getKey(VALUE value)
    {
        assert(reverse_dictionary_.find(value) != reverse_dictionary_.end());
        return reverse_dictionary_[value];
    }
  private:
    std::map<KEY, VALUE> dictionary_;
    std::map<VALUE, KEY> reverse_dictionary_;
    VALUE counter_;
};

typedef Loki::SingletonHolder<STRING_TO_ID_DICT> THE_STRING_TO_ID_DICT;

class ITEM_TAGS
{
  public:
    typedef UINT32 CYCLE_TYPE;

    void addStringTag(std::string name, CYCLE_TYPE cycle, std::string value)
    {
        // Perform string to id conversions for name and value
//        std::cout << "Adding Tag: " << name << " - " << value << " Key: " << THE_STRING_TO_ID_DICT::Instance().getValue(name) << std::endl;
        const STRING_TO_ID_DICT::VALUE value_id = THE_STRING_TO_ID_DICT::Instance().getValue(value);
        const std::pair<CYCLE_TYPE, STRING_TO_ID_DICT::VALUE> data(cycle, value_id);
        string_tags_[THE_STRING_TO_ID_DICT::Instance().getValue(name)].push_back(data);
    };

    void addNumberTag(std::string name, CYCLE_TYPE cycle, UINT64 value)
    {
        const std::pair<CYCLE_TYPE, UINT64> data(cycle, value);
        // Perform string to id conversion for name
        number_tags_[THE_STRING_TO_ID_DICT::Instance().getValue(name)].push_back(data);
    };

    void dumpState(std::ostream& o_stream, const std::string prefix) const
    {
    }

    bool hasValue(std::string name, std::string value) const;
    bool hasValue(std::string name, UINT64 value) const;

    std::string getStringTag(std::string name) 
    {
        STRING_TO_ID_DICT::VALUE id = THE_STRING_TO_ID_DICT::Instance().getValue(name);
        return THE_STRING_TO_ID_DICT::Instance().getKey(string_tags_[id].front().second);
    }

    bool getTagValue(std::string name, std::string* value) const
    {
        STRING_TO_ID_DICT::VALUE id = THE_STRING_TO_ID_DICT::Instance().getValue(name);
        StringTags::const_iterator it = string_tags_.find(id); 
        if (it != string_tags_.end())
        {
            *value = THE_STRING_TO_ID_DICT::Instance().getKey(it->second.front().second);
//            std::cout << "TAG: " << name << " VALUE: " << *value << std::endl;
            return true;
        }
        else
        {
            return false;
        }
    }
    bool getTagValue(std::string name, UINT64* value)
    {
        STRING_TO_ID_DICT::VALUE id = THE_STRING_TO_ID_DICT::Instance().getValue(name);
        NumberTags::const_iterator it = number_tags_.find(id);
        if (it != number_tags_.end())
        {
            *value = it->second.front().second;
            return true;
        }
        else
        {
            return false;
        }
    }

  private:
    typedef UINT32 TagId;

    typedef std::map<TagId, std::list< std::pair<CYCLE_TYPE, STRING_TO_ID_DICT::VALUE> > > StringTags;
    StringTags string_tags_;
    typedef std::map<TagId, std::list< std::pair<CYCLE_TYPE, UINT64> > > NumberTags;
    NumberTags number_tags_;
};

class ITEM_TAGS_DB
{
  public:
    typedef UINT32 ItemId;

    void addStringTag(ItemId id, std::string name, ITEM_TAGS::CYCLE_TYPE cycle, std::string value);
    void addNumberTag(ItemId id, std::string name, ITEM_TAGS::CYCLE_TYPE cycle, UINT64 value)
    {
        if (registeredTags_.find(name) != registeredTags_.end())
            db_[id].addNumberTag(name, cycle, value);
    };

    std::string getStringTag(ItemId id, std::string name) 
    {
        if (db_.find(id) == db_.end())
        {
            return "";
        }
        else
        {
            std::string value;
            if (getTags(id)->getTagValue(name, &value))
            {
                return value;
            }
            else
            {
                return "";
            }
        }
    }

    const ITEM_TAGS* getTags(ItemId id) const;

    void registerTag(std::string name)
    {
        DEBUG(std::cout << "Registering tag: " << name << std::endl);
        if (registeredTags_.find(name) == registeredTags_.end())
        {
            registeredTags_.insert(name);
        }
    }

  private:
    std::map<ItemId, ITEM_TAGS> db_;
    std::set<std::string> registeredTags_;

};

typedef Loki::SingletonHolder<ITEM_TAGS_DB> THE_ITEM_TAGS_DB;


#endif

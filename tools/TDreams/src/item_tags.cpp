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

#include "item_tags.h"

using namespace std;

namespace {
    template <class T>
    struct IsValue : public binary_function< pair<ITEM_TAGS::CYCLE_TYPE, T>, T, bool >
    {
        bool operator()(pair<ITEM_TAGS::CYCLE_TYPE, T> pair, T value) const
        {
            return pair.second == value;
        };
    };
}

bool
ITEM_TAGS::hasValue(string name, string value) const
{
    const STRING_TO_ID_DICT::VALUE key_id = THE_STRING_TO_ID_DICT::Instance().getValue(name);
    const STRING_TO_ID_DICT::VALUE value_id = THE_STRING_TO_ID_DICT::Instance().getValue(value);

    const StringTags::const_iterator it = string_tags_.find(key_id);
    
    if(it == string_tags_.end()) { return false; }

    return ((find_if(
                it->second.begin(), 
                it->second.end(), 
                bind2nd(IsValue<STRING_TO_ID_DICT::VALUE>(), value_id))
            ) != it->second.end()
            );
}

bool
ITEM_TAGS::hasValue(string name, UINT64 value) const
{
    const STRING_TO_ID_DICT::VALUE key_id = THE_STRING_TO_ID_DICT::Instance().getValue(name);

    const NumberTags::const_iterator it = number_tags_.find(key_id);

    return ((find_if(
                it->second.begin(), 
                it->second.end(), 
                bind2nd(IsValue<STRING_TO_ID_DICT::VALUE>(), value))
            ) != it->second.end()
            );
}

void
ITEM_TAGS_DB::addStringTag(ItemId id, std::string name, ITEM_TAGS::CYCLE_TYPE cycle, std::string value)
{
    if (registeredTags_.find(name) != registeredTags_.end())
    {
        db_[id].addStringTag(name, cycle, value);
//        std::map<ItemId, ITEM_TAGS>::const_iterator it = db_.find(id);
    }
};

const ITEM_TAGS*
ITEM_TAGS_DB::getTags(ItemId id) const
{
    std::map<ItemId, ITEM_TAGS>::const_iterator it = db_.find(id);
    if (it == db_.end())
    {
        return NULL;
    }
    return &(it->second);
};


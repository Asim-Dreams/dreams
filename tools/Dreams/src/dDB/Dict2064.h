// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

/**
  * @file  Dict2064.h
  */

#ifndef _DRALDB_DICT2064_H
#define _DRALDB_DICT2064_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/Hash6431.h"
#include "dDB/DRALTag.h"

#define DICT2064_MAXDICT         128
#define DICT2064_MAXKEY          1048574
#define DICT2064_MAXKEYDELTA     (DICT2064_MAXKEY >> 4)
#define DICT2064_UNDEFINED_VALUE 1048575
#define DICT2064_BUCKETS         1048589  // must be a prime number bigger than DICT2064_MAXKEY
// Therefore we support a max of 128*1048575= 134,217,600 different values.
// Hmm... I think that this is not actually true, as different dictionaries
// may hold repeated values

// Forward declaration
class ClockDomainMgr;

/**
 * @brief
 * This class maps 64 bits values with 32 bit keys.
 *
 * @description
 * The main use of this class is reduce the memory usage of the
 * program using 32 bit keys to map 64 bit values. To improve the
 * performance the mapping is stored in mutiple dictionaries of
 * DICT2064_MAXKEY entries. The problem having multiple dictionaries
 * is that the keys are repeated. To solve this the cycle is used
 * to distinguish between keys of different dictionaries (each
 * dict stores values starting at one cycle to another one).
 *
 * @author Federico Ardanaz
 * @date started at 2002-07-15
 * @version 0.1
 */
class Dict2064;

class Dict2064Node
{
    public:
        Dict2064Node();
        ~Dict2064Node();

        void setNumClocks(UINT32 num_clocks);
        void detach();

        Hash6431 * getDict() const;

    protected:
        UINT32 * cycle;       ///< First cycle 
        UINT64 * dict_values; ///< Holds the values of this dictinary.
        UINT32 next_key;      ///< Next entry in the dict array.
        Hash6431 * dict;      ///< I keep a hash-like dictionary for O(1) value look-up for the node.
        UINT32 counter;       ///< Counts the number of clock domains using this node. When 0, clears the dict.

        friend class Dict2064;
} ;

class Dict2064
{
    public:
        Dict2064();
        ~Dict2064();

        void startDict(ClockDomainMgr * clocks);
        void clearDict();
        void allocNewDict(UINT32 cycle, CLOCK_ID clock_id);

        inline INT32 getKeyFor(UINT64 value, CLOCK_ID clock_id);
        inline UINT64 getValueByCycle(INT32 key, UINT32 cycle, CLOCK_ID clock_id);

        inline INT32 getNumDictionaries() const;
        inline bool getNeedsNewDict(CLOCK_ID clock_id) const;

    private:
        UINT32 * next_dict;                      ///< First free position in the dict_vec array.
        bool * alloc_new_dict;                   ///< Needs to allocate another dict.
        Hash6431 ** current_dict;                ///< I keep a hash-like dictionary for O(1) value look-up for the "current" only (last created) dictionary.
        Dict2064Node dict_vec[DICT2064_MAXDICT]; ///< The dictionary array.
        UINT32 num_clocks;                       ///< Number of clock domains.
} ;

/*
 * Inserts a new 64 bit integer in the dictionary. When the number
 * of entries in the actual dictionary gets DICT2064_MAXKEY a new
 * one is used. This is done to avoid too many collisions inside
 * the hash. To distinguish between the same key of different
 * dictionaries the cycle is used.
 *
 * @return the key for the value.
 */
INT32
Dict2064::getKeyFor(UINT64 value, CLOCK_ID clock_id)
{
    INT32 key = current_dict[clock_id]->find(value);

    // Get a new entry then.
    if(key < 0)
    {
        // 1) check for room in the current dict
        if(dict_vec[next_dict[clock_id] - 1].next_key >= (DICT2064_MAXKEY - DICT2064_MAXKEYDELTA))
        {
            // We need to allocate a new dict.
            alloc_new_dict[clock_id] = true;
        }

        // This is an internal error... if happens, change DICT2064_MAXKEYDELTA to a bigger value,
        // but more than 64K different values in 1 cycle seems a model bug... so blame the user!!!
        Q_ASSERT(dict_vec[next_dict[clock_id] - 1].next_key != DICT2064_MAXKEY);

        // 2) put it, be aware that if allocNewDict() is called, next_dict is incremented!
        key = dict_vec[next_dict[clock_id] - 1].next_key++;
        dict_vec[next_dict[clock_id] - 1].dict_values[key] = value;
        current_dict[clock_id]->insert(value, key);
    }

    return key;
}

/*
 * This functions searches the dictionary that has the values for
 * the cycle cycle and then returns the value of the key inside
 * the dictionary.
 *
 * @return the value mapped to the key key.
 */
UINT64
Dict2064::getValueByCycle(INT32 key, UINT32 cycle, CLOCK_ID clock_id)
{
    UINT32 dict_num = 0;
    bool fnd = false;
    while(!fnd && (dict_num < next_dict[clock_id]))
    {
        fnd = dict_vec[dict_num].cycle[clock_id] > cycle;
        dict_num++;
    }

    // recover extra inc
    dict_num--;
    if(fnd)
    { 
        dict_num--;
    }
    return dict_vec[dict_num].dict_values[key];
}

/*
 * Just returns the number of dictionaries used.
 *
 * @return the number of dictionaries.
 */
INT32
Dict2064::getNumDictionaries() const
{
    return next_dict[0];
}

/*
 * Returns if a clock domain needs to use a new dictionary.
 *
 * @return if clock domains needs a new dict.
 */
bool
Dict2064::getNeedsNewDict(CLOCK_ID clock_id) const
{
    return alloc_new_dict[clock_id];
}

#endif

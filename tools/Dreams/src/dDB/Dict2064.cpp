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
  * @file Dict2064.cpp
  */

// Dreams includes.
#include "dDB/Dict2064.h"
#include "dDB/ClockDomainMgr.h"

/**
 * Creator of this class. Creates a new hash and allocs the first
 * dictionary.
 *
 * @return new object.
 */
Dict2064::Dict2064()
{
    next_dict = NULL;
    current_dict = NULL;
    alloc_new_dict = NULL;
    num_clocks = 0;
}

/**
 * Destructor of this class. Just deletes the hash.
 *
 * @return destroys the object.
 */
Dict2064::~Dict2064()
{
    if(current_dict != NULL)
    {
        delete [] current_dict;
        delete [] next_dict;
        delete [] alloc_new_dict;
    }
}

/*
 * Starts the dictionary with a number of known clocks.
 *
 * @return void.
 */
void
Dict2064::startDict(ClockDomainMgr * clocks)
{
    num_clocks = clocks->getNumClocks();

    // Starts the dictionaries.
    for(UINT32 i = 0; i < DICT2064_MAXDICT; i++)
    {
        dict_vec[i].setNumClocks(num_clocks);
    }

    // Creates an array of dictionaries (each clock domain might point to a
    // different dictionary, but they will usually point to the same one).
    current_dict = new Hash6431 *[num_clocks]; 
    next_dict = new UINT32[num_clocks];
    alloc_new_dict = new bool[num_clocks];

    // In the beginning all the clocks use the same one.
    for(UINT32 i = 0; i < num_clocks; i++)
    {
        current_dict[i] = NULL;
        next_dict[i] = 0;
        alloc_new_dict[i] = false;
        allocNewDict(0, i);
    }
}

/*
 * Clears the allocated dictionaries. After calling these, no more
 * data can be introduced using dictionaries.
 *
 * @return void.
 */
void
Dict2064::clearDict()
{
    // Frees the dictionaries.
    for(UINT32 i = 0; i < DICT2064_MAXDICT; i++)
    {
        delete dict_vec[i].dict;
        dict_vec[i].dict = NULL;
    }

    // This assures no incorrect accesses in the future.
    for(UINT32 i = 0; i < num_clocks; i++)
    {
        current_dict[i] = NULL;
    }
}

/*
 * Allocs a new dictionary that holds values that are set after
 * the cycle cycle.
 *
 * @return void.
 */
void
Dict2064::allocNewDict(UINT32 cycle, CLOCK_ID clock_id)
{
    UINT32 tmp_next_dict = next_dict[clock_id];

    // Checks the state of the next dictionary
    if(dict_vec[tmp_next_dict].dict_values == NULL)
    {
        // This is the first clock domain to create this dictionary
        dict_vec[tmp_next_dict].dict_values = new UINT64[DICT2064_MAXKEY + 1];
        dict_vec[tmp_next_dict].dict = new Hash6431(DICT2064_BUCKETS);
        Q_ASSERT(dict_vec[tmp_next_dict].dict_values != NULL);
    }

    // Allocs a new dictionary for the actual clock domain
    dict_vec[tmp_next_dict].cycle[clock_id] = cycle;

    if(tmp_next_dict != 0)
    {
        dict_vec[tmp_next_dict - 1].detach();
    }

    current_dict[clock_id] = dict_vec[tmp_next_dict].dict;
    next_dict[clock_id]++;
    alloc_new_dict[clock_id] = false;
}

/**
 * Creator of this class. Sets the fields of the class.
 *
 * @return new object.
 */
Dict2064Node::Dict2064Node()
{
    dict_values = NULL;
    cycle = NULL;
    dict = NULL;
    next_key = 0;
    counter = 0;
}

/*
 * Free the dictionary array if it was allocated.
 *
 * @return destroys the object.
 */
Dict2064Node::~Dict2064Node()
{
    delete [] cycle;
    delete dict;
    if(dict_values != NULL)
    {
        delete [] dict_values;
    }
}

/*
 * Sets the number of clocks.
 *
 * @return void.
 */
void
Dict2064Node::setNumClocks(UINT32 num_clocks)
{
    cycle = new UINT32[num_clocks];
    dict = NULL;
    counter = num_clocks;
    for(UINT32 i = 0; i < num_clocks; i++)
    {
        cycle[i] = 0;
    }
}
/*
 * Detachs a clock domain from this node. Decreases the number of
 * clocks domain that have used this node and when 0, clears the
 * hash to free memory.
 *
 * @return void.
 */
void
Dict2064Node::detach()
{
    counter--;
    if(counter == 0)
    {
        delete dict;
        dict = NULL;
    }
}

/*
 * Gets the dict of this node.
 *
 * @return the dict of this node.
 */
Hash6431 *
Dict2064Node::getDict() const
{
    return dict;
}

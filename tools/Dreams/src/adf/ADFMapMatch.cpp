// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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

/*
 * @file ADFMapMatch.h
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ADFMapMatch.h"

// Qt includes.
#include <qglobal.h>

/*
 * Creator of the class.
 *
 * @return the new object.
 */
ADFMapMatch::ADFMapMatch(QString mapName)
    : ADFMap(mapName)
{
    has_default = false;
}

/*
 * Destructor of the class. Nothing done.
 *
 * @return the new object.
 */
ADFMapMatch::~ADFMapMatch()
{
}

/*
 * This function accesses to the map and gets the value defined in
 * the key entry. First gets if the key is defined, if so the value
 * is returned. Otherwise returns the default value (if defined).
 *
 * @param key is the position of the map.
 * @param value points to an integer where the value will be returned.
 *
 * @return if a value is inside de 0..100 range.
 */
bool
ADFMapMatch::getValue(INT32 key, UINT64 * value) const
{
    ADFMapEntryIterator it;
    ADFMapEntry entry;

    entry.key = key;

    // Tries to find the key.
    it = entries.find(entry);

    if(it != entries.end())
    {
        // If the value is found, returns the value.
        * value = it->value;
        return true;
    }
    else if(has_default)
    {
        // If not found and default defined.
        * value = default_value;
        return true;
    }

    // No value found.
    return false;
}

/*
 * This function creates the map. Just copies the entries.
 *
 * @param entries is the set of defined entries.
 *
 * @return void.
 */
void
ADFMapMatch::createMap(ADFMapEntrySet * entries)
{
    // Sanity checks.
    Q_ASSERT(entries->size() > 0);

    this->entries = * entries;
}

/*
 * Defines the default value of the map.
 *
 * @param value is the default value.
 *
 * @return void.
 */
void
ADFMapMatch::defineDefaultValue(UINT64 value)
{
    has_default = true;
    default_value = value;
}

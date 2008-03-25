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
 * @file ADFMap.h
 */

#ifndef _ADFMAP_H
#define _ADFMAP_H

#include "DreamsDefs.h"

#include "dDB/DralDBSyntax.h"

#include <qdict.h>

#include <set>
using namespace std;

/*
 * @typedef ADFMapEntry
 * @brief
 * One entry of a Map. Used in the createMap function.
 */
typedef struct
{
    INT32 key;    ///< Key for the map.
    UINT64 value; ///< Value for this key.
} ADFMapEntry;

/*
 * @typedef ADFMapEntryCmp
 * @brief
 * Compare function for the map entries.
 */
struct ADFMapEntryCmp
{
    bool operator()(const ADFMapEntry e1, const ADFMapEntry e2) const
    {
        return (e1.key < e2.key);
    }
} ;

/*
 * @typedef ADFMapEntrySet
 * @brief
 * A set of map entries.
 */
typedef set<ADFMapEntry, ADFMapEntryCmp> ADFMapEntrySet;

/*
 * @typedef ADFMapEntryIterator
 * @brief
 * Iterator for the map entries.
 */
typedef set<ADFMapEntry, ADFMapEntryCmp>::iterator ADFMapEntryIterator;

/**
  * @brief
  * This class defines an interface for the maps that are used
  * in the ExpressionMap class.
  *
  * @description
  * The classes that inherit ADFMap must implement a function
  * that given an index returns if a value is defined and the
  * actual value.
  *
  * @author Guillem Sole
  * @date started at 2004-10-01
  * @version 0.1
  */
class ADFMap
{
    public:
        /*
         * Creator of the class. Just copies the map name.
         *
         * @return the new object.
         */
        ADFMap(QString _mapName)
        {
            mapName = _mapName;
        }

        /*
         * Destructor of the class. Nothing done.
         *
         * @return destroys the object.
         */
        virtual ~ADFMap()
        {
        }

        /*
         * This function accesses to the map and gets the value defined in
         * the key entry. If no value is defined a false is returned.
         *
         * @param key is the position of the map.
         * @param value points to an integer where the value will be returned.
         *
         * @return if a value is defined in the key.
         */
        virtual bool getValue(INT32 key, UINT64 * value) const = 0;

        /*
         * This function adds a new entry into the map. Each implementation
         * will change the map state as defined.
         *
         * @param key is the position of the map.
         * @param value the value defined for the key.
         *
         * @return void.
         */
        virtual void createMap(ADFMapEntrySet * entries) = 0;

        inline QString getMapName() const;

    protected:
        QString mapName; ///< The name of this map.
} ;

/*
 * @typedef ADFMapSet
 * @brief
 * This class stores the set of defined maps.
 */
typedef QDict<ADFMap> ADFMapSet;

/*
 * Returns the map name.
 *
 * @return the name.
 */
QString
ADFMap::getMapName() const
{
    return mapName;
}

#endif // _ADFMAP_H.

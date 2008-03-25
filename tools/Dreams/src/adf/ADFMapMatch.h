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

#ifndef _ADFMAPMATCH_H
#define _ADFMAPMATCH_H

#include "DreamsDefs.h"
#include "ADFMap.h"

/*
 * @brief
 * This class implements a table that behaves like a switch statement
 * with a default return value (if requested).
 *
 * @description
 * This class stores a set with all the different defined values with
 * its correspondent return value. When a value is requested, this class
 * searches the value in the set and if found the value is returned. In
 * case that no value is found and a default one is defined, this last
 * value will be returned.
 *
 * @author Guillem Sole
 * @date started at 2004-10-01
 * @version 0.1
 */
class ADFMapMatch : public ADFMap
{
    public:
        ADFMapMatch(QString mapName);
        ~ADFMapMatch();

        bool getValue(INT32 key, UINT64 * value) const;
        void createMap(ADFMapEntrySet * entries);

        void defineDefaultValue(UINT64 value);

    private:
        bool has_default;       ///< True if the default value is set.
        UINT64 default_value;   ///< The default value.
        ADFMapEntrySet entries; ///< Set of defined entries.
} ;

#endif // _ADFMAPMATCH_H.

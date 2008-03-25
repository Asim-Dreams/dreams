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

/**
  * @file ADFMapColorInterpolate.h
  */

#ifndef _ADFMAPCOLORINTERPOLATE_H
#define _ADFMAPCOLORINTERPOLATE_H

#include "DreamsDefs.h"
#include "ADFMap.h"

/*
 * @brief
 * This class implements a table that stores the colors for
 * values that range from 0 to 100.
 *
 * @description
 * The map automatically generates the intermediate colors
 * interpolating the nearby defined colors. The entries are entered
 * by descending order using the map entry set (100 and 0 entries
 * are mandatory).
 *
 * @author Guillem Sole
 * @date started at 2004-10-01
 * @version 0.1
 */
class ADFMapColorInterpolate : public ADFMap
{
    public:
        ADFMapColorInterpolate(QString mapName);
        ~ADFMapColorInterpolate();

        bool getValue(INT32 key, UINT64 * value) const;
        void createMap(ADFMapEntrySet * entries);

    private:
        UINT64 colors[102]; ///< Vector with all the colors (stored as integer).
} ;

#endif // _ADFMAPCOLORINTERPOLATE_H.

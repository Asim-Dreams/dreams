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
 * @file ADFMapColorInterpolate.h
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ADFMapColorInterpolate.h"

// Qt includes.
#include <qglobal.h>
#include <qcolor.h>

/*
 * Creator of the class.
 *
 * @return the new object.
 */
ADFMapColorInterpolate::ADFMapColorInterpolate(QString mapName)
    : ADFMap(mapName)
{
}

/*
 * Destructor of the class.
 *
 * @return the new object.
 */
ADFMapColorInterpolate::~ADFMapColorInterpolate()
{
}

/*
 * This function accesses to the map and gets the value defined in
 * the key entry. If the key is negative or bigger than 100, a not
 * defined is returned. Otherwise, the precomputed interpolated color
 * is returned.
 *
 * @param key is the position of the map.
 * @param value points to an integer where the value will be returned.
 *
 * @return if a value is inside de 0..100 range.
 */
bool
ADFMapColorInterpolate::getValue(INT32 key, UINT64 * value) const
{
    // Checks if the key is out of bounds. Negative values will be seen as big values, so
    // the if will evaluate to true.
    if((UINT32) key > 100)
    {
        return false;
    }

    // Sets the color and returns true.
    * value = colors[key];
    return true;
}

/*
 * This function adds a new entry into the map. When a new entry
 * is added, all the colors between this and the last entry are
 * computed using interpolation.
 *
 * @param key is the position of the map.
 * @param value the value defined for the key.
 *
 * @return void.
 */
void
ADFMapColorInterpolate::createMap(ADFMapEntrySet * entries)
{
    // Sanity checks.
    Q_ASSERT(entries->size() > 1);

    ADFMapEntryIterator it;

    INT32 lo_entry;  ///< Entry index for the low entry.
    QColor lo_color; ///< Color for the low entry.
    INT32 hi_entry;  ///< Entry index for the high entry.
    QColor hi_color; ///< Color for the high entry.

    it = entries->begin();
    colors[it->key] = it->value;
    hi_entry = it->key;
    hi_color = QColor(colors[hi_entry]);
    it++;

    INT32 red;    ///< Red component of the interpolated color.
    INT32 green;  ///< Green component of the interpolated color.
    INT32 blue;   ///< Blue component of the interpolated color.
    double prop;  ///< Proportion of the colors.

    // Computes all the interpolation.
    do
    {
        // Copies the previous high to the actual low.
        lo_entry = hi_entry;
        lo_color = hi_color;

        // Gets the actual high and adds it to the map.
        colors[it->key] = it->value;
        hi_entry = it->key;
        hi_color = it->value;

        // Generates all the intermediate values.
        for(INT32 i = lo_entry + 1; i < hi_entry; i++)
        {
            // Computes the linear distance;
            prop = (double) (hi_entry - i) / (double) (hi_entry - lo_entry);

            // Interpolates the values.
            red = (INT32) ((double) hi_color.red() * (1.0 - prop) + (double) lo_color.red() * prop);
            green = (INT32) ((double) hi_color.green() * (1.0 - prop) + (double) lo_color.green() * prop);
            blue = (INT32) ((double) hi_color.blue() * (1.0 - prop) + (double) lo_color.blue() * prop);

            colors[i] = static_cast<UINT64>(QColor(red, green, blue).rgb());
        }

        it++;
    } while(it != entries->end());
}

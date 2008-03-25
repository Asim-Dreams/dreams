/*
 * Copyright (C) 2003-2006 Intel Corporation
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
 */

/**
  * @file AColorPalette.cpp
  */

#include "AColorPalette.h"

QColor* AColorPalette::colors = NULL;
INT32 AColorPalette::nextColor=0;

INT32
AColorPalette::createColor(QColor color)
{
    INT32 idx;

    // 1) look for that color in the palette
    idx = lookForColor (color);
    if (idx>=0) return (idx);

    // 2) if not found try to create a new entry for such a color
    Q_ASSERT(nextColor<ACPALETTE_NUMCOLORS);
    idx = nextColor;
    nextColor++;
    colors[idx] = color;

    return (idx);
}

INT32
AColorPalette::lookForColor (QColor color)
{
    INT32 i = 0;
    bool fnd = false;

    while ( !fnd && (i<nextColor) )
    {
        fnd = (colors[i] == color);       ///< QColor == operator is overloaded by QT library
        ++i;
    }
    
    if (fnd)
    {
       return (i-1);
    }   
    else
    {
       return (-1);
    }   
}




// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file AColorPalette.h
  */

#ifndef _ACOLORPALETTE_H_
#define _ACOLORPALETTE_H_

#include <stdio.h>
#include <stdlib.h>

#include <qcolor.h>

#include "asim/AGTTypes.h"
#include "DefaultSizes.h"


/**
  * This \em singleton class holds A_NUMCOLORS colors palette.
  * Put long desc. here...
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class AColorPalette
{
    public:
        /**
          * To create a brand new color 
          * @return palette index
          */
        static INT32 createColor(QColor);

        /**
          * To modify a prevously allocated color
          */
        static inline void setColor(INT32 idx, QColor);

        /**
          * To get back an allocated color
          */
        static inline QColor getColor(INT32 idx);

        /**
          * Function description
          */
        static inline void reset();


    protected:
        static INT32 lookForColor (QColor c);

    private:
        static QColor* colors;
        static INT32 nextColor;
};

void 
AColorPalette::setColor(INT32 idx, QColor c)
{
    Q_ASSERT(idx<nextColor);
    colors[idx] = c;
}

QColor 
AColorPalette::getColor(INT32 idx)
{
    Q_ASSERT(idx<nextColor);
    return colors[idx];
}

void
AColorPalette::reset()
{
    //printf ("AColorPalette::reset() called\n");fflush(stdout);
    if (colors==NULL)
    {
    	colors = new QColor[ACPALETTE_NUMCOLORS];
    }
	nextColor=0;
}

#endif


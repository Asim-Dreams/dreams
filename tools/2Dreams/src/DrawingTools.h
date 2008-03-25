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
  * @file DrawingTools.h
  */

#ifndef _DRAWINGTOOLS_H
#define _DRAWINGTOOLS_H

// standard C
#include <math.h>

// QT includes
#include <qpainter.h>

#include "asim/AGTTypes.h"

#include "EventShape.h"

/**
  * Static class with event-related drawing methods.
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class DrawingTools
{
    public:
    
        static inline void drawShape(int x,int y, QPainter* painter,EventShape shape, int cube_side);
        static inline void drawRectangle(int x,int y, QPainter* painter, int cube_side);
        static inline void drawCircle(int x,int y, QPainter* painter, int cube_side);
        static inline void drawTriangle(int x,int y, QPainter* painter, int cube_side);
        static inline void drawRhomb(int x,int y, QPainter* painter, int cube_side);
};

inline void DrawingTools::drawShape(int x,int y, QPainter* painter,EventShape shape, int cube_side)
{
    switch (shape)
    {
        case EVENT_SHAPE_RECTANGLE:
        drawRectangle(x,y,painter,cube_side);
        break;

        case EVENT_SHAPE_CIRCLE:
        drawCircle(x,y,painter,cube_side);
        break;

        case EVENT_SHAPE_TRIANGLE:
        drawTriangle(x,y,painter,cube_side);
        break;

        case EVENT_SHAPE_RHOMB:
        drawRhomb(x,y,painter,cube_side);
        break;

        default:
        drawRectangle(x,y,painter,cube_side);
    };
}

inline void 
DrawingTools::drawRectangle(int x,int y, QPainter* painter, int cube_side)
{painter->drawRect(x,y,cube_side,cube_side);}

inline void 
DrawingTools::drawCircle(int x,int y, QPainter* painter, int cube_side)
{painter->drawChord(x,y,cube_side,cube_side,0,(16*360));}

inline void 
DrawingTools::drawTriangle(int x,int y, QPainter* painter, int cube_side)
{
    int cube_size = cube_side - 1;
    QPointArray parray(3);
    parray.setPoint(0,x,y+cube_size);
    parray.setPoint(1,x+(int)rint((double)cube_size/2.0),y);
    parray.setPoint(2,x+cube_size,y+cube_size);
    painter->drawConvexPolygon (parray);
}

inline void 
DrawingTools::drawRhomb(int x,int y, QPainter* painter, int cube_side)
{
    int cube_size = cube_side - 1;
    QPointArray parray(4);
    parray.setPoint(0,x,y+(int)rint((double)cube_size/2.0));
    parray.setPoint(1,x+(int)rint((double)cube_size/2.0),y);
    parray.setPoint(2,x+cube_size,y+(int)rint((double)cube_size/2.0));
    parray.setPoint(3,x+(int)rint((double)cube_size/2.0),y+cube_size);
    painter->drawConvexPolygon (parray);
}

#endif


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
  * @file DrawingTools.h
  */

#ifndef _DRAWINGTOOLS_H
#define _DRAWINGTOOLS_H

// standard C/C++
#include <math.h>
#include <iostream>
using namespace std;

#include "DreamsDefs.h"

// QT includes
#include <qpainter.h>

// AGT
#include "asim/AScrollView.h"

#include "asim/AGTTypes.h"
#include "EventShape.h"

/**
  * Static class with event-related drawing methods.
  * To prevent ugly pixelation when drawing text I need to access to AScrollView methods so an
  * ugly dependency with this class has been added
  * @version 1.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class DrawingTools
{
    public:

        // Drawings without letter.
        static inline void drawShape    (int x, int y, QPainter * painter, EventShape shape, int width, int height);
        static inline void drawRectangle(int x, int y, QPainter * painter, int width, int height);
        static inline void drawCircle   (int x, int y, QPainter * painter, int width, int height);
        static inline void drawTriangle (int x, int y, QPainter * painter, int width, int height);
        static inline void drawRhomb    (int x, int y, QPainter * painter, int width, int height);

        // for full scale drawing use shape and letter    
        static inline void drawShape    (int x, int y, QPainter * painter, EventShape shape, QChar c, QColor lc, int width, int height, AScrollView * asv);
        static inline void drawRectangle(int x, int y, QPainter * painter, QChar c, QColor lc, int width, int height, AScrollView * asv);
        static inline void drawCircle   (int x, int y, QPainter * painter, QChar c, QColor lc, int width, int height, AScrollView * asv);
        static inline void drawTriangle (int x, int y, QPainter * painter, QChar c, QColor lc, int width, int height, AScrollView * asv);
        static inline void drawRhomb    (int x, int y, QPainter * painter, QChar c, QColor lc, int width, int height, AScrollView * asv);

        /*static inline void drawShape(int x, int y, QPainter * painter, EventShape shape, int cube_side);
        static inline void drawRectangle(int x, int y, QPainter * painter, int cube_side);
        static inline void drawCircle(int x, int y, QPainter * painter, int cube_side);
        static inline void drawTriangle(int x, int y, QPainter * painter, int cube_side);
        static inline void drawRhomb(int x, int y, QPainter * painter, int cube_side);
        static inline void drawShape(int x,int y,QPainter* painter,EventShape shape,QChar c,QColor lc,int cube_side,AScrollView*);
        static inline void drawRectangle(int x,int y, QPainter* painter, QChar c, QColor lc, int cube_side,AScrollView*);
        static inline void drawCircle(int x,int y, QPainter* painter, QChar c, QColor lc, int cube_side,AScrollView*);
        static inline void drawTriangle(int x,int y, QPainter* p, QChar c, QColor lc, int cube_side,AScrollView* asv);
        static inline void drawRhomb(int x,int y, QPainter* painter, QChar c, QColor lc, int cube_side,AScrollView*);*/
};

// ------------------------------------------------------------------------------------------------
// -- No-letter routines
// ------------------------------------------------------------------------------------------------
inline void DrawingTools::drawShape(int x,int y, QPainter * painter, EventShape shape, int width, int height)
{
    switch (shape)
    {
        case EVENT_SHAPE_RECTANGLE:
            drawRectangle(x, y, painter, width, height);
            break;

        case EVENT_SHAPE_CIRCLE:
            drawCircle(x, y, painter, width, height);
            break;

        case EVENT_SHAPE_TRIANGLE:
            drawTriangle(x, y, painter, width, height);
            break;

        case EVENT_SHAPE_RHOMB:
            drawRhomb(x, y, painter, width, height);
            break;

        default:
            drawRectangle(x, y, painter, width, height);
    };
}

inline void 
DrawingTools::drawRectangle(int x, int y, QPainter * painter, int width, int height) 
{
    painter->drawRect(x, y, width, height);
} 

inline void 
DrawingTools::drawCircle(int x, int y, QPainter * painter, int width, int height) 
{
    painter->drawChord(x, y, width, height, 0, 16 * 360);
}

inline void 
DrawingTools::drawTriangle(int x, int y, QPainter * painter, int width, int height) 
{
    int cube_height = height - 1;
    int cube_width = width - 1;
    QPointArray parray(3);

    parray.setPoint(0, x, y + cube_height);
    parray.setPoint(1, x + (int) rint((double) cube_width / 2.0), y);
    parray.setPoint(2, x + cube_width, y + cube_height);
    painter->drawConvexPolygon(parray);
}

inline void 
DrawingTools::drawRhomb(int x, int y, QPainter * painter, int width, int height) 
{
    int cube_height = height - 1;
    int cube_width = width - 1;
    QPointArray parray(4);

    parray.setPoint(0, x, y + (int) rint((double) cube_height / 2.0));
    parray.setPoint(1, x + (int) rint((double) cube_width / 2.0), y);
    parray.setPoint(2, x + cube_width, y + (int) rint((double) cube_height / 2.0));
    parray.setPoint(3, x + (int) rint((double) cube_width / 2.0), y + cube_height);
    painter->drawConvexPolygon(parray);
}

// ------------------------------------------------------------------------------------------------
// -- letter routines
// ------------------------------------------------------------------------------------------------
inline void 
DrawingTools::drawShape(int x, int y, QPainter * painter, EventShape shape, QChar c, QColor lc, int width, int height, AScrollView * asv)
{
    switch (shape)
    {
        case EVENT_SHAPE_RECTANGLE:
            drawRectangle(x, y, painter, c, lc, width, height, asv);
            break;

        case EVENT_SHAPE_CIRCLE:
            drawCircle(x, y, painter, c, lc, width, height, asv);
            break;

        case EVENT_SHAPE_TRIANGLE:
            drawTriangle(x, y, painter, c, lc, width, height, asv);
            break;

        case EVENT_SHAPE_RHOMB:
            drawRhomb(x, y, painter, c, lc, width, height, asv);
            break;

        default:
            drawRectangle(x, y, painter, c, lc, width, height, asv);
    }
}

inline void 
DrawingTools::drawRectangle(int x, int y, QPainter * p, QChar c, QColor lc, int width, int height, AScrollView * asv)
{
    p->drawRect(x, y, width, height);

    // Paints the letter if necessary.
    if(!c.isSpace())
    {
        QPen ptmp = p->pen();
        p->setPen(lc);

        if(width >= height)
        {
            asv->drawPseudoScalledText(p, QString(c), Qt::AlignCenter, x + ((width - height) >> 1) + 1, y + 1, height - 2, height - 2);
        }
        else
        {
            asv->drawPseudoScalledText(p, QString(c), Qt::AlignCenter, x + 1, y + ((height - width) >> 1) + 1, width - 2, width - 2);
        }

        // Restore shape settings for pen (repainting methods relies on it).
        p->setPen(ptmp);       
    }
}

inline void 
DrawingTools::drawCircle(int x, int y, QPainter * p, QChar c, QColor lc, int width, int height, AScrollView * asv)
{
    p->drawChord(x, y, width, height, 0, 16 * 360);

    // Paints the letter if necessary.
    if(!c.isSpace())
    {
        QPen ptmp = p->pen();
        p->setPen(lc);

        // Compute inner square.
        int size = QMIN(width, height) - 2;
        int ax = x + ((width - size) >> 1);
        int ay = y + ((height - size) >> 1);

        // Draws the letter.
        asv->drawPseudoScalledText(p, QString(c), Qt::AlignHCenter | Qt::AlignVCenter, ax, ay, size, size);

        // Restore shape settings for pen (repainting methods relies on it).
        p->setPen(ptmp);       
    }
}

inline void 
DrawingTools::drawTriangle(int x, int y, QPainter * p, QChar c, QColor lc, int width, int height, AScrollView * asv)
{
    int cube_height = height - 1;
    int cube_width = width - 1;
    QPointArray parray(3);

    parray.setPoint(0, x, y + cube_height);
    parray.setPoint(1, x + (int) rint((double) cube_width / 2.0), y);
    parray.setPoint(2, x + cube_width, y + cube_height);
    p->drawConvexPolygon(parray);

    // Paints the letter if necessary.
    if(!c.isSpace())
    {
        QPen ptmp = p->pen();
        p->setPen(lc);

        // Compute inner square.
        double size = (double) (height * width) / (double) (height + width);
        int int_size = (int) rint(size);
        double ax = (double) x + (((double) width - size) / 2.0);
        double ay = (double) (y + height - 1) - size;

        asv->drawPseudoScalledText(p, QString(c), Qt::AlignHCenter | Qt::AlignVCenter, (int) rint(ax), (int) rint(ay), int_size, int_size);

        // Restore shape settings for pen (repainting methods relies on it).
        p->setPen(ptmp);
    }
}

inline void 
DrawingTools::drawRhomb(int x, int y, QPainter * p, QChar c, QColor lc, int width, int height, AScrollView * asv)
{
    int cube_height = height - 1;
    int cube_width = width - 1;
    QPointArray parray(4);

    parray.setPoint(0, x, y + (int) rint((double) cube_height / 2.0));
    parray.setPoint(1, x + (int) rint((double) cube_width / 2.0), y);
    parray.setPoint(2, x + cube_width, y + (int) rint((double) cube_height / 2.0));
    parray.setPoint(3, x + (int) rint((double) cube_width / 2.0), y + cube_height);
    p->drawConvexPolygon(parray);

    // Paints the letter if necessary.
    if(!c.isSpace())
    {
        QPen ptmp = p->pen();
        p->setPen(lc);

        // Compute inner square.
        double size = (double) (height * width) / (double) (height + width);
        int int_size = (int) rint(size);
        double ax = (double) x + (((double) width - size) / 2.0);
        double ay = (double) (y + height - 2) - size;

        asv->drawPseudoScalledText(p, QString(c), Qt::AlignHCenter | Qt::AlignVCenter, (int) rint(ax), (int) rint(ay), int_size, int_size);

        // Restore shape settings for pen (repainting methods relies on it).
        p->setPen(ptmp);
    }
}

/*
inline void 
DrawingTools::drawRectangle(int x, int y, QPainter * painter, int cube_side)
{
    painter->drawRect(x, y, cube_side, cube_side);
}

inline void 
DrawingTools::drawCircle(int x, int y, QPainter * painter, int cube_side)
{
    painter->drawChord(x, y, cube_side, cube_side, 0, 16 * 360);
}

inline void 
DrawingTools::drawTriangle(int x,int y, QPainter * painter, int cube_side)
{
    int cube_size = cube_side - 1;
    QPointArray parray(3);
    parray.setPoint(0, x, y + cube_size);
    parray.setPoint(1, x + (int) rint((double) cube_size / 2.0), y);
    parray.setPoint(2, x + cube_size, y + cube_size);
    painter->drawConvexPolygon (parray);
}

inline void 
DrawingTools::drawRhomb(int x, int y, QPainter * painter, int cube_side)
{
    int cube_size = cube_side - 1;
    QPointArray parray(4);
    parray.setPoint(0, x, y + (int) rint((double) cube_size / 2.0));
    parray.setPoint(1, x + (int) rint((double) cube_size / 2.0), y);
    parray.setPoint(2, x + cube_size, y + (int) rint((double) cube_size / 2.0));
    parray.setPoint(3, x + (int) rint((double) cube_size / 2.0), y + cube_size);
    painter->drawConvexPolygon (parray);
}

inline void 
DrawingTools::drawShape(int x, int y, QPainter * painter, EventShape shape, QChar c, QColor lc, int cube_side, AScrollView * asv)
{
    switch (shape)
    {
        case EVENT_SHAPE_RECTANGLE:
            drawRectangle(x, y, painter, c, lc, cube_side, asv);
            break;

        case EVENT_SHAPE_CIRCLE:
            drawCircle(x, y, painter, c, lc, cube_side, asv);
            break;

        case EVENT_SHAPE_TRIANGLE:
            drawTriangle(x, y, painter, c, lc, cube_side, asv);
            break;

        case EVENT_SHAPE_RHOMB:
            drawRhomb(x, y, painter, c, lc, cube_side, asv);
            break;

        default:
            drawRectangle(x, y, painter, c, lc, cube_side, asv);
    }
}

inline void 
DrawingTools::drawRectangle(int x, int y, QPainter * p, QChar c, QColor lc, int cube_side, AScrollView * asv)
{
    p->drawRect(x, y, cube_side, cube_side);

    // letter
    if (!c.isSpace())
    {
        QPen ptmp = p->pen();
        p->setPen(lc);
        asv->drawPseudoScalledText(p, QString(c),Qt::AlignHCenter|Qt::AlignVCenter, 
            x+1,y+1,cube_side-2,cube_side-2);
        // restore shape settings for pen (repainting methods relies on it)
        p->setPen(ptmp);       
    }
}

inline void 
DrawingTools::drawCircle(int x,int y, QPainter* p, QChar c, QColor lc, int cube_side, AScrollView* asv)
{
    p->drawChord(x,y,cube_side,cube_side,0,(16*360));
    // letter
    if (!c.isSpace())
    {
        QPen ptmp = p->pen();
        p->setPen(lc);
        
        // compute inner square
        double cs=(double)cube_side;
        // FIXME: optimize this operation pre-cumputing it per-cube_side
        // compute center
        double rad = cs/2.0;
        double xc = (double)x+rad;
        double yc = (double)y+rad;
        // compute center->left corner vector
        double vx = (double)x-xc;
        double vy = (double)y-yc;
        // normalize it
        double ln = sqrt(vx*vx + vy*vy);
        vx /= ln;
        vy /= ln;
        // compute intersection point B
        double bx= xc + rad*vx;
        double by= yc + rad*vy;
        //cout << "bx="<<bx<<" by="<<by<<" vx="<<vx<<" vy="<<vy<<"\n";
        // compute intersectin point C
        double cx= xc + rad*(-vx);
        int effw = (int)floor(cx-bx);         
        //cout << "cx="<<cx<<" vx="<<vx<<" vy="<<vy<<"effw="<<effw<<"\n";
        
        asv->drawPseudoScalledText(p, QString(c),Qt::AlignHCenter|Qt::AlignVCenter, 
            (int)rint(bx),(int)rint(by),effw,effw);
        
        // restore shape settings for pen (repainting methods relies on it)
        p->setPen(ptmp);       
    }
}

inline void 
DrawingTools::drawTriangle(int x,int y, QPainter* p, QChar c, QColor lc, int cube_side,AScrollView* asv)
{
    int cube_size = cube_side - 1;
    QPointArray parray(3);
    parray.setPoint(0,x,y+cube_size);
    parray.setPoint(1,x+(int)rint((double)cube_size/2.0),y);
    parray.setPoint(2,x+cube_size,y+cube_size);
    p->drawConvexPolygon (parray);
    
    // letter
    if (!c.isSpace())
    {
        QPen ptmp = p->pen();
        p->setPen(lc);
        
        // compute inner square
        // FIXME: optimize this operation pre-cumputing it per-cube_size
        double cs=(double)cube_size;
        // compute intersection point A 
        double ax= (double)x + cs/4.0;
        double ay= (double)y + cs/2.0;
        // compute intersectin point B
        double bx= (double)x + 3.0*cs/4.0;
        int effw = (int)ceil(bx-ax);         

        asv->drawPseudoScalledText(p, QString(c),Qt::AlignHCenter|Qt::AlignVCenter, 
            (int)rint(ax),(int)rint(ay),effw,effw);
        
        // restore shape settings for pen (repainting methods relies on it)
        p->setPen(ptmp);       
    }
}

inline void 
DrawingTools::drawRhomb(int x,int y, QPainter* p, QChar c, QColor lc, int cube_side,AScrollView* asv)
{
    int cube_size = cube_side - 1;
    double cs=(double)cube_size;
    double rad = cs/2.0;

    QPointArray parray(4);
    parray.setPoint(0,x,y+(int)rint((double)cube_size/2.0));
    parray.setPoint(1,x+(int)rint((double)cube_size/2.0),y);
    parray.setPoint(2,x+cube_size,y+(int)rint((double)cube_size/2.0));
    parray.setPoint(3,x+(int)rint((double)cube_size/2.0),y+cube_size);
    p->drawConvexPolygon (parray);

    // letter
    if (!c.isSpace())
    {
        QPen ptmp = p->pen();
        p->setPen(lc);
        
        // compute inner square
        // FIXME: optimize this operation pre-cumputing it per-cube_side
        // compute intersection point A 
        double ax= (double)x + cs/4.0;
        double ay= (double)y + cs/4.0;
        // compute intersectin point B
        double bx= (double)x + 3.0*cs/4.0;
        int effw = (int)ceil(bx-ax);         

        asv->drawPseudoScalledText(p, QString(c),Qt::AlignHCenter|Qt::AlignVCenter, 
            (int)rint(ax),(int)rint(ay),effw,effw);
        
        // restore shape settings for pen (repainting methods relies on it)
        p->setPen(ptmp);       
    }
}
*/

#endif

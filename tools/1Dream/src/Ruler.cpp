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

#include "Ruler.h"

#include <qpainter.h>
#include <qglobal.h>

#include <stdio.h>
#include <iostream>
#include "StripChartView.h"

using namespace std;

#define MARKER_WIDTH 11
#define MARKER_HEIGHT 6

#define RULER_SIZE 40

Ruler::Ruler (Orientation o, 
              QWidget *parent,
              const char *name) 
: QFrame (parent, name) {
     setFrameStyle (Box | Raised);
     setLineWidth (1);
     setMidLineWidth (0);
     orientation = o;
     zoom = 1;
     firstVisible = 0;
     buffer = 0L;
     currentPosition = -1;

     if (orientation == Horizontal)
     {
         setFixedHeight (RULER_SIZE);
         initMarker (MARKER_WIDTH, MARKER_HEIGHT);
     }
     else
     {
         setFixedWidth (RULER_SIZE);
         initMarker (MARKER_HEIGHT, MARKER_WIDTH);
     }
}

void Ruler::setStripChartView(StripChartView *vw)
{
     scv=vw;
}

void Ruler::initMarker (int w, int h) {
     QPainter p;
     QPointArray pts (3);
     static QCOORD hpoints [] = { 0,0, MARKER_WIDTH-1,0, 
                    (MARKER_WIDTH-1)/2,MARKER_HEIGHT-1 };
     static QCOORD vpoints [] = { 0,0, MARKER_HEIGHT-1,(MARKER_WIDTH-1)/2, 
                    0,MARKER_WIDTH-1 };

     if (orientation == Horizontal)
     {
         pts.putPoints (0, 3, hpoints);
     }    
     else
     {
         pts.putPoints (0, 3, vpoints);
     }  

     marker = new QPixmap (w, h);
     p.begin (marker);
     p.setPen (black);
     p.setBrush (black);
     p.setBackgroundColor (colorGroup ().background ());
     p.eraseRect (0, 0, w, h);
     p.drawPolygon (pts);
     p.end ();

     bg = new QPixmap (w, h);
     p.begin (bg);
     p.setBackgroundColor (colorGroup ().background ());
     p.eraseRect (0, 0, w, h);
     p.end ();
}

void Ruler::recalculateSize (QResizeEvent *) 
{
    if (buffer != 0L) 
    {
        delete buffer;
        buffer = 0L;
    }

    if (! isVisible ()) 
    {
        return;
    }  

    int w, h;
  
    int maxsize = (int)(1000.0 * zoom);

    if (orientation == Horizontal) 
    {
        w = QMAX(width (), maxsize);
        h = RULER_SIZE;
    }
    else 
    {
        w = RULER_SIZE;
        h = QMAX(height (), maxsize);
    }
  
    buffer = new QPixmap (w, h);
    drawRuler ();
    updatePointer (currentPosition, currentPosition);
}

void Ruler::setZoomFactor (float zf)
{
    zoom = zf;
    recalculateSize (0L);
    drawRuler ();
    updatePointer (currentPosition, currentPosition);
    repaint ();
}

void Ruler::updatePointer (int x, int y) 
{
    if (! buffer) {
        return;
    }
    QRect r1, r2;
    int pos = 0;

    if (orientation == Horizontal) 
    {
        if (currentPosition != -1) 
        {
            pos = qRound (currentPosition * zoom  
                - (firstVisible >= 0 ? 0 : firstVisible) 
                - MARKER_WIDTH / 2);
            r1 = QRect (pos - (firstVisible >= 0 ? firstVisible : 0), 
                1, MARKER_WIDTH, MARKER_HEIGHT);
            bitBlt (buffer, pos, 1, bg, 0, 0, MARKER_WIDTH, MARKER_HEIGHT);
        }
        if (x != -1) 
        {
            pos = qRound (x * zoom - (firstVisible >= 0 ? 0 : firstVisible) 
                - MARKER_WIDTH / 2);
            r2 = QRect (pos - (firstVisible >= 0 ? firstVisible : 0), 
                1, MARKER_WIDTH, MARKER_HEIGHT);
            bitBlt (buffer, pos, 1, marker, 0, 0, MARKER_WIDTH, MARKER_HEIGHT);
            currentPosition = x;
        }
    }
    else 
    {
        if (currentPosition != -1) 
        {
            pos = qRound (currentPosition * zoom
                - (firstVisible >= 0 ? 0 : firstVisible) 
                - MARKER_HEIGHT / 2);
            r1 = QRect (1, pos - (firstVisible >= 0 ? firstVisible : 0), 
                MARKER_HEIGHT, MARKER_WIDTH);
            bitBlt (buffer, 1, pos, bg, 0, 0, MARKER_HEIGHT, MARKER_WIDTH);
        }
        if (y != -1) 
        {
            pos = qRound (y * zoom - (firstVisible >= 0 ? 0 : firstVisible) 
                - MARKER_HEIGHT / 2);
            r2 = QRect (1, pos -  (firstVisible >= 0 ? firstVisible : 0), 
                MARKER_HEIGHT, MARKER_WIDTH);
            bitBlt (buffer, 1, pos, marker, 0, 0, MARKER_HEIGHT, MARKER_WIDTH);
            currentPosition = y;
        }
    }
    repaint (r1.unite (r2));
}

void Ruler::updateVisibleArea (int xpos, int ypos) 
{
    if (orientation == Horizontal)
    {
        firstVisible = xpos;
    }    
    else
    {
        firstVisible = ypos;
    }
        
    drawRuler ();
    repaint ();
}

void Ruler::paintEvent (QPaintEvent *e) 
{
    if (! buffer)
        return;

    const QRect& rect = e->rect ();

    if (orientation == Horizontal) 
    {
        if (firstVisible >= 0) 
        {
            bitBlt (this, rect.x (), rect.y (), buffer,
                    rect.x () , rect.y (),
                    rect.width (), rect.height ());
        } 
        else 
        {
            bitBlt (this, rect.x (), rect.y (), buffer,
                    rect.x (), rect.y (),
                    rect.width (), rect.height ());
        }
    }
    else 
    {
        if (firstVisible >= 0)
        {
            bitBlt (this, rect.x (), rect.y (), buffer,
                    rect.x (), rect.y (),
                    rect.width (), rect.height ());
        }
        else
        {
            bitBlt (this, rect.x (), rect.y (), buffer,
                    rect.x (), rect.y (),
                    rect.width (), rect.height ());
        }    
    }
    QFrame::paintEvent (e);
}

void Ruler::drawRuler (int dummy) 
{
    QPainter p;
    char buf[10];
    int step = 0, step1 = 0, step2 = 0, start = 0, ioff = 0;
    float iniStep=1.0;
    int decs=0;
    int val,q,i;
    int x0,x1,y0,y1;
    int line;

    float ContentsX,ContentsY,ContentsH,ContentsW;
    float stepsEvery;
    int items,tmp;

    int initialPosition, discreteSteps;
    int partInt, partDec;
    int pixels;
    int pixels_step_0,step_1,step_2,step_3,step_4;
    int counterPass;
    int iNext;
    float initVal, endVal;
    float actualVal;

    float fPosition;

    if (! buffer)
    {
        return;
    }  

    update();

    p.begin (buffer);
    p.setBackgroundColor (colorGroup ().background ());
    p.setPen (black);
    p.setFont (QFont ("helvetica", 8));
    buffer->fill (backgroundColor ());
    p.eraseRect (0, 0, width (), height ());

    if (orientation == Horizontal) 
    {
        ContentsX=((float)scv->contentsX())/zoom;
        ContentsW=((float)scv->visibleWidth())/zoom;

        initVal=ContentsX;
        endVal=ContentsX+ContentsW;

        if((scv->width()-scv->visibleWidth())<scv->verticalScrollBar()->width()){
            pixels=width();
        } else {
            pixels=width()-(scv->verticalScrollBar()->width());
        }

        pixels_step_0=pixels/16;
 
        if(pixels>0) 
        {
 
            step_1=pixels_step_0/5;
            step_2=pixels_step_0*2/5;
            step_3=pixels_step_0*3/5;
            step_4=pixels_step_0*4/5;

            counterPass=0;

            // Print the first Number
 
            p.drawText (3,10, QString::number((int)initVal)+QString("k"));

            for(i=0;i<pixels;) 
            {
                counterPass++;
                iNext=pixels*counterPass/16;
                p.drawLine(i+pixels_step_0,10,i+pixels_step_0,RULER_SIZE);

                actualVal=((endVal-initVal)*((float)(16-counterPass))/16.0);
                actualVal=endVal-actualVal;
                p.drawText (i+pixels_step_0+3,10 , QString::number((int)actualVal)+QString("k"));
                p.drawLine(i+step_1,20,i+step_1,RULER_SIZE);
                p.drawLine(i+step_2,20,i+step_2,RULER_SIZE);
                p.drawLine(i+step_3,20,i+step_3,RULER_SIZE);
                p.drawLine(i+step_4,20,i+step_4,RULER_SIZE);
                i=iNext;
            }
        }
    } 
    else 
    {
        ContentsY=((float)(scv->contentsHeight()-scv->contentsY()))/(zoom*1000.0);
        ContentsH=((float)scv->visibleHeight())/(zoom*1000.0);

        initVal=ContentsY-ContentsH;
        endVal=ContentsY;

        if((scv->height()-scv->visibleHeight())<scv->horizontalScrollBar()->height())
        {
            pixels=height();
        } 
        else 
        {
            pixels=height()-(scv->horizontalScrollBar()->height());
        }
        
        pixels_step_0=pixels/10;

        if(pixels_step_0>75) 
        {
            step_1=pixels_step_0/5;
            step_2=pixels_step_0*2/5;
            step_3=pixels_step_0*3/5;
            step_4=pixels_step_0*4/5;

            counterPass=0;

            // Print the first Number
 
            p.drawText (10, 9 , QString::number(endVal,'f',2));

            for(i=0;i<pixels;) 
            {
                counterPass++;
                iNext=pixels*counterPass/10;
                p.drawLine(10,i+pixels_step_0,RULER_SIZE,i+pixels_step_0);

                actualVal=((endVal-initVal)*((float)counterPass)/10.0);
                actualVal=endVal-actualVal;

                p.drawText (10, i+pixels_step_0+9 , QString::number(actualVal,'f',2));
                p.drawLine(20,i+step_1,RULER_SIZE,i+step_1);
                p.drawLine(20,i+step_2,RULER_SIZE,i+step_2);
                p.drawLine(20,i+step_3,RULER_SIZE,i+step_3);
                p.drawLine(20,i+step_4,RULER_SIZE,i+step_4);
                i=iNext;
            }
        } 
        else if ( pixels_step_0>30) 
        {
            pixels_step_0=pixels/5;
            step_1=pixels_step_0/3;
            step_2=pixels_step_0*2/3;

            counterPass=0;

            // Print the first Number
 
            p.drawText (10, 9 , QString::number(endVal,'f',1));

            for(i=0;i<pixels;) 
            {
                counterPass++;
                iNext=pixels*counterPass/5;
                p.drawLine(10,i+pixels_step_0,RULER_SIZE,i+pixels_step_0);

                actualVal=((endVal-initVal)*((float)counterPass)/5.0);
                actualVal=endVal-actualVal;
                p.drawText (10, i+pixels_step_0+9 , QString::number(actualVal,'f',2));
                p.drawLine(20,i+step_1,RULER_SIZE,i+step_1);
                p.drawLine(20,i+step_2,RULER_SIZE,i+step_2);
                i=iNext;
            }
        }
        else 
        {
            pixels_step_0=pixels/3;
            counterPass=0;

            // Print the first Number
 
            p.drawText (10, 9 , QString::number(endVal,'f',1));

            for(i=0;i<pixels;) 
            {
                counterPass++;
                iNext=pixels*counterPass/3;
                p.drawLine(10,i+pixels_step_0,RULER_SIZE,i+pixels_step_0);

                actualVal=((endVal-initVal)*((float)counterPass)/3.0);
                actualVal=endVal-actualVal;
                p.drawText (10, i+pixels_step_0+9 , QString::number(actualVal,'f',2));
                i=iNext;
            }
        }
    }

    p.end ();
    return;
}

void Ruler::resizeEvent (QResizeEvent *e) 
{
  recalculateSize (e);
}

void Ruler::show () 
{
  if (orientation == Horizontal) 
  {
    setFixedHeight (RULER_SIZE);
    initMarker (MARKER_WIDTH, MARKER_HEIGHT);
  }
  else 
  {
    setFixedWidth (RULER_SIZE);
    initMarker (MARKER_HEIGHT, MARKER_WIDTH);
  }
  QWidget::show ();
}

void Ruler::hide () 
{
  if (orientation == Horizontal)
  {
    setFixedHeight (1);
  }  
  else
  {
    setFixedWidth (1);
  }
    
  QWidget::hide ();
}

void Ruler::mousePressEvent ( QMouseEvent * )
{
  isMousePressed = true;
}

void Ruler::mouseMoveEvent ( QMouseEvent * me){
   if (isMousePressed) 
   {
     emit drawHelpline (me->x () + 
               (orientation == Horizontal ? firstVisible : 0) -
               RULER_SIZE, 
               me->y () +
               (orientation == Vertical ? firstVisible : 0) -
               RULER_SIZE, 
               (orientation==Horizontal) ? true : false );
   }
}

void Ruler::mouseReleaseEvent ( QMouseEvent * me)
{
  if (isMousePressed) 
  {
     isMousePressed = false;
     emit addHelpline (me->x () + 
               (orientation == Horizontal ? firstVisible : 0) -
               RULER_SIZE, 
               me->y () +
               (orientation == Vertical ? firstVisible : 0) -
               RULER_SIZE, 
               (orientation==Horizontal) ? true : false );
  }
}

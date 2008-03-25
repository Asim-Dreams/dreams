/*
 * Copyright (C) 2005-2006 Intel Corporation
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
  * @file JoystickWidget.cpp
  */

// General includes.
#include <math.h>
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "JoystickWidget.h"

// Qt includes.
#include <qpainter.h>

#define JOYSTICK_WIDGET_MAIN_FRAME 0
#define JOYSTICK_WIDGET_STICK_SIZE 10.0

JoystickWidget::JoystickWidget(QWidget *parent, bool repeat) : QWidget(parent)
{
    SetStick(0,0);
    repeatMovement=repeat;
    if (repeat)
    {
        repTimer = new QTimer(this,"joystick_timer");
        connect(repTimer,SIGNAL(timeout()),this,SLOT(timerTick()));
    }
    dragged = false;
}

void 
JoystickWidget::SetStick(double _x,double _y)
{
    if (x==_x && y==_y)
    { return; }

    x=_x;
    y=_y;

    emit Moved(x,y);
    repaint();
}

void 
JoystickWidget::paintEvent (QPaintEvent* pe)
{
    QPainter p(this);
    double y_null=height()/2;
    double x_null=width()/2;

    double y_fac=2.0/(height()-2*JOYSTICK_WIDGET_MAIN_FRAME-JOYSTICK_WIDGET_STICK_SIZE);
    double x_fac=2.0/(width()-2*JOYSTICK_WIDGET_MAIN_FRAME-JOYSTICK_WIDGET_STICK_SIZE);

    double b_x=x/x_fac+x_null;
    double b_y=-y/y_fac+y_null;


    p.drawLine(JOYSTICK_WIDGET_MAIN_FRAME,height()/2,width()-JOYSTICK_WIDGET_MAIN_FRAME,height()/2);
    p.drawLine(width()/2,JOYSTICK_WIDGET_MAIN_FRAME,width()/2,height()-JOYSTICK_WIDGET_MAIN_FRAME);

    p.drawEllipse((int)(b_x-JOYSTICK_WIDGET_STICK_SIZE/2),
            (int)(b_y-JOYSTICK_WIDGET_STICK_SIZE/2),
            (int)JOYSTICK_WIDGET_STICK_SIZE,
            (int)JOYSTICK_WIDGET_STICK_SIZE);
}

void 
JoystickWidget::mousePressEvent(QMouseEvent *e)
{
    mouseMoveEvent(e);
    dragged=true;
}

void 
JoystickWidget::mouseMoveEvent(QMouseEvent *e)
{
    if ( e->state() | LeftButton)
    {
        double y_null=height()/2;
        double x_null=width()/2;

        double y_fac=2.0/(height()-2*JOYSTICK_WIDGET_MAIN_FRAME-JOYSTICK_WIDGET_STICK_SIZE);
        double x_fac=2.0/(width()-2*JOYSTICK_WIDGET_MAIN_FRAME-JOYSTICK_WIDGET_STICK_SIZE);

        double b_x=e->x();
        double b_y=e->y();

        if (b_x<JOYSTICK_WIDGET_STICK_SIZE/2+JOYSTICK_WIDGET_MAIN_FRAME) 
        { b_x=JOYSTICK_WIDGET_STICK_SIZE/2+JOYSTICK_WIDGET_MAIN_FRAME; }
        
        if (b_y<JOYSTICK_WIDGET_STICK_SIZE/2+JOYSTICK_WIDGET_MAIN_FRAME) 
        { b_y=JOYSTICK_WIDGET_STICK_SIZE/2+JOYSTICK_WIDGET_MAIN_FRAME; }
        
        if (b_x>width()-JOYSTICK_WIDGET_STICK_SIZE/2-JOYSTICK_WIDGET_MAIN_FRAME) 
        { b_x=width()-JOYSTICK_WIDGET_STICK_SIZE/2-JOYSTICK_WIDGET_MAIN_FRAME; }
        
        if (b_y>height()-JOYSTICK_WIDGET_STICK_SIZE/2-JOYSTICK_WIDGET_MAIN_FRAME) 
        { b_y=height()-JOYSTICK_WIDGET_STICK_SIZE/2-JOYSTICK_WIDGET_MAIN_FRAME; }

        y=-(b_y-y_null)*y_fac;
        x=(b_x-x_null)*x_fac;
        repaint();
        emit Moved(x,y);
        if (repeatMovement)
        {
            int ticks=1000;
            ticks -= static_cast<int>(QMAX(fabs(x),fabs(y))*1000.0);
            if (ticks<10) ticks = 10;

            //cerr << "\t jx " << x << " jy " << y << " ticks " << ticks << endl;
            repTimer->start(ticks);
        }
    }
}

void 
JoystickWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (repeatMovement) { repTimer->stop(); }
    dragged=false;
    if ( e->button()==LeftButton )
    { SetStick(0,0); }
}

void
JoystickWidget::timerTick()
{
    if (!dragged) { return; }
    emit Moved(x,y);
}


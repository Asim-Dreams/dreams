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
  * @file JoystickDockWin.cpp
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "JoystickDockWin.h"

// Qt includes.
#include <qstrlist.h>
#include <qmainwindow.h>
#include <qtextedit.h>

JoystickDockWin::JoystickDockWin( QMainWindow* mw)
{
    setResizeEnabled(TRUE);
    setCloseMode(QDockWindow::Always);
    setMovingEnabled(FALSE);
    setCaption("Joystick Window");

    joystick = new JoystickWidget(this,true);
    Q_ASSERT(joystick!=NULL);
    setWidget(joystick);
    mw->moveDockWindow (this,Qt::DockRight,true,999); 

    connect (joystick,SIGNAL(Moved(double,double)),this,SLOT(joystickMoved(double,double)));

    show();
}

JoystickDockWin::~JoystickDockWin()
{
    if (joystick!=NULL) { delete joystick; }
}

/*
void
JoystickDockWin::resizeEvent ( QResizeEvent * e )
{
}
*/

void
JoystickDockWin::joystickMoved( double x, double y )
{
    //cerr << "JoystickDockWin::joystickMoved x " << x << " y " << y << endl;
    emit JoystickMoved(x,y);
}

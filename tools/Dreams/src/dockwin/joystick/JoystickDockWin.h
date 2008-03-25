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
  * @file JoystickDockWin.h
  */

#ifndef _JOYSTICKDOCKWIN_H_
#define _JOYSTICKDOCKWIN_H_

#include "DreamsDefs.h"
#include <qlistview.h>
#include <qdockwindow.h>
#include "JoystickWidget.h"

extern bool dockInit;

/**
  * Docked window to display a simple joystick GUI 
  *
  * @version 0.1
  * @date started at 2004-11-19
  * @author Federico Ardanaz
  */
class JoystickDockWin : public QDockWindow
{
    Q_OBJECT
    public:
        JoystickDockWin(QMainWindow*);
        ~JoystickDockWin();

    public slots:
        void joystickMoved(double,double);

    signals:
        void JoystickMoved(double,double);
    
    protected:
        //void resizeEvent ( QResizeEvent * e );

    private:
        JoystickWidget* joystick;
};

#endif


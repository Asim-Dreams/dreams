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
  * @file  MDI2DWindow.h
  * @brief 
  */


#ifndef _MDI2DWINDOW_H
#define _MDI2DWINDOW_H

#include <qmainwindow.h>
#include "asim/MDIWindow.h"

class MDI2DWindow: public MDIWindow
{
Q_OBJECT
public:
    MDI2DWindow( QMainWindow* m, QWidget* parent, const char* name, int wflags );
    ~MDI2DWindow();
    virtual bool init(void);

    inline bool getVHIGH();
    inline bool getVNHIGH();
    inline bool getVBHIGH();

    inline void setVHIGH(bool);
    inline void setVNHIGH(bool);
    inline void setVBHIGH(bool);

signals:
    void mdiCloseEvent(MDIWindow*);
    
protected:
    void closeEvent( QCloseEvent * );
    
protected:
    QMainWindow* mainWin;
    QWidget* myParent;

    bool VHIGH;
    bool VNHIGH;
    bool VBHIGH;
};

bool 
MDI2DWindow::getVHIGH()
{ return VHIGH; }

bool 
MDI2DWindow::getVNHIGH()
{ return VNHIGH;     }

bool 
MDI2DWindow::getVBHIGH()
{ return VBHIGH;     }


void 
MDI2DWindow::setVHIGH(bool value)
{
    VHIGH=value;
    VNHIGH=!value;
    VBHIGH=!value;
}

void 
MDI2DWindow::setVNHIGH(bool value)
{
    VHIGH=!value;
    VNHIGH=value;
    VBHIGH=!value;
}

void 
MDI2DWindow::setVBHIGH(bool value)
{
    VHIGH=!value;
    VNHIGH=!value;
    VBHIGH=value;
}

#endif


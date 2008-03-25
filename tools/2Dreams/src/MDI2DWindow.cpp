/*
 * Copyright (C) 2004-2006 Intel Corporation
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

#include "MDI2DWindow.h"
#include "TwoDAvtView.h"

MDI2DWindow::MDI2DWindow( QMainWindow* m, QWidget* parent, const char* name, int wflags ) : MDIWindow(parent,name,wflags)
{
    myParent=parent;
    mainWin=m;
    
    VHIGH=false;
    VNHIGH=false;
    VBHIGH=true;
    setOpaqueMoving(false);
}

MDI2DWindow::~MDI2DWindow()
{
    // MDIWindow deltes asv!
}

bool
MDI2DWindow::init (void)
{
    asv = new TwoDAvtView(this,mainWin);
    return MDIWindow::init();
}

void 
MDI2DWindow::closeEvent(QCloseEvent *e)
{
    emit mdiCloseEvent(this);
    e->accept();
}



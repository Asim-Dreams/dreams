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

/*
 * @file  MDIResourceWindow.h
 * @brief 
 */

#ifndef _MDIRESOURCEWINDOW_H
#define _MDIRESOURCEWINDOW_H

// Dreams includes.
#include "avt/ResourceDAvtView.h"
#include "MDIDreamsWindow.h"
#include "DreamsDefs.h"

// Qt includes.
#include <qmainwindow.h>

// Class forwarding.
class MDIResourceWindowCombo;

class MDIResourceWindow : public MDIDreamsWindow
{
    Q_OBJECT

  public:
    MDIResourceWindow(DreamsDB * dreamsdb, QMainWindow * m, QWidget * parent, const char * name, int wflags);
    ~MDIResourceWindow();
    bool init();

    bool getMDILocked(void) { return asv->getMDILocked(); }
    bool getIsClocked(void) { return false; }

    inline void showCycle(CYCLE cycle);
    inline void showItem(QString edgeName, QString fromNodeName, QString toNodeName, QString tagName, UINT64 tagValue);

    // View synch methods
    void attendCycleChanged(INT64 ps);
    void attendResourceChanged(double resource);

  signals:
    void mdiCloseEvent(MDIWindow*);
 
  protected:
    void closeEvent( QCloseEvent * );
    
  protected:
    QMainWindow* mainWin;
    QWidget* myParent;
    MDIResourceWindowCombo * combo;

    UINT32 nFPS;
    UINT32 nCPF;

    // Direction of the time
    // If 1, forward, if -1 backward
    int nextCycle;

    // Indicates if we are playng at normat rate (play) or fast (rewind/forward)
    UINT32 fastStep;
    double ratio; ///< Ratio used to convert LCM cycles to the smallest Dreams cycle.
} ;

void 
MDIResourceWindow::showCycle(CYCLE cycle)
{
    static_cast<ResourceDAvtView *>(getAScrollView())->showCycle(cycle);
}

void
MDIResourceWindow::showItem(QString edgeName, QString fromNodeName, QString toNodeName, QString tagName, UINT64 tagValue)
{
    static_cast<ResourceDAvtView *>(getAScrollView())->showItem(edgeName, fromNodeName, toNodeName, tagName, tagValue);
}

#endif

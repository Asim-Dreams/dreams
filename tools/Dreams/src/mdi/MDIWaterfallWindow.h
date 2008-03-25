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
  * @file  MDIWaterfallWindow.h
  * MDI Support for Waterfall view
  */


#ifndef _MDIWATERFALLWINDOW_H
#define _MDIWATERFALLWINDOW_H

// Dreams includes.
#include "dockwin/joystick/JoystickDockWin.h"
#include "mdi/MDIDreamsWindow.h"
#include "avt/WaterfallDAvtView.h"
#include "DreamsDefs.h"
#include "ItemWFCombo.h"

#include <qtabwidget.h>

// Class forwarding.
class QSlider;
class QTimer;
class QVBox;
class QMainWindow;
class DreamsDB;

/**
  * @file  MDIWaterfallWindow.h
  * @brief Waterfall MDI
  * @author Federico Ardanaz
  */
class MDIWaterfallWindow : public MDIDreamsWindow
{
  Q_OBJECT
  public:
    MDIWaterfallWindow(DreamsDB * dreamsdb, QMainWindow* m, QWidget* parent, const char* name, int wflags);
    ~MDIWaterfallWindow();
 
    bool init();
    virtual inline AScrollView* getAScrollView();
    vector<AScrollView *> getListAScrollView() const;
    
    // TODO
    // Implement the synch mechanism for WaterFall
    bool getMDILocked(void) { return MDILocked; }
    bool getIsClocked(void) { return false; }

    void switchMDILocked();

    inline void attendCycleChanged(INT64 ps);
    void attendCycleChanged(INT64 ps, AScrollView * asv);
    inline void attendItemChanged(double item);
    void attendItemChanged(double item, AScrollView * asv);
    inline void showCycle(CYCLE cycle);

  protected:
    bool createTabs();

  public slots:
    void tabChanged(QWidget*);
    void JoystickMoved(double,double);

  signals:
    void mdiCloseEvent(MDIWindow*);

  protected:
    void closeEvent( QCloseEvent * );

  protected:
    QMainWindow*        mainWin;
    QWidget*            myParent;
    QTabWidget *        tabWidget;
    QWidget*            currentTab;
    JoystickDockWin*    joyDock;    

    double ratio;
    bool MDILocked;
};

AScrollView* 
MDIWaterfallWindow::getAScrollView()
{
    ItemWFCombo* combo = (ItemWFCombo*)currentTab;
    Q_ASSERT(combo); 
    return combo->getDAvtView();
}

void 
MDIWaterfallWindow::showCycle(CYCLE cycle)
{
    static_cast<WaterfallDAvtView *>(getAScrollView())->showCycle(cycle);
}

void
MDIWaterfallWindow::attendCycleChanged(INT64 ps)
{
    attendCycleChanged(ps, NULL);
}

void
MDIWaterfallWindow::attendItemChanged(double item)
{
    attendItemChanged(item, NULL);
}
#endif

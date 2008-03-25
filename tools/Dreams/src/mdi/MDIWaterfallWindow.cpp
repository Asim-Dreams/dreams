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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "dockwin/tag/TagDockWin.h"
#include "mdi/MDIWaterfallWindow.h"
#include "avt/WaterfallDAvtView.h"
#include "ItemWFCombo.h"
#include "DreamsDB.h"

// Qt includes.
#include <qslider.h>
#include <qtimer.h>
#include <qvbox.h>
#include <qmainwindow.h>

MDIWaterfallWindow::MDIWaterfallWindow(DreamsDB * dreamsdb, QMainWindow * m, QWidget * parent, const char * name, int wflags)
    : MDIDreamsWindow(dreamsdb, parent, name, wflags)
{
    myParent=parent;
    mainWin=m;

    VHIGH=false;
    VNHIGH=false;
    VBHIGH=true;
    setOpaqueMoving(false);

    tabWidget = new QTabWidget(this);
    Q_ASSERT(tabWidget!=NULL);
    connect (tabWidget,SIGNAL(currentChanged(QWidget*)),this,SLOT(tabChanged(QWidget*)));

    // add joystick docked
    joyDock = new JoystickDockWin(this);    
    connect (joyDock,SIGNAL(JoystickMoved(double,double)),this,SLOT(JoystickMoved(double,double)));

    currentTab=NULL;

    MDILocked = true;
}

MDIWaterfallWindow::~MDIWaterfallWindow()
{
    //delete pqvbLayout;
}

bool
MDIWaterfallWindow::init()
{
    setCentralWidget(tabWidget);
    if(!createTabs())
    {
        return false;
    }
    // WF can have tagWindow...
    createTagWindow();
    connect( tagWin, SIGNAL(visibilityChanged(bool))         , mainWin, SLOT(tagWinVisibilityChanged(bool)));
    connect( tagWin, SIGNAL(placeChanged(QDockWindow::Place)), mainWin, SLOT(tagWinPlaceChanged(QDockWindow::Place)));
    connect( tagWin, SIGNAL(orientationChanged(Orientation)) , mainWin, SLOT(tagWinOrientationChanged(Orientation)));
    if(!MDIWindow::init())
    {
        return false;
    }
    return true;
}

bool
MDIWaterfallWindow::createTabs()
{
    //cerr << "MDIWaterfallWindow::createTabs called ..." << endl;
    QApplication::setOverrideCursor( Qt::WaitCursor );

    // check for non split by rules in DreamsDB
    ItemTabList* itablist = dreamsdb->getItemTabList();
    ItemTab* itab = itablist->first();
    int cnt=0;
    if (itablist->count()<1) 
    {
        //cerr << "no tabs to create..." << endl;
        return false;
    }

    ItemWFCombo* firstTab=NULL;
    ItemTab* firstITab=NULL;

    while(itab != NULL)
    {
        //cerr << "MDIWaterfallWindow::createTabs creating combo " << cnt << endl;
        ItemWFCombo* tab = new ItemWFCombo(dreamsdb, mainWin, this, itab);
        tabWidget->addTab(tab,itab->getTabSpec()->getName());
        if (cnt==0)
        {
            firstTab=tab; 
            firstITab=itab;
        }

        currentTab=tab;
        dreamsdb->layoutWaterfallSetCurrentItemTab(itab);
        //tab->getDAvtView()->setRealContentsSize((int)(lwf->getWidth()), (int)(lwf->getHeight()));
        //cerr << "MDIWaterfallWindow::size recompute..."<< endl;
        tab->recomputeSize();

        itab = itablist->next();
        ++cnt;
    }
    currentTab=firstTab;
    dreamsdb->layoutWaterfallSetCurrentItemTab(firstITab);

    // Gets the ratio.
    ratio = (double) QMAX(1, ((WaterfallDAvtView *) getAScrollView())->getInternalBaseElementSize()) / (double) currentCycle.clock->getLCMRatio();

    // If no divisions used, then the ratio is divided by 2.
    if(!dreamsdb->layoutWaterfallGetUsesDivisions())
    {
        ratio /= 2;
    }

    QApplication::restoreOverrideCursor();
    //cerr << "MDIWaterfallWindow::createTabs finished" << endl;
    return true;
}

void 
MDIWaterfallWindow::closeEvent(QCloseEvent *e)
{
    emit mdiCloseEvent(this);
    e->accept();
}

void
MDIWaterfallWindow::tabChanged(QWidget* widget)
{ 
    //cerr << "MDIWaterfallWindow::tabChanged called" << endl;
    currentTab = widget; 

    ItemWFCombo* combo = (ItemWFCombo*)currentTab;
    Q_ASSERT(combo); 
    
    dreamsdb->layoutWaterfallSetCurrentItemTab(combo->getItemTabWidget()->getItemTab());
}
    
void 
MDIWaterfallWindow::JoystickMoved(double x, double y)
{
    if (currentTab!=NULL)
    {
        ItemWFCombo* combo = (ItemWFCombo*)currentTab;
        combo->applyJoystickMovement(x,y);
    }
}

void
MDIWaterfallWindow::attendCycleChanged(INT64 ps, AScrollView * asv)
{
    if(!MDILocked)
    {
        return;
    }

    // XXX 
    // This is a temporarly hack and should be revisited when 
    // implementing a real synch mechanism
    //
    // When we are attending the signal we shouldn't generate more
    // events in order to avoid an infinite loop
    bool oldMdiLocked = MDILocked;
    MDILocked = false;

    // Adds the cycle accumulation.
    displacement += ps;
    CYCLE dispCycle = getAccCycleChanged(currentCycle.clock, (currentCycle.clock->getDivisions() > 1));

    // As the lcm cycles are returned using divisions, if no divisions
    // are used, then the cycles must be divided by two.
    INT64 cycles = dispCycle.toLCMCycles();

    // Translates the matrix of all the tabs.
    for(INT32 i = 0; i < tabWidget->count(); i++)
    {
        ItemWFCombo * combo = (ItemWFCombo *) tabWidget->page(i);
        DAvtView * avt = combo->getDAvtView();

        // Only moves the avtview that are different from the avt that
        // is sending the movement.
        if(avt != asv)
        {
            avt->scrollByMatrix((double) cycles * ratio, 0);
        }
    }

    MDILocked = oldMdiLocked;
}

void
MDIWaterfallWindow::attendItemChanged(double item, AScrollView * asv)
{
    if(!MDILocked)
    {
        return;
    }

    // XXX 
    // This is a temporarly hack and should be revisited when 
    // implementing a real synch mechanism
    //
    // When we are attending the signal we shouldn't generate more
    // events in order to avoid an infinite loop
    bool oldMdiLocked = MDILocked;
    MDILocked = false;

    // Translates the matrix of all the tabs.
    for(INT32 i = 0; i < tabWidget->count(); i++)
    {
        ItemWFCombo * combo = (ItemWFCombo *) tabWidget->page(i);
        if(combo->getDAvtView() != asv)
        {
            combo->getDAvtView()->scrollByMatrix(0.0, item);
        }
    }

    MDILocked = oldMdiLocked;
}

vector<AScrollView *>
MDIWaterfallWindow::getListAScrollView() const
{
    vector<AScrollView *> ret;

    // Translates the matrix of all the tabs.
    for(INT32 i = 0; i < tabWidget->count(); i++)
    {
        ItemWFCombo * combo = (ItemWFCombo *) tabWidget->page(i);
        ret.push_back(combo->getDAvtView());
    }

    return ret;
}

void
MDIWaterfallWindow::switchMDILocked()
{
    MDILocked = !MDILocked;

    // Updates the state of all the tabs.
    for(INT32 i = 0; i < tabWidget->count(); i++)
    {
        ItemWFCombo * combo = (ItemWFCombo *) tabWidget->page(i);
        ((WaterfallDAvtView *) combo->getDAvtView())->switchRebel(MDILocked);
    }
}

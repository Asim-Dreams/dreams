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
#include "dockwin/tag/TagDockWin.h"
#include "mdi/MDIResourceWindowCombo.h"
#include "mdi/MDIResourceWindow.h"

/*
 * Sets the initial values and forwards the creation parameters to
 * its super class.
 *
 * @return the new object.
 */
MDIResourceWindow::MDIResourceWindow(DreamsDB * dreamsdb, QMainWindow * m, QWidget * parent, const char * name, int wflags)
    : MDIDreamsWindow(dreamsdb, parent, name, wflags)
{
    // Gets the parent and main window.
    myParent = parent;
    mainWin = m;
    combo = NULL;

    VHIGH = false;
    VNHIGH = false;
    VBHIGH = true;

    setOpaqueMoving(false);

    ratio = 0.0;
}

/*
 * Destructor of the class. Nothing to be done.
 *
 * @return destroys the object.
 */
MDIResourceWindow::~MDIResourceWindow()
{
    // MDIWindow deltes asv!
}

bool
MDIResourceWindow::init()
{
    combo = new MDIResourceWindowCombo(dreamsdb, mainWin, this);
    asv = combo->getAScrollView();
    if(!MDIWindow::init())
    {
        return false;
    }

    // WARNING: keep this line after MDIWindow::init as
    // we want combo as the central widget and the init
    // function sets the asv as the central one.
    setCentralWidget(combo);

    // resource can have tagWindow...
    createTagWindow();
    connect(tagWin, SIGNAL(visibilityChanged(bool))         ,mainWin, SLOT(tagWinVisibilityChanged(bool)));
    connect(tagWin, SIGNAL(placeChanged(QDockWindow::Place)),mainWin, SLOT(tagWinPlaceChanged(QDockWindow::Place)));
    connect(tagWin, SIGNAL(orientationChanged(Orientation)) ,mainWin, SLOT(tagWinOrientationChanged(Orientation)));

    // Gets the ratio.
    ratio = (double) QMAX(1, ((ResourceDAvtView *) asv)->getInternalBaseElementSize()) / (double) currentCycle.clock->getLCMRatio();
    return true;
}

void 
MDIResourceWindow::closeEvent(QCloseEvent *e)
{
    emit mdiCloseEvent(this);
    e->accept();
}

void 
MDIResourceWindow::attendCycleChanged(INT64 ps)
{
    if(!asv->getMDILocked())
    {
        return;
    }

    // XXX 
    // This is a temporarly hack and should be revisited when 
    // implementing a real synch mechanism
    //
    // When we are attending the signal we shouldn't generate more
    // events in order to avoid an infinite loop
    bool oldMdiLocked = asv->getMDILocked();
    asv->setMDILocked(false);

    // Adds the cycle accumulation.
    displacement += ps;
    CYCLE dispCycle = getAccCycleChanged(currentCycle.clock, dreamsdb->layoutResourceGetUsesDivisions());

    // As the lcm cycles are returned using divisions, if no divisions
    // are used, then the cycles must be divided by two.
    INT64 cycles = dispCycle.toLCMCycles();

    if(!dreamsdb->layoutResourceGetUsesDivisions())
    {
        cycles = cycles >> 1;
    }

    // Translates the resource matrix.
    asv->scrollByMatrix((double) cycles * ratio, 0);

    asv->setMDILocked(oldMdiLocked);
}

void 
MDIResourceWindow::attendResourceChanged(double resource)
{
    if(!asv->getMDILocked())
    {
        return;
    }

    // XXX 
    // This is a temporarly hack and should be revisited when 
    // implementing a real synch mechanism
    bool oldMdiLocked = asv->getMDILocked();
    asv->setMDILocked(false);

    asv->scrollByMatrix(0, resource);
    combo->getResourceViewFold()->repaint();

    asv->setMDILocked(oldMdiLocked);
}

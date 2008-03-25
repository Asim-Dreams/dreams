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
#include "dialogs/find/FindDialogImpl.h"
#include "dockwin/disassembly/DisDockWin.h"
#include "dockwin/tag/TagDockWin.h"
#include "DAvtView.h"
#include "PreferenceMgr.h"

DAvtView::DAvtView(QWidget* parent, QMainWindow* main, int width, int height)
    : AScrollView(parent, width, height), mainWin(main), numCycles(0), hlmgr(NULL) 
{ 
    prefMgr = PreferenceMgr::getInstance();
    connect(this, SIGNAL(asvMoved(double, double)), this, SLOT(raiseMovementSignal(double, double)));
    mdiLocked = true;

    fndDialog = NULL;
    disWin = NULL;
    hlmgr = NULL;
}

DAvtView::~DAvtView()
{
    delete disWin;
    delete hlmgr;
    delete fndDialog;
}

void
DAvtView::initDisWindow()
{
    // create the docked window
    Q_ASSERT(disWin!=NULL);
    bool val = prefMgr->getShowDisassemblyWindow();  
    if(val)
    {
        disWin->show();
        setEnabledDisWindow(true);
    }
    else
    {
        disWin->hide();
        setEnabledDisWindow(false);
    }

    connect(disWin, SIGNAL(visibilityChanged(bool))         , mainWin, SLOT(disWinVisibilityChanged(bool)));
    connect(disWin, SIGNAL(placeChanged(QDockWindow::Place)), mainWin, SLOT(disWinPlaceChanged(QDockWindow::Place)));
    connect(disWin, SIGNAL(orientationChanged(Orientation)) , mainWin, SLOT(disWinOrientationChanged(Orientation)));
}

void 
DAvtView::setMDINumber(int v) 
{
    AScrollView::setMDINumber(v);
}

void
DAvtView::find(void)
{
    if(fndDialog)
    {
        fndDialog->aboutToFind();
        fndDialog->exec();
    }
}

void
DAvtView::findNext(void)
{
    if(fndDialog)
    {
        fndDialog->find_next();
    }
}

void
DAvtView::disableLastHightlight()
{
    bool needRep = hlmgr->purgeLast();
    if(needRep)
    {
        repaintContents(false);
    }
    if(mdiLocked)
    {
        emit purgeLastHighlight();
    }
}

void
DAvtView::disableAllHighlight()
{
    bool needRep = hlmgr->purgeAll();
    if(needRep)
    {
        repaintContents(false);
    }
    if(mdiLocked)
    {
        emit purgeAllHighlight();
    }
}

void
DAvtView::showHighlightCriteria()
{
    hlmgr->showCriteria();
}

void 
DAvtView::viewHEvents()
{
    vHEvents  = true;
    vNHEvents = false;
    repaintContents(false);
}

void 
DAvtView::viewNHEvents()
{
    vHEvents  = false;
    vNHEvents = true;
    repaintContents(false);
}

void 
DAvtView::viewAllEvents()
{
    vHEvents  = true;
    vNHEvents = true;
    repaintContents(false);
}

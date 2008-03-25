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
  * @file PushoutDockWin.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dockwin/pushout/PushoutWidgetImpl.h"
#include "layout/resource/LayoutResourceCache.h"
#include "highlight/HighLightMgr.h"
#include "PushoutDockWin.h"
#include "DreamsDB.h"
#include "PreferenceMgr.h"

// Qt includes.
#include <qpopupmenu.h>
#include <qmainwindow.h>
#include <qscrollview.h>

extern bool dockInit;

/*
 * Creator of the class. Gets the parameters and resets the state.
 *
 * @return the new object.
 */
PushoutDockWin::PushoutDockWin(DreamsDB * _dreamsdb, LayoutResourceCache * _cache, HighLightMgr * hmgr, PreferenceMgr * _prefMgr, QMainWindow * mw, QScrollView * qsv)
{
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
    cache = _cache;
    myHG = hmgr;
    prefMgr = _prefMgr;
    myMW = mw;
    mySV = qsv;

    // Sets the docking state.
    setResizeEnabled(TRUE);
    setCloseMode(QDockWindow::Always);
    setMovingEnabled(true);
    setCaption("Pushout Window");

    mw->addToolBar(this);
    mw->moveDockWindow(this, prefMgr->getPushoutWindowDockPolicy(), prefMgr->getPushoutWindowNl(), prefMgr->getPushoutWindowIdx(), prefMgr->getPushoutWindowOffset());

    setWidget(new PushoutWidgetImpl(this, "pushDialog", 0));

    addContextMenu();
    show();

    // Resets the state.
    reset();
}

/*
 * Destructor of the class.
 *
 * @return void.
 */
PushoutDockWin::~PushoutDockWin()
{
    delete contextMenu;
}

/*
 * Resets the state of the widget.
 *
 * @return void.
 */
void
PushoutDockWin::reset()
{
}

/*
 * Adds the right-click menu to the widget.
 *
 * @return void.
 */
void
PushoutDockWin::addContextMenu()
{
    contextMenu = new QPopupMenu(this);
}

/*
 * Attends a resize event.
 *
 * @return void.
 */
void
PushoutDockWin::resizeEvent(QResizeEvent * e)
{
    if(!dockInit)
    {
        prefMgr->setPushoutWindowWidth(width());
        prefMgr->setPushoutWindowHeight(height());
    }
}

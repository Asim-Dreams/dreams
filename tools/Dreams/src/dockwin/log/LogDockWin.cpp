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
  * @file LogDockWin.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/LogMgr.h"
#include "LogDockWin.h"
#include "PreferenceMgr.h"

LogDockWin::LogDockWin(QMainWindow* mw)
{
    myMW = mw;
    timer = new QTimer(mw);Q_ASSERT(timer!=NULL);
    connect( timer, SIGNAL(timeout()),this, SLOT(do_timer()) );
    docVisible = false;

    setResizeEnabled(true);
    setCloseMode(QDockWindow::Always);
    setMovingEnabled(true);

    setCaption("Log Window");

    prefMgr = PreferenceMgr::getInstance();
    mw->addToolBar( this);
    mw->moveDockWindow ( this, prefMgr->getLogWindowDockPolicy(),
    prefMgr->getLogWindowNl(), prefMgr->getLogWindowIdx(),
    prefMgr->getLogWindowOffset());

    browser = new LogTextBrowser(this,"logbrowser");Q_ASSERT(browser!=NULL);

    /** @todo check path & mime type */
    browser->mimeSourceFactory()->setExtensionType("txt", "text/ascii");
    browser->mimeSourceFactory()->setFilePath(".");
    browser->setSource((PreferenceMgr::getInstance())->getLogFileName());
    setWidget(browser);
    show();
}

LogDockWin::~LogDockWin()
{
    if (browser!=NULL) delete browser;
    if (timer!=NULL)   delete timer;
}

void
LogDockWin::logFileChanged()
{
    //printf(">>logFileChanged...\n");fflush(stdout);
    browser->setSource((PreferenceMgr::getInstance())->getLogFileName());
}

QSize
LogTextBrowser::sizeHint () const
{
    PreferenceMgr* prefMgr = PreferenceMgr::getInstance();
    if (dockInit)
    {
        QSize result;
        result.setWidth(prefMgr->getLogWindowWidth());
        result.setHeight(prefMgr->getLogWindowHeight());
        return (result);
    }
    else
    {
        return QTextBrowser::sizeHint();
    }
}

void
LogDockWin::refresh()
{
    do_timer();
}

void
LogDockWin::do_timer()
{
    static bool inside = false;
    if (inside) { return; }
    if (!docVisible) { return ; }


    inside = true;
    QTime cstamp = (LogMgr::getInstance())->getLastTimestamp();
    if (cstamp>lastTimestamp)
    {
        //printf(">>getting in...\n");fflush(stdout);
        (LogMgr::getInstance())->flush();
        browser->reload();
        browser->scrollToBottom();
        lastTimestamp = cstamp;
    }
    inside=false;
}

void
LogDockWin::doVisibilityChanged(bool visible)
{
    docVisible = visible;
    if (visible)
    {
        do_timer();                          // inmediate refresh
        timer->start(LOG_REFRESH_INTERVAL);  // start timer for periodic refreshes...
    }
    else
    {
        timer->stop();
    }
}

void
LogDockWin::resizeEvent ( QResizeEvent * e )
{
    //printf("event resize!\n");fflush(stdout);
    if (!dockInit)
    {
        prefMgr->setLogWindowWidth(width());
        prefMgr->setLogWindowHeight(height());
        //printf ("Log pref width=%d height=%d\n",width(),height());fflush(stdout);
    }
}


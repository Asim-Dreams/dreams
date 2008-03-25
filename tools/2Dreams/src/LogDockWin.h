// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file LogDockWin.h
  */

#ifndef _LOGDOCKWIN_H
#define _LOGDOCKWIN_H

// Standard C/C++
#include <stdio.h>

// QT Library
#include <qdockwindow.h>
#include <qmainwindow.h>
#include <qtextbrowser.h>
#include <qdatetime.h>
#include <qtimer.h>

#include "asim/DralDB.h"

// 2Dreams
#include "PreferenceMgr.h"

#define LOG_REFRESH_INTERVAL 5000 // 5 second

class LogDockWin;
extern bool dockInit;

class LogTextBrowser : public QTextBrowser
{
    public:
           LogTextBrowser(QWidget * parent = 0, const char * name = 0 ) :
           QTextBrowser (parent, name)
           {
               myDockWin = (LogDockWin*)parent;
               resize(sizeHint());
           }
           QSize sizeHint() const;
    private:
        LogDockWin* myDockWin;
};

/**
  * Docked window to display log file.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-09-25
  * @author Federico Ardanaz
  */
class LogDockWin : public QDockWindow
{
    Q_OBJECT
    public:
        LogDockWin(QMainWindow*);
        ~LogDockWin();
        void refresh();
        void logFileChanged();
        void doVisibilityChanged(bool visible);
        friend class LogTextBrowser;
        
    public slots:
        void do_timer();
        
    protected:
        void  resizeEvent ( QResizeEvent * e );

    private:
        QMainWindow*   myMW;
        QTextBrowser*  browser;
        QTimer*        timer;
        QTime          lastTimestamp;
        bool           docVisible;
        PreferenceMgr* prefMgr;
};

#endif

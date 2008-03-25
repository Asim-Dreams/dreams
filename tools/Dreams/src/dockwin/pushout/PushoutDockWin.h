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
  * @file PushoutDockWin.h
  */

#ifndef _PUSHOUTDOCKWIN_H
#define _PUSHOUTDOCKWIN_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"

// Qt includes.
#include <qdockwindow.h>

// Class forwarding.
class DreamsDB;
class DralDB;
class LayoutResourceCache;
class PreferenceMgr;
class QMainWindow;
class HighLightMgr;
class QScrollView;
class QPopupMenu;
class QResizeEvent;

/*
 * @brief
 * Widget that lets the user see the maximums push-outs of edges.
 *
 * @description
 * This widget is an interface that lets the user see a sorted list
 * of all the push-outs computed using one edge (distance between one
 * event and the following) or two edges (difference between the edges).
 *
 * @version 0.1
 * @date started at 2005-04-07
 * @author Guillem Sole
 */
class PushoutDockWin : public QDockWindow
{
    Q_OBJECT
    public:
        PushoutDockWin(DreamsDB * _dreamsdb, LayoutResourceCache * _cache, HighLightMgr * hmgr, PreferenceMgr * _prefMgr, QMainWindow * mw, QScrollView * qsv);
        ~PushoutDockWin();

        void reset();

        void addContextMenu();

    public slots:

    protected:
        void resizeEvent(QResizeEvent * e);

    private:
        DreamsDB *      dreamsdb; ///< Pointer to the trace.
        DralDB *        draldb;   ///< Pointer to the database.
        LayoutResourceCache * cache;    ///< State of the layout.
        PreferenceMgr * prefMgr;  ///< Preference manager
        QMainWindow *   myMW;     ///< Pointer to the mdi.
        HighLightMgr *  myHG;     ///< Highlight manager.
        QScrollView *   mySV;     ///< Pointer to the ascrollview.

        QPopupMenu * contextMenu; ///< Right-click menu.
} ;

#endif

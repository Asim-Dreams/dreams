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
  * @file EventDockWin.h
  */

#ifndef _EVENTDOCKWIN_H
#define _EVENTDOCKWIN_H

#include <stdio.h>

#include <qdockwindow.h>
#include <qmainwindow.h>
#include <qlistview.h>
#include <qpixmap.h>
#include <qtextedit.h>

#include "asim/DralDB.h"

#include "ZDBase.h"
#include "FavoriteTags.h"
#include "TagValueType.h"
#include "PreferenceMgr.h"
#include "ltagformImpl.h"

#define MAX_EDW_TAGS 1024

class EventDockWin;
extern bool dockInit;

class EventListView : public QListView
{
    public:
        EventListView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 ) :
        QListView ( parent,name,f)
        {
           myDockWin = (EventDockWin*)parent;
           resize(sizeHint());
        }

        QSize sizeHint() const;

    private:
        EventDockWin* myDockWin;
};


/**
  * Docked window to display event related tags as long as user moves the mouse.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class EventDockWin : public QDockWindow
{
    Q_OBJECT
    public:
        EventDockWin(QMainWindow*);
        ~EventDockWin();

        void updateEvent(ItemHandler * hnd, INT32 cycle);
        void updateEvent(INT32 trackId, INT32 cycle);
        void updateTagList();

        void reset();
        friend class EventListView;

    public slots:
        void doubleClicked ( QListViewItem * );

    protected:
        void resizeEvent ( QResizeEvent * e );
        void updateEventAllTags(ItemHandler * handler, INT32 cycle);
        void updateEventFavorite(ItemHandler * handler, INT32 cycle);
        void updateEventAllTags(INT32, INT32 cycle);
        void updateEventFavorite(INT32, INT32 cycle);
        void updateTagListFavorite();

    private:
        //QMainWindow*   myMW;
        QListView*     eventList;
        FavoriteTags*  myFT;

        QListViewItem* itemArray[MAX_EDW_TAGS];
        PreferenceMgr* prefMgr;

        ltagformImpl* longstrte;
};

#endif


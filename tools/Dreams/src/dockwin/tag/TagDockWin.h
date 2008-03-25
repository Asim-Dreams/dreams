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
  * @file TagDockWin.h
  */

#ifndef _TAGDOCKWIN_H
#define _TAGDOCKWIN_H

// Dreams includes.
#include "DreamsDefs.h"
#include "dDB/DralDBSyntax.h"
#include "dDB/ClockDomainMgr.h"
#include "dDB/DRALTag.h"

// Qt includes.
#include <qlistview.h>
#include <qdockwindow.h>

// Class forwarding.
class DralDB;

#define MAX_EDW_TAGS 1024

// Class forwarding.
class TagDockWin;
class QListView;
class QWidget;
class QListView;
class FavoriteTags;
class PreferenceMgr;
class LongTagDialogFormImpl;
class ItemHandler;

extern bool dockInit;

class EventListView : public QListView
{
    public:
        EventListView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 ) :
        QListView ( parent,name,f)
        {
           myDockWin = (TagDockWin *) parent;
           resize(sizeHint());
        }

        QSize sizeHint() const;

    private:
        TagDockWin * myDockWin;
};


/**
  * Docked window to display event related tags as long as user moves the mouse.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class TagDockWin : public QDockWindow
{
    Q_OBJECT
    public:
        TagDockWin(QMainWindow * main, DralDB * _draldb);
        ~TagDockWin();

        void updateItemTag(ItemHandler * hnd, CYCLE cycle);
        void updateTrackTag(TRACK_ID trackId, CYCLE cycle);
        void updateTagList();

        void reset();
        friend class EventListView;

    public slots:
        void doubleClicked(QListViewItem *);

    protected:
        void resizeEvent(QResizeEvent * e);
        void updateItemAllTags(ItemHandler * handler, CYCLE cycle);
        void updateItemFavorite(ItemHandler * handler, CYCLE cycle);
        void updateTrackAllTags(TRACK_ID trackId, CYCLE cycle);
        void updateTrackFavorite(TRACK_ID trackId, CYCLE cycle);
        void updateTagListFavorite();

    private:
        QListView *     eventList;
        FavoriteTags *  myFT;
        QListViewItem * itemArray[MAX_EDW_TAGS];
        PreferenceMgr * prefMgr;
        DralDB *        draldb;
} ;

#endif

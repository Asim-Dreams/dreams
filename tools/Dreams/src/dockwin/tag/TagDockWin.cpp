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
  * @file TagDockWin.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "TagDockWin.h"
#include "ShowTags.h"
#include "dialogs/tag/MutableTagDialogFormImpl.h"
#include "dialogs/tag/LongTagDialogFormImpl.h"
#include "FavoriteTags.h"
#include "PreferenceMgr.h"

// Qt includes.
#include <qstrlist.h>
#include <qmainwindow.h>
#include <qtextedit.h>

// XPM includes.
#include "xpm/mutabletag.xpm"

// Asim includes.
#include "asim/IconFactory.h"

TagDockWin::TagDockWin(QMainWindow * main, DralDB * _draldb)
{
    myFT = FavoriteTags::getInstance();
    draldb = _draldb;

    setResizeEnabled(TRUE);
    setCloseMode(QDockWindow::Always);
    setMovingEnabled(true);
    setCaption("Tag Window");

    prefMgr = PreferenceMgr::getInstance();

    eventList = new EventListView(this);
    Q_ASSERT(eventList != NULL);
    eventList->addColumn("Tag");
    eventList->setColumnAlignment(0, Qt::AlignLeft );
    eventList->addColumn("Value", 100);
    eventList->setColumnAlignment(1, Qt::AlignLeft );
    eventList->addColumn("Cycle", 100);
    eventList->setColumnAlignment(2, Qt::AlignLeft );
    eventList->addColumn("Division", 100);
    eventList->setColumnAlignment(3, Qt::AlignLeft );
    eventList->addColumn("Frequency", 100);
    eventList->setColumnAlignment(4, Qt::AlignLeft );

    eventList->setSelectionMode(QListView::Single);
    eventList->setShowSortIndicator(false);
    eventList->setAllColumnsShowFocus(true);
    eventList->setSorting(0);

    updateTagList();

    setWidget(eventList);

    main->addToolBar(this);

    main->moveDockWindow(this, prefMgr->getTagWindowDockPolicy(), prefMgr->getTagWindowNl(), prefMgr->getTagWindowIdx(), prefMgr->getTagWindowOffset());

    connect(eventList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(doubleClicked(QListViewItem *)));
    connect(eventList, SIGNAL(returnPressed(QListViewItem *)), this, SLOT(doubleClicked(QListViewItem *)));
    connect(eventList, SIGNAL(spacePressed(QListViewItem *)), this, SLOT(doubleClicked(QListViewItem *)));

    show();
}

TagDockWin::~TagDockWin()
{
    delete eventList;
}

QSize
EventListView::sizeHint() const
{
    PreferenceMgr * prefMgr = PreferenceMgr::getInstance();
    if(dockInit)
    {
        QSize result;
        result.setWidth(prefMgr->getTagWindowWidth());
        result.setHeight(prefMgr->getTagWindowHeight());
        return result;
    }
    else
    {
        return QListView::sizeHint();
    }
}

void
TagDockWin::updateItemTag(ItemHandler * hnd, CYCLE cycle)
{
    if(prefMgr->getTagWinFavouriteOnly())
    {
        updateItemFavorite(hnd, cycle);
    }
    else
    {
        updateItemAllTags(hnd, cycle);
    }
}

void
TagDockWin::updateTrackTag(TRACK_ID trackId, CYCLE cycle)
{
    if(prefMgr->getTagWinFavouriteOnly())
    {
        updateTrackFavorite(trackId, cycle);
    }
    else
    {
        updateTrackAllTags(trackId, cycle);
    }
}

void
TagDockWin::updateTrackAllTags(TRACK_ID trackId, CYCLE cycle)
{
    eventList->clear();
    ShowTags::showTrackTags(draldb, eventList, trackId, cycle);
}

void
TagDockWin::updateItemAllTags(ItemHandler * hnd, CYCLE cycle)
{
    eventList->clear();
    ShowTags::showItemTags(draldb, eventList, hnd, cycle);
}

void
TagDockWin::updateTrackFavorite(TRACK_ID trackId, CYCLE cycle)
{
}

void
TagDockWin::updateItemFavorite(ItemHandler * hnd, CYCLE cycle)
{
    int cnt = 0;

    TagValueBase tgbase;
    TagValueType tgvtype;
    UINT64 tgvalue;
    CYCLE tgwhen;

    // for each tag try to get its value
    QStrList strlist = myFT->getList();
    QStrListIterator it_str(strlist);
    char * str;
    ItemHandler temp = * hnd;

    while((str = it_str.current()) != NULL)
    {
        ++it_str;
        if(temp.getTagByName(QString(str), &tgvalue, &tgvtype, &tgbase, &tgwhen, cycle))
        {
            QString dbstr = temp.getTagFormatedValue();
            itemArray[cnt]->setText(1, dbstr.stripWhiteSpace());
            itemArray[cnt]->setText(2, QString::number((long int) tgwhen.cycle));
            if(tgwhen.division == 0)
            {
                itemArray[cnt]->setText(3, "High");
            }
            else
            {
                itemArray[cnt]->setText(3, "Low");
            }
            itemArray[cnt]->setText(4, tgwhen.clock->getFormattedFreq(CLOCK_GHZ));
        }
        else
        {
            itemArray[cnt]->setText(1, "");
        }
        if(temp.getTagIsMutable())
        {
            itemArray[cnt]->setPixmap(2, IconFactory::getInstance()->image20);
        }
        ++cnt;
    }
}

void
TagDockWin::updateTagList()
{
    if(prefMgr->getTagWinFavouriteOnly())
    {
        updateTagListFavorite();
    }
}

void
TagDockWin::updateTagListFavorite()
{
    eventList->clear();
    int cnt = 0;
    QListViewItem* item;
    QStrList strlist = myFT->getList();
    QStrListIterator it_str(strlist);
    char * str;

    while(((str = it_str.current()) != NULL) && (cnt < MAX_EDW_TAGS))
    {
        ++it_str;
        itemArray[cnt] = new QListViewItem(eventList, QString::number(cnt + 1) + ". " + QString(str));

        Q_ASSERT(itemArray[cnt]!=NULL);

        //printf("TagDockWin:: inserted %s \n",str);
        ++cnt;
    }
}

void
TagDockWin::reset()
{
    if(prefMgr->getTagWinFavouriteOnly())
    {
        int cnt = 0;
        QStrList strlist = myFT->getList();
        QStrListIterator it_str(strlist);
        char * str;

        while((str = it_str.current()) != NULL)
        {
            ++it_str;
            itemArray[cnt]->setText(1, "");
            ++cnt;
        }
    }
    else
    {
        eventList->clear();
    }
}

void
TagDockWin::resizeEvent(QResizeEvent * e)
{
    if(!dockInit)
    {
        prefMgr->setTagWindowWidth(width());
        prefMgr->setTagWindowHeight(height());
    }
}

void
TagDockWin::doubleClicked(QListViewItem * item)
{
    TagListViewItem * tgitem = (TagListViewItem *) item;
    ItemHandler hnd = tgitem->getItemHandler();
    INT32 trackId = tgitem->getTrackId();
    QString longStr = tgitem->getLongStr();

    if(hnd.isValidTagHandler() && hnd.getTagIsMutable())
    {
        MutableTagDialogFormImpl mdialog(this, "mlist", true);
        ShowTags::showItemMutableTagValue(draldb, mdialog.getListView(), &hnd);
        mdialog.exec();
    }

    if((trackId >= 0) && (longStr != QString::null))
    {
        QPoint p = (item->listView())->mapToGlobal(QPoint(0, item->itemPos()));
        LongTagDialogFormImpl longstrte(NULL);
        longstrte.textEdit1->setReadOnly(true);
        longstrte.setGeometry(0, 0, 400, 200);
        longstrte.setGeometry(p.x(), p.y(), 400, 200);
        longstrte.textEdit1->setText(tgitem->getLongStr());
        longstrte.exec();
    }
}

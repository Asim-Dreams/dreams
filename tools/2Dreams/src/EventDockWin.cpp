/*
 * Copyright (C) 2003-2006 Intel Corporation
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
  * @file EventDockWin.cpp
  */

#include <qstrlist.h>

#include "EventDockWin.h"
#include "xpm/mutabletag.xpm"
#include "ShowTags.h"
#include "mtagformImpl.h"
#include "asim/IconFactory.h"

EventDockWin::EventDockWin( QMainWindow* mw)
{
    //myMW = mw;
    myFT = FavoriteTags::getInstance();

    setResizeEnabled(TRUE);
    setCloseMode(QDockWindow::Always);
    setMovingEnabled(true);
    setCaption("Tag Window");

    prefMgr = PreferenceMgr::getInstance();

    eventList = new EventListView(this);Q_ASSERT(eventList!=NULL);
    eventList->addColumn("Tag");
    eventList->setColumnAlignment( 0, Qt::AlignLeft );
    eventList->addColumn("Value",100);
    eventList->setColumnAlignment( 1, Qt::AlignLeft );
    eventList->addColumn("When",100);
    eventList->setColumnAlignment( 2, Qt::AlignLeft );

    eventList->setSelectionMode( QListView::Single );
    eventList->setShowSortIndicator (false);
    eventList->setAllColumnsShowFocus(true);
    eventList->setSorting (0);

    updateTagList();

    setWidget(eventList);

    mw->addToolBar(this);

    mw->moveDockWindow ( this, prefMgr->getEventWindowDockPolicy(),
    prefMgr->getEventWindowNl(), prefMgr->getEventWindowIdx(),
    prefMgr->getEventWindowOffset());

    connect (eventList,SIGNAL(doubleClicked(QListViewItem*)),this,SLOT(doubleClicked(QListViewItem*)));
    connect (eventList,SIGNAL(returnPressed(QListViewItem*)),this,SLOT(doubleClicked(QListViewItem*)));
    connect (eventList,SIGNAL(spacePressed(QListViewItem*)),this,SLOT(doubleClicked(QListViewItem*)));

    longstrte = new ltagformImpl(NULL);
    longstrte->textEdit1->setReadOnly(true);
    longstrte->setGeometry(0,0,400,200);
    
    show();
}

EventDockWin::~EventDockWin()
{
    if (eventList!=NULL)
    {
        delete eventList;
    }
}

QSize
EventListView::sizeHint () const
{
    PreferenceMgr* prefMgr = PreferenceMgr::getInstance();
    if (dockInit)
    {
        QSize result;
        result.setWidth(prefMgr->getEventWindowWidth());
        result.setHeight(prefMgr->getEventWindowHeight());
        return (result);
    }
    else
    {
        return QListView::sizeHint();
    }
}

void
EventDockWin::updateEvent(ItemHandler * hnd, INT32 cycle)
{
    if (prefMgr->getTagWinFavouriteOnly())
    {
        updateEventFavorite(hnd, cycle);
    }
    else
    {
        updateEventAllTags(hnd, cycle);
    }
}

void
EventDockWin::updateEvent(INT32 trackId, INT32 cycle)
{
    if (prefMgr->getTagWinFavouriteOnly())
    {
        updateEventFavorite(trackId,cycle);
    }
    else
    {
        updateEventAllTags(trackId,cycle);
    }
}

void
EventDockWin::updateEventAllTags(INT32 trackId, INT32 cycle)
{
    eventList->clear();
    ShowTags::showItemTags(eventList,trackId,cycle);
}

void
EventDockWin::updateEventAllTags(ItemHandler * hnd, INT32 cycle)
{
    eventList->clear();
    ShowTags::showItemTags(eventList,hnd,cycle);
}

void
EventDockWin::updateEventFavorite(INT32 trackId, INT32 cycle)
{
}

void
EventDockWin::updateEventFavorite(ItemHandler * hnd, INT32 cycle)
{
    int cnt = 0;

    INT16 tgbase;
    UINT64 tgvalue;
    UINT64 tgwhen;
    TagValueType  tgvtype;

    //printf (">>updateEvent called over %d\n",idx);

    // for each tag try to get its value
    QStrList strlist = myFT->getList();
    char* str;
    ItemHandler temp = * hnd;

    for (str=strlist.first();str!=NULL;str=strlist.next())
    {
        if(temp.getTagByName(QString(str),&tgvalue,&tgvtype,&tgbase,&tgwhen,cycle))
        {
            QString dbstr = temp.getFormatedTagValue();
            itemArray[cnt]->setText(1,dbstr.stripWhiteSpace());
            itemArray[cnt]->setText(2,QString::number((long int)tgwhen));
        }
        else
        {
            itemArray[cnt]->setText(1,"");
        }
        if(temp.isMutableTag())
        {
            itemArray[cnt]->setPixmap(2,IconFactory::getInstance()->image20);
        }
        ++cnt;
    }
}

void
EventDockWin::updateTagList()
{
    if (prefMgr->getTagWinFavouriteOnly())
    {
        updateTagListFavorite();
    }
}

void
EventDockWin::updateTagListFavorite()
{
    eventList->clear();
    int cnt = 0;
    QListViewItem* item;
    QStrList strlist = myFT->getList();
    char* str = strlist.first();
    while ( (str!=NULL) && (cnt<MAX_EDW_TAGS) )
    {
        itemArray[cnt] = new QListViewItem(eventList,QString::number(cnt+1)+". "+QString(str));Q_ASSERT(itemArray[cnt]!=NULL);
        //printf("EventDockWin:: inserted %s \n",str);
        ++cnt;
        str=strlist.next();
    }
}

void
EventDockWin::reset()
{
    if (prefMgr->getTagWinFavouriteOnly())
    {
        int cnt = 0;
        QStrList strlist = myFT->getList();
        char* str;
        for (str=strlist.first();str!=NULL;str=strlist.next())
        {
            itemArray[cnt]->setText(1,"");
            ++cnt;
        }
    }
    else
    {
        eventList->clear();
    }
}

void
EventDockWin::resizeEvent ( QResizeEvent * e )
{
    if (!dockInit)
    {
        prefMgr->setEventWindowWidth(width());
        prefMgr->setEventWindowHeight(height());
    }
}

void
EventDockWin::doubleClicked ( QListViewItem *item )
{
    TagListViewItem* tgitem = (TagListViewItem*)item;
    ItemHandler hnd = tgitem->getItemHandler();
    INT32 trackId = tgitem->getTrackId();
    QString longStr = tgitem->getLongStr();

    if (hnd.isValidTagHandler() && hnd.isMutableTag())
    {
        mtagformImpl* mdialog = new mtagformImpl(this,"mlist",true);
        ShowTags::showItemMutableTagValue(mdialog->getListView(), &hnd);
        mdialog->exec();
    }

    if ((trackId>=0) && (longStr!=QString::null) )
    {
        QPoint p = (item->listView())->mapToGlobal(QPoint(0,item->itemPos()));
        longstrte->setGeometry(p.x(),p.y(),400,200);
        longstrte->textEdit1->setText(tgitem->getLongStr());
        longstrte->exec();
    }
}

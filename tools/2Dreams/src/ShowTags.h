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
  * @file  ShowTags.h
  * @brief
  */

#ifndef _SHOWTAGS_H_
#define _SHOWTAGS_H_

#include <qlistview.h>
#include <qpixmap.h>

#include "asim/DralDB.h"

class TagListViewItem : public QListViewItem
{
    public:
        TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListView * parent );
        TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListViewItem * parent );
        TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListView * parent, QListViewItem * after );
        TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListViewItem * parent, QListViewItem * after );
        TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListView * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
        TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListViewItem * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
        TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListView * parent, QListViewItem * after, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );
        TagListViewItem ( ItemHandler * hnd, INT32 trackId, QListViewItem * parent, QListViewItem * after, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null, QString label7 = QString::null, QString label8 = QString::null );

        ItemHandler getItemHandler() { return myhnd; }
        INT32  getTrackId() { return myTrackId; }
        
        QString getLongStr() { return longStr; }
        void    setLongStr(QString value ) { longStr = value; }
        
    private:
        QString longStr;
        ItemHandler myhnd;
		INT32		myTrackId;
        QPixmap     mutpix;
};

class ShowTags
{
    public:
        static void showItemTags(QListView* list, ItemHandler * hnd, INT32 cycle);
        static void showItemTags(QListView* list, INT32 trackId, INT32 cycle);
        static void showItemMutableTagValue(QListView* list, ItemHandler * hnd);
        
        static QString getItemTags(ItemHandler * hnd, INT32 cycle);
        static QString getItemTags(INT32 trackId, INT32 cycle);
        
    protected:
        static bool isMultipleLines(QString str);
        static QString summarizeMultiline(QString str);
};

#endif


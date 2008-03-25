/*
 * Copyright (C) 2004-2006 Intel Corporation
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

// Dreams includes.
#include "DreamsDefs.h"
#include "dDB/itemheap/ItemHeap.h"

// Qt includes.
#include <qlistview.h>
#include <qpixmap.h>

// Class forwarding.
class DralDB;

class TagListViewItem : public QListViewItem
{
    public:
        TagListViewItem(ItemHandler * hnd, TRACK_ID track_id, INT32 sort_cycle, QListView * parent, QString tag_name, QString value, QString cycle, QString phase, QString clock);

        inline ItemHandler getItemHandler() const;
        inline TRACK_ID getTrackId() const;
        inline QString getLongStr() const;
        inline void setLongStr(QString value);

        int compare(QListViewItem * i, int col, bool ascending) const;

    protected:
        void paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align);

    private:
        static INT32 cycle_col;
        QString long_str;
        ItemHandler my_hnd;
        TRACK_ID my_track_id;
        INT32 my_sort_cycle;
        QPixmap mutpix;
} ;

class ShowTags
{
    public:
        static void showItemTags(DralDB * draldb, QListView * list, ItemHandler * hnd, CYCLE cycle);
        static QString getItemTags(DralDB * draldb, ItemHandler * hnd, CYCLE cycle);
        static void showTrackTags(DralDB * draldb, QListView * list, TRACK_ID trackId, CYCLE cycle);
        static QString getTrackTags(DralDB * draldb, TRACK_ID trackId, CYCLE cycle);
        static void showItemMutableTagValue(DralDB * draldb, QListView * list, ItemHandler * hnd);

    protected:
        static bool isMultipleLines(QString str);
        static QString summarizeMultiline(QString str);
} ;

ItemHandler
TagListViewItem::getItemHandler() const
{
    return my_hnd;
}

TRACK_ID
TagListViewItem::getTrackId() const
{
    return my_track_id;
}

QString
TagListViewItem::getLongStr() const
{
    return long_str;
}

void
TagListViewItem::setLongStr(QString value)
{
    long_str = value;
}

#endif

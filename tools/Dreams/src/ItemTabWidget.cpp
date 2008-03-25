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
  * @file ItemTabWidget.cpp
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ItemTabWidget.h"
#include "ItemTabWidgetColumn.h"
#include "PreferenceMgr.h"
#include "DreamsDB.h"

ItemTabWidget::ItemTabWidget(DreamsDB * _dreamsdb, ItemTab * itab, int cside, QWidget * parent, const char * name)
    : QTable(parent, name)
{
    myItemTab = itab; 
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();

    TabSpec * tspec = itab->getTabSpec();
    strlist = tspec->getShowList();
    ilist = itab->getItemList();
    cube_side = cside;

    setSelectionMode(QTable::NoSelection);
    setReadOnly(true);
    setColumnMovingEnabled(true);

    // Size of the table.
    setNumRows(1);
    setNumCols(strlist->count());

    // Inits header.
    INT32 hcnt = 0;
    QStringList::Iterator it = strlist->begin();
    while(it != strlist->end())
    { 
        horizontalHeader()->setLabel(hcnt, * it);
        ItemTabWidgetColumn * item = new ItemTabWidgetColumn(dreamsdb, ilist, draldb->getTagId(* it), cube_side, this, QTableItem::Never);
        setItem(0, hcnt, item);
        hcnt++;
        ++it;
    }

    // Sets the height of the unique row.
    adjustRow(0);

    setLeftMargin(0);
    setVScrollBarMode(QScrollView::AlwaysOff);
    myCurrentVerticalZoom = 1.0;
}

ItemTabWidget::~ItemTabWidget()
{
}

void 
ItemTabWidget::putVerticalZoom(double dy)
{
    myCurrentVerticalZoom = dy;
    updateVerticalZoom();
}

void 
ItemTabWidget::updateVerticalZoom()
{
    for(UINT32 i = 0; i < strlist->count(); i++)
    {
        ItemTabWidgetColumn * col = (ItemTabWidgetColumn *) item(0, i);
        col->updateVerticalZoom(myCurrentVerticalZoom);
    }
    setRowHeight(0, item(0, 0)->sizeHint().height());
    repaintContents(false);
}

void 
ItemTabWidget::synchWithASVTop(double y)
{
    setContentsPos(contentsX(), (int)(y * myCurrentVerticalZoom));
}

void
ItemTabWidget::setSelectedRow(INT32 row)
{
    selectedRow = row;
    repaintContents(false);
}

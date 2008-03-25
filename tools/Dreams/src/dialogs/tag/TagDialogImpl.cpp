/*
 * Copyright (C) 2006 Intel Corporation
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
  * @file TagDialogImpl.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "dialogs/tag/TagDialogImpl.h"
#include "ShowTags.h"
#include "MutableTagDialogFormImpl.h"
#include "LongTagDialogFormImpl.h"

// Qt includes.
#include <qapplication.h>
#include <qtextedit.h>

/*
 *  Constructs a TagDialogImpl which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
TagDialogImpl::TagDialogImpl(DralDB * draldb, ItemHandler * hnd, CYCLE cycle,
                    QWidget* parent,  const char* name, bool modal, WFlags fl)
                   : TagDialog( parent, name, modal, fl)
{
    rtype = SimpleEdgeRow;
    this->draldb = draldb;
    ShowTags::showItemTags(draldb, EventListView, hnd, cycle);
    connect(EventListView, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(doubleClicked(QListViewItem *)));
    connect(EventListView, SIGNAL(returnPressed(QListViewItem *)), this, SLOT(returnPressed(QListViewItem *)));
    connect(EventListView, SIGNAL(spacePressed(QListViewItem *)), this, SLOT(spacePressed(QListViewItem *)));
}

TagDialogImpl::TagDialogImpl(DralDB * draldb, TRACK_ID trackId, CYCLE cycle, QWidget* parent, const char* name, bool modal, WFlags fl)
    : TagDialog(parent, name, modal, fl)
{
    rtype = SimpleNodeRow;
    ShowTags::showTrackTags(draldb, EventListView, trackId, cycle);
    connect(EventListView, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(doubleClicked(QListViewItem *)));
    connect(EventListView, SIGNAL(returnPressed(QListViewItem *)), this, SLOT(returnPressed(QListViewItem *)));
    connect(EventListView, SIGNAL(spacePressed(QListViewItem *)), this, SLOT(spacePressed(QListViewItem *)));
}

/*
 *  Destroys the object and frees any allocated resources
 */
TagDialogImpl::~TagDialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 * public slot
 */
void
TagDialogImpl::AcceptButton_clicked()
{
    done(1);
}

void
TagDialogImpl::doubleClicked(QListViewItem * item)
{
    mutableShowAllRequest(item);
}

void
TagDialogImpl::returnPressed(QListViewItem * item)
{
    mutableShowAllRequest(item);
}

void
TagDialogImpl::spacePressed(QListViewItem * item)
{
    mutableShowAllRequest(item);
}

void
TagDialogImpl::mutableShowAllRequest(QListViewItem * item)
{
    // check if this is a mutable or a long string
    TagListViewItem * tgitem = (TagListViewItem*)item;
    ItemHandler hnd = tgitem->getItemHandler();
    TRACK_ID trackId = tgitem->getTrackId();
    QString longStr = tgitem->getLongStr();

    if(trackId == TRACK_ID_INVALID)
    {
        if(rtype != SimpleEdgeRow)
        {
            return;
        }
        if(hnd.isValidTagHandler())
        {
            if(hnd.getTagIsMutable())
            {
                MutableTagDialogFormImpl mdialog(this, "mlist", true);
                ShowTags::showItemMutableTagValue(draldb, mdialog.getListView(), &hnd);
                mdialog.exec();
            }
        }
    }
    else if(longStr!=QString::null)
    {
        LongTagDialogFormImpl longstrte(NULL);
        QPoint p = (item->listView())->mapToGlobal(QPoint(0,item->itemPos()));
        longstrte.setGeometry(p.x(),p.y(),400,200);
        longstrte.textEdit1->setText(longStr);
        longstrte.exec();
    }
}

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
  * @file eventTagDialogImpl.cpp
  */

#include <qapplication.h>
#include <qtextedit.h>

#include "eventTagDialogImpl.h"
#include "ShowTags.h"
#include "mtagformImpl.h"
#include "ltagformImpl.h"

/*
 *  Constructs a eventTagDialogImpl which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
eventTagDialogImpl::eventTagDialogImpl(ItemHandler * hnd, INT32 cycle,
                    QWidget* parent,  const char* name, bool modal, WFlags fl)
                   : EventTagDialog( parent, name, modal, fl)
{
	rtype=SimpleEdgeRow;
    ShowTags::showItemTags(EventListView,hnd,cycle);
    connect (EventListView,SIGNAL(doubleClicked(QListViewItem*)),this,SLOT(doubleClicked(QListViewItem*)));
    connect (EventListView,SIGNAL(returnPressed(QListViewItem*)),this,SLOT(returnPressed(QListViewItem*)));
    connect (EventListView,SIGNAL(spacePressed(QListViewItem*)),this,SLOT(spacePressed(QListViewItem*)));
}

eventTagDialogImpl::eventTagDialogImpl(INT32 trackId, INT32 cycle, QWidget* parent, 
					const char* name, bool modal, WFlags fl) 
					: EventTagDialog(parent, name, modal, fl)
{
	rtype=SimpleNodeRow;
	ShowTags::showItemTags(EventListView,trackId,cycle);
    connect (EventListView,SIGNAL(doubleClicked(QListViewItem*)),this,SLOT(doubleClicked(QListViewItem*)));
    connect (EventListView,SIGNAL(returnPressed(QListViewItem*)),this,SLOT(returnPressed(QListViewItem*)));
    connect (EventListView,SIGNAL(spacePressed(QListViewItem*)),this,SLOT(spacePressed(QListViewItem*)));
}
					
/*
 *  Destroys the object and frees any allocated resources
 */
eventTagDialogImpl::~eventTagDialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 * public slot
 */
void
eventTagDialogImpl::AcceptButton_clicked()
{ done(1); }

void
eventTagDialogImpl::doubleClicked ( QListViewItem *item )
{ mutableShowAllRequest(item); }

void
eventTagDialogImpl::returnPressed ( QListViewItem *item )
{ mutableShowAllRequest(item); }

void
eventTagDialogImpl::spacePressed  ( QListViewItem *item )
{ mutableShowAllRequest(item); }

void
eventTagDialogImpl::mutableShowAllRequest ( QListViewItem *item )
{
    // check if this is a mutable or a long string
    TagListViewItem* tgitem = (TagListViewItem*)item;
    ItemHandler hnd = tgitem->getItemHandler();
    INT32 trackId = tgitem->getTrackId();
    QString longStr = tgitem->getLongStr();
    
    if (trackId<0)
    {
        if (rtype!=SimpleEdgeRow) return;
        if (hnd.isValidTagHandler())
        {
            if (hnd.isMutableTag())
            {
                mtagformImpl* mdialog = new mtagformImpl(this,"mlist",true);
                ShowTags::showItemMutableTagValue(mdialog->getListView(),&hnd);
                mdialog->exec();
            }
        }
    }
    else if (longStr!=QString::null)
    {
        ltagformImpl* longstrte = new ltagformImpl(NULL);
        QPoint p = (item->listView())->mapToGlobal(QPoint(0,item->itemPos()));
        longstrte->setGeometry(p.x(),p.y(),400,200);
        longstrte->textEdit1->setText(longStr);
        longstrte->exec();
        delete longstrte;
    }
}



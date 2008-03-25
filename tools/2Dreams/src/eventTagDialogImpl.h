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
  * @file eventTagDialogImpl.h
  */

#ifndef EVENTTAGDIALOGIMPL_H
#define EVENTTAGDIALOGIMPL_H

#include <qlistview.h>

#include "asim/DralDB.h"

#include "eventTagDialog.h"
#include "ZDBase.h"

/**
  * Just a QT dialog for showing all the event attached tags.
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class eventTagDialogImpl : public EventTagDialog
{
    Q_OBJECT

public:
    eventTagDialogImpl(ItemHandler * handler, INT32 cycle, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    eventTagDialogImpl(INT32 trackId, INT32 cycle, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );

    ~eventTagDialogImpl();

public slots:
    void AcceptButton_clicked();
    void doubleClicked ( QListViewItem * );
    void returnPressed ( QListViewItem * );
    void spacePressed  ( QListViewItem * );

protected:
    void mutableShowAllRequest (QListViewItem *);

private:
    ZDBase* db;
	RowType rtype;

};

#endif // EVENTTAGDIALOGIMPL_H

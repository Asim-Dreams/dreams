// ==================================================
// Copyright (C) 2006 Intel Corporation
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
  * @file TagDialogImpl.h
  */

#ifndef TAGDIALOGIMPL_H
#define TAGDIALOGIMPL_H

// Dreams includes.
#include "DreamsDefs.h"
#include "adf/ADFDefinitions.h"
#include "dDB/ClockDomainMgr.h"
#include "dialogs/tag/TagDialog.h"

// Class forwarding.
class ItemHandler;
class DralDB;
class QListViewItem;

/**
  * Just a QT dialog for showing all the event attached tags.
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class TagDialogImpl : public TagDialog
{
    Q_OBJECT

  public:
    TagDialogImpl(DralDB * draldb, ItemHandler * handler, CYCLE cycle, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    TagDialogImpl(DralDB * draldb, TRACK_ID trackId, CYCLE cycle, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );

    ~TagDialogImpl();

  public slots:
    void AcceptButton_clicked();
    void doubleClicked(QListViewItem * item);
    void returnPressed(QListViewItem * item);
    void spacePressed(QListViewItem * item);

  protected:
    void mutableShowAllRequest(QListViewItem * item);

  private:
    RowType rtype;
    DralDB * draldb;
} ;

#endif // TAGDIALOGIMPL_H

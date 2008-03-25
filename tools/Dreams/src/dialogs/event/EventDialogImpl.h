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

#ifndef EVENTDIALOGIMPL_H
#define EVENTDIALOGIMPL_H

// Dreams includes.
#include "dDB/ClockDomainMgr.h"
#include "dialogs/event/EventDialog.h"

// Class forwarding.
class DralDB;
class ItemHandler;

class EventDialogImpl : public EventDialog
{
    Q_OBJECT

public:
    EventDialogImpl(DralDB * draldb, ItemHandler * hnd, CYCLE cycle, QWidget * parent = 0, const char * name = 0, bool modal = FALSE, WFlags fl = 0);
    ~EventDialogImpl();

public slots:
    void AcceptButton_clicked();

};

#endif // EVENTDIALOGIMPL_H

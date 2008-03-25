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
  * @file hlightDialogImpl.h
  */

#ifndef HLIGHTDIALOGIMPL_H
#define HLIGHTDIALOGIMPL_H

#include "hlightDialog.h"

#include "asim/DralDB.h"

/**
  * QT dialog to view currently used highlighting criteria.
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class hlightDialogImpl : public HLightForm
{ 
    Q_OBJECT

public slots:
    void accept_clicked();
    
public:
    hlightDialogImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~hlightDialogImpl();

    inline QListView* getListView();
};

QListView* 
hlightDialogImpl::getListView()
{ return qlv; }


#endif // HLIGHTDIALOGIMPL_H

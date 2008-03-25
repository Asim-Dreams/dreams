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
  * @file HighLightMgrDialogImpl.h
  */

#ifndef _HIGHLIGHTMGRDIALOGIMPL_H
#define _HIGHLIGHTMGRDIALOGIMPL_H

#include "DreamsDefs.h"
#include "dialogs/highlightmgr/HighLightMgrDialog.h"

/**
  * QT dialog to view currently used highlighting criteria.
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class HighLightMgrDialogImpl : public HighLightMgrDialog
{ 
    Q_OBJECT

public slots:
    void accept_clicked();
    
public:
    HighLightMgrDialogImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~HighLightMgrDialogImpl();

    inline QListView* getListView();
};

QListView* 
HighLightMgrDialogImpl::getListView()
{ return qlv; }


#endif // _HIGHLIGHTMGRDIALOGIMPL_H.

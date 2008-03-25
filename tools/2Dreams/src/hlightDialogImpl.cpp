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
  * @file hlightDialogImpl.cpp
  */

#include "hlightDialogImpl.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a hlightDialogImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
hlightDialogImpl::hlightDialogImpl( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : HLightForm( parent, name, modal, fl )
{
    connect (AcceptButton,SIGNAL(clicked()),SLOT(accept_clicked())) ;

    // adjust (by hand) some properies
    qlv->setColumnAlignment( 0, Qt::AlignCenter );
    qlv->setColumnAlignment( 1, Qt::AlignLeft );
    qlv->setColumnAlignment( 2, Qt::AlignRight );

    qlv->setColumnWidth (0,40);
    qlv->setColumnWidth (1,60);
    qlv->setColumnWidth (2,120);

    qlv->setColumnWidthMode(0,QListView::Maximum);
    qlv->setColumnWidthMode(1,QListView::Maximum);
    qlv->setColumnWidthMode(2,QListView::Maximum);

    qlv->setSelectionMode(QListView::NoSelection);
    qlv->setShowSortIndicator(false);
    qlv->setSorting( -1 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
hlightDialogImpl::~hlightDialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void 
hlightDialogImpl::accept_clicked()
{ done(1); }


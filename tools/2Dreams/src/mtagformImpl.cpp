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

#include "mtagformImpl.h"

#include <qlistview.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qlayout.h>

/* 
 *  Constructs a mtagformImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
mtagformImpl::mtagformImpl( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : MTagForm( parent, name, modal, fl )
{
    connect(AcceptPushButton,SIGNAL(clicked()),this,SLOT(AcceptButton_clicked()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
mtagformImpl::~mtagformImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void
mtagformImpl::AcceptButton_clicked()
{ done(1); }


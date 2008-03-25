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

/****************************************************************************
** Form implementation generated from reading ui file 'hlightDialog.ui'
**
** Created: Tue Jul 2 19:03:31 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "hlightDialog.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a HLightForm which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
HLightForm::HLightForm( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "HLightForm" );
    resize( 261, 369 ); 
    setCaption( trUtf8( "Current Highlight Criteria List" ) );
    HLightFormLayout = new QGridLayout( this, 1, 1, 11, 6, "HLightFormLayout"); 

    AcceptButton = new QPushButton( this, "AcceptButton" );
    AcceptButton->setText( trUtf8( "&Accept" ) );

    HLightFormLayout->addWidget( AcceptButton, 1, 0 );

    qlv = new QListView( this, "qlv" );
    qlv->addColumn( trUtf8( "Color" ) );
    qlv->header()->setClickEnabled( FALSE, qlv->header()->count() - 1 );
    qlv->addColumn( trUtf8( "Tag" ) );
    qlv->header()->setClickEnabled( FALSE, qlv->header()->count() - 1 );
    qlv->addColumn( trUtf8( "Value" ) );
    qlv->header()->setClickEnabled( FALSE, qlv->header()->count() - 1 );

    HLightFormLayout->addMultiCellWidget( qlv, 0, 0, 0, 1 );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    HLightFormLayout->addItem( spacer, 1, 1 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
HLightForm::~HLightForm()
{
    // no need to delete child widgets, Qt does it all for us
}


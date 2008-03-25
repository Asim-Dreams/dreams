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
** Form implementation generated from reading ui file 'error.ui'
**
** Created: Thu May 30 15:40:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "error.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a Error which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Error::Error( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
    setName( "Error" );
    resize( 212, 76 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setCaption( trUtf8( "Error!!!" ) );
    setSizeGripEnabled( TRUE );
    ErrorLayout = new QGridLayout( this, 1, 1, 11, 6, "ErrorLayout"); 

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "OK" ) );
    buttonOk->setAccel( 0 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    Layout2->addLayout( Layout1, 1, 0 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( trUtf8( "This file has been opened before" ) );

    Layout2->addWidget( TextLabel1, 0, 0 );

    ErrorLayout->addLayout( Layout2, 0, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Error::~Error()
{
    // no need to delete child widgets, Qt does it all for us
}


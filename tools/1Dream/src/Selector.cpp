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
** Form implementation generated from reading ui file 'Selector.ui'
**
** Created: Tue Jul 9 08:42:48 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "Selector.h"

#include <qvariant.h>
#include <qframe.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a Selector which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Selector::Selector( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
    setName( "Selector" );
    resize( 796, 677 ); 
    setCaption( trUtf8( "StripChart Selector" ) );
    setSizeGripEnabled( TRUE );
    SelectorLayout = new QGridLayout( this, 1, 1, 11, 6, "SelectorLayout"); 

    StripSelector = new QFrame( this, "StripSelector" );
    StripSelector->setFrameShape( QFrame::StyledPanel );
    StripSelector->setFrameShadow( QFrame::Raised );
    StripSelectorLayout = new QGridLayout( StripSelector, 1, 1, 11, 6, "StripSelectorLayout"); 

    Layout43 = new QVBoxLayout( 0, 0, 6, "Layout43"); 

    FilesOpened = new QListBox( StripSelector, "FilesOpened" );
    FilesOpened->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, FilesOpened->sizePolicy().hasHeightForWidth() ) );
    FilesOpened->setMinimumSize( QSize( 0, 150 ) );
    FilesOpened->setMaximumSize( QSize( 32767, 150 ) );
    Layout43->addWidget( FilesOpened );

    Layout12 = new QGridLayout( 0, 1, 1, 0, 6, "Layout12"); 

    StripsAvailable = new QListBox( StripSelector, "StripsAvailable" );

    Layout12->addWidget( StripsAvailable, 1, 0 );

    ThreadsOnStrip = new QListBox( StripSelector, "ThreadsOnStrip" );
    ThreadsOnStrip->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, ThreadsOnStrip->sizePolicy().hasHeightForWidth() ) );
    ThreadsOnStrip->setMinimumSize( QSize( 200, 0 ) );

    Layout12->addWidget( ThreadsOnStrip, 1, 1 );
    Layout43->addLayout( Layout12 );

    StripSelectorLayout->addLayout( Layout43, 0, 0 );

    SelectorLayout->addWidget( StripSelector, 0, 0 );

    Layout44 = new QHBoxLayout( 0, 0, 6, "Layout44"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "Help" ) );
    buttonHelp->setAccel( 4144 );
    buttonHelp->setAutoDefault( TRUE );
    Layout44->addWidget( buttonHelp );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout44->addItem( spacer );

    Properties = new QPushButton( this, "Properties" );
    Properties->setText( trUtf8( "Properties..." ) );
    Layout44->addWidget( Properties );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "OK" ) );
    buttonOk->setAccel( 0 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout44->addWidget( buttonOk );

    SelectorLayout->addLayout( Layout44, 1, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Selector::~Selector()
{
    // no need to delete child widgets, Qt does it all for us
}


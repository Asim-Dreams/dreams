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
** Form implementation generated from reading ui file 'colors.ui'
**
** Created: Mon Jul 15 12:20:57 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "colors.h"

#include <qvariant.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a Colors which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Colors::Colors( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
    setName( "Colors" );
    resize( 190, 257 ); 
    setCaption( trUtf8( "Selected Strips" ) );
    ColorsLayout = new QGridLayout( this, 1, 1, 11, 6, "ColorsLayout"); 

    listColors = new QListBox( this, "listColors" );

    ColorsLayout->addWidget( listColors, 0, 0 );

    listColorsActived = new QListBox( this, "listColorsActived" );

    ColorsLayout->addWidget( listColorsActived, 2, 0 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    Down = new QPushButton( this, "Down" );
    Down->setText( trUtf8( "Hide" ) );
    Layout1->addWidget( Down );

    Up = new QPushButton( this, "Up" );
    Up->setText( trUtf8( "Unhide" ) );
    Layout1->addWidget( Up );

    ColorsLayout->addLayout( Layout1, 1, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Colors::~Colors()
{
    // no need to delete child widgets, Qt does it all for us
}


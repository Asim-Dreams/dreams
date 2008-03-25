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
** Form implementation generated from reading ui file 'Properties.ui'
**
** Created: Tue May 28 16:47:49 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "Properties.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a Properties which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Properties::Properties( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
    setName( "Properties" );
    resize( 423, 392 ); 
    setCaption( trUtf8( "Properties" ) );
    setSizeGripEnabled( TRUE );
    PropertiesLayout = new QGridLayout( this, 1, 1, 11, 6, "PropertiesLayout"); 

    Layout35 = new QVBoxLayout( 0, 0, 6, "Layout35"); 

    Layout28 = new QHBoxLayout( 0, 0, 6, "Layout28"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( trUtf8( "Horizontal Grid Size" ) );
    Layout28->addWidget( TextLabel1 );

    HGridSize = new QSpinBox( this, "HGridSize" );
    HGridSize->setMaxValue( 1000000000 );
    Layout28->addWidget( HGridSize );
    Layout35->addLayout( Layout28 );

    Layout28_3 = new QHBoxLayout( 0, 0, 6, "Layout28_3"); 

    TextLabel1_3 = new QLabel( this, "TextLabel1_3" );
    TextLabel1_3->setText( trUtf8( "Horizontal Grid Steps" ) );
    Layout28_3->addWidget( TextLabel1_3 );

    HGridSteps = new QSpinBox( this, "HGridSteps" );
    HGridSteps->setMaxValue( 100000 );
    Layout28_3->addWidget( HGridSteps );
    Layout35->addLayout( Layout28_3 );

    Layout28_2 = new QHBoxLayout( 0, 0, 6, "Layout28_2"); 

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    TextLabel1_2->setText( trUtf8( "Vertical Grid Size" ) );
    Layout28_2->addWidget( TextLabel1_2 );

    VGridSize = new QSpinBox( this, "VGridSize" );
    VGridSize->setMaxValue( 1000000000 );
    Layout28_2->addWidget( VGridSize );
    Layout35->addLayout( Layout28_2 );

    Layout28_2_2 = new QHBoxLayout( 0, 0, 6, "Layout28_2_2"); 

    TextLabel1_2_2 = new QLabel( this, "TextLabel1_2_2" );
    TextLabel1_2_2->setText( trUtf8( "Vertical Grid Steps" ) );
    Layout28_2_2->addWidget( TextLabel1_2_2 );

    VGridSteps = new QSpinBox( this, "VGridSteps" );
    VGridSteps->setMaxValue( 100000 );
    Layout28_2_2->addWidget( VGridSteps );
    Layout35->addLayout( Layout28_2_2 );

    HGridColor = new QPushButton( this, "HGridColor" );
    HGridColor->setText( trUtf8( "Horizontal Grid Color" ) );
    Layout35->addWidget( HGridColor );

    VGridColor = new QPushButton( this, "VGridColor" );
    VGridColor->setText( trUtf8( "Vertical Grid Color" ) );
    Layout35->addWidget( VGridColor );

    MarkersLineColor = new QPushButton( this, "MarkersLineColor" );
    MarkersLineColor->setText( trUtf8( "Markers Line Color" ) );
    Layout35->addWidget( MarkersLineColor );

    HAxisColor = new QPushButton( this, "HAxisColor" );
    HAxisColor->setText( trUtf8( "Horizontal Axis Color" ) );
    Layout35->addWidget( HAxisColor );

    VAxisColor = new QPushButton( this, "VAxisColor" );
    VAxisColor->setText( trUtf8( "Vertical Axis Color" ) );
    Layout35->addWidget( VAxisColor );

    BackgroundColor = new QPushButton( this, "BackgroundColor" );
    BackgroundColor->setText( trUtf8( "Background Color" ) );
    Layout35->addWidget( BackgroundColor );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "Help" ) );
    buttonHelp->setAccel( 4144 );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "OK" ) );
    buttonOk->setAccel( 0 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );
    Layout35->addLayout( Layout1 );

    PropertiesLayout->addLayout( Layout35, 0, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Properties::~Properties()
{
    // no need to delete child widgets, Qt does it all for us
}


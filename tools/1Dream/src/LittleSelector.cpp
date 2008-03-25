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
** Form implementation generated from reading ui file 'LittleSelector.ui'
**
** Created: Mon Jul 15 15:03:37 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "LittleSelector.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a LittleSelector which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
LittleSelector::LittleSelector( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
    setName( "LittleSelector" );
    resize( 388, 351 ); 
    setCaption( trUtf8( "StripChart Selector" ) );
    setSizeGripEnabled( TRUE );
    LittleSelectorLayout = new QGridLayout( this, 1, 1, 11, 6, "LittleSelectorLayout"); 

    StripProperties = new QFrame( this, "StripProperties" );
    StripProperties->setFrameShape( QFrame::StyledPanel );
    StripProperties->setFrameShadow( QFrame::Raised );
    StripPropertiesLayout = new QGridLayout( StripProperties, 1, 1, 11, 6, "StripPropertiesLayout"); 

    Layout29 = new QHBoxLayout( 0, 0, 6, "Layout29"); 

    Visible = new QCheckBox( StripProperties, "Visible" );
    Visible->setText( trUtf8( "Visible" ) );
    Layout29->addWidget( Visible );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout29->addItem( spacer );

    Hidden = new QCheckBox( StripProperties, "Hidden" );
    Hidden->setText( trUtf8( "Hidden" ) );
    Layout29->addWidget( Hidden );

    StripPropertiesLayout->addLayout( Layout29, 0, 0 );

    Tablet = new QTabWidget( StripProperties, "Tablet" );

    tab = new QWidget( Tablet, "tab" );
    tabLayout = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 

    Layout47 = new QVBoxLayout( 0, 0, 6, "Layout47"); 

    Layout46 = new QVBoxLayout( 0, 0, 6, "Layout46"); 

    TextLabel4 = new QLabel( tab, "TextLabel4" );
    TextLabel4->setText( trUtf8( "Scale:" ) );
    Layout46->addWidget( TextLabel4 );

    Layout45 = new QHBoxLayout( 0, 0, 6, "Layout45"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 1, 20, QSizePolicy::Maximum, QSizePolicy::Minimum );
    Layout45->addItem( spacer_2 );

    Layout16 = new QVBoxLayout( 0, 0, 6, "Layout16"); 

    Absolute = new QCheckBox( tab, "Absolute" );
    Absolute->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, Absolute->sizePolicy().hasHeightForWidth() ) );
    Absolute->setMaximumSize( QSize( 300, 32767 ) );
    Absolute->setText( trUtf8( "Absolute" ) );
    Layout16->addWidget( Absolute );

    Layout15 = new QHBoxLayout( 0, 0, 6, "Layout15"); 

    Layout14 = new QVBoxLayout( 0, 0, 6, "Layout14"); 

    RelativeHardwareMAX = new QCheckBox( tab, "RelativeHardwareMAX" );
    RelativeHardwareMAX->setText( trUtf8( "Relative to hardware maximum" ) );
    Layout14->addWidget( RelativeHardwareMAX );

    RelativeFileMAX = new QCheckBox( tab, "RelativeFileMAX" );
    RelativeFileMAX->setText( trUtf8( "Relative to file maximum" ) );
    Layout14->addWidget( RelativeFileMAX );
    Layout15->addLayout( Layout14 );
    QSpacerItem* spacer_3 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout15->addItem( spacer_3 );

    Layout13 = new QVBoxLayout( 0, 0, 6, "Layout13"); 

    HardwareMAX = new QLabel( tab, "HardwareMAX" );
    HardwareMAX->setLineWidth( 1 );
    HardwareMAX->setText( trUtf8( "      0" ) );
    HardwareMAX->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    Layout13->addWidget( HardwareMAX );

    FileMAX = new QLabel( tab, "FileMAX" );
    FileMAX->setText( trUtf8( "      0" ) );
    FileMAX->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    Layout13->addWidget( FileMAX );
    Layout15->addLayout( Layout13 );
    Layout16->addLayout( Layout15 );
    Layout45->addLayout( Layout16 );
    Layout46->addLayout( Layout45 );
    Layout47->addLayout( Layout46 );

    Layout21 = new QHBoxLayout( 0, 0, 6, "Layout21"); 

    SkewY = new QCheckBox( tab, "SkewY" );
    SkewY->setEnabled( FALSE );
    SkewY->setText( trUtf8( "Skew    " ) );
    Layout21->addWidget( SkewY );

    SkewYValue = new QSpinBox( tab, "SkewYValue" );
    SkewYValue->setEnabled( FALSE );
    SkewYValue->setMaxValue( 1000000 );
    SkewYValue->setMinValue( -1000000 );
    Layout21->addWidget( SkewYValue );
    Layout47->addLayout( Layout21 );

    tabLayout->addLayout( Layout47, 0, 0 );
    Tablet->insertTab( tab, trUtf8( "Y axis" ) );

    tab_2 = new QWidget( Tablet, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2, 1, 1, 11, 6, "tabLayout_2"); 

    Layout23 = new QHBoxLayout( 0, 0, 6, "Layout23"); 

    SkewX = new QCheckBox( tab_2, "SkewX" );
    SkewX->setEnabled( FALSE );
    SkewX->setText( trUtf8( "Skew" ) );
    Layout23->addWidget( SkewX );

    SkewXValue = new QSpinBox( tab_2, "SkewXValue" );
    SkewXValue->setEnabled( FALSE );
    SkewXValue->setMaxValue( 1000000 );
    SkewXValue->setMinValue( -1000000 );
    Layout23->addWidget( SkewXValue );

    tabLayout_2->addLayout( Layout23, 0, 0 );
    Tablet->insertTab( tab_2, trUtf8( "X axis" ) );

    tab_3 = new QWidget( Tablet, "tab_3" );
    tabLayout_3 = new QGridLayout( tab_3, 1, 1, 11, 6, "tabLayout_3"); 

    Layout26 = new QVBoxLayout( 0, 0, 6, "Layout26"); 

    Layout24 = new QHBoxLayout( 0, 0, 6, "Layout24"); 

    BreakView = new QCheckBox( tab_3, "BreakView" );
    BreakView->setText( trUtf8( "Break View" ) );
    Layout24->addWidget( BreakView );

    TextLabel1_2 = new QLabel( tab_3, "TextLabel1_2" );
    TextLabel1_2->setText( trUtf8( "Pixels:" ) );
    Layout24->addWidget( TextLabel1_2 );

    BreakValue = new QSpinBox( tab_3, "BreakValue" );
    BreakValue->setMaxValue( 1000000 );
    BreakValue->setMinValue( -1000000 );
    Layout24->addWidget( BreakValue );
    Layout26->addLayout( Layout24 );

    Layout25 = new QHBoxLayout( 0, 0, 6, "Layout25"); 

    Smooth = new QCheckBox( tab_3, "Smooth" );
    Smooth->setText( trUtf8( "Smooth" ) );
    Layout25->addWidget( Smooth );

    TextLabel1 = new QLabel( tab_3, "TextLabel1" );
    TextLabel1->setText( trUtf8( "Pixels:" ) );
    Layout25->addWidget( TextLabel1 );

    SmoothValue = new QSpinBox( tab_3, "SmoothValue" );
    SmoothValue->setMaxValue( 1000000 );
    SmoothValue->setMinValue( -1000000 );
    Layout25->addWidget( SmoothValue );
    Layout26->addLayout( Layout25 );

    tabLayout_3->addLayout( Layout26, 0, 0 );
    Tablet->insertTab( tab_3, trUtf8( "Data filters" ) );

    tab_4 = new QWidget( Tablet, "tab_4" );
    tabLayout_4 = new QGridLayout( tab_4, 1, 1, 11, 6, "tabLayout_4"); 

    Layout28 = new QVBoxLayout( 0, 0, 6, "Layout28"); 

    Color = new QPushButton( tab_4, "Color" );
    Color->setText( trUtf8( "Color" ) );
    Color->setToggleButton( FALSE );
    Color->setOn( FALSE );
    Color->setAutoRepeat( FALSE );
    Color->setAutoDefault( FALSE );
    Color->setDefault( FALSE );
    Color->setFlat( FALSE );
    Layout28->addWidget( Color );

    Layout27 = new QHBoxLayout( 0, 0, 6, "Layout27"); 

    LineWidth = new QLabel( tab_4, "LineWidth" );
    LineWidth->setText( trUtf8( "Line Width" ) );
    Layout27->addWidget( LineWidth );

    LineWidthValue = new QSpinBox( tab_4, "LineWidthValue" );
    LineWidthValue->setMaxValue( 10 );
    LineWidthValue->setMinValue( 1 );
    Layout27->addWidget( LineWidthValue );
    Layout28->addLayout( Layout27 );

    LineStyle = new QComboBox( FALSE, tab_4, "LineStyle" );
    LineStyle->insertItem( trUtf8( "SolidLine" ) );
    LineStyle->insertItem( trUtf8( "DashLine" ) );
    LineStyle->insertItem( trUtf8( "DotLine" ) );
    LineStyle->insertItem( trUtf8( "DashDotLine" ) );
    LineStyle->insertItem( trUtf8( "DashDotDotLine" ) );
    Layout28->addWidget( LineStyle );

    tabLayout_4->addLayout( Layout28, 0, 0 );
    Tablet->insertTab( tab_4, trUtf8( "Line Properties" ) );

    StripPropertiesLayout->addWidget( Tablet, 1, 0 );

    LittleSelectorLayout->addWidget( StripProperties, 1, 0 );

    Layout11 = new QVBoxLayout( 0, 0, 6, "Layout11"); 

    Filename = new QLabel( this, "Filename" );
    Filename->setText( trUtf8( "Filename:" ) );
    Layout11->addWidget( Filename );

    StripChart = new QLabel( this, "StripChart" );
    StripChart->setText( trUtf8( "StripChart:" ) );
    Layout11->addWidget( StripChart );

    Thread = new QLabel( this, "Thread" );
    Thread->setText( trUtf8( "Thread:" ) );
    Layout11->addWidget( Thread );

    LittleSelectorLayout->addLayout( Layout11, 0, 0 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setText( trUtf8( "Help" ) );
    buttonHelp->setAccel( 4144 );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer_4 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer_4 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( trUtf8( "OK" ) );
    buttonOk->setAccel( 0 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    LittleSelectorLayout->addLayout( Layout1, 2, 0 );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
LittleSelector::~LittleSelector()
{
    // no need to delete child widgets, Qt does it all for us
}


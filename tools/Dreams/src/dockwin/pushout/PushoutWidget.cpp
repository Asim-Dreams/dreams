/*
 * Copyright (C) 2005-2006 Intel Corporation
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
** Form implementation generated from reading ui file 'PushoutWidget.ui'
**
** Created: Tue Apr 12 14:40:58 2005
**      by: The User Interface Compiler ($Id: PushoutWidget.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "PushoutWidget.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a PushoutWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
PushoutWidget::PushoutWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "PushoutWidget" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );

    QWidget* privateLayoutWidget = new QWidget( this, "layout4" );
    privateLayoutWidget->setGeometry( QRect( 10, 210, 228, 64 ) );
    layout4 = new QVBoxLayout( privateLayoutWidget, 11, 6, "layout4"); 

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    nextButton = new QPushButton( privateLayoutWidget, "nextButton" );
    layout2->addWidget( nextButton );
    spacer1 = new QSpacerItem( 41, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer1 );

    prevButton = new QPushButton( privateLayoutWidget, "prevButton" );
    layout2->addWidget( prevButton );
    layout4->addLayout( layout2 );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 
    spacer2 = new QSpacerItem( 61, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacer2 );

    syncButton = new QPushButton( privateLayoutWidget, "syncButton" );
    layout3->addWidget( syncButton );
    spacer2_2 = new QSpacerItem( 61, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacer2_2 );
    layout4->addLayout( layout3 );
    languageChange();
    resize( QSize(249, 287).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PushoutWidget::~PushoutWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PushoutWidget::languageChange()
{
    setCaption( tr( "Pushout Docked Window" ) );
    nextButton->setText( tr( "&Next" ) );
    nextButton->setAccel( QKeySequence( tr( "Alt+N" ) ) );
    prevButton->setText( tr( "&Prev" ) );
    prevButton->setAccel( QKeySequence( tr( "Alt+P" ) ) );
    syncButton->setText( tr( "&Synchronize" ) );
    syncButton->setAccel( QKeySequence( tr( "Alt+S" ) ) );
}


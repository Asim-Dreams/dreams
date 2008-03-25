/*
 * Copyright (C) 2006 Intel Corporation
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
** Form implementation generated from reading ui file 'ViewChoiceDialog.ui'
**
** Created: Fri Jan 27 18:13:45 2006
**      by: The User Interface Compiler ($Id: ViewChoiceDialog.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "ViewChoiceDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a ViewChoiceDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ViewChoiceDialog::ViewChoiceDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ViewChoiceDialog" );

    pushButton1 = new QPushButton( this, "pushButton1" );
    pushButton1->setGeometry( QRect( 270, 230, 94, 28 ) );

    buttonGroup2 = new QButtonGroup( this, "buttonGroup2" );
    buttonGroup2->setGeometry( QRect( 20, 30, 340, 190 ) );

    QWidget* privateLayoutWidget = new QWidget( buttonGroup2, "layout1" );
    privateLayoutWidget->setGeometry( QRect( 20, 40, 300, 120 ) );
    layout1 = new QVBoxLayout( privateLayoutWidget, 11, 6, "layout1"); 

    checkBox2 = new QCheckBox( privateLayoutWidget, "checkBox2" );
    layout1->addWidget( checkBox2 );

    checkBox3 = new QCheckBox( privateLayoutWidget, "checkBox3" );
    layout1->addWidget( checkBox3 );

    checkBox4 = new QCheckBox( privateLayoutWidget, "checkBox4" );
    layout1->addWidget( checkBox4 );

    checkBox5 = new QCheckBox( privateLayoutWidget, "checkBox5" );
    layout1->addWidget( checkBox5 );

    timeout = new QLabel( buttonGroup2, "timeout" );
    timeout->setGeometry( QRect( 21, 160, 300, 21 ) );
    timeout->setAlignment( int( QLabel::AlignCenter ) );
    languageChange();
    resize( QSize(389, 278).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton1, SIGNAL( released() ), this, SLOT( accept() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ViewChoiceDialog::~ViewChoiceDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ViewChoiceDialog::languageChange()
{
    setCaption( tr( "Choose the views to compute" ) );
    pushButton1->setText( tr( "&Accept" ) );
    pushButton1->setAccel( QKeySequence( tr( "Alt+A" ) ) );
    buttonGroup2->setTitle( tr( "Available Views" ) );
    checkBox2->setText( tr( "Resource" ) );
    checkBox3->setText( tr( "Floorplan" ) );
    checkBox4->setText( tr( "WatchWindow" ) );
    checkBox5->setText( tr( "WaterFall" ) );
    timeout->setText( tr( "Autoloading in 0 seconds" ) );
}


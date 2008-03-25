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
** Form implementation generated from reading ui file 'GoToCycleDialog.ui'
**
** Created: Tue May 24 14:51:33 2005
**      by: The User Interface Compiler ($Id: GoToCycleDialog.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "GoToCycleDialog.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a GoToCycleDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
GoToCycleDialog::GoToCycleDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "GoToCycleDialog" );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 10, 10, 261, 21 ) );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 10, 70, 261, 21 ) );

    comboBox1 = new QComboBox( FALSE, this, "comboBox1" );
    comboBox1->setGeometry( QRect( 10, 30, 370, 31 ) );

    buttonGroup2 = new QButtonGroup( this, "buttonGroup2" );
    buttonGroup2->setGeometry( QRect( 100, 140, 210, 60 ) );
    buttonGroup2->setExclusive( TRUE );

    radioButton1_2 = new QRadioButton( buttonGroup2, "radioButton1_2" );
    radioButton1_2->setGeometry( QRect( 108, 21, 81, 21 ) );

    radioButton1 = new QRadioButton( buttonGroup2, "radioButton1" );
    radioButton1->setGeometry( QRect( 21, 21, 81, 21 ) );
    radioButton1->setChecked( TRUE );

    spinBox1 = new QSpinBox( this, "spinBox1" );
    spinBox1->setGeometry( QRect( 10, 100, 370, 30 ) );

    pushButton1 = new QPushButton( this, "pushButton1" );
    pushButton1->setGeometry( QRect( 260, 210, 40, 30 ) );

    pushButton1_2 = new QPushButton( this, "pushButton1_2" );
    pushButton1_2->setGeometry( QRect( 310, 210, 70, 30 ) );
    pushButton1_2->setAutoDefault( FALSE );
    languageChange();
    resize( QSize(394, 250).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton1, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pushButton1_2, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( spinBox1, comboBox1 );
    setTabOrder( comboBox1, radioButton1 );
    setTabOrder( radioButton1, radioButton1_2 );
    setTabOrder( radioButton1_2, pushButton1 );
    setTabOrder( pushButton1, pushButton1_2 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
GoToCycleDialog::~GoToCycleDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void GoToCycleDialog::languageChange()
{
    setCaption( tr( "Go To Cycle" ) );
    textLabel1_2->setText( tr( "Select Clock Domain:" ) );
    textLabel1->setText( tr( "Enter Cycle Number:" ) );
    buttonGroup2->setTitle( tr( "Phase" ) );
    radioButton1_2->setText( tr( "Low" ) );
    radioButton1->setText( tr( "High" ) );
    pushButton1->setText( tr( "OK" ) );
    pushButton1_2->setText( tr( "Cancel" ) );
}


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
** Form implementation generated from reading ui file 'TagDialog.ui'
**
** Created: Wed May 25 12:51:03 2005
**      by: The User Interface Compiler ($Id: TagDialog.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "TagDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const unsigned char image0_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x00,
    0xc2, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xc5, 0x92, 0xb1, 0x0d, 0xc2,
    0x30, 0x10, 0x45, 0x1f, 0x28, 0xc5, 0x35, 0x0c, 0xc0, 0x00, 0x99, 0xc0,
    0x1b, 0xd0, 0x33, 0x00, 0x3d, 0x2b, 0xd0, 0xa7, 0xcf, 0x04, 0x8e, 0xd8,
    0x08, 0x67, 0x01, 0x68, 0x91, 0xe8, 0x23, 0x21, 0x5d, 0xc5, 0x51, 0x38,
    0x09, 0xa0, 0x24, 0x82, 0x84, 0x22, 0x5f, 0xb2, 0xfc, 0x2d, 0xff, 0x7f,
    0xfe, 0x67, 0x1d, 0xcc, 0x8d, 0xc5, 0x64, 0xa7, 0xc7, 0x22, 0xc9, 0x1b,
    0x32, 0x02, 0x39, 0xd6, 0xf8, 0x96, 0xa3, 0x8a, 0x78, 0xcc, 0x79, 0x67,
    0xdc, 0x81, 0xc3, 0x7b, 0xfa, 0x80, 0xbd, 0x22, 0x0d, 0xbc, 0x58, 0x62,
    0x2e, 0x38, 0x23, 0xeb, 0xd3, 0x79, 0x4c, 0x2a, 0xb1, 0x4e, 0x92, 0x0c,
    0x23, 0xd4, 0x77, 0x81, 0x5e, 0x73, 0x02, 0xc0, 0xad, 0x3e, 0x6d, 0x6a,
    0x13, 0xc0, 0x1a, 0x64, 0x27, 0xe8, 0x4a, 0xd1, 0x42, 0xe1, 0x04, 0x1c,
    0xbb, 0x9f, 0x1e, 0x0b, 0x5c, 0x41, 0x2f, 0x8a, 0xa4, 0x82, 0x6e, 0x15,
    0x00, 0x49, 0x25, 0x2a, 0x0a, 0x06, 0xcd, 0x9d, 0x36, 0x28, 0xe3, 0x92,
    0x4a, 0x22, 0xef, 0xed, 0xf7, 0x13, 0xcb, 0x96, 0x95, 0xd0, 0xc8, 0xf5,
    0xac, 0xe0, 0x81, 0xec, 0xfb, 0x9c, 0x24, 0x2d, 0x7b, 0xd4, 0x7b, 0xf8,
    0x35, 0x72, 0x1f, 0x32, 0x8c, 0xfd, 0x84, 0xc1, 0x6a, 0xe1, 0xfe, 0x31,
    0xcf, 0x85, 0x27, 0x59, 0x89, 0x4d, 0xd7, 0x20, 0x17, 0xaf, 0xfc, 0x00,
    0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};


/*
 *  Constructs a TagDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
TagDialog::TagDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QImage img;
    img.loadFromData( image0_data, sizeof( image0_data ), "PNG" );
    image0 = img;
    if ( !name )
	setName( "TagDialog" );
    TagDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "TagDialogLayout"); 

    layout19 = new QGridLayout( 0, 1, 1, 0, 6, "layout19"); 

    layout18 = new QHBoxLayout( 0, 0, 6, "layout18"); 
    Spacer1 = new QSpacerItem( 209, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout18->addItem( Spacer1 );

    AcceptButton = new QPushButton( this, "AcceptButton" );
    AcceptButton->setEnabled( TRUE );
    AcceptButton->setMinimumSize( QSize( 75, 0 ) );
    AcceptButton->setPixmap( image0 );
    layout18->addWidget( AcceptButton );

    layout19->addLayout( layout18, 1, 0 );

    EventListView = new QListView( this, "EventListView" );
    EventListView->addColumn( tr( "Tag" ) );
    EventListView->addColumn( tr( "Value" ) );
    EventListView->addColumn( tr( "Cycle" ) );
    EventListView->addColumn( tr( "Division" ) );
    EventListView->addColumn( tr( "Domain" ) );
    EventListView->setSelectionMode( QListView::Single );
    EventListView->setAllColumnsShowFocus( TRUE );
    EventListView->setResizeMode( QListView::AllColumns );

    layout19->addWidget( EventListView, 0, 0 );

    TagDialogLayout->addLayout( layout19, 0, 0 );
    languageChange();
    resize( QSize(321, 254).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( AcceptButton, SIGNAL( clicked() ), this, SLOT( AcceptButton_clicked() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
TagDialog::~TagDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TagDialog::languageChange()
{
    setCaption( tr( "Tags for selected event" ) );
    AcceptButton->setText( QString::null );
    AcceptButton->setAccel( QKeySequence( QString::null ) );
    EventListView->header()->setLabel( 0, tr( "Tag" ) );
    EventListView->header()->setLabel( 1, tr( "Value" ) );
    EventListView->header()->setLabel( 2, tr( "Cycle" ) );
    EventListView->header()->setLabel( 3, tr( "Division" ) );
    EventListView->header()->setLabel( 4, tr( "Domain" ) );
    QToolTip::add( EventListView, tr( "The warning symbol pn `when' means other values are available in different cycles." ) );
}

void TagDialog::AcceptButton_clicked()
{
    qWarning( "TagDialog::AcceptButton_clicked(): Not implemented yet" );
}


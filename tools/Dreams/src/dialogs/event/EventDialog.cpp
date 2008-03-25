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
** Form implementation generated from reading ui file 'EventDialog.ui'
**
** Created: Wed May 25 12:48:40 2005
**      by: The User Interface Compiler ($Id: EventDialog.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "EventDialog.h"

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
    0xc9, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xc5, 0x92, 0x31, 0x0a, 0x02,
    0x41, 0x0c, 0x45, 0x9f, 0x62, 0x91, 0xc6, 0xd2, 0x1b, 0x78, 0x83, 0x39,
    0x82, 0x8d, 0x95, 0x37, 0xb0, 0xf3, 0x0a, 0xf6, 0xd3, 0x5b, 0xef, 0xa2,
    0xe0, 0x7d, 0x84, 0x45, 0x14, 0x3b, 0x0b, 0x4b, 0x3b, 0x3b, 0x41, 0x48,
    0x21, 0xc6, 0x62, 0x66, 0x57, 0x65, 0x57, 0x74, 0xd7, 0xc2, 0x0f, 0x61,
    0xfe, 0x30, 0xf9, 0x99, 0x9f, 0x10, 0xf8, 0x37, 0x5a, 0x8d, 0x95, 0x09,
    0x16, 0xc8, 0x2c, 0x27, 0x35, 0x30, 0xc3, 0x72, 0x5d, 0xbb, 0x56, 0x91,
    0x04, 0x73, 0x0b, 0x67, 0x5c, 0x80, 0xe9, 0xb3, 0xfb, 0x0c, 0x7b, 0x58,
    0x7a, 0xf3, 0xe3, 0x06, 0x73, 0x6b, 0x67, 0xf8, 0xaa, 0xbc, 0x04, 0x93,
    0xb3, 0x58, 0xc9, 0x89, 0xc7, 0xc8, 0xe2, 0x5b, 0x46, 0xa5, 0xb8, 0x03,
    0xc0, 0x29, 0xde, 0x06, 0x51, 0x04, 0xd0, 0x03, 0x19, 0x0b, 0xda, 0x55,
    0x34, 0x55, 0xd8, 0x02, 0xcb, 0xf2, 0xd0, 0x43, 0x81, 0x23, 0xe8, 0x41,
    0x91, 0xbe, 0xa0, 0x23, 0x05, 0x40, 0xfa, 0x12, 0x32, 0x52, 0xde, 0x8a,
    0x4b, 0x6d, 0xb0, 0x09, 0x21, 0x67, 0x09, 0xbc, 0xb2, 0xdf, 0x57, 0xb4,
    0x0b, 0xb6, 0x03, 0xae, 0x81, 0xea, 0x5e, 0x61, 0x0e, 0xf8, 0xcf, 0x7b,
    0xd2, 0x29, 0xd8, 0x2d, 0x9e, 0xab, 0x6f, 0x2d, 0x57, 0xc1, 0x63, 0x4c,
    0x1a, 0x2c, 0x56, 0x81, 0xe1, 0x2f, 0xe2, 0x7f, 0xe1, 0x0e, 0x33, 0x50,
    0x4d, 0xd8, 0xee, 0x74, 0x9d, 0x34, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};


/*
 *  Constructs a EventDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
EventDialog::EventDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QImage img;
    img.loadFromData( image0_data, sizeof( image0_data ), "PNG" );
    image0 = img;
    if ( !name )
	setName( "EventDialog" );
    EventDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "EventDialogLayout"); 

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
    EventListView->addColumn( tr( "Type" ) );
    EventListView->addColumn( tr( "DralID" ) );
    EventListView->addColumn( tr( "Position" ) );
    EventListView->addColumn( tr( "Cycle" ) );
    EventListView->addColumn( tr( "Division" ) );
    EventListView->addColumn( tr( "Domain" ) );
    EventListView->setSelectionMode( QListView::Single );
    EventListView->setAllColumnsShowFocus( TRUE );
    EventListView->setResizeMode( QListView::AllColumns );

    layout19->addWidget( EventListView, 0, 0 );

    EventDialogLayout->addLayout( layout19, 0, 0 );
    languageChange();
    resize( QSize(469, 265).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( AcceptButton, SIGNAL( clicked() ), this, SLOT( AcceptButton_clicked() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
EventDialog::~EventDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void EventDialog::languageChange()
{
    setCaption( tr( "Tags for selected event" ) );
    AcceptButton->setText( QString::null );
    AcceptButton->setAccel( QKeySequence( QString::null ) );
    EventListView->header()->setLabel( 0, tr( "Type" ) );
    EventListView->header()->setLabel( 1, tr( "DralID" ) );
    EventListView->header()->setLabel( 2, tr( "Position" ) );
    EventListView->header()->setLabel( 3, tr( "Cycle" ) );
    EventListView->header()->setLabel( 4, tr( "Division" ) );
    EventListView->header()->setLabel( 5, tr( "Domain" ) );
    QToolTip::add( EventListView, tr( "The warning symbol pn `when' means other values are available in different cycles." ) );
}

void EventDialog::AcceptButton_clicked()
{
    qWarning( "EventDialog::AcceptButton_clicked(): Not implemented yet" );
}


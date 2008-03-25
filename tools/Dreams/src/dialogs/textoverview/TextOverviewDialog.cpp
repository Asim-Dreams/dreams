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
** Form implementation generated from reading ui file 'TextOverviewDialog.ui'
**
** Created: Wed May 25 09:57:36 2005
**      by: The User Interface Compiler ($Id: TextOverviewDialog.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "TextOverviewDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const unsigned char image0_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x01,
    0x91, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x85, 0x92, 0xcf, 0x6b, 0x13,
    0x51, 0x10, 0xc7, 0x3f, 0x29, 0x53, 0x9c, 0x07, 0x5b, 0x4c, 0xa4, 0x87,
    0x6c, 0xa9, 0xe0, 0x3b, 0x78, 0x88, 0xf4, 0x60, 0x0b, 0x1e, 0xf2, 0x27,
    0x14, 0xff, 0xd7, 0x5e, 0x8a, 0xf4, 0x56, 0x4f, 0x22, 0x88, 0xed, 0x49,
    0x58, 0x41, 0xb0, 0x9e, 0x9a, 0xdc, 0xf6, 0x89, 0x0b, 0x6f, 0x24, 0x0f,
    0xd6, 0xc3, 0xdb, 0x6c, 0x93, 0x34, 0xa9, 0x73, 0x99, 0x65, 0x1e, 0xf3,
    0xfd, 0xb5, 0x33, 0xb8, 0xf9, 0x72, 0xdf, 0x8e, 0x0a, 0x05, 0x81, 0x68,
    0x06, 0x28, 0x60, 0x54, 0xdf, 0x67, 0x1c, 0x1d, 0xff, 0x1d, 0xf0, 0x9f,
    0x12, 0xc4, 0xf0, 0x93, 0xf2, 0x61, 0xb2, 0x80, 0xd8, 0x00, 0x02, 0xd5,
    0xb7, 0xbb, 0xf6, 0xe8, 0x78, 0xf0, 0x24, 0xc8, 0x9e, 0x8a, 0xae, 0x4f,
    0xf6, 0xb3, 0x88, 0xf9, 0xaf, 0x9a, 0xf9, 0xac, 0xe6, 0xd3, 0xc7, 0xfb,
    0xf6, 0x49, 0x00, 0x4b, 0xf6, 0x78, 0x9a, 0x60, 0x72, 0xe2, 0x99, 0x4e,
    0xcf, 0xf0, 0xaf, 0x4b, 0x3e, 0x5c, 0xfe, 0xdc, 0x09, 0x22, 0x8f, 0x14,
    0x64, 0x63, 0x58, 0x63, 0x98, 0x19, 0xe5, 0xe1, 0x10, 0x9d, 0x4e, 0x78,
    0x26, 0x65, 0x6b, 0xc9, 0xb0, 0x26, 0xe2, 0x8a, 0xdf, 0xbd, 0x2d, 0xd9,
    0x54, 0x10, 0xff, 0x40, 0x68, 0x22, 0xb3, 0x10, 0x38, 0x7d, 0xeb, 0x71,
    0x07, 0x0f, 0x73, 0xb3, 0x88, 0x19, 0xdc, 0x7e, 0xad, 0xda, 0x17, 0xe3,
    0x1c, 0xf0, 0xba, 0x82, 0x08, 0x96, 0xf2, 0xe7, 0x50, 0x15, 0xb7, 0xf2,
    0x94, 0x81, 0x1c, 0xd6, 0x44, 0x42, 0x1d, 0xa8, 0x7e, 0xd4, 0x6d, 0x08,
    0x61, 0x3d, 0x83, 0xd8, 0x2f, 0x3b, 0xb6, 0x65, 0xe3, 0x24, 0xdb, 0x2b,
    0x5f, 0x8e, 0xf1, 0xbe, 0x44, 0x8b, 0xe1, 0xca, 0x5f, 0x58, 0xe4, 0xa6,
    0x02, 0x06, 0xec, 0xca, 0x66, 0x58, 0x38, 0xc6, 0x87, 0x23, 0xfc, 0x2b,
    0x8f, 0x02, 0x7b, 0xab, 0xec, 0x4e, 0xd6, 0x41, 0x36, 0x2b, 0x36, 0x30,
    0x0f, 0x75, 0x0f, 0x86, 0x80, 0x58, 0x63, 0x39, 0xa0, 0x06, 0x42, 0x8a,
    0x00, 0x59, 0x7e, 0xda, 0xdc, 0xce, 0xe1, 0x2a, 0x0a, 0x05, 0x8c, 0x24,
    0x5f, 0xaf, 0x5c, 0x5c, 0x5d, 0xa3, 0x57, 0x8a, 0x61, 0x28, 0xb9, 0x9b,
    0xc1, 0xe9, 0x89, 0xe7, 0xec, 0x5d, 0xd9, 0xdb, 0xab, 0x0d, 0x54, 0x1d,
    0x96, 0x22, 0x9a, 0xb2, 0x3d, 0x07, 0xc8, 0xfb, 0xf3, 0x37, 0x5b, 0x4f,
    0x35, 0x36, 0xcf, 0xfb, 0xe3, 0x89, 0xa9, 0xb3, 0x95, 0x40, 0xa5, 0x03,
    0x11, 0x47, 0x5c, 0xcd, 0x60, 0xb3, 0x54, 0xbb, 0x60, 0x17, 0x99, 0x69,
    0x99, 0x0d, 0xbd, 0xcd, 0xd8, 0x47, 0xb1, 0x1d, 0x40, 0xba, 0xb5, 0xfd,
    0xdc, 0x9c, 0x40, 0x34, 0xd0, 0xc2, 0x81, 0x45, 0x96, 0xb0, 0x3b, 0x01,
    0xae, 0x3f, 0xdf, 0x52, 0x55, 0x77, 0x99, 0x49, 0x5c, 0xc7, 0xbc, 0x94,
    0x0f, 0xa4, 0x7c, 0xee, 0xff, 0x00, 0xe8, 0x68, 0xb6, 0xe9, 0x6f, 0x97,
    0x44, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42,
    0x60, 0x82
};

static const unsigned char image1_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x01,
    0x14, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xa5, 0x93, 0xad, 0x4e, 0x03,
    0x51, 0x10, 0x85, 0xbf, 0xbb, 0xa9, 0x58, 0xb9, 0x06, 0x07, 0x0e, 0xd3,
    0x47, 0xe8, 0x24, 0x24, 0xf4, 0x26, 0x98, 0xca, 0x26, 0x88, 0xd6, 0xaf,
    0xe0, 0x05, 0x78, 0x14, 0x2c, 0x82, 0x07, 0x40, 0x54, 0xd4, 0x2d, 0xa2,
    0xc9, 0x94, 0xa4, 0x02, 0x92, 0x15, 0x15, 0x20, 0x36, 0x01, 0x89, 0xa8,
    0xa8, 0x18, 0xd1, 0xe4, 0x22, 0x58, 0xd8, 0x6e, 0x28, 0x3f, 0xdb, 0x3d,
    0xc9, 0x55, 0x37, 0xdf, 0xc9, 0x9c, 0x93, 0x19, 0xa7, 0xaa, 0xb4, 0x51,
    0xd4, 0x8a, 0x06, 0x3a, 0xfb, 0x82, 0x3e, 0x97, 0x40, 0x07, 0x22, 0x99,
    0x4b, 0x68, 0x0a, 0xcb, 0x5c, 0x82, 0xad, 0x21, 0xeb, 0xaa, 0x8b, 0x62,
    0x62, 0xfc, 0xc2, 0xff, 0xcb, 0x64, 0xf0, 0x34, 0x08, 0xe3, 0xd7, 0x71,
    0xc0, 0x40, 0x7b, 0xea, 0x00, 0xa2, 0xac, 0x97, 0xb9, 0xa4, 0x9b, 0xe0,
    0xf3, 0x9f, 0x4d, 0xfc, 0xc2, 0x87, 0x61, 0x3c, 0x0c, 0xc9, 0x51, 0x42,
    0xb1, 0x2c, 0xd0, 0xfe, 0x07, 0x0c, 0x65, 0x89, 0x2b, 0x5b, 0xc1, 0x08,
    0x64, 0x56, 0x8f, 0x23, 0x77, 0x12, 0x64, 0x23, 0x81, 0x14, 0x6c, 0x6d,
    0x14, 0xd3, 0x02, 0x3d, 0xab, 0x60, 0x28, 0x4b, 0xb4, 0xdc, 0xe0, 0x14,
    0x38, 0x07, 0xb9, 0x96, 0x80, 0x01, 0x27, 0x40, 0x0a, 0x1c, 0x80, 0x4d,
    0x0c, 0xbb, 0x35, 0x34, 0xad, 0xc3, 0x5f, 0x06, 0x3c, 0x97, 0xef, 0x18,
    0x18, 0x95, 0x3f, 0x87, 0x80, 0x01, 0x57, 0xc0, 0x3d, 0xe8, 0xc5, 0x77,
    0x18, 0xc0, 0x7d, 0x2e, 0x92, 0x2c, 0x25, 0xd0, 0x07, 0x36, 0x40, 0x0c,
    0xbc, 0x01, 0x53, 0x6a, 0x79, 0x77, 0xa9, 0x5a, 0xa4, 0xc7, 0x12, 0xb2,
    0x72, 0x9a, 0x9b, 0xbf, 0xe1, 0x2a, 0x02, 0xc0, 0xcb, 0x96, 0xc1, 0x84,
    0x9d, 0x79, 0x7f, 0x9d, 0x40, 0x2f, 0xd5, 0x31, 0x6b, 0x06, 0xd7, 0x0c,
    0x00, 0x78, 0x68, 0x06, 0xc3, 0x56, 0x89, 0xfb, 0xaa, 0xf5, 0x35, 0xbe,
    0x03, 0x1c, 0x9f, 0x69, 0xaa, 0xea, 0xf6, 0x40, 0xfd, 0x00, 0x00, 0x00,
    0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};


/*
 *  Constructs a TextOverviewDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
TextOverviewDialog::TextOverviewDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QImage img;
    img.loadFromData( image0_data, sizeof( image0_data ), "PNG" );
    image0 = img;
    img.loadFromData( image1_data, sizeof( image1_data ), "PNG" );
    image1 = img;
    if ( !name )
	setName( "TextOverviewDialog" );
    TextOverviewDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "TextOverviewDialogLayout"); 

    layout131 = new QVBoxLayout( 0, 0, 6, "layout131"); 

    layout128 = new QHBoxLayout( 0, 0, 6, "layout128"); 

    SAllButton = new QPushButton( this, "SAllButton" );
    layout128->addWidget( SAllButton );

    SNondeButton = new QPushButton( this, "SNondeButton" );
    layout128->addWidget( SNondeButton );
    spacer62 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout128->addItem( spacer62 );

    CopyButton = new QToolButton( this, "CopyButton" );
    CopyButton->setMinimumSize( QSize( 24, 24 ) );
    CopyButton->setIconSet( QIconSet( image0 ) );
    layout128->addWidget( CopyButton );
    layout131->addLayout( layout128 );

    textEdit = new QTextEdit( this, "textEdit" );
    layout131->addWidget( textEdit );

    layout129 = new QHBoxLayout( 0, 0, 6, "layout129"); 
    spacer63 = new QSpacerItem( 330, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout129->addItem( spacer63 );

    AcceptButton = new QToolButton( this, "AcceptButton" );
    AcceptButton->setMinimumSize( QSize( 75, 24 ) );
    AcceptButton->setIconSet( QIconSet( image1 ) );
    layout129->addWidget( AcceptButton );
    layout131->addLayout( layout129 );

    TextOverviewDialogLayout->addLayout( layout131, 0, 0 );
    languageChange();
    resize( QSize(600, 515).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // tab order
    setTabOrder( SAllButton, SNondeButton );
    setTabOrder( SNondeButton, textEdit );
}

/*
 *  Destroys the object and frees any allocated resources
 */
TextOverviewDialog::~TextOverviewDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TextOverviewDialog::languageChange()
{
    setCaption( tr( "Text Overview" ) );
    SAllButton->setText( tr( "Select &All" ) );
    SAllButton->setAccel( QKeySequence( tr( "Alt+A" ) ) );
    SNondeButton->setText( tr( "Select &None" ) );
    SNondeButton->setAccel( QKeySequence( tr( "Alt+N" ) ) );
    CopyButton->setText( QString::null );
    QToolTip::add( CopyButton, tr( "Copy" ) );
    AcceptButton->setText( QString::null );
}

void TextOverviewDialog::SAllButton_stateChanged(int)
{
    qWarning( "TextOverviewDialog::SAllButton_stateChanged(int): Not implemented yet" );
}


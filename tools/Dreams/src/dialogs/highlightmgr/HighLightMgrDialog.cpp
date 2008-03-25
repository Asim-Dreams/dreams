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
** Form implementation generated from reading ui file 'HighLightMgrDialog.ui'
**
** Created: Wed May 25 14:41:56 2005
**      by: The User Interface Compiler ($Id: HighLightMgrDialog.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "HighLightMgrDialog.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const unsigned char image0_data[] = { 
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
 *  Constructs a HighLightMgrDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
HighLightMgrDialog::HighLightMgrDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QImage img;
    img.loadFromData( image0_data, sizeof( image0_data ), "PNG" );
    image0 = img;
    if ( !name )
	setName( "HighLightMgrDialog" );
    HighLightMgrDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "HighLightMgrDialogLayout"); 

    layout22 = new QVBoxLayout( 0, 0, 6, "layout22"); 

    qlv = new QListView( this, "qlv" );
    qlv->addColumn( tr( "Color" ) );
    qlv->header()->setClickEnabled( FALSE, qlv->header()->count() - 1 );
    qlv->addColumn( tr( "Tag" ) );
    qlv->header()->setClickEnabled( FALSE, qlv->header()->count() - 1 );
    qlv->addColumn( tr( "Value" ) );
    qlv->header()->setClickEnabled( FALSE, qlv->header()->count() - 1 );
    layout22->addWidget( qlv );

    layout21 = new QHBoxLayout( 0, 0, 6, "layout21"); 
    Spacer2 = new QSpacerItem( 160, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout21->addItem( Spacer2 );

    AcceptButton = new QPushButton( this, "AcceptButton" );
    AcceptButton->setMinimumSize( QSize( 75, 0 ) );
    AcceptButton->setPixmap( image0 );
    layout21->addWidget( AcceptButton );
    layout22->addLayout( layout21 );

    HighLightMgrDialogLayout->addLayout( layout22, 0, 0 );
    languageChange();
    resize( QSize(263, 370).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
HighLightMgrDialog::~HighLightMgrDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void HighLightMgrDialog::languageChange()
{
    setCaption( tr( "Current Highlight Criteria List" ) );
    qlv->header()->setLabel( 0, tr( "Color" ) );
    qlv->header()->setLabel( 1, tr( "Tag" ) );
    qlv->header()->setLabel( 2, tr( "Value" ) );
    AcceptButton->setText( QString::null );
    AcceptButton->setAccel( QKeySequence( QString::null ) );
}


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
** Form implementation generated from reading ui file 'mtagform.ui'
**
** Created: Tue Feb 3 16:21:52 2004
**      by: The User Interface Compiler ($Id: mtagform.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "mtagform.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"16 16 45 1",
". c None",
"Q c #004800",
"M c #005000",
"D c #005d00",
"z c #006100",
"H c #006d00",
"L c #007100",
"s c #007500",
"P c #007900",
"v c #007d00",
"l c #008100",
"g c #008500",
"o c #008900",
"b c #008d00",
"C c #009500",
"E c #009d00",
"O c #00ae00",
"w c #00b200",
"N c #00b600",
"K c #00be00",
"p c #00c200",
"I c #00ca00",
"A c #00ce00",
"J c #00d200",
"B c #00d600",
"F c #00da00",
"u c #00de00",
"G c #00e200",
"x c #00ea00",
"t c #00f200",
"y c #00ff00",
"c c #088d08",
"d c #089108",
"# c #089900",
"i c #089908",
"a c #089d00",
"m c #08b208",
"n c #08f208",
"q c #08f608",
"e c #10a110",
"r c #10c210",
"h c #18ba18",
"j c #18ce18",
"f c #20aa20",
"k c #20be20",
"................",
"................",
"..............#a",
"b............bb.",
"cd.........efg..",
".hi......djkl...",
".mno...gpqrs....",
".gtuvswxywz.....",
".zAtBBtxCD......",
"..EuFGAg........",
"..HIJKL.........",
"..MNOz..........",
"...Pz...........",
"...Q............",
"................",
"................"};


/* 
 *  Constructs a MTagForm as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MTagForm::MTagForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
,
      image0( (const char **) image0_data )
{
    if ( !name )
	setName( "MTagForm" );
    MTagFormLayout = new QGridLayout( this, 1, 1, 11, 6, "MTagFormLayout"); 

    layout26 = new QVBoxLayout( 0, 0, 6, "layout26"); 

    MTagListView = new QListView( this, "MTagListView" );
    MTagListView->addColumn( tr( "Cycle" ) );
    MTagListView->addColumn( tr( "Value" ) );
    MTagListView->setSelectionMode( QListView::NoSelection );
    MTagListView->setShowSortIndicator( TRUE );
    MTagListView->setResizeMode( QListView::AllColumns );
    layout26->addWidget( MTagListView );

    layout25 = new QHBoxLayout( 0, 0, 6, "layout25"); 
    QSpacerItem* spacer = new QSpacerItem( 174, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout25->addItem( spacer );

    AcceptPushButton = new QPushButton( this, "AcceptPushButton" );
    AcceptPushButton->setMinimumSize( QSize( 75, 0 ) );
    AcceptPushButton->setPixmap( image0 );
    layout25->addWidget( AcceptPushButton );
    layout26->addLayout( layout25 );

    MTagFormLayout->addLayout( layout26, 0, 0 );
    languageChange();
    resize( QSize(279, 316).expandedTo(minimumSizeHint()) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MTagForm::~MTagForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MTagForm::languageChange()
{
    setCaption( tr( "Values for Mutable Tag" ) );
    MTagListView->header()->setLabel( 0, tr( "Cycle" ) );
    MTagListView->header()->setLabel( 1, tr( "Value" ) );
    AcceptPushButton->setText( QString::null );
    AcceptPushButton->setAccel( QKeySequence( QString::null ) );
}


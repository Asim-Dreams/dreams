/*
 * Copyright (C) 2004-2006 Intel Corporation
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
** Form implementation generated from reading ui file 'textOverview.ui'
**
** Created: Tue Feb 3 16:18:24 2004
**      by: The User Interface Compiler ($Id: textOverview.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "textOverview.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"16 16 56 1",
"e c None",
"Q c #73759c",
"G c #7b799c",
"r c #7b79a4",
"O c #7b7d9c",
"P c #837da4",
"1 c #8381a4",
"I c #8385ac",
"o c #8b85ac",
"F c #8b89ac",
"N c #8b8dac",
"T c #948dac",
"n c #9491b4",
"Z c #9495b4",
"0 c #9c95b4",
"W c #9c99b4",
"Y c #9c99bd",
"X c #9c9dbd",
"E c #a49dbd",
"R c #a4a1bd",
"k c #acaac5",
"d c #acaacd",
"z c #b4aecd",
"u c #b4b2cd",
"m c #bdb6d5",
"w c #bdbad5",
"t c #c5bed5",
"U c #c5c2d5",
"s c #c5c2de",
"v c #c5c6de",
"L c #cdc6de",
"H c #cdcade",
". c #cdcae6",
"f c #cdcede",
"J c #cdcee6",
"C c #d5cee6",
"b c #d5d2e6",
"c c #d5d2ee",
"j c #d5d6e6",
"# c #ded6ee",
"a c #dedaee",
"K c #dedeee",
"D c #e6def6",
"q c #e6e2f6",
"x c #e6e6f6",
"V c #eee6f6",
"M c #eee6ff",
"y c #eeeaf6",
"S c #eeeaff",
"B c #eeeeff",
"i c #f6eeff",
"p c #f6f2ff",
"l c #f6f6ff",
"A c #fff6ff",
"h c #fffaff",
"g c #ffffff",
".#abcbdeeeeeeeee",
"fggghijkeeeeeeee",
"bgggglmnoeeeeeee",
"fggggpq.kreeeeee",
"bggggl#stu.vweee",
"fgggpxvpggpyqzee",
"bggABxChgggpDEFG",
"fggpBiHhgggAqwkI",
"bghppBChggghiC.o",
"bglAppJhgggiKCJF",
"LpMqqK.hggAqDaDN",
"OPOPOQRgghBSxqqT",
"eeeeeewgglpSSqMN",
"eeeeeeUhhApiVqqN",
"eeeeeeLhghhppySN",
"eeeeeeNEWXWYZW01"};

static const char* const image1_data[] = { 
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
 *  Constructs a TextOverviewForm as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
TextOverviewForm::TextOverviewForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
,
      image0( (const char **) image0_data ),
      image1( (const char **) image1_data )
{
    if ( !name )
	setName( "TextOverviewForm" );
    TextOverviewFormLayout = new QGridLayout( this, 1, 1, 11, 6, "TextOverviewFormLayout"); 

    layout131 = new QVBoxLayout( 0, 0, 6, "layout131"); 

    layout128 = new QHBoxLayout( 0, 0, 6, "layout128"); 

    SAllButton = new QPushButton( this, "SAllButton" );
    layout128->addWidget( SAllButton );

    SNondeButton = new QPushButton( this, "SNondeButton" );
    layout128->addWidget( SNondeButton );
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout128->addItem( spacer );

    CopyButton = new QToolButton( this, "CopyButton" );
    CopyButton->setMinimumSize( QSize( 24, 24 ) );
    CopyButton->setIconSet( QIconSet( image0 ) );
    layout128->addWidget( CopyButton );
    layout131->addLayout( layout128 );

    textEdit = new QTextEdit( this, "textEdit" );
    layout131->addWidget( textEdit );

    layout129 = new QHBoxLayout( 0, 0, 6, "layout129"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 330, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout129->addItem( spacer_2 );

    AcceptButton = new QToolButton( this, "AcceptButton" );
    AcceptButton->setMinimumSize( QSize( 75, 24 ) );
    AcceptButton->setIconSet( QIconSet( image1 ) );
    layout129->addWidget( AcceptButton );
    layout131->addLayout( layout129 );

    TextOverviewFormLayout->addLayout( layout131, 0, 0 );
    languageChange();
    resize( QSize(600, 515).expandedTo(minimumSizeHint()) );

    // tab order
    setTabOrder( SAllButton, SNondeButton );
    setTabOrder( SNondeButton, textEdit );
}

/*
 *  Destroys the object and frees any allocated resources
 */
TextOverviewForm::~TextOverviewForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TextOverviewForm::languageChange()
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

void TextOverviewForm::SAllButton_stateChanged(int)
{
    qWarning( "TextOverviewForm::SAllButton_stateChanged(int): Not implemented yet" );
}


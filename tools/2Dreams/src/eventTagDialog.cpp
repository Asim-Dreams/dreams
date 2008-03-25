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
** Form implementation generated from reading ui file 'eventTagDialog.ui'
**
** Created: Tue Feb 3 16:17:59 2004
**      by: The User Interface Compiler ($Id: eventTagDialog.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "eventTagDialog.h"

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
"16 16 66 2",
".7 c None",
".9 c None",
".3 c None",
".X c None",
".V c None",
".R c None",
".Q c None",
".W c None",
".8 c None",
".E c None",
".e c None",
".m c None",
".d c None",
".c c None",
".f c None",
".g c None",
".b c None",
".h c None",
".l c None",
".a c None",
".# c None",
"Qt c None",
"## c #004800",
".4 c #005000",
".P c #005d00",
".L c #006100",
".Y c #006d00",
".2 c #007100",
".D c #007500",
"#. c #007900",
".H c #007d00",
".w c #008100",
".r c #008500",
".z c #008900",
".k c #008d00",
".O c #009500",
".S c #009d00",
".6 c #00ae00",
".I c #00b200",
".5 c #00b600",
".1 c #00be00",
".A c #00c200",
".Z c #00ca00",
".M c #00ce00",
".0 c #00d200",
".N c #00d600",
".T c #00da00",
".G c #00de00",
".U c #00e200",
".J c #00ea00",
".F c #00f200",
".K c #00ff00",
".n c #088d08",
".o c #089108",
".i c #089900",
".t c #089908",
".j c #089d00",
".x c #08b208",
".y c #08f208",
".B c #08f608",
".p c #10a110",
".C c #10c210",
".s c #18ba18",
".u c #18ce18",
".q c #20aa20",
".v c #20be20",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
".#.a.b.c.d.e.d.c.f.g.h.h.a.#.i.j",
".k.l.b.f.m.e.m.c.f.g.b.h.l.k.k.#",
".n.o.g.c.d.e.d.c.f.c.h.p.q.r.l.#",
".f.s.t.d.m.e.m.d.c.o.u.v.w.g.h.#",
".m.x.y.z.e.e.e.r.A.B.C.D.f.b.l.#",
".E.r.F.G.H.D.I.J.K.I.L.m.f.b.h.#",
".E.L.M.F.N.N.F.J.O.P.E.d.g.b.l.#",
".Q.R.S.G.T.U.M.r.V.W.E.d.f.b.h.#",
".V.X.Y.Z.0.1.2.3.R.W.e.d.g.b.l.#",
".3.3.4.5.6.L.7.X.R.8.E.d.f.b.h.#",
".3.9.X#..L.7.3.X.Q.W.e.d.g.b.l.#",
".9.3.9##.9.3.9.X.R.8.E.d.f.b.h.#",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt"};


/* 
 *  Constructs a EventTagDialog as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
EventTagDialog::EventTagDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
,
      image0( (const char **) image0_data )
{
    if ( !name )
	setName( "EventTagDialog" );
    EventTagDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "EventTagDialogLayout"); 

    layout19 = new QGridLayout( 0, 1, 1, 0, 6, "layout19"); 

    layout18 = new QHBoxLayout( 0, 0, 6, "layout18"); 
    QSpacerItem* spacer = new QSpacerItem( 209, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout18->addItem( spacer );

    AcceptButton = new QPushButton( this, "AcceptButton" );
    AcceptButton->setMinimumSize( QSize( 75, 0 ) );
    AcceptButton->setPixmap( image0 );
    layout18->addWidget( AcceptButton );

    layout19->addLayout( layout18, 1, 0 );

    EventListView = new QListView( this, "EventListView" );
    EventListView->addColumn( tr( "Tag" ) );
    EventListView->addColumn( tr( "Value" ) );
    EventListView->addColumn( tr( "When" ) );
    EventListView->setSelectionMode( QListView::Single );
    EventListView->setAllColumnsShowFocus( TRUE );
    EventListView->setResizeMode( QListView::AllColumns );

    layout19->addWidget( EventListView, 0, 0 );

    EventTagDialogLayout->addLayout( layout19, 0, 0 );
    languageChange();
    resize( QSize(321, 254).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( AcceptButton, SIGNAL( clicked() ), this, SLOT( AcceptButton_clicked() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
EventTagDialog::~EventTagDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void EventTagDialog::languageChange()
{
    setCaption( tr( "Tags for selected event" ) );
    AcceptButton->setText( QString::null );
    AcceptButton->setAccel( QKeySequence( QString::null ) );
    EventListView->header()->setLabel( 0, tr( "Tag" ) );
    EventListView->header()->setLabel( 1, tr( "Value" ) );
    EventListView->header()->setLabel( 2, tr( "When" ) );
    QToolTip::add( EventListView, tr( "The warning symbol pn `when' means other values are available in different cycles." ) );
}

void EventTagDialog::AcceptButton_clicked()
{
    qWarning( "EventTagDialog::AcceptButton_clicked(): Not implemented yet" );
}


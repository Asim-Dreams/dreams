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
** Form implementation generated from reading ui file 'find.ui'
**
** Created: Tue Feb 3 16:18:22 2004
**      by: The User Interface Compiler ($Id: find.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "find.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"16 16 162 2",
"#A c None",
"#1 c None",
"#X c None",
"au c None",
"ab c None",
"#o c None",
"#Y c None",
"#B c None",
"#m c None",
"#9 c None",
"aj c None",
"#n c None",
"#d c None",
"am c None",
"#Z c None",
"aD c None",
"#e c None",
"aa c None",
"av c None",
"ak c None",
".6 c None",
"#u c None",
".7 c None",
"a. c None",
"#0 c None",
"a# c None",
"ay c None",
"ai c None",
".8 c None",
"al c None",
"aC c None",
"#z c None",
".W c None",
".Y c None",
".Z c None",
"ap c None",
".X c None",
"#2 c None",
".P c None",
"#8 c None",
"#M c None",
"#y c None",
"an c None",
"aw c None",
".O c None",
".Q c None",
".N c None",
"#W c None",
"#3 c None",
"ac c None",
".C c None",
".D c None",
".u c None",
"ad c None",
"ax c None",
"#v c None",
".j c None",
"Qt c None",
".s c None",
"ao c None",
"#x c None",
".t c None",
".i c None",
".h c None",
".k c None",
"#L c None",
".# c None",
".g c None",
".a c None",
"#K c None",
"#w c None",
"#J c None",
"#R c #393c5a",
"#N c #39486a",
"#f c #394873",
"aF c #4a4462",
".9 c #5271a4",
"#p c #5279a4",
".0 c #5281bd",
"az c #5a2c10",
"#c c #5a5d7b",
"#Q c #5a6583",
"#O c #5a6d8b",
"#C c #5a7da4",
".R c #5a7db4",
".5 c #626983",
"#P c #627594",
".E c #62759c",
".v c #6291cd",
"at c #6a3008",
"aq c #6a3010",
"aB c #6a4029",
"aE c #6a4c39",
"#t c #6a6983",
"#G c #6a6d83",
"#H c #6a6d94",
".V c #6a7594",
".l c #6a99d5",
".F c #6abaf6",
"#S c #73697b",
".m c #73baf6",
".c c #7ba5d5",
".d c #7ba5de",
"#g c #7bc6f6",
".w c #7bcaff",
".n c #7bceff",
".p c #7bd2ff",
"ah c #833c08",
"ae c #834410",
"#I c #83757b",
".B c #8399b4",
".b c #8399bd",
".r c #83a5c5",
".q c #83cef6",
".o c #83d2ff",
"#. c #83daff",
".f c #8b99b4",
".e c #8baacd",
".1 c #8bd6ff",
".2 c #8bdeff",
"#7 c #944808",
".z c #94daff",
".A c #94deff",
".G c #94e2ff",
".M c #9cd6e6",
"#D c #9ce2f6",
"#h c #9ce2ff",
".x c #9ce6ff",
"#q c #9ceeff",
".3 c #a4e6ff",
".K c #a4eeff",
"#V c #ac5508",
"#T c #ac7941",
".J c #aceaff",
".T c #aceeff",
".y c #acf2ff",
".L c #acfaff",
"#l c #b4c2cd",
"#i c #b4faff",
"#E c #b4ffff",
"## c #bdfaff",
".U c #bdffff",
"#F c #c5d6e6",
".H c #c5ffff",
"#4 c #cd9541",
"ar c #cdaa62",
".4 c #cdffff",
"#U c #d57d18",
"ag c #d58118",
"af c #d5ae62",
".I c #d5ffff",
"#6 c #de8118",
".S c #defaff",
"#a c #deffff",
"#b c #e6ffff",
"aA c #eeb25a",
"#s c #eef2f6",
"#j c #eeffff",
"#r c #f6ffff",
"#5 c #ffda7b",
"as c #ffde7b",
"#k c #ffffff",
"Qt.#.a.b.c.d.e.f.g.h.i.i.i.i.i.i",
".j.k.l.m.n.o.p.q.r.i.s.i.i.i.t.i",
".u.v.w.x.y.x.z.A.x.B.C.C.D.C.D.C",
".E.F.G.H.I.J.y.K.L.M.N.O.P.Q.P.Q",
".R.n.K.I.S.T.H.U.H.U.V.W.X.Y.Z.Y",
".0.1.2.3.T.H.4.4.4.I.5.6.7.8.7.8",
".9#..z.x##.4.I#a#b#a#c#d#e#e#e#e",
"#f#g.1#h#i.H.I#j#k#l#d#m#n#n#n#n",
"#o#p#q.K#i.H#a#r#s#t#u#v#w#x#y#z",
"#A#B#C#D#E.4.I#F#G#H#I#J#K#L#M#z",
"#B#A#o#N#O#P#Q#R.Y#S#T#U#V#x#W#z",
"#X#Y#A#B#Z#0#d#1#2#3#4#5#6#7#8#z",
"#B#Y#Y#9a.a#aaabacadaeaf#5agahai",
"#X#Y#Aajakalam#manaoapaqarasagat",
"#Bau#Y#9a.a#avabawax#WayazafaAaB",
"#X#Y#Aajakalam#manao#8aCaDazaEaF"};

static const char* const image1_data[] = { 
"16 16 177 2",
"#y c None",
"aR c None",
".R c None",
"#l c None",
"#v c None",
"#x c None",
"#K c None",
".Q c None",
"#m c None",
"#k c None",
".P c None",
"#w c None",
"#J c None",
".F c None",
".G c None",
"ay c None",
"ab c None",
"#I c None",
".E c None",
".H c None",
".3 c None",
".2 c None",
"#b c None",
"#Y c None",
"an c None",
"#H c None",
".t c None",
".j c None",
"#a c None",
"#X c None",
"am c None",
".i c None",
".h c None",
".s c None",
"aa c None",
"#L c None",
"a# c None",
".g c None",
"#W c None",
".u c None",
"ac c None",
"#V c None",
".k c None",
"Qt c None",
"az c None",
"ao c None",
".# c None",
"#Z c None",
".a c None",
"aU c #5a5573",
"#n c #6a7d9c",
"#S c #737194",
"#R c #73758b",
"#E c #737594",
"#j c #73798b",
".4 c #7399cd",
"aH c #7b655a",
"#u c #7b7d9c",
"#. c #7b859c",
"#4 c #7b85a4",
"#3 c #7b89a4",
"#c c #7b99c5",
"aS c #834008",
"aT c #835d39",
"#G c #838194",
"#i c #8389a4",
"#N c #839dbd",
".d c #83baee",
".p c #83c2ee",
".1 c #8b89a4",
"#2 c #8b95ac",
"#z c #8ba5c5",
".S c #8ba5d5",
".D c #8baac5",
".m c #8bb2e6",
".q c #8bc2e6",
".n c #8bc6ee",
"aK c #945929",
"aB c #946541",
"#T c #94919c",
"## c #9491ac",
"#t c #9499ac",
".r c #94b2d5",
".c c #94beee",
".o c #94cef6",
"aD c #9c4c08",
"#5 c #9c99ac",
"aQ c #9c9db4",
"#F c #9c9dbd",
"aP c #9ca1b4",
".I c #9caac5",
".v c #9cbee6",
".C c #9cd2ee",
"aM c #a4a5b4",
"aO c #a4a5bd",
"aN c #a4aabd",
".f c #a4bad5",
".e c #a4c2e6",
".O c #a4c6de",
".J c #a4cef6",
"ax c #ac5508",
"#7 c #ac9da4",
"#M c #acaec5",
".b c #acc2e6",
".B c #acd2ee",
"#o c #acd6f6",
".T c #acdaf6",
"al c #b45d08",
"#U c #b47129",
"#6 c #b4b2cd",
".A c #b4d6f6",
".w c #b4deff",
"#8 c #bd8141",
"aL c #bdbacd",
"#s c #bdc2d5",
".l c #bdc2de",
".8 c #bdd6e6",
".5 c #bddef6",
"#O c #bde6f6",
"a. c #c56508",
"#Q c #c5d6e6",
".9 c #c5dae6",
".z c #c5e6f6",
".M c #c5eaff",
".N c #c5eef6",
"#p c #c5eeff",
".6 c #c5f2ff",
"au c #cd996a",
"ah c #cda583",
"aA c #cdd2de",
".7 c #cdeaee",
".0 c #cdeef6",
".x c #cdeeff",
".Z c #cdf2f6",
".K c #cdf2ff",
"#A c #cdf6ff",
"#h c #d5e2ee",
".Y c #d5eef6",
".y c #d5f2ff",
".X c #d5f6ff",
".U c #d5faff",
"ak c #de8518",
"ai c #de9d39",
"aC c #deb25a",
"aI c #deba62",
"#1 c #ded6e6",
"aG c #dee6ff",
"#P c #def6ff",
"#B c #defaff",
"#d c #deffff",
"#9 c #e68110",
"av c #e6b65a",
"at c #e6dee6",
"ag c #e6e6ee",
"#D c #e6eaf6",
"#g c #e6eeff",
"#q c #e6f2ff",
"#f c #e6f6ff",
"#e c #e6faff",
".L c #e6ffff",
"aJ c #eeb65a",
"#0 c #eeeaee",
"ad c #eeeaf6",
"as c #eeeaff",
"ar c #eeeef6",
"aF c #eeeeff",
"#r c #eef2f6",
"#C c #eef6ff",
".V c #eeffff",
"af c #f6f2ff",
"ap c #f6f6ff",
".W c #f6ffff",
"aj c #ffd273",
"aw c #ffda7b",
"aq c #fff6ff",
"aE c #fffaff",
"ae c #ffffff",
"Qt.#.a.b.c.d.e.f.g.h.i.j.h.i.h.j",
".k.l.m.d.n.o.p.q.r.s.t.h.j.h.j.h",
".u.v.w.x.y.z.A.B.C.D.E.F.G.F.H.F",
".I.J.K.L.L.y.z.M.N.O.P.Q.R.Q.R.Q",
".S.T.U.V.W.X.Y.Z.L.0.1.G.2.3.3.3",
".4.5.6.K.X.L.7.8.8.9#.##.j#a#b#a",
"#c.w.x.K#d.L#e#f#g#h#i#j#k#l#k#m",
"#n#o#p.K.U.L#f#q#r#s#t#u#v#w#x#w",
"#y#z#A.U#B#B#f#C#D#E#F#G#H#I#J#K",
"#L#M#N#O#A.X#P#Q#R#S#T#U#V#W#X#Y",
"#Z#0#1#2#3#.#4#5#6#7#8#9a.a#aaab",
"acadaeafadadagad#Cahaiajakalaman",
"aoagaeapaqafafarasatauavawakaxay",
"azadaeapafafararagasaAaBaCawakaD",
"azadaeaeaeaEaEafafaFaGaHayaIaJaK",
"#ZaLaMaNaMaNaMaOaPaMaQ#GaRaSaTaU"};

static const char* const image2_data[] = { 
"16 16 79 2",
".8 c None",
".3 c None",
".S c None",
".q c None",
".z c None",
".h c None",
".f c None",
"#m c None",
".g c None",
".e c None",
".d c None",
".n c None",
".U c None",
".m c None",
".c c None",
".Z c None",
".b c None",
".l c None",
".T c None",
".a c None",
".i c None",
".# c None",
"Qt c None",
"#k c #bd0000",
"#l c #c50000",
"#g c #cd0000",
".y c #cd1000",
".H c #cd1400",
"#h c #cd1800",
"#e c #d50000",
"#a c #d50400",
".7 c #d50c00",
".p c #d51800",
".o c #d52000",
"#f c #d52400",
".4 c #d52800",
".E c #de2808",
".v c #de2c08",
".R c #de3008",
".j c #e63c00",
".M c #e63c10",
".D c #e64410",
".r c #ee3c08",
"#i c #ee4810",
".k c #ee4c10",
".I c #ee4c18",
".u c #ee5018",
"#j c #f60000",
".x c #f64810",
"#d c #ff0000",
"#c c #ff0800",
"#b c #ff1000",
"## c #ff1400",
"#. c #ff2408",
".6 c #ff2800",
".9 c #ff2c08",
".5 c #ff3c10",
".2 c #ff4410",
".G c #ff5018",
".1 c #ff5520",
".F c #ff6120",
".N c #ff6129",
".w c #ff6529",
".Y c #ff6929",
".0 c #ff6d31",
".X c #ff7131",
".Q c #ff8139",
".V c #ff8141",
".W c #ff854a",
".O c #ff9152",
".L c #ff9552",
".P c #ff9962",
".s c #ff9d62",
".C c #ffa162",
".J c #ffa562",
".K c #ffae7b",
".A c #ffb67b",
".t c #ffba7b",
".B c #ffc294",
"Qt.#Qt.a.a.b.c.d.d.e.d.e.e.f.g.h",
".#.i.j.k.b.l.m.n.e.d.e.e.o.p.q.h",
"Qt.r.s.t.u.m.c.d.d.e.e.v.w.x.y.z",
".a.k.A.B.C.D.d.m.e.g.E.w.F.G.H.z",
"Qt.b.I.J.K.L.M.e.e.E.w.N.G.p.z.h",
".i.a.m.D.O.P.Q.R.E.w.F.G.p.z.S.h",
"Qt.T.b.U.M.V.W.X.Y.N.G.p.z.z.h.h",
".i.a.Z.m.e.R.X.0.F.G.p.z.S.h.S.h",
".d.e.d.e.e.v.Y.F.1.2.p.S.z.3.3.3",
".e.d.e.g.4.w.F.G.2.5.6.7.8.3.8.3",
".d.e.e.v.w.N.G.p.p.9#.###a.3.3.3",
".e.e.E.w.F.G.p.z.3.7#b#c#d#e.8.3",
".e#f.w.w.G.p.z.z.z.3#a#d#d#d#g.8",
".f#h#i.G.p.z.S.h.S.S.8#e#d#j#k.3",
".e.h.y.H.z.z.h.z.z.3.S.3#g#l.3.3",
"#m.f.S.z.S.h.S.h.S.S.3.3.8.3.3.3"};


/* 
 *  Constructs a FindForm as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FindForm::FindForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
,
      image0( (const char **) image0_data ),
      image1( (const char **) image1_data ),
      image2( (const char **) image2_data )
{
    if ( !name )
	setName( "FindForm" );
    setFocusPolicy( QDialog::NoFocus );
    FindFormLayout = new QGridLayout( this, 1, 1, 11, 6, "FindFormLayout"); 

    layout30 = new QVBoxLayout( 0, 0, 6, "layout30"); 

    layout12 = new QHBoxLayout( 0, 0, 6, "layout12"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    layout12->addWidget( TextLabel1 );

    LineEdit_TAGName = new QLineEdit( this, "LineEdit_TAGName" );
    LineEdit_TAGName->setFocusPolicy( QLineEdit::StrongFocus );
    layout12->addWidget( LineEdit_TAGName );

    ToolButton_Search = new QToolButton( this, "ToolButton_Search" );
    ToolButton_Search->setMinimumSize( QSize( 22, 22 ) );
    ToolButton_Search->setIconSet( QIconSet( image0 ) );
    layout12->addWidget( ToolButton_Search );
    layout30->addLayout( layout12 );

    layout13 = new QHBoxLayout( 0, 0, 6, "layout13"); 

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    layout13->addWidget( TextLabel1_2 );

    LineEdit_TAGValue = new QLineEdit( this, "LineEdit_TAGValue" );
    LineEdit_TAGValue->setFocusPolicy( QLineEdit::StrongFocus );
    layout13->addWidget( LineEdit_TAGValue );
    layout30->addLayout( layout13 );

    layout14 = new QHBoxLayout( 0, 0, 6, "layout14"); 

    TextLabel1_2_2 = new QLabel( this, "TextLabel1_2_2" );
    layout14->addWidget( TextLabel1_2_2 );

    SpinBox_Cycle = new QSpinBox( this, "SpinBox_Cycle" );
    SpinBox_Cycle->setMaxValue( 999999999 );
    layout14->addWidget( SpinBox_Cycle );
    layout30->addLayout( layout14 );

    textLabel1 = new QLabel( this, "textLabel1" );
    layout30->addWidget( textLabel1 );

    layout29 = new QHBoxLayout( 0, 0, 6, "layout29"); 

    CheckBox_CaseSensitive = new QCheckBox( this, "CheckBox_CaseSensitive" );
    CheckBox_CaseSensitive->setEnabled( FALSE );
    layout29->addWidget( CheckBox_CaseSensitive );
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout29->addItem( spacer );

    CheckBox_ExactMatch = new QCheckBox( this, "CheckBox_ExactMatch" );
    CheckBox_ExactMatch->setEnabled( FALSE );
    layout29->addWidget( CheckBox_ExactMatch );
    QSpacerItem* spacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout29->addItem( spacer_2 );

    CheckBox_Regular = new QCheckBox( this, "CheckBox_Regular" );
    CheckBox_Regular->setEnabled( FALSE );
    layout29->addWidget( CheckBox_Regular );
    layout30->addLayout( layout29 );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 250, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem( spacer_3 );

    FindButton = new QPushButton( this, "FindButton" );
    FindButton->setMinimumSize( QSize( 75, 0 ) );
    FindButton->setPixmap( image1 );
    FindButton->setDefault( FALSE );
    layout7->addWidget( FindButton );

    CancelButton = new QPushButton( this, "CancelButton" );
    CancelButton->setMinimumSize( QSize( 75, 0 ) );
    CancelButton->setPixmap( image2 );
    layout7->addWidget( CancelButton );
    layout30->addLayout( layout7 );

    FindFormLayout->addLayout( layout30, 0, 0 );
    languageChange();
    resize( QSize(440, 210).expandedTo(minimumSizeHint()) );

    // tab order
    setTabOrder( LineEdit_TAGName, LineEdit_TAGValue );
    setTabOrder( LineEdit_TAGValue, FindButton );
    setTabOrder( FindButton, CheckBox_CaseSensitive );
    setTabOrder( CheckBox_CaseSensitive, CheckBox_ExactMatch );
    setTabOrder( CheckBox_ExactMatch, CheckBox_Regular );
    setTabOrder( CheckBox_Regular, CancelButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
FindForm::~FindForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void FindForm::languageChange()
{
    setCaption( tr( "Find" ) );
    TextLabel1->setText( tr( "TAG Name:" ) );
    ToolButton_Search->setText( QString::null );
    TextLabel1_2->setText( tr( "TAG Value:" ) );
    TextLabel1_2_2->setText( tr( "Starting at cycle:" ) );
    textLabel1->setText( tr( "Search Options:" ) );
    CheckBox_CaseSensitive->setText( tr( "Case Sensitive" ) );
    CheckBox_ExactMatch->setText( tr( "Exact Match" ) );
    CheckBox_Regular->setText( tr( "Regular Expression" ) );
    FindButton->setText( QString::null );
    FindButton->setAccel( QKeySequence( QString::null ) );
    CancelButton->setText( QString::null );
    CancelButton->setAccel( QKeySequence( QString::null ) );
}


/*
 * Copyright (C) 2005-2006 Intel Corporation
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
** Form implementation generated from reading ui file 'ancestorwidget.ui'
**
** Created: Fri Feb 4 10:37:17 2005
**      by: The User Interface Compiler ($Id: AncestorWidget.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "AncestorWidget.h"

#include <qvariant.h>
#include <qsplitter.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a AncestorWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
AncestorWidget::AncestorWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "AncestorWidget" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );
    AncestorWidgetLayout = new QGridLayout( this, 1, 1, 0, 4, "AncestorWidgetLayout"); 

    theSplitter = new QSplitter( this, "theSplitter" );
    theSplitter->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)7, 0, 0, theSplitter->sizePolicy().hasHeightForWidth() ) );
    theSplitter->setMinimumSize( QSize( 20, 46 ) );
    theSplitter->setLineWidth( 0 );
    theSplitter->setMidLineWidth( 0 );
    theSplitter->setOrientation( QSplitter::Vertical );

    ancestorsListView = new QListView( theSplitter, "ancestorsListView" );
    ancestorsListView->addColumn( tr( "Relationship Tree" ) );
    ancestorsListView->header()->setClickEnabled( FALSE, ancestorsListView->header()->count() - 1 );
    ancestorsListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, ancestorsListView->sizePolicy().hasHeightForWidth() ) );
    ancestorsListView->setMinimumSize( QSize( 20, 20 ) );
    ancestorsListView->setResizePolicy( QListView::AutoOneFit );
    ancestorsListView->setHScrollBarMode( QListView::Auto );
    ancestorsListView->setSelectionMode( QListView::Single );
    ancestorsListView->setResizeMode( QListView::AllColumns );

    tagsListView = new QListView( theSplitter, "tagsListView" );
    tagsListView->addColumn( tr( "Tag" ) );
    tagsListView->header()->setClickEnabled( FALSE, tagsListView->header()->count() - 1 );
    tagsListView->addColumn( tr( "Value" ) );
    tagsListView->header()->setClickEnabled( FALSE, tagsListView->header()->count() - 1 );
    tagsListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, tagsListView->sizePolicy().hasHeightForWidth() ) );
    tagsListView->setMinimumSize( QSize( 20, 20 ) );
    tagsListView->setResizePolicy( QListView::AutoOneFit );
    tagsListView->setResizeMode( QListView::AllColumns );

    AncestorWidgetLayout->addWidget( theSplitter, 0, 0 );
    languageChange();
    resize( QSize(261, 302).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
AncestorWidget::~AncestorWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AncestorWidget::languageChange()
{
    setCaption( tr( "Ancestor Docked Window" ) );
    ancestorsListView->header()->setLabel( 0, tr( "Relationship Tree" ) );
    tagsListView->header()->setLabel( 0, tr( "Tag" ) );
    tagsListView->header()->setLabel( 1, tr( "Value" ) );
}


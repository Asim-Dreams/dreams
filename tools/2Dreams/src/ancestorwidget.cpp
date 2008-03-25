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
** Form implementation generated from reading ui file 'ancestorwidget.ui'
**
** Created: Wed Jan 15 18:06:17 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "ancestorwidget.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a AncestorWidget which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
AncestorWidget::AncestorWidget( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "AncestorWidget" );
    resize( 261, 302 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setCaption( trUtf8( "Ancestor Docked Window" ) );
    AncestorWidgetLayout = new QGridLayout( this, 1, 1, 0, 4, "AncestorWidgetLayout"); 

    theSplitter = new QSplitter( this, "theSplitter" );
    theSplitter->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)7, 0, 0, theSplitter->sizePolicy().hasHeightForWidth() ) );
    theSplitter->setMinimumSize( QSize( 20, 46 ) );
    theSplitter->setLineWidth( 0 );
    theSplitter->setMidLineWidth( 0 );
    theSplitter->setOrientation( QSplitter::Vertical );

    ancestorsListView = new QListView( theSplitter, "ancestorsListView" );
    ancestorsListView->addColumn( trUtf8( "Relationship Tree" ) );
    ancestorsListView->header()->setClickEnabled( FALSE, ancestorsListView->header()->count() - 1 );
    ancestorsListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, ancestorsListView->sizePolicy().hasHeightForWidth() ) );
    ancestorsListView->setMinimumSize( QSize( 20, 20 ) );
    ancestorsListView->setResizePolicy( QListView::AutoOneFit );
    ancestorsListView->setHScrollBarMode( QListView::Auto );
    ancestorsListView->setSelectionMode( QListView::Single );
    ancestorsListView->setResizeMode( QListView::AllColumns );

    tagsListView = new QListView( theSplitter, "tagsListView" );
    tagsListView->addColumn( trUtf8( "Tag" ) );
    tagsListView->header()->setClickEnabled( FALSE, tagsListView->header()->count() - 1 );
    tagsListView->addColumn( trUtf8( "Value" ) );
    tagsListView->header()->setClickEnabled( FALSE, tagsListView->header()->count() - 1 );
    tagsListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, tagsListView->sizePolicy().hasHeightForWidth() ) );
    tagsListView->setMinimumSize( QSize( 20, 20 ) );
    tagsListView->setResizePolicy( QListView::AutoOneFit );
    tagsListView->setResizeMode( QListView::AllColumns );

    AncestorWidgetLayout->addWidget( theSplitter, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
AncestorWidget::~AncestorWidget()
{
    // no need to delete child widgets, Qt does it all for us
}


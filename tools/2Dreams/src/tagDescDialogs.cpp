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

// -----------------------------------------------------------------------------
// -- ChooseTagDialog
// -----------------------------------------------------------------------------

#include "tagDescDialogs.h"

ChooseTagDialog::ChooseTagDialog( QWidget* parent, const char* name, bool modal, WFlags fl) :
                 QDialog( parent, name, modal, fl )
{
    selected = QString::null;
	setName( "Tag Selection Dialog" );
    resize( 250, 350 );
    setCaption("Tag Selection Dialog");
    setSizeGripEnabled( TRUE );
    ChooseDialogLayout = new QVBoxLayout( this, 11, 6, "ChooseDialogLayout");

    tagList = new QListView( this, "ListBox_TagList" );
    tagList->addColumn( tr( "Tag" ) );
    tagList->addColumn( tr( "Description" ) );
    tagList->setSelectionMode( QListView::Single );
    tagList->setAllColumnsShowFocus( TRUE );
    tagList->setResizeMode( QListView::AllColumns );
    //tagList->setSortColumn(0);  // this is QT 3.2 only
    tagList->setSorting(0);       // this is both QT 3.0.x and 3.2.x       
    ChooseDialogLayout->addWidget( tagList );
    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1");
    acceptBtn = new QPushButton( this, "acceptBtn" );
    acceptBtn->setText("&Accept");
    acceptBtn->setAutoDefault( FALSE );
    acceptBtn->setEnabled(false);
    cancelBtn = new QPushButton( this, "cancelBtn" );
    cancelBtn->setText("&Cancel");
    cancelBtn->setAutoDefault( TRUE );
    Layout1->addWidget( acceptBtn );
    Layout1->addWidget( cancelBtn );
    ChooseDialogLayout->addLayout( Layout1 );
    connect (tagList,SIGNAL(selectionChanged(QListViewItem*)),this,SLOT(mySelectionChanged(QListViewItem*)));
    connect (acceptBtn,SIGNAL(clicked ()), this, SLOT(acceptBtnClicked()));
    connect (cancelBtn,SIGNAL(clicked ()), this, SLOT(cancelBtnClicked()));
}

ChooseTagDialog::~ChooseTagDialog()
{
}

void
ChooseTagDialog::mySelectionChanged(QListViewItem* item)
{
    selected = item->text(0);
    acceptBtn->setEnabled(true);
}

QString
ChooseTagDialog::getSelected()
{ return selected; }

void 
ChooseTagDialog::acceptBtnClicked()
{
    done(1);
}

void
ChooseTagDialog::cancelBtnClicked()
{
    selected = QString::null;
    done(-1);
}

void
ChooseTagDialog::insertItem(QString value,QString longDesc)
{
    tagList->insertItem(new QListViewItem(tagList,value,longDesc));
}


// -----------------------------------------------------------------------------
// -- ShowTagDescDialog
// -----------------------------------------------------------------------------

ShowTagDescDialog::ShowTagDescDialog( QWidget* parent, const char* name, bool modal, WFlags fl) :
                 QDialog( parent, name, modal, fl )
{
    selected = QString::null;
	setName( "Tag Description Dialog" );
    resize( 250, 350 );
    setCaption("Tag Description Dialog");
    setSizeGripEnabled( TRUE );
    ChooseDialogLayout = new QVBoxLayout( this, 11, 6, "ChooseDialogLayout");

    tagList = new QListView( this, "ListBox_TagList" );
    tagList->addColumn( tr( "Tag" ) );
    tagList->addColumn( tr( "Description" ) );
    tagList->setSelectionMode( QListView::Single );
    tagList->setAllColumnsShowFocus( TRUE );
    tagList->setResizeMode( QListView::AllColumns );
    //tagList->setSortColumn(0);  // this is QT 3.2 only
    tagList->setSorting(0);       // this is both QT 3.0.x and 3.2.x       
    
    ChooseDialogLayout->addWidget( tagList );
    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1");
    acceptBtn = new QPushButton( this, "acceptBtn" );
    acceptBtn->setText("&Accept");
    acceptBtn->setAutoDefault(true);
    acceptBtn->setEnabled(true);
    Layout1->addWidget( acceptBtn );
    ChooseDialogLayout->addLayout( Layout1 );
    connect (acceptBtn,SIGNAL(clicked ()), this, SLOT(acceptBtnClicked()));
}

ShowTagDescDialog::~ShowTagDescDialog()
{
}

void 
ShowTagDescDialog::acceptBtnClicked()
{
    done(1);
}

void
ShowTagDescDialog::insertItem(QString value,QString longDesc)
{
    tagList->insertItem(new QListViewItem(tagList,value,longDesc));
}


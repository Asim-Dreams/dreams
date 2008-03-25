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

/**
  * @file ancestorwidgetImpl.cpp
  */

#include <qvaluelist.h>

#include "ancestorwidgetImpl.h"
#include "PreferenceMgr.h"
#include "AncestorDockWin.h"

AncTagEntry::AncTagEntry(QString name, QString value)
{
    tagName = name;
    tagStringValue = value;
    fakeColorTag = false;
}

AncTagEntry::AncTagEntry(QString name, QColor color)
{
    tagName = name;
    fakeColorTag = true;
    tagColor = color;
}

AncTagEntry::~AncTagEntry() {}


QString
AncTagEntry::getName()
{ return tagName; }

QString
AncTagEntry::getFormatedValue()
{ return tagStringValue; }

bool
AncTagEntry::isFakeColorTag()
{ return fakeColorTag; }


// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

AncFolder::AncFolder( AncFolder *parent, ItemHandler * hnd, ZDBase* db)
    : QObject(parent)
{
    myDB = db;
    handler = * hnd;
    myItemId = hnd->getItemId();
    Q_ASSERT(myItemId>=0);

    lstTags.setAutoDelete( TRUE );
    originCol=-1;
    originRow=-1;
}

void 
AncFolder::addTagEntry(AncTagEntry* t)
{ lstTags.append(t); }

QString 
AncFolder::folderName()
{
    QString fName;
    fName = fName + QString("ItemID ");
    fName = fName + QString::number(myItemId,10);
    return (fName);
}

ItemHandler
AncFolder::getHandler()
{ return handler; }

INT32 
AncFolder::getItemId()
{ return myItemId; }

AncTagEntry* 
AncFolder::firstTag()
{ return lstTags.first(); }

AncTagEntry* 
AncFolder::nextTag()
{ return lstTags.next(); }

// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

QPtrList<AncFolderListItem> AncFolderListItem::flatList;

QPtrList<AncFolderListItem>
AncFolderListItem::getGlobalList()
{ return flatList; }

AncFolderListItem::AncFolderListItem( QListView *parent, AncFolder *f )
    : QListViewItem( parent )
{
    myFolder = f;
    expanded=false;
    arrowed=false;
    flatList.append(this);
    setText( 0, f->folderName() );

    if ( myFolder->children() )
        insertSubFolders( myFolder->children() );

}

AncFolderListItem::AncFolderListItem( AncFolderListItem *parent, AncFolder *f )
    : QListViewItem( parent )
{
    myFolder = f;
    expanded=false;
    arrowed=false;
    flatList.append(this);

    setText( 0, f->folderName() );

    if ( myFolder->children() )
        insertSubFolders( myFolder->children() );

}

AncFolderListItem::~AncFolderListItem()
{
    flatList.remove(this);
}

void 
AncFolderListItem::insertSubFolders( const QObjectList *lst )
{
    AncFolder *f;
    for ( f = ( AncFolder* )( ( QObjectList* )lst )->first(); f!=NULL;
          f = ( AncFolder* )( ( QObjectList* )lst )->next() )
    {
        AncFolderListItem* dummy = new AncFolderListItem(this,f);
        Q_ASSERT(dummy!=NULL);
    }
}

AncFolder* 
AncFolderListItem::folder()
{ return myFolder; }





// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

AncTagListItem::AncTagListItem( QListView *parent, AncTagEntry *t )
    : QListViewItem( parent )
{
    myTag = t;

    setText( 0, t->getName());
    setText( 1, t->getFormatedValue());
}

AncTagEntry* 
AncTagListItem::getTag()
{ return myTag; }

/*
void AncTagListItem::paintCell( QPainter *p, const QColorGroup &cg,
                 int column, int width, int alignment )
{
    QColorGroup _cg( cg );
    QColor c = _cg.text();

    if ( myMessage->state() == Message::Unread )
    _cg.setColor( QColorGroup::Text, Qt::red );

    QListViewItem::paintCell( p, _cg, column, width, alignment );

    _cg.setColor( QColorGroup::Text, c );
}
*/



// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------

ancestorwidgetImpl::ancestorwidgetImpl( QWidget* parent,  const char* name, WFlags fl,ZDBase* db)
    : AncestorWidget( parent, name, fl )
{
    theSplitter->setOpaqueResize(true);
    //theSplitter->setResizeMode(ancestorsListView,QSplitter::FollowSizeHint);
    //theSplitter->setResizeMode(tagsListView,QSplitter::FollowSizeHint);
    theSplitter->setResizeMode(ancestorsListView,QSplitter::KeepSize);
    theSplitter->setResizeMode(tagsListView,QSplitter::KeepSize);
    

    myDB = db;
    myDockWin = (AncestorDockWin*)parent;
    resize(sizeHint());
}

ancestorwidgetImpl::~ancestorwidgetImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

QSize
ancestorwidgetImpl::sizeHint () const
{
    if (dockInit)
    {
        PreferenceMgr* prefMgr = PreferenceMgr::getInstance();
        QSize result;
        result.setWidth(prefMgr->getRelationshipWindowWidth());
        result.setHeight(prefMgr->getRelationshipWindowHeight());
        return (result);
    }
    else
    {
        return AncestorWidget::sizeHint();
    }
}


QListView* 
ancestorwidgetImpl::getAncestorsListView()
{ return ancestorsListView; }

QListView* 
ancestorwidgetImpl::getTagsListView()
{ return tagsListView; }


void
ancestorwidgetImpl::refreshSplitter()
{
    QValueList<int> list;
    list.append(ancestorsListView->sizeHint().width());
    list.append(tagsListView->sizeHint().width());
    theSplitter->setSizes (list);
    theSplitter->refresh();
}


// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

/**
  * @file AncestorWidgetImpl.h
  */

#ifndef _ANCESTORWIDGETIMPL_H
#define _ANCESTORWIDGETIMPL_H

#include "DreamsDefs.h"
// QT includes.
#include <qstring.h>
#include <qcolor.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qlistview.h>
#include <qwidget.h>

// Dreams includes.
#include "AncestorWidget.h"
#include "dDB/itemheap/ItemHeap.h"

// Class forwarding.
class AncestorDockWin;

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-00-00
  * @version 0.1
  */
class AncTagEntry
{
  public:
    AncTagEntry(QString name, QString value);
    AncTagEntry(QString name, QColor color);
    ~AncTagEntry();

    inline QString getName() const;
    inline QString getFormatedValue() const;
    inline bool isFakeColorTag() const;

  protected:
    QString tagName;
    QString tagStringValue;

    bool fakeColorTag;
    QColor tagColor;        /** < only for fake highlight color tags */
} ;

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-00-00
  * @version 0.1
  */
class AncFolder : public QObject
{
    Q_OBJECT

  public:
    AncFolder(AncFolder * parent, ItemHandler * hnd);
    ~AncFolder();

    inline void addTagEntry(AncTagEntry * entry);

    inline AncTagEntry * firstTag();
    inline AncTagEntry * nextTag();

    inline QString folderName() const;
    inline ITEM_ID getItemId() const;
    inline ItemHandler getHandler() const;
    inline CYCLE getOriginCycle() const;
    inline INT32 getOriginRow() const;

    inline void setOriginCycle(CYCLE v);
    inline void setOriginRow(INT32 v);

  protected:
    ITEM_ID myItemId;
    ItemHandler handler;
    CYCLE originCycle;
    INT32 originRow;
    QPtrList<AncTagEntry> lstTags;
} ;

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-00-00
  * @version 0.1
  */
class AncFolderListItem : public QListViewItem
{
  public:
    AncFolderListItem(QListView * parent, AncFolder * f);
    AncFolderListItem(AncFolderListItem * parent, AncFolder * f);
    ~AncFolderListItem();

    void insertSubFolders(const QObjectList * lst);

    inline AncFolder * folder() const;
    inline bool isArrowed() const;
    inline bool isExpanded() const;

    inline void setArrowed(bool v);
    inline void setExpanded(bool v);

  public:
    static inline QPtrList<AncFolderListItem> getGlobalList();
    
  protected:
    AncFolder * myFolder;
    bool expanded;
    bool arrowed;

  private:
    // I keep a flat list of all AncFolderListItem's
    static QPtrList<AncFolderListItem> flatList;
};

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-00-00
  * @version 0.1
  */
class AncTagListItem : public QListViewItem
{
  public:
    AncTagListItem(QListView * parent, AncTagEntry * t);

    inline AncTagEntry * getTag() const;

    /*virtual void paintCell( QPainter *p, const QColorGroup &cg,
                int column, int width, int alignment );
    */

  protected:
    AncTagEntry * myTag;
} ;

/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-00-00
  * @version 0.1
  */
class AncestorWidgetImpl : public AncestorWidget
{
    Q_OBJECT

  public:
    AncestorWidgetImpl(QWidget * parent = 0, const char * name = 0, WFlags fl = 0);
    ~AncestorWidgetImpl();

    inline QListView * getAncestorsListView() const;
    inline QListView * getTagsListView() const;

    QSize sizeHint() const;
    void refreshSplitter();

  private:
    AncestorDockWin * myDockWin;
} ;

QString
AncTagEntry::getName() const
{
    return tagName;
}

QString
AncTagEntry::getFormatedValue() const
{
    return tagStringValue;
}

bool
AncTagEntry::isFakeColorTag() const
{
    return fakeColorTag;
}

void
AncFolder::addTagEntry(AncTagEntry * entry)
{
    lstTags.append(entry);
}

QString
AncFolder::folderName() const
{
    QString fName;
    fName = fName + QString("ItemID ");
    fName = fName + QString::number(myItemId, 10);
    return fName;
}

AncTagEntry *
AncFolder::firstTag()
{
    return lstTags.first();
}

AncTagEntry *
AncFolder::nextTag()
{
    return lstTags.next();
}

ITEM_ID
AncFolder::getItemId() const
{
    return myItemId;
}

ItemHandler
AncFolder::getHandler() const
{
    return handler;
}

CYCLE
AncFolder::getOriginCycle() const
{
    return originCycle;
}

INT32
AncFolder::getOriginRow() const
{
    return originRow;
}

void
AncFolder::setOriginCycle(CYCLE v)
{
    originCycle = v;
}

void
AncFolder::setOriginRow(INT32 v)
{
    originRow = v;
}

AncFolder *
AncFolderListItem::folder() const
{
    return myFolder;
}

bool
AncFolderListItem::isArrowed() const
{
    return arrowed;
}

bool
AncFolderListItem::isExpanded() const
{
    return expanded;
}

void
AncFolderListItem::setArrowed(bool v)
{
    arrowed = v;
}

void
AncFolderListItem::setExpanded(bool v)
{
    expanded = v;
}

QPtrList<AncFolderListItem>
AncFolderListItem::getGlobalList()
{
    return flatList;
}

AncTagEntry * 
AncTagListItem::getTag() const
{
    return myTag;
}

QListView *
AncestorWidgetImpl::getAncestorsListView() const
{
    return ancestorsListView;
}

QListView *
AncestorWidgetImpl::getTagsListView() const
{
    return tagsListView;
}

#endif // ANCESTORWIDGETIMPL_H

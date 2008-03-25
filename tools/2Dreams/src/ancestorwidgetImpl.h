// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file ancestorwidgetImpl.h
  */

#ifndef _ANCESTORWIDGETIMPL_H
#define _ANCESTORWIDGETIMPL_H

#include <qstring.h>
#include <qobjectlist.h>
#include <qvaluelist.h>
#include <qpainter.h>
#include <qwidget.h>
#include <qlistview.h>
#include <qsplitter.h>

#include "asim/DralDB.h"

#include "ancestorwidget.h"
#include "ZDBase.h"

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
    AncTagEntry (QString name, QString value);
    AncTagEntry (QString name, QColor color);
    ~AncTagEntry ();

     QString getName();
     QString getFormatedValue();
     bool  isFakeColorTag();

protected:
    QString tagName;
    QString tagStringValue;

    bool fakeColorTag;
    QColor tagColor;        /** < only for fake highlight color tags */
};

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
    AncFolder( AncFolder *parent, ItemHandler * hnd, ZDBase* db );
    ~AncFolder(){};

    void addTagEntry(AncTagEntry* );

    QString folderName();

    AncTagEntry* firstTag();
    AncTagEntry* nextTag();

    INT32 getItemId();
    ItemHandler getHandler();

    inline INT32 getOriginCol() {return originCol;}
    inline INT32 getOriginRow() {return originRow;}
    inline void setOriginCol(INT32 v)  {originCol=v;}
    inline void setOriginRow(INT32 v)  {originRow=v;}
protected:
    ZDBase* myDB;
    INT32 myItemId;
    ItemHandler handler;
    INT32 originCol;
    INT32 originRow;
    QPtrList<AncTagEntry> lstTags;
};

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
    AncFolderListItem( QListView *parent, AncFolder *f );
    AncFolderListItem( AncFolderListItem *parent, AncFolder *f );
    ~AncFolderListItem();

    void insertSubFolders( const QObjectList *lst );

    AncFolder *folder();

    inline bool isArrowed() { return arrowed; }
    inline bool isExpanded() { return expanded; }
    inline void setArrowed(bool v) { arrowed=v; }
    inline void setExpanded(bool v) { expanded=v; }

public:
    static QPtrList<AncFolderListItem> getGlobalList();
    
protected:
    AncFolder *myFolder;
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
    AncTagListItem( QListView *parent, AncTagEntry* t);

    /*virtual void paintCell( QPainter *p, const QColorGroup &cg,
                int column, int width, int alignment );
    */

    AncTagEntry* getTag();

protected:
    AncTagEntry *myTag;

};



/**
  * Short desc.
  * Long  desc.
  *
  * @author Federico Ardanaz
  * @date started at 2002-00-00
  * @version 0.1
  */
class ancestorwidgetImpl : public AncestorWidget
{
    Q_OBJECT

public:
    ancestorwidgetImpl( QWidget* parent = 0, const char* name = 0, WFlags fl = 0, ZDBase* db =NULL );
    ~ancestorwidgetImpl();

    QListView* getAncestorsListView();
    QListView* getTagsListView();

    QSize sizeHint() const;
    void refreshSplitter();
protected:
    ZDBase* myDB;

private:
    AncestorDockWin* myDockWin;
};


#endif // ANCESTORWIDGETIMPL_H

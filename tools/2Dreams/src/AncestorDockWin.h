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
  * @file AncestorDockWin.h
  */

#ifndef _ANCESTORDOCKWIN_H
#define _ANCESTORDOCKWIN_H

// Standard C
#include <stdio.h>

// QT library
#include <qdockwindow.h>
#include <qmainwindow.h>
#include <qptrlist.h>
#include <qlistview.h>
#include <qapplication.h>
#include <qevent.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qscrollview.h>
#include <qvaluelist.h>
#include <qintdict.h>

// AGT library
#include "asim/AnnotationCtrl.h"

#include "asim/DralDB.h"

// 2Dreams
#include "2DreamsDefs.h"
#include "ZDBase.h"
#include "HighLightMgr.h"
#include "PreferenceMgr.h"

class AncFolder;
class AncFolderListItem;
class AncTagListItem;
class ancestorwidgetImpl;

extern bool dockInit;

/**
  * Docked window to display relationships (like ancestors).
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-07-08
  * @author Federico Ardanaz
  */
class AncestorDockWin : public QDockWindow
{
    Q_OBJECT
    public:
        AncestorDockWin(AnnotationCtrl*,QMainWindow*, QScrollView*, HighLightMgr* );
        ~AncestorDockWin();

        /** Establishes the Tag used for tracking relationships.
            @param id The Tag Identifier
            @see TagDescriptor
        */
        void setTrackingTag(UINT16 id);

        /** Just clean-up any ongoing relationship tracking. */
        void reset();

        /** Start to track a relationship starting with the given item handler
            @param hnd the starting point to look for in the tag heap
            @see TagHeap
        */
        void initTrackingWidth(ItemHandler * hnd,INT32 originCol,INT32 originRow);

        bool hasActiveScan();

        friend class ancestorwidgetImpl;

   public slots:
        void ancFolderChanged(QListViewItem* item);
        void contextMenuReq(QListViewItem* item,const QPoint &point, int column);
        void do_expandNode();
        void do_collapseNode();
        void do_clearAll();
        void do_switchHighlight();
        void do_switchArrows();

    protected:

        void clearArrows();
        bool getArrowDest(INT32 newItemId,INT32 cCol, INT32 cRow, INT32* destCol, INT32* destRow);
        /** Nest one level in the tracking of the current relationship.
            This can imply adding several "sun" nodes.
            @param node the node to expand from
        */
        void expandNode(AncFolderListItem* node);
        void collapseNode(AncFolderListItem* node);

        /**
          * Add a new "sun" given the current relationship.
          * @param parent_node the node where the new subfolder will be attached to
          * @param newItemId the ItemID of the new "sun" node.
          */
        void addChildToNode(AncFolderListItem* parent_node, INT32 newItemId);

        /** Look for attached tags and insert them into the Tag list view
            @param folder the folder we are quering about
        */
        void addTagsForFolder(AncFolder* folder);

        /** Helpher method to pre-build a context popup menu */
        void addContextMenu();

    protected:
        void enterEvent (QEvent *e );
        void leaveEvent (QEvent *e );
        void resizeEvent ( QResizeEvent * e );

    private:
        DralDB*             draldb;
        ZDBase*             myDB;
        QMainWindow*        myMW;
        HighLightMgr*       myHG;
        QScrollView*        mySV;
        ancestorwidgetImpl*    myAncDialog;
        AnnotationCtrl*        myAnnCtrl;
        QListView* ancestorsListView;
        QListView* tagsListView;
        UINT16  trackingTagId;

        QPtrList<AncFolder> folderList;
        AncFolderListItem* lastSelectedFolder;

        QPopupMenu* contextMenu;
        int expnode_id;
        int reset_id;
        int highlight_id;
        int arrows_id;
        PreferenceMgr* prefMgr;
        UINT16 myOriginEdgeId;
        QIntDict<AnnotationItem>* arrowList;
};

#endif

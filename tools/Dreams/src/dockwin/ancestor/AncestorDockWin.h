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
  * @file AncestorDockWin.h
  */

#ifndef _ANCESTORDOCKWIN_H
#define _ANCESTORDOCKWIN_H

// Dreams includes.
#include "dDB/DRALTag.h"
#include "dDB/ClockDomainMgr.h"
#include "DreamsDefs.h"

// Asim includes.
#include "asim/AnnotationItem.h"

// Qt includes.
#include <qdockwindow.h>
#include <qintdict.h>

// Class forwarding.
class ADFParserInterface;
class AncFolder;
class AncFolderListItem;
class AncTagListItem;
class AncestorWidgetImpl;
class PreferenceMgr;
class DreamsDB;
class HighLightMgr;
class DralDB;
class QMainWindow;
class QScrollView;
class QListViewItem;
class QListView;
class AnnotationCtrl;
class QPopupMenu;
class ItemHandler;
class LayoutResourceCache;

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
        AncestorDockWin(DreamsDB * _dreamsdb, LayoutResourceCache * _cache, PreferenceMgr * _prefMgr, AnnotationCtrl * ann, QMainWindow * mw, QScrollView * qsv, HighLightMgr * hmgr);
        ~AncestorDockWin();

        void setTrackingTag(TAG_ID id);
        void initTrackingWidth(ItemHandler * hnd, CYCLE originCycle, INT32 originRow);
        bool hasActiveScan();

        void reset();

        friend class AncestorWidgetImpl;

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
        bool getArrowDest(ITEM_ID newItemId, CYCLE cCyc, INT32 cRow, CYCLE * destCyc, INT32 * destRow);
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
        void addChildToNode(AncFolderListItem * parent_node, ITEM_ID newItemId);

        /** Look for attached tags and insert them into the Tag list view
            @param folder the folder we are quering about
        */
        void addTagsForFolder(AncFolder* folder);

        /** Helpher method to pre-build a context popup menu */
        void addContextMenu();

    protected:
        void enterEvent(QEvent * e);
        void leaveEvent(QEvent * e);
        void resizeEvent(QResizeEvent * e);

    private:
        DreamsDB *           dreamsdb;
        DralDB *             draldb;
        LayoutResourceCache *      cache;
        PreferenceMgr *      prefMgr;
        QMainWindow *        myMW;
        HighLightMgr *       myHG;
        QScrollView *        mySV;
        AncestorWidgetImpl * myAncDialog;
        AnnotationCtrl *     myAnnCtrl;
        QListView *          ancestorsListView;
        QListView *          tagsListView;
        QPopupMenu *         contextMenu;

        QPtrList<AncFolder> folderList;
        AncFolderListItem * lastSelectedFolder;
        QIntDict<AnnotationItem>* arrowList;

        int expnode_id;
        int reset_id;
        int highlight_id;
        int arrows_id;
        TRACK_ID myOriginEdgeTrackId;
        TAG_ID trackingTagId;
        UINT64 * values;
        bool * valids;
} ;

#endif

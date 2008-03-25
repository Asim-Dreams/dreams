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
  * @file AncestorDockWin.cpp
  */

#include "AncestorDockWin.h"
#include "PreferenceMgr.h"
#include "ancestorwidgetImpl.h"
#include "TwoDAvtView.h"

// QT Library
#include <qmessagebox.h>

AncestorDockWin::AncestorDockWin(AnnotationCtrl* ann,QMainWindow* mw, QScrollView* qsv, HighLightMgr* hmgr)
{
    draldb = DralDB::getInstance();
    myDB = ZDBase::getInstance();
    myMW = mw;
    myHG = hmgr;
    mySV = qsv;
    myAnnCtrl = ann;

    trackingTagId = 0;

    // pre-build the AncDialog
    myAncDialog = new ancestorwidgetImpl(this,"ancDialog",0,myDB);Q_ASSERT(myAncDialog!=NULL);
    ancestorsListView = myAncDialog->getAncestorsListView();
    tagsListView = myAncDialog->getTagsListView();
    ancestorsListView->setRootIsDecorated(TRUE);

    connect(ancestorsListView,SIGNAL(selectionChanged(QListViewItem*)),this,
           SLOT(ancFolderChanged(QListViewItem*)));

    connect(ancestorsListView,SIGNAL(rightButtonPressed(QListViewItem*,const QPoint &,int)),this,
           SLOT(contextMenuReq(QListViewItem*,const QPoint &,int)));

    setResizeEnabled(TRUE);
    setCloseMode(QDockWindow::Always);
    setMovingEnabled(true);
    setCaption("Relationship Window");

    prefMgr = PreferenceMgr::getInstance();
    mw->addToolBar( this);
    mw->moveDockWindow ( this, prefMgr->getRelationshipWindowDockPolicy(),
    prefMgr->getRelationshipWindowNl(), prefMgr->getRelationshipWindowIdx(),
    prefMgr->getRelationshipWindowOffset());

    arrowList = new QIntDict<AnnotationItem>(31);

    // pre-build the popup assoc menu.
    addContextMenu();

    setWidget(myAncDialog);
    show();
}

AncestorDockWin::~AncestorDockWin()
{
    if (myAncDialog!=NULL) delete myAncDialog;
    if (arrowList!=NULL) delete arrowList;
}

void
AncestorDockWin::setTrackingTag(UINT16 id)
{ trackingTagId = id; }

void
AncestorDockWin::addContextMenu()
{
    contextMenu = new QPopupMenu(this);Q_ASSERT(contextMenu!=NULL);
    expnode_id = contextMenu->insertItem("&Expand Node", this, SLOT(do_expandNode()));
    //contextMenu->insertItem("&Collapse Node", this, SLOT(do_collapseNode()));
    reset_id   = contextMenu->insertItem("&Reset Scan", this, SLOT(do_clearAll()));
    contextMenu->insertSeparator();
    highlight_id= contextMenu->insertItem("Switch &Highlighting", this, SLOT(do_switchHighlight()));

    arrows_id   = contextMenu->insertItem("Switch &Arrow", this, SLOT(do_switchArrows()));
}

void
AncestorDockWin::reset()
{
    // clear arrows if any
    int cnt = arrowList->count();
    clearArrows();

    // clear relationship scan highlights
    bool needRep=false;
    QPtrList<AncFolderListItem> gl = AncFolderListItem::getGlobalList();
    AncFolderListItem* current;

    for ( current = gl.first(); current!=NULL; current = gl.next() )
    {
        AncFolder* theFolder = current->folder();
        ItemHandler hnd = theFolder->getHandler();

        // check whether the node was highlighted or not
        //printf (">>> AncDocWin::reset, lookForEvent on %d\n",itemIdx);
        int stackIdx = myHG->lookForEvent(&hnd);
        if (stackIdx>=0)
        {
             // in this case we un-highlight such criteria
            myHG->purgeLayer(stackIdx);
            needRep = true;
        }
    }
    if ((needRep)||(cnt>0))
    { mySV->repaintContents(false); }

    ancestorsListView->clear();
    tagsListView->clear();
}

bool
AncestorDockWin::hasActiveScan()
{
    QPtrList<AncFolderListItem> gl = AncFolderListItem::getGlobalList();
    return (gl.count() > 0);
}

void
AncestorDockWin::initTrackingWidth(ItemHandler * handler,INT32 originCol,INT32 originRow)
{
    if (myDB->getRowType(originRow) != SimpleEdgeRow)
    {
        return;
    }

    clearArrows();
    myOriginEdgeId = myDB->getRowEdgeId(originRow);
    //printf ("origin col =%d, origin row = %d, edge_id = %d\n",originCol, originRow,myOriginEdgeId);

    QString trackingStr = (PreferenceMgr::getInstance())->getRelationshipTag();
    trackingTagId = draldb->scanTagName(trackingStr);

    // ok I got a entry point into the tag heap
    // 0) Clear any previous data
    reset();

    // 1) Add this ItemId to the tree view

    // a) create the folder object and insert it on internal list
    AncFolder* folder = new AncFolder(NULL,handler,myDB);Q_ASSERT(folder!=NULL);
    folder->setOriginRow(originRow);
    folder->setOriginCol(originCol);
    folderList.append(folder);

    // b) create the folder item list
    AncFolderListItem* item = new AncFolderListItem(ancestorsListView,folder);Q_ASSERT(item!=NULL);

    // c) highlight (by ItemID) the selected item
    //printf (">>> AncDocWin::init, lookForEvent on %d\n",tagIdx);
    int stackIdx = myHG->lookForEvent(handler);
    if (stackIdx<0)
    {
        //printf (">>> not found so adding...\n");
        bool needRep = myHG->contentsSelectEvent(handler, true,
             ((TwoDAvtView*)mySV)->getCurrentTagSelector(),HLT_DEFAULT);
        if (needRep)
        { mySV->repaintContents(false); }
    }

    // 2) Add the related tags-value pairs in the tag view
    addTagsForFolder(folder);

    // 3) Add the parents of ItemId
    expandNode(item);
}

void
AncestorDockWin::addTagsForFolder(AncFolder* folder)
{
    // purge QT widget
    tagsListView->clear();

    //printf ("addTagsFolder called\n");

    ItemHandler hnd = folder->getHandler();
    // walk trought the tags and put them into the tag-value list
    // by-pass Item-Id!

    INT16 tgbase;
    UINT64 tgvalue;
    UINT64 tgwhen;
    TagValueType  tgvtype;

    // for each tag get its value, type & so on
    bool eol;

    do
    {
        UINT16 tagId;
        bool qok = hnd.getCurrentTagValue(&tagId,&tgvalue,&tgvtype,&tgbase,&tgwhen);
        Q_ASSERT(qok);

        // skip ITEMID & tracking criteria tags
        bool c1 = (INT32)tagId != TagDescVector::getItemId_TagId();
        bool c2 = (INT32)tagId != trackingTagId;
        if (c1 && c2)
        {
            QString tagName = draldb->getTagDescription(tagId);
            QString tagFormatedValue = hnd.getFormatedTagValue();
            AncTagEntry* ate = new AncTagEntry(tagName,tagFormatedValue);Q_ASSERT(ate!=NULL);
            AncTagListItem* atli = new AncTagListItem( tagsListView,ate);Q_ASSERT(atli!=NULL);
        }
        // check loop condition
        eol = !(hnd.skipToNextTag());
    } while (!eol);
}


void
AncestorDockWin::collapseNode(AncFolderListItem* node)
{
    delete node;
}

void
AncestorDockWin::expandNode(AncFolderListItem* node)
{
    //printf ("AncestorDockWin::expandNode() called\n");
    if (node->isExpanded())
    {
        // this should never happen (context menu must prevent it...)
        qApp->beep();
        return;
    }

    INT16 tgbase;
    UINT64 tgvalue;
    UINT64 tgwhen;
    TagValueType  tgvtype;

    // get the item index of such a node
    AncFolder* theFolder = node->folder();
    ItemHandler hnd = theFolder->getHandler();

    // now look for the target tag  (typically ANCESTOR); use latest known value
    INT32 pos = hnd.getTagById(trackingTagId,&tgvalue,&tgvtype,&tgbase,&tgwhen,4294967295U);
    if (pos<0)
    {
        QMessageBox::warning (this, "Scanning error","The relationship tag criteria was not found.",
        QMessageBox::Ok ,QMessageBox::NoButton);
        node->setExpanded(true);
        return;
    }

    // if this is not a list we are done!
    if (tgvtype!=TagSetOfValues)
    {
        QMessageBox::warning (this, "Scanning error",
        "The relationship tag criteria must of type 'set of values'",
        QMessageBox::Ok ,QMessageBox::NoButton);
        return;
    }

    QValueList<INT32> insertedList;

    addChildToNode(node,tgvalue);
    insertedList.append(tgvalue);

    // ok, this is a list, iterate through..
    UINT16 cTagId;
    bool eol = !hnd.skipToNextEntry();
    bool qok = hnd.getCurrentTagValue(&cTagId,&tgvalue,&tgvtype,&tgbase,&tgwhen);
    Q_ASSERT(qok);
    while (!eol && (cTagId==trackingTagId))
    {
        //printf ("expanding node with value=%lu\n",tgvalue);
        if (insertedList.find(tgvalue)==insertedList.end())
        {
            addChildToNode(node,tgvalue);
            insertedList.append(tgvalue);
        }
        else
        {
            QMessageBox::warning (this, "Scanning incoherence","Ignoring repeated item in relationship scan.",
            QMessageBox::Ok ,QMessageBox::NoButton);
        }

        eol = !hnd.skipToNextEntry();
        // TODO: is necessary?
/*        if (!eol)
        {
            bool qok = hnd.getCurrentTagValue(&cTagId,&tgvalue,&tgvtype,&tgbase,&tgwhen);
            Q_ASSERT(qok);
        }*/
    }

    // autoexpand & select:
    node->setExpanded(true);
    node->setOpen(TRUE);
    ancestorsListView->setSelected(node,TRUE);
    lastSelectedFolder = node;
    //ancestorsListView->setMinimumSize(ancestorsListView->sizeHint());
    ancestorsListView->triggerUpdate();
    myAncDialog->refreshSplitter();
}

void
AncestorDockWin::addChildToNode(AncFolderListItem* parent_node, INT32 newItemId)
{
    ItemHandler hnd;
    draldb->lookForItemId(&hnd, newItemId);
    /*
    INT32 tagIdx = myDB->lookForTaggedValue(TagDescVector::getItemId_TagId(),
                  (UINT64)newItemId);
    */
    if (!hnd.isValidItemHandler())
    {
        QMessageBox::warning (this, "Scanning error","Unable to find referenced ItemID(s).",
        QMessageBox::Ok ,QMessageBox::NoButton);
        return;
    }

    // a) create the folder object and insert it on internal list
    AncFolder* folder = new AncFolder(parent_node->folder(), &hnd, myDB);
    Q_ASSERT(folder!=NULL);
    folderList.append(folder);

    // b) create the folder item list
    AncFolderListItem* item = new AncFolderListItem(parent_node,folder);
    Q_ASSERT(item!=NULL);

    // c) and highlight...
    //printf (">>> AncDocWin::addChildNode, lookForEvent on %d\n",tagIdx);
    QColor nextColor = myHG->peekNextColor();
    int stackIdx = myHG->lookForEvent(&hnd);
    if (stackIdx<0)
    {
        //printf (">>> not found so adding...\n");
        bool needRep = myHG->contentsSelectEvent(&hnd, true,
        ((TwoDAvtView*)mySV)->getCurrentTagSelector(),HLT_DEFAULT);
        if (needRep)
        {
            mySV->repaintContents(false);
        }
    }

    // d) add dependecy arrows
    // get origin point
    INT32 originRow = parent_node->folder()->getOriginRow();
    INT32 originCol = parent_node->folder()->getOriginCol();
    if ((originRow<0)||(originCol<0))
    {
        return;
    }

    // get destination point
    INT32 destCol,destRow;

    bool putArrow = getArrowDest(newItemId,originCol,originRow,&destCol,&destRow);
    if (putArrow)
    {
        originCol -= myDB->getFirstEffectiveCycle();
        double x0=originCol*CUBE_SIDE;
        double y0=originRow*CUBE_SIDE;
        double x1;
        double y1;

        destCol -= myDB->getFirstEffectiveCycle();
        x1 = destCol*CUBE_SIDE;
        y1 = destRow*CUBE_SIDE;
        x0+=CUBE_SIDE/2;
        x1+=CUBE_SIDE/2;
        y0+=CUBE_SIDE/2;
        y1+=CUBE_SIDE/2;
        AnnotationAutoBezierArrow* arrow = new AnnotationAutoBezierArrow(x0,y0,x1,y1);
        arrow->setPen(QPen(nextColor,3));
        arrow->setSelectable(false);
        arrow->setBuilt();
        myAnnCtrl->addItem(arrow);
        //printf ("inserting arrow with itemIdx=%d, computed coords (%g,%g)->(%g,%g)\n",tagIdx,x0,y0,x1,y1);
        arrowList->insert(hnd.uniqueIndex(), arrow);
        folder->setOriginCol(destCol+myDB->getFirstEffectiveCycle());
        folder->setOriginRow(destRow);
        mySV->repaintContents(false);
    }
}

bool
AncestorDockWin::getArrowDest(INT32 newItemId,INT32 cCol, INT32 cRow, INT32* destCol, INT32* destRow)
{
    bool wfnd;
    INT32 fcol,lcol;
    INT32 pcol=-1;
    INT32 prow=-1;
    INT32 row=-1;
    UINT16 oedge_id = myDB->getRowEdgeId(cRow);

    wfnd = myDB->findItemWindow(newItemId,&fcol,&lcol);
    if (!wfnd)
    {
        return (false);
    }

    //printf ("fcol=%d, lcol=%d ccol=%d,crow=%d \n",fcol,lcol,cCol,cRow);

    lcol = QMIN(lcol,cCol);
    INT32 col=lcol;
    bool fnd = false;
    bool pfnd = false;
    while ((!fnd) && (col>=fcol) )
    {
        row = myDB->hasItemId(col,newItemId);
        if (row>=0)
        {
            fnd = (oedge_id == myDB->getRowEdgeId(row));
            if (!pfnd)
            {
                pfnd = true;
                pcol=col;
                prow=row;
            }
        }
        --col;
    }

    if (fnd)
    {
        (*destCol)=col+1;
        (*destRow)=row;
        return true;
    }
    else if (pfnd)
    {
        (*destCol)=pcol;
        (*destRow)=prow;
        return true;
    }
    else
    {
        return false;
    }
}

void
AncestorDockWin::clearArrows()
{
    QIntDictIterator<AnnotationItem> it (*arrowList);
    AnnotationItem* item = it.current();
    while (item!=NULL)
    {
        myAnnCtrl->annotationRemove(item);
        ++it;
        item = it.current();
    }
}

void
AncestorDockWin::ancFolderChanged(QListViewItem* item)
{
    // I know this a AncFolderListItem subclass!
    AncFolderListItem* myItem = (AncFolderListItem*) item;
    AncFolder* theFolder = myItem->folder();
    addTagsForFolder(theFolder);

    // keep a reference to this folder
    lastSelectedFolder = myItem;
}

void
AncestorDockWin::contextMenuReq(QListViewItem* item,const QPoint &point, int column)
{
    // check validity of some state dependent options:
    AncFolderListItem* myItem = (AncFolderListItem*) item;
    if (myItem==NULL)
    {
        contextMenu->setItemEnabled(expnode_id,false);
        contextMenu->setItemEnabled(highlight_id,false);
        contextMenu->setItemEnabled(arrows_id,false);
    }
    else
    {
        contextMenu->setItemEnabled(expnode_id,!myItem->isExpanded());
        contextMenu->setItemEnabled(highlight_id,true);
        contextMenu->setItemEnabled(arrows_id,true);
    }
    contextMenu->setItemEnabled(reset_id,hasActiveScan());
    // launch popup menu
    contextMenu->exec(QCursor::pos());
}

void
AncestorDockWin::enterEvent (QEvent *e )
{
    QMainWindow* mw = (QMainWindow*)qApp->mainWidget();
    mw->setDockMenuEnabled (false);
}

void
AncestorDockWin::leaveEvent (QEvent *e )
{
    QMainWindow* mw = (QMainWindow*)qApp->mainWidget();
    mw->setDockMenuEnabled (true);
}

void
AncestorDockWin::do_expandNode()
{ expandNode(lastSelectedFolder); }

void
AncestorDockWin::do_collapseNode()
{ collapseNode(lastSelectedFolder); }

void
AncestorDockWin::do_clearAll()
{ reset(); }

void
AncestorDockWin::do_switchHighlight()
{
    bool needRep;
    AncFolder* theFolder = lastSelectedFolder->folder();
    ItemHandler hnd = theFolder->getHandler();

    // check whether the node was highlighted or not
    //printf (">>> AncDocWin::switch, lookForEvent on %d\n",itemIdx);
    int stackIdx = myHG->lookForEvent(&hnd);
    if (stackIdx>=0)
    {
         // in this case we un-highlight such criteria
        myHG->purgeLayer(stackIdx);
        needRep = true;
    }
    else
    {
        needRep = myHG->contentsSelectEvent(&hnd, true,
        ((TwoDAvtView*)mySV)->getCurrentTagSelector(),HLT_DEFAULT);
    }

    if (needRep)
    { mySV->repaintContents(false); }
}

void
AncestorDockWin::do_switchArrows()
{
    AncFolder* theFolder = lastSelectedFolder->folder();
    ItemHandler hnd = theFolder->getHandler();
    //printf ("arrow switch on itemIdx=%d\n",itemIdx);
    AnnotationItem* item = arrowList->find(hnd.uniqueIndex());
    if (item!=NULL)
    {
        item->setVisible(! (item->isVisible()) );
        mySV->repaintContents(false);
    }
    else
    {
        qApp->beep();
    }
}

void
AncestorDockWin::resizeEvent ( QResizeEvent * e )
{
    //printf("anc resize!\n");fflush(stdout);
    if (!dockInit)
    {
        PreferenceMgr* prefMgr = PreferenceMgr::getInstance();
        prefMgr->setRelationshipWindowWidth(width());
        prefMgr->setRelationshipWindowHeight(height());
        //printf ("anc pref width=%d height=%d\n",width(),height());fflush(stdout);
    }
}


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

/**
  * @file AncestorDockWin.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/ResourceDAvtView.h"
#include "layout/resource/LayoutResourceCache.h"
#include "AncestorDockWin.h"
#include "PreferenceMgr.h"
#include "AncestorWidgetImpl.h"

// Asim includes.
#include "asim/AnnotationCtrl.h"

// Qt includes.
#include <qmessagebox.h>
#include <qapplication.h>

AncestorDockWin::AncestorDockWin(DreamsDB * _dreamsdb, LayoutResourceCache * _cache, PreferenceMgr * _prefMgr, AnnotationCtrl * ann, QMainWindow * mw, QScrollView * qsv, HighLightMgr * hmgr)
{
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
    cache = _cache;
    prefMgr = _prefMgr;
    myMW = mw;
    myHG = hmgr;
    mySV = qsv;
    myAnnCtrl = ann;

    trackingTagId = TAG_ID_INVALID;

    // Pre-build the AncDialog.
    myAncDialog = new AncestorWidgetImpl(this, "ancDialog", 0);
    Q_ASSERT(myAncDialog != NULL);
    ancestorsListView = myAncDialog->getAncestorsListView();
    tagsListView = myAncDialog->getTagsListView();
    ancestorsListView->setRootIsDecorated(TRUE);

    // Connect some signals.
    connect(ancestorsListView, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(ancFolderChanged(QListViewItem *)));
    connect(ancestorsListView, SIGNAL(rightButtonPressed(QListViewItem *, const QPoint &, int)), this, SLOT(contextMenuReq(QListViewItem *, const QPoint &, int)));

    // Sets the docking state.
    setResizeEnabled(TRUE);
    setCloseMode(QDockWindow::Always);
    setMovingEnabled(true);
    setCaption("Relationship Window");

    mw->addToolBar(this);
    mw->moveDockWindow(this, prefMgr->getRelationshipWindowDockPolicy(), prefMgr->getRelationshipWindowNl(), prefMgr->getRelationshipWindowIdx(), prefMgr->getRelationshipWindowOffset());

    arrowList = new QIntDict<AnnotationItem>(31);

    // pre-build the popup assoc menu.
    addContextMenu();

    setWidget(myAncDialog);
    show();

    INT32 num_tags;

    num_tags = draldb->getNumTags();
    valids = new bool[num_tags];
    values = new UINT64[num_tags];
}

AncestorDockWin::~AncestorDockWin()
{
    delete myAncDialog;
    delete arrowList;
    delete contextMenu;
    delete [] valids;
    delete [] values;
}

/*
 * Establishes the Tag used for tracking relationships.
 *
 * @return void.
 */
void
AncestorDockWin::setTrackingTag(TAG_ID id)
{
    trackingTagId = id;
}

void
AncestorDockWin::addContextMenu()
{
    contextMenu = new QPopupMenu(this);
    Q_ASSERT(contextMenu != NULL);
    expnode_id = contextMenu->insertItem("&Expand Node", this, SLOT(do_expandNode()));
    //contextMenu->insertItem("&Collapse Node", this, SLOT(do_collapseNode()));
    reset_id = contextMenu->insertItem("&Reset Scan", this, SLOT(do_clearAll()));
    contextMenu->insertSeparator();
    highlight_id = contextMenu->insertItem("Switch &Highlighting", this, SLOT(do_switchHighlight()));
    arrows_id = contextMenu->insertItem("Switch &Arrow", this, SLOT(do_switchArrows()));
}

/*
 * Just clean-up any ongoing relationship tracking.
 *
 * @return void.
 */
void
AncestorDockWin::reset()
{
    // clear arrows if any
    int cnt = arrowList->count();
    clearArrows();

    // clear relationship scan highlights
    bool needRep = false;
    QPtrList<AncFolderListItem> gl = AncFolderListItem::getGlobalList();
    QPtrListIterator<AncFolderListItem> it_anc(gl);
    AncFolderListItem * current;

    while((current = it_anc.current()) != NULL)
    {
        ++it_anc;
        AncFolder * theFolder = current->folder();
        ItemHandler hnd = theFolder->getHandler();

        // check whether the node was highlighted or not
        //printf (">>> AncDocWin::reset, lookForEvent on %d\n",itemIdx);
        int stackIdx = myHG->lookForEvent(&hnd);
        if(stackIdx >= 0)
        {
            // in this case we un-highlight such criteria
            myHG->purgeLayer(stackIdx, true);
            needRep = true;
        }
    }
    if(needRep || (cnt > 0))
    {
        mySV->repaintContents(false);
    }

    ancestorsListView->clear();
    tagsListView->clear();
}

bool
AncestorDockWin::hasActiveScan()
{
    QPtrList<AncFolderListItem> gl = AncFolderListItem::getGlobalList();
    return (gl.count() > 0);
}

/*
 * Start to track a relationship starting with the given item handler.
 * hnd is the starting point to look for in the tag heap.
 *
 * @return void.
 */
void
AncestorDockWin::initTrackingWidth(ItemHandler * handler, CYCLE originCycle, INT32 originRow)
{
    if(dreamsdb->layoutResourceGetRowType(originRow, cache) != SimpleEdgeRow)
    {
        return;
    }

    myOriginEdgeTrackId = dreamsdb->layoutResourceGetRowTrackId(originRow, cache);
    //printf ("origin col =%d, origin row = %d, edge_id = %d\n",originCol, originRow,myOriginEdgeTrackId);

    QString trackingStr = prefMgr->getRelationshipTag();
    trackingTagId = draldb->getTagId(trackingStr);

    // ok I got a entry point into the tag heap
    // 0) Clear any previous data
    reset();

    // 1) Add this ItemId to the tree view

    // a) create the folder object and insert it on internal list
    AncFolder * folder = new AncFolder(NULL, handler);
    Q_ASSERT(folder != NULL);
    folder->setOriginRow(originRow);
    folder->setOriginCycle(originCycle);
    folderList.append(folder);

    // b) create the folder item list
    AncFolderListItem * item = new AncFolderListItem(ancestorsListView, folder);
    Q_ASSERT(item != NULL);

    // c) highlight (by ItemID) the selected item
    //printf (">>> AncDocWin::init, lookForEvent on %d\n",tagIdx);
    INT32 stackIdx = myHG->lookForEvent(handler);
    if(stackIdx < 0)
    {
        //printf (">>> not found so adding...\n");
        bool needRep = myHG->contentsSelectEvent(handler, true, ((ResourceDAvtView *) mySV)->getCurrentTagSelector(), HLT_DEFAULT, true, draldb->getLastCycle());
        if(needRep)
        {
            mySV->repaintContents(false);
        }
    }

    // 2) Add the related tags-value pairs in the tag view
    addTagsForFolder(folder);

    // 3) Add the parents of ItemId
    expandNode(item);
}

void
AncestorDockWin::addTagsForFolder(AncFolder * folder)
{
    ItemHandler hnd; ///< Handler to get the tags.
    INT32 num_tags;

    num_tags = draldb->getNumTags();
    hnd = folder->getHandler();

    // Clears the valid array.
    memset(valids, 0, num_tags * sizeof(bool));

    // Purge Qt widget.
    tagsListView->clear();

    // Gets all the values.
    while(hnd.isValidTagHandler())
    {
        // Caches the information.
        hnd.cacheTag();

        // Updates the state of the values.
        TAG_ID tag_id = hnd.getTagCachedId();
        valids[tag_id] = hnd.getTagCachedDefined();
        values[tag_id] = hnd.getTagCachedValue();
        hnd.nextTag();
    }

    // We now have the mutable tags "fused", so we just have to
    // iterate along all the tags.
    for(TAG_ID i = 0; i < num_tags; i++)
    {
        // We have a tag if is valid, isn't the item id and is not the tracking tag id.
        if(valids[i] && (i != draldb->getItemIdTagId()) && (i != trackingTagId))
        {
            QString tagName = draldb->getTagName(i);
            QString tagFormatedValue = draldb->getFormatedTagValue(i, values[i]);

            AncTagEntry * ate = new AncTagEntry(tagName, tagFormatedValue);
            Q_ASSERT(ate != NULL);
            AncTagListItem * atli = new AncTagListItem(tagsListView, ate);
            Q_ASSERT(atli != NULL);
        }
    }
}

void
AncestorDockWin::collapseNode(AncFolderListItem* node)
{
    delete node;
}

void
AncestorDockWin::expandNode(AncFolderListItem * node)
{
    //printf ("AncestorDockWin::expandNode() called\n");
    if(node->isExpanded())
    {
        // This should never happen (context menu must prevent it...)
        qApp->beep();
        return;
    }

    CYCLE tgwhen;
    UINT64 tgvalue;
    TagValueBase tgbase;
    TagValueType tgvtype;

    // get the item index of such a node
    AncFolder * theFolder = node->folder();
    ItemHandler hnd = theFolder->getHandler();
    CYCLE cycle = draldb->getLastCycle();

    // now look for the target tag  (typically ANCESTOR); use latest known value
    if(!hnd.getTagById(trackingTagId, &tgvalue, &tgvtype, &tgbase, &tgwhen, cycle))
    {
        QMessageBox::warning(this, "Scanning error", "The relationship tag criteria was not found.", QMessageBox::Ok, QMessageBox::NoButton);
        node->setExpanded(true);
        return;
    }

    // If this is not a list we are done!
    if(tgvtype != TAG_SOV_TYPE)
    {
        QMessageBox::warning(this, "Scanning error", "The relationship tag criteria must of type 'set of values'", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    QValueList<INT32> insertedList;

    addChildToNode(node, tgvalue);
    insertedList.append(tgvalue);

    // ok, this is a list, iterate through..
    TAG_ID cTagId;

    // The first tag is skipped in the condition, and each
    // new iteration skips the last tag also in the condition.
    while(hnd.nextTag() && (hnd.getTagId() == trackingTagId))
    {
        tgvalue = hnd.getTagValue();
        //printf ("expanding node with value=%lu\n",tgvalue);
        if(insertedList.find(tgvalue) == insertedList.end())
        {
            addChildToNode(node, tgvalue);
            insertedList.append(tgvalue);
        }
        else
        {
            QMessageBox::warning(this, "Scanning incoherence", "Ignoring repeated item in relationship scan.", QMessageBox::Ok, QMessageBox::NoButton);
        }
    }

    // autoexpand & select:
    node->setExpanded(true);
    node->setOpen(TRUE);
    ancestorsListView->setSelected(node, TRUE);
    lastSelectedFolder = node;
    //ancestorsListView->setMinimumSize(ancestorsListView->sizeHint());
    ancestorsListView->triggerUpdate();
    myAncDialog->refreshSplitter();
}

/*
 * Adds the item newItemId to the parent_node node and adds
 * a row between these two items.
 *
 * @return void.
 */
void
AncestorDockWin::addChildToNode(AncFolderListItem * parent_node, ITEM_ID newItemId)
{
    ItemHandler hnd;
    draldb->lookForItemId(&hnd, newItemId);

    if(!hnd.isValidItemHandler())
    {
        QMessageBox::warning(this, "Scanning error", "Unable to find referenced ItemID(s).", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    // a) create the folder object and insert it on internal list.
    AncFolder * folder = new AncFolder(parent_node->folder(), &hnd);
    Q_ASSERT(folder != NULL);
    folderList.append(folder);

    // b) create the folder item list.
    AncFolderListItem * item = new AncFolderListItem(parent_node,folder);
    Q_ASSERT(item != NULL);

    // c) and highlight...
    QColor nextColor = myHG->peekNextColor();
    INT32 stackIdx = myHG->lookForEvent(&hnd);
    if(stackIdx < 0)
    {
        bool needRep = myHG->contentsSelectEvent(&hnd, true, ((ResourceDAvtView *) mySV)->getCurrentTagSelector(), HLT_DEFAULT, false, draldb->getLastCycle());
        if(needRep)
        {
            mySV->repaintContents(false);
        }
    }

    // d) add dependecy arrows
    // get origin point
    INT32 originRow = parent_node->folder()->getOriginRow();
    CYCLE originCyc = parent_node->folder()->getOriginCycle();

    if((originRow < 0) || (originCyc.cycle < 0))
    {
        return;
    }

    // get destination point
    CYCLE destCyc;
    INT32 destRow;

    bool putArrow = getArrowDest(newItemId, originCyc, originRow, &destCyc, &destRow);
    if(putArrow)
    {
        double x0;
        double y0;
        double x1;
        double y1;

        folder->setOriginCycle(destCyc);
        folder->setOriginRow(destRow);

        destCyc.cycle -= draldb->getFirstEffectiveCycle(destCyc.clock->getId()).cycle;
        originCyc.cycle -= draldb->getFirstEffectiveCycle(originCyc.clock->getId()).cycle;

        if(dreamsdb->layoutResourceGetUsesDivisions())
        {
            INT32 temp;

            // Gets the total number of cubes.
            temp = (originCyc.cycle << 1) + originCyc.division;
            // Multiplies the cubes by the cube side and the fastest ratio.
            x0 = (INT32) ((float) temp * (float) CUBE_SIDE * originCyc.clock->getFastestRatio());

            temp = (destCyc.cycle << 1) + destCyc.division;
            x1 = (INT32) ((float) temp * (float) CUBE_SIDE * destCyc.clock->getFastestRatio());
        }
        else
        {
            x0 = (INT32) ((float) originCyc.cycle * (float) CUBE_SIDE * originCyc.clock->getFastestRatio());
            x1 = (INT32) ((float) destCyc.cycle * (float) CUBE_SIDE * destCyc.clock->getFastestRatio());
        }

        y0 = originRow * CUBE_SIDE;
        y1 = destRow * CUBE_SIDE;
        x0 += (INT32) ((float) CUBE_SIDE * originCyc.clock->getFastestRatio()) >> 1;
        x1 += (INT32) ((float) CUBE_SIDE * destCyc.clock->getFastestRatio()) >> 1;
        y0 += CUBE_SIDE >> 1;
        y1 += CUBE_SIDE >> 1;

        AnnotationAutoBezierArrow * arrow = new AnnotationAutoBezierArrow(x0, y0, x1, y1);
        arrow->setPen(QPen(nextColor, 3));
        arrow->setSelectable(false);
        arrow->setBuilt();
        myAnnCtrl->addItem(arrow);

        //printf ("inserting arrow with itemIdx=%d, computed coords (%g,%g)->(%g,%g)\n",tagIdx,x0,y0,x1,y1);
        arrowList->insert(hnd.uniqueIndex(), arrow);
        mySV->repaintContents(false);
    }
}

/*
 * This function tries to find the event shown in the resource
 * layout that is nearest to the point (cCyc, cRow) and returns
 * the position of this point.
 *
 * @return if a destiny has been found.
 */
bool
AncestorDockWin::getArrowDest(ITEM_ID newItemId, CYCLE cCyc, INT32 cRow, CYCLE * destCyc, INT32 * destRow)
{
    CYCLE baseCycle;
    float bestDist;

    // The base cycle works in the fastest clock domain (resource base clock).
    baseCycle.clock = * draldb->getClockDomainFreqIterator();
    baseCycle.fromLCMCycles(cCyc.toLCMCycles());

    // Sets the best distance as a big distance.
    bestDist = 100000000.0f;

    // Iterate along the events of the handler to know in what cycles
    // and edges has happened something.
    ItemHandler hnd;     ///< Handler used to iterate along the events.
    ItemHandler tempHnd; ///< Temporal handler.

    // Gets the handler for the item.
    draldb->lookForItemId(&hnd, newItemId);
    Q_ASSERT(hnd.isValidItemHandler());

    // Iterates until all the events are used.
    while(hnd.isValidEventHandler())
    {
        // Gets the type of event.
        ItemHeapEventSubType type = hnd.getEventType();

        // TODO: add slot support...
        // We only care about move items.
        if(type == IDB_MOVE_ITEM)
        {
            // Gets the edge were the move item has happened.
            DRAL_ID edgeId = hnd.getEventEdgeId();
            CYCLE eventCycle = hnd.getEventCycle();
            const rowList * rows;
            rowList::const_iterator it;

            // Gets all the rows where this edge appears.
            dreamsdb->layoutResourceGetRowsWithDralId(edgeId, SimpleEdgeRow, &rows);

            it = rows->begin();

            // Checks all the rows.
            while(it != rows->end())
            {
                // Gets the distance.
                INT32 row_dst;
                INT32 col_dst;
                INT64 temp_col_dst;
                TRACK_ID trackId;

                trackId = dreamsdb->layoutResourceGetRowTrackId(* it, dreamsdb->layoutResourceGetFlatCache());
                draldb->getItemInSlot(trackId, eventCycle, &tempHnd);

                // If no item is inside the row or the item inside is different,
                // then we must skip this row.
                if(tempHnd.isValidItemHandler() && (tempHnd.getItemId() == newItemId))
                {
                    CYCLE drawCycle;
                    CYCLE tempCycle;

                    // This moves the destiny cycle to the drawn cycle, to support
                    // draw on move rows.
                    drawCycle = eventCycle;
                    if(dreamsdb->layoutResourceGetRowIsDrawOnReceive(* it, &tempCycle, dreamsdb->layoutResourceGetFlatCache()))
                    {
                        drawCycle = drawCycle + tempCycle;
                    }

                    // Gets the square row distance.
                    row_dst = (* it) - cRow;
                    row_dst = row_dst * row_dst;

                    // Gets the distance in LCM cycles.
                    temp_col_dst = abs(baseCycle.toLCMCycles() - drawCycle.toLCMCycles());

                    // Gets the result in resource base cycles.
                    tempCycle = baseCycle;
                    tempCycle.fromLCMCycles(temp_col_dst);

                    // Computes the col distance depending if divisions are showed or not.
                    if(dreamsdb->layoutResourceGetUsesDivisions())
                    {
                        col_dst = (tempCycle.cycle << 1) + tempCycle.division;
                    }
                    else
                    {
                        col_dst = tempCycle.cycle;
                    }

                    col_dst = col_dst * col_dst;

                    // Gets the entire distance.
                    float dist = sqrt((float) (row_dst + col_dst));

                    // Updates the destiny event if better result.
                    if(dist < bestDist)
                    {
                        bestDist = dist;
                        * destCyc = drawCycle;
                        * destRow = * it;
                    }
                }

                ++it;
            }
        }

        hnd.nextEvent();
    }

    return (bestDist != 100000000.0f);
}

void
AncestorDockWin::clearArrows()
{
    QIntDictIterator<AnnotationItem> it (* arrowList);
    AnnotationItem * item;

    while((item = it.current()) != NULL)
    {
        ++it;
        myAnnCtrl->annotationRemove(item);
    }

    arrowList->clear();
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
        contextMenu->setItemEnabled(expnode_id, false);
        contextMenu->setItemEnabled(highlight_id, false);
        contextMenu->setItemEnabled(arrows_id, false);
    }
    else
    {
        contextMenu->setItemEnabled(expnode_id, !myItem->isExpanded());
        contextMenu->setItemEnabled(highlight_id, true);
        contextMenu->setItemEnabled(arrows_id, true);
    }
    contextMenu->setItemEnabled(reset_id, hasActiveScan());
    // launch popup menu
    contextMenu->exec(QCursor::pos());
}

void
AncestorDockWin::enterEvent(QEvent * e)
{
    QMainWindow * mw = (QMainWindow *) qApp->mainWidget();
    mw->setDockMenuEnabled(false);
}

void
AncestorDockWin::leaveEvent(QEvent * e)
{
    QMainWindow * mw = (QMainWindow *) qApp->mainWidget();
    mw->setDockMenuEnabled(true);
}

void
AncestorDockWin::do_expandNode()
{
    expandNode(lastSelectedFolder);
}

void
AncestorDockWin::do_collapseNode()
{
    collapseNode(lastSelectedFolder);
}

void
AncestorDockWin::do_clearAll()
{
    reset();
}

void
AncestorDockWin::do_switchHighlight()
{
    bool needRep;
    AncFolder* theFolder = lastSelectedFolder->folder();
    ItemHandler hnd = theFolder->getHandler();

    // check whether the node was highlighted or not
    //printf (">>> AncDocWin::switch, lookForEvent on %d\n",itemIdx);
    INT32 stackIdx = myHG->lookForEvent(&hnd);
    if(stackIdx >= 0)
    {
         // in this case we un-highlight such criteria
        myHG->purgeLayer(stackIdx, true);
        needRep = true;
    }
    else
    {
        needRep = myHG->contentsSelectEvent(&hnd, true, ((ResourceDAvtView*)mySV)->getCurrentTagSelector(), HLT_DEFAULT, false, draldb->getLastCycle());
    }

    if(needRep)
    {
        mySV->repaintContents(false);
    }
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
AncestorDockWin::resizeEvent(QResizeEvent * e)
{
    //printf("anc resize!\n");fflush(stdout);
    if(!dockInit)
    {
        prefMgr->setRelationshipWindowWidth(width());
        prefMgr->setRelationshipWindowHeight(height());
        //printf ("anc pref width=%d height=%d\n",width(),height());fflush(stdout);
    }
}

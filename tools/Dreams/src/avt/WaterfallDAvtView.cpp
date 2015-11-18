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
  * @file WaterfallDAvtView.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/WaterfallDAvtView.h"
#include "dialogs/event/EventDialogImpl.h"
#include "dialogs/tag/TagDialogImpl.h"
#include "dockwin/tag/TagDockWin.h"
#include "highlight/HighLightMgrWFall.h"
#include "mdi/MDIWaterfallWindow.h"
#include "Dreams.h"
#include "ShowTags.h"
#include "FavoriteTags.h"
#include "DrawingTools.h"
#include "PreferenceMgr.h"
#include "DreamsDB.h"

// Qt includes.
#include <qstringlist.h>
#include <qtooltip.h>
#include <qaccel.h>

// Externs.
extern QWidget * focusMDI;

WaterfallDAvtView::WaterfallDAvtView(DreamsDB * _dreamsdb, QWidget * parent, QMainWindow * mdi, QMainWindow * main)
    : DAvtView(parent, main, cube_side, cube_side)
{
    myMDI = mdi;
    mdiLocked = true;

    // filter out some events
    horizontalScrollBar()->installEventFilter(this);
    verticalScrollBar()->installEventFilter(this);
    viewport()->installEventFilter(this);

    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
    prefMgr = PreferenceMgr::getInstance();

    AScrollView::setRealContentsSize((INT32) dreamsdb->layoutWaterfallGetWidth(), (INT32) dreamsdb->layoutWaterfallGetHeight());
    totalPixels = (INT64) dreamsdb->layoutWaterfallGetWidth();
    CYCLE first = draldb->getFirstEffectiveCycle((* draldb->getClockDomainFreqIterator())->getId());
    CYCLE last = draldb->getLastEffectiveCycle((* draldb->getClockDomainFreqIterator())->getId());
    if(dreamsdb->layoutWaterfallGetUsesDivisions())
    {
        if(last.division == 0)
        {
            last.division++;
        }
        else
        {
            last.cycle++;
            last.division = 0;
        }
    }
    else
    {
        last.cycle++;
    }

    totalPs = last.toPs() - first.toPs();
    //updateCloseMenuState();

    annCtrl->setBaseSize(getInternalBaseElementSize());
    gridSize = (int) ceil(getInternalBaseElementSize() * 10.0);
    //annCtrl->setGridSize(gridSize);
    annCtrl->setGridSize(getInternalBaseElementSize()); // tmp hack

    initShadeFlags();

    // create tag-favorite class
    ft = FavoriteTags::getInstance();
    currentTagSelector = ft->getList().first();

    // HighLight manager
    hlmgr = new HighLightMgrWFall(dreamsdb, cube_side);
    connect(hlmgr, SIGNAL(highlightChanged(int)), mainWin, SLOT(highlightChanged(int)));
    connect(hlmgr, SIGNAL(purgeLastHighlight()), mainWin, SLOT(purgeLastHighlight()));
    connect(hlmgr, SIGNAL(unhighlightCriteriaSignal(TAG_ID, UINT64)), mainWin, SLOT(unhighlightCriteria(TAG_ID, UINT64)));
    connect(hlmgr, SIGNAL(contentsSelectedEvent(TAG_ID, UINT64, bool, HighLightType, bool)), mainWin, SLOT(contentsSelectedEvent(TAG_ID, UINT64, bool, HighLightType, bool)));

    htype = HLT_DEFAULT;

    // by default we see both highlighted and non-highlighted guys
    vHEvents  = true;
    vNHEvents = true;

    // I add a popup menu to ease some common operations
    addContextMenu();
    rc_pressed_x=-1;
    rc_pressed_y=-1;
    rc_released_x=-1;
    rc_released_y=-1;

    shaderBrush = QBrush(prefMgr->getShadingColor());
    bgColor = prefMgr->getBackgroundColor();

    tip = new TipWaterfallDreams(this);

    copyAnnItem = NULL;
    selectedRow=-1;
    setMaxScallingFactor(3.0);

    // Sets the currentCycle as the first cycle of the fastest clock.
    currentCycle = draldb->getFirstEffectiveCycle((* draldb->getClockDomainFreqIterator())->getId());
}

WaterfallDAvtView::~WaterfallDAvtView()
{
    delete tip;
    delete contextMenu;
}

void 
WaterfallDAvtView::contentsSelectEvent(INT32 col, INT32 row, bool adding, bool shifted)
{
    bool needRep;
    UINT32 myRow;
    CLOCK_ID clockId;
    const ClockDomainEntry * clock;
    ItemHandler hnd;
    ExpressionList * rlist;

    myRow = row / getInternalBaseElementSize();
    myRow = QMAX(myRow, 0);
    if(myRow >= dreamsdb->layoutWaterfallGetNumRows())
    {
        return;
    }

    setSelectedRow(myRow);

    INT64 ps = (totalPs * col) / totalPixels;

    if(dreamsdb->layoutWaterfallHasValidData(myRow, ps, &hnd, &rlist))
    {
        needRep = hlmgr->contentsSelectEvent(0, myRow, hnd.getEventCycle(), adding, currentTagSelector, htype, false);
    }
    else
    {
        needRep = hlmgr->purgeLast();
        if(mdiLocked)
        {
            emit purgeLastHighlight();
        }
    }

    if(needRep)
    {
        // we can have other windows in synch...
        repaintContents(false);
    }

    emit contentSelected(col / getInternalBaseElementSize(), row / getInternalBaseElementSize(), adding, shifted);
}

void
WaterfallDAvtView::reset()
{
    AScrollView::setRealContentsSize(0,0);
    totalPixels = 0;
    totalPs = 0;

    hlmgr->purgeAll();
    updateCloseMenuState();

    // annotation tools
    annCtrl->reset();

    // do a forced repaint
    repaintContents(false);
    setPointerType (POINTER_SELECT);
}

// --------------------------------------------------------------------
// --------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// Context menu methods
// -----------------------------------------------------------------------------------
void 
WaterfallDAvtView::addContextMenu()
{
    contextMenu = new QPopupMenu(this);
    contextMenuItemTools = new QPopupMenu(contextMenu);
    contextMenuEventTools = new QPopupMenu(contextMenu);
    contextMenuSyncTools = new QPopupMenu(contextMenu);

    // Item tools.
    contextMenu->insertItem("&Item Tools", contextMenuItemTools);
    show_all_events_id = contextMenuItemTools->insertItem("Show All &Events", this, SLOT(do_showAllEvents()));
    show_all_tags_id = contextMenuItemTools->insertItem("Show All &Tags", this, SLOT(do_showAllTags()));

    // Event tools.
    contextMenu->insertItem("&Event Tools", contextMenuEventTools);

    // Synchronization tools.
    contextMenu->insertItem("&Sync Tools", contextMenuSyncTools);
    contextMenuSyncTools->insertItem("Sync To This &Cycle", this, SLOT(do_syncToCycle()));
    contextMenuSyncTools->insertItem("Sync To This Cycle (same &trace)", this, SLOT(do_syncToCycleTrace()));

    // Free window.
    contextMenu->insertSeparator();
    rebel_id = contextMenu->insertItem("&Free Window", this, SLOT(do_switchRebel()));
    // By defaul MDI is NOT rebel.
    contextMenu->setItemChecked(rebel_id, false);
}

void
WaterfallDAvtView::do_showAllEvents()
{
    ItemHandler hnd;
    ITEM_ID itemid = dreamsdb->layoutWaterfallGetItemIdForRow(popup_row);
    draldb->lookForItemId(&hnd, itemid);

    if(hnd.isValidItemHandler())
    {
        EventDialogImpl ed(draldb, &hnd, popup_cycle, this, "ed", true);
        ed.exec();
    }
}

void
WaterfallDAvtView::do_showAllTags()
{
    ItemHandler hnd;
    ITEM_ID itemid = dreamsdb->layoutWaterfallGetItemIdForRow(popup_row);
    draldb->lookForItemId(&hnd, itemid);

    if(hnd.isValidItemHandler())
    {
        TagDialogImpl td(draldb, &hnd, popup_cycle, this, "td", true);
        td.exec();
    }
}

void
WaterfallDAvtView::do_syncToCycle()
{
    emit syncToCycle(popup_cycle_sync);
}

void
WaterfallDAvtView::do_syncToCycleTrace()
{
    emit syncToCycleTrace(popup_cycle_sync);
}

void
WaterfallDAvtView::do_switchRebel()
{
    ((MDIWaterfallWindow *) myMDI)->switchMDILocked();
}

QString
WaterfallDAvtView::getRowInfo(INT32 selrow)
{
    QString report("");
    return report;
}

void 
WaterfallDAvtView::rightClickPressEvent(QMouseEvent* e)
{
    rc_pressed_x = e->x();
    rc_pressed_y = e->y();
}

void 
WaterfallDAvtView::rightClickReleaseEvent(QMouseEvent* e)
{
    rc_released_x = e->x();
    rc_released_y = e->y();

    bool c1 = abs(rc_pressed_x - rc_released_x) < 3;
    bool c2 = abs(rc_pressed_y - rc_released_y) < 3;
    if(c1 && c2)
    {
        const ClockDomainEntry * clock;
        double wx, wy;
        INT32 cx, cy;
        CLOCK_ID clockId;

        viewportToContents(rc_released_x, rc_released_y, cx, cy);
        wx = ((double) cx / scf_x);
        wy = ((double) (cy - myVerticalOffset) / scf_y);

        popup_row = (INT32) (wy / (double) getInternalBaseElementSize());

        // Coherency check.
        if(((UINT32) popup_row >= dreamsdb->layoutWaterfallGetNumRows()) || (popup_row < 0))
        {
            return;
        }

        clockId = (* draldb->getClockDomainFreqIterator())->getId();
        popup_cycle.clock = draldb->getClockDomain(clockId);
        popup_cycle_sync.clock = draldb->getClockDomain(clockId);
        toCycle((INT32) wx, popup_row, true, &popup_cycle);
        toCycle((INT32) wx, popup_row, false, &popup_cycle_sync);
        popup_cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;
        popup_cycle_sync.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

        INT64 ps = (totalPs * (INT32) wx) / totalPixels;
        ExpressionList * tmplist;
        ItemHandler hnd;

        if(dreamsdb->layoutWaterfallHasValidData(popup_row, ps, &hnd, &tmplist))
        {
            contextMenuEventTools->setEnabled(true);
        }
        else
        {
            contextMenuEventTools->setEnabled(false);
        }

        // check if there is something there
        bool ena = true; //dreamsdb->(popup_row, popup_cycle);

        contextMenu->setItemEnabled(show_all_events_id, ena);
        contextMenu->setItemEnabled(show_all_tags_id, ena);
        contextMenu->exec(QCursor::pos());
    }
}


/**
 * Draws a section of the world, that is shown in the current view.
 *
 * @param p Painter to paint to
 * @param ncx X coordinate of the viewport
 * @param ncy Y coordinate of the viewport
 * @param ncw Width of the viewport
 * @param nch Height of the viewport
 */
bool
WaterfallDAvtView::drawWorldCoordinatesContents(QPainter* p, int ncx, int ncy, int ncw, int nch)
{
    bool ok = true;

    // compute affected area with the cube_side...
    int x1 = ncx - (ncx % cube_side);
    int x2 = ncx + ncw + 2 * (ncx % cube_side);
    int y1 = ncy - (ncy % cube_side);
    int y2 = ncy + nch + 2 * (ncy % cube_side);

    x1 -= cube_side;
    x2 += cube_side;
    y1 -= cube_side;
    y2 += cube_side;

    x1 = QMAX(0, x1);
    y1 = QMAX(0, y1);
    x2 = QMIN(realContentsWidth - 1, x2);
    y2 = QMIN(realContentsHeight - 1, y2);

    UINT32 col1 = x1 / cube_side;
    UINT32 col2 = (x2 / cube_side) + 1;
    UINT32 row1 = y1 / cube_side;
    UINT32 row2 = (y2 / cube_side) + 1;

    //printf ("refreshing wf col1=%d col2=%d, row1=%d row2=%d\n",col1,col2,row1,row2);
    
    // Draw selection row if any
    if(selectedRow >= 0)
    {
        //cerr << "drawing selected row at " << selectedRow << endl;
        p->fillRect(ncx, selectedRow * cube_side, ncw, cube_side, QBrush(colorGroup().highlight()));
    }

    p->setPen(QPen(QColor(0,0,0), cube_border_size));
    p->setBrush(Qt::NoBrush);
    ItemHandler hnd;
    ExpressionList *elist;
    bool highlighting = false;
    TAG_ID hl_tag = TAG_ID_INVALID;

    // Checks if highlighting must be done.
    if(hlmgr->getNumActiveCriteria() > 0)
    {
        // As we know that just one tag is used for all the highlighting, we
        // just get the tag id of the first criteria.
        highlighting = true;
        hl_tag = hlmgr->getCriteriaAt(0).tagId;
    }

    CYCLE firstCycle = draldb->getFirstEffectiveCycle((* draldb->getClockDomainFreqIterator())->getId());
    // Base cycle works with the fastest clock.
    CYCLE baseCycle(* draldb->getClockDomainFreqIterator());
    row2 = QMIN(row2 + 1, dreamsdb->layoutWaterfallGetNumRows());

    for(UINT32 row = row1; row < row2; row++)
    {
        if(dreamsdb->layoutWaterfallGetItemIdForRow(row) == ITEM_ID_INVALID)
        {
            p->setBrush(QBrush(background, Qt::Dense5Pattern));
            p->setPen(Qt::NoPen);
            DrawingTools::drawRectangle((col1 * cube_side) - 1, (row * cube_side) - 1, p, (cube_side * (col2 - col1)) + 2, cube_side + 2);
            p->setPen(QPen(QColor(0, 0, 0), CUBE_BORDER));
            continue;
        }

        // First we paints all the life time line of the item.
        // Gets the range of the line.
        dreamsdb->layoutWaterfallComputeRangeForRow(row);
        CYCLE minCycle = dreamsdb->layoutWaterfallGetCurrentRowMinCycle();
        CYCLE maxCycle = dreamsdb->layoutWaterfallGetCurrentRowMaxCycle();
        UINT32 currRow_mincol;
        UINT32 currRow_maxcol;

        if(dreamsdb->layoutWaterfallGetUsesDivisions())
        {
            currRow_mincol = (INT32) floorf((float) (minCycle.cycle << 1) * minCycle.clock->getFastestRatio()) - (firstCycle.cycle << 1);
            currRow_maxcol = (INT32) ceilf((float) (maxCycle.cycle << 1) * maxCycle.clock->getFastestRatio()) - (firstCycle.cycle << 1);
        }
        else
        {
            currRow_mincol = (INT32) floorf((float) minCycle.cycle * minCycle.clock->getFastestRatio()) - firstCycle.cycle;
            currRow_maxcol = (INT32) ceilf((float) maxCycle.cycle * maxCycle.clock->getFastestRatio()) - firstCycle.cycle;
        }

        UINT32 myCol1 = QMAX(col1, currRow_mincol);
        UINT32 myCol2 = QMIN(col2, currRow_maxcol);

        // Draws continuation line.
        if((currRow_mincol <= col2) && (currRow_maxcol >= col1))
        {
            p->setPen(QPen(QColor(10, 10, 10), cube_border_size, Qt::DotLine));
            p->drawLine(myCol1 * cube_side, row * cube_side + cube_size, myCol2 * cube_side, row * cube_side + cube_size);
        }
        else
        {
            continue;
        }

        // Now draws all the events that happened to the item.
        // Gets a handler for the item inside the current row.
        ITEM_ID itemid = dreamsdb->layoutWaterfallGetItemIdForRow(row);
        draldb->lookForItemId(&hnd, itemid);
        Q_ASSERT(hnd.isValidItemHandler());

        // This sets the layout wf to a correct state to compute the
        // tags of the item. First sets to invalid all the tags in the
        // rule eval environment with the reset call and then caches
        // the first tag of the item.
        dreamsdb->layoutWaterfallResetColorShapeLetter();
        hnd.cacheTag();

        // Gets the left screen boundary in ps.
        if(dreamsdb->layoutWaterfallGetUsesDivisions())
        {
            baseCycle.cycle = col1 >> 1;
            baseCycle.division = col1 & 1;
        }
        else
        {
            baseCycle.cycle = col1;
            baseCycle.division = 0;
        }

        INT64 leftPs = baseCycle.toPs();

        // First skips all the events that happened before the left boundary of the screen.
        while(hnd.isValidEventHandler())
        {
            // Gets if the event is inside the screen.
            CYCLE cyc = hnd.getEventCycle();
            // We look if the rightest part of the event might appear in the screen.
            cyc.cycle++;
            if(cyc.toPs() >= leftPs)
            {
                // When the actual event gets in the screen the loop is ended.
                break;
            }
            hnd.nextEvent();
        }

        // Gets the right screen boundary in ps.
        if(dreamsdb->layoutWaterfallGetUsesDivisions())
        {
            baseCycle.cycle = (col2 + 1) >> 1;
            baseCycle.division = (col2 + 1) & 1;
        }
        else
        {
            baseCycle.cycle = col2 + 1;
            baseCycle.division = 0;
        }

        INT64 rightPs = baseCycle.toPs();

        // Draws events that happened before the right boundary of the screen.
        while(hnd.isValidEventHandler())
        {
            // Gets if the event is inside the screen.
            CYCLE cyc = hnd.getEventCycle();
            INT64 actPs = cyc.toPs();
            if(actPs > rightPs)
            {
                // When the actual event gets out of the screen, the loop is ended.
                break;
            }

            // Starts the drawing of the event.
            if(dreamsdb->layoutWaterfallGetRules(row, &hnd, &elist))
            {
                EventShape shape;
                char letter;
                QColor letterColor;

                //printf("event detected for col=%d row=%d\n",col,row);
                ColorShapeItem csh = dreamsdb->layoutWaterfallGetColorShapeLetter(&hnd, elist);     

                // Set the fill color
                if(!csh.getTransparencySet() && !csh.getFillColorSet())
                {
                    // default transparent...
                    p->setBrush(Qt::NoBrush);
                }
                else
                {
                    if(!csh.getTransparency())
                    {
                        p->setBrush(QBrush(AColorPalette::getColor(csh.getColorIdx())));
                    }
                    else
                    {
                        p->setBrush(Qt::NoBrush);
                    }
                }

                INT64 hl_tag_value;

                // If highlighting is activated, then tries to get a matching criteria.
                // Uses the actual value for the item stored in the layout waterfall.
                if(highlighting && dreamsdb->layoutWaterfallGetCurrentITagValue(hl_tag, &hl_tag_value))
                {
                    INT32 criteria = hlmgr->getMatchingCriteria(hl_tag, hl_tag_value);
                    if(criteria >= 0)
                    {
                        hlmgr->selectPen(p, hlmgr->getCriteriaAt(criteria).type);
                    }
                    else
                    {
                        p->setPen(QPen(AColorPalette::getColor(csh.getBColorIdx()), cube_border_size));
                    }
                }
                else
                {
                    p->setPen(QPen(AColorPalette::getColor(csh.getBColorIdx()), cube_border_size));
                }

                shape = csh.getShape();
                letter = csh.getLetter();
                letterColor = AColorPalette::getColor(csh.getLetterColorIdx());

                if(dreamsdb->layoutWaterfallGetUsesDivisions())
                {
                    DrawingTools::drawShape((INT32) ((float) ((cyc.cycle * (cube_side << 1)) + (cyc.division * cube_side)) * cyc.clock->getFastestRatio()), row * cube_side, p, shape, QChar(letter), letterColor, (INT32) ((float) cube_side * cyc.clock->getFastestRatio()), cube_side, this);
                }
                else
                {
                    DrawingTools::drawShape((INT32) ((float) ((cyc.cycle * cube_side) + (cyc.division * (cube_side >> 1))) * cyc.clock->getFastestRatio()), row * cube_side, p, shape, QChar(letter), letterColor, (INT32) ((float) cube_side * cyc.clock->getFastestRatio()), cube_side, this);
                }
            }

            hnd.nextEvent();
        }
    }
    return ok;
}

void 
WaterfallDAvtView::updateCycleBar(double x)
{
    if(focusMDI == myMDI)
    {
        CYCLE cycle = currentCycle;

        x = QMIN(x, realContentsWidth - 1.0);
        x = QMAX(x, 0.0);

        toCycle((INT32) x, 0, false, &cycle);
        cycle.cycle += draldb->getFirstEffectiveCycle(cycle.clock->getId()).cycle;

        emit cycle_msg_changed(cycle.toString(CYCLE_MASK_ALL) + " ");
    }
}

/**
 * update the tag window with the ItemHandler selected by the user
 *
 * @param hnd trackId of the selected item
 */
void
WaterfallDAvtView::updateTagWindow(ItemHandler * hnd)
{
    static_cast<MDIDreamsWindow*>(myMDI)->getTagWindow()->updateItemTag(hnd, draldb->getFirstEffectiveCycle(0));
}

void
WaterfallDAvtView::updateRowBar(double x, double y)
{
    if(focusMDI == myMDI)
    {
        // Gets the current row.
        y = QMIN(y, realContentsHeight - 1.0);
        y = QMAX(y, 0.0);
        UINT32 row = (UINT32) (y / (double) getInternalBaseElementSize());

        // Coherency check.
        if(row >= dreamsdb->layoutWaterfallGetNumRows())
        {
            return;
        }

        // Computes the row range and gets the item id.
        dreamsdb->layoutWaterfallComputeRangeForRow(row);
        ITEM_ID itemid = dreamsdb->layoutWaterfallGetItemIdForRow(row);

        CYCLE min = dreamsdb->layoutWaterfallGetCurrentRowMinCycle();
        QString min_str = min.toString(CYCLE_MASK_NUM | CYCLE_MASK_DIV | CYCLE_MASK_CLK);
        CYCLE max = dreamsdb->layoutWaterfallGetCurrentRowMaxCycle();
        QString max_str = max.toString(CYCLE_MASK_NUM | CYCLE_MASK_DIV | CYCLE_MASK_CLK);

        QString rowdesc = "Item " + QString::number(itemid) + ", from " + min_str + " to " + max_str + " ";
        emit row_msg_changed(rowdesc.leftJustify(30, ' ', false));
    }
}

void
WaterfallDAvtView::mouseWorldDoubleClickEvent (QMouseEvent* e, double x, double y)
{
    /** @todo This function became deprecated, remove !? */
}

/**
 * Respond to the mouse movement across the viewport.
 * Update the Tag window if it is enabled and visible
 */
void
WaterfallDAvtView::mouseWorldMoveEvent(QMouseEvent* e, double x, double y)
{
    current_x = static_cast<int>(x);
    current_y = static_cast<int>(y);

    current_x = QMAX(current_x, 0);
    current_y = QMAX(current_y, 0);

    // invoke parent implementation
    AScrollView::mouseWorldMoveEvent(e, x, y);

    // and now update the tagwindow if needed
    // look for the event on the matrix:
    if(static_cast<MDIDreamsWindow*>(myMDI)->getEnabledTagWindow() && !(e->state() & AltButton) )
    {
        ItemHandler handler;
        bool dt = dreamsdb->layoutWaterfallGetHasTags(current_y / cube_side, &handler);
        if(dt)
        {
            CYCLE cycle = currentCycle;

            toCycle((INT32) x, 0, false, &cycle);
            cycle.cycle += draldb->getFirstEffectiveCycle((* draldb->getClockDomainFreqIterator())->getId()).cycle;

            static_cast<MDIDreamsWindow*>(myMDI)->getTagWindow()->updateItemTag(&handler, cycle);
        }
        else
        {
            static_cast<MDIDreamsWindow*>(myMDI)->getTagWindow()->reset();
        }
    }
}

void
WaterfallDAvtView::updateCloseMenuState()
{
    //printf ("updateCloseMenuState() called with nc=%d\n",numCycles);
    Dreams* tdr = (Dreams*)mainWin;
    tdr->enableAncestorMenu(false);
    tdr->enableHighlightSubMenu(false);
}

void
WaterfallDAvtView::drawDstRubber(int x, int y, int mx)
{
    QPainter p;
    p.begin(viewport());
    p.setRasterOp(NotROP);
    p.setPen(QPen(color0, 1));
    p.setBrush(NoBrush);
    p.setFont(QFont("Helvetica", (qApp->font()).pointSize()));

    UINT32 srcRow;
    UINT32 dstRow;
    INT32 dummy;
    INT32 temp;
    CLOCK_ID srcClockId;
    CLOCK_ID dstClockId;

    // Gets the row where the distance was pressed.
    srcRow = pressed_my / getInternalBaseElementSize();
    srcRow = QMIN(srcRow, dreamsdb->layoutWaterfallGetNumRows() - 1);
    srcRow = QMAX(srcRow, 0);
    srcClockId = (* draldb->getClockDomainFreqIterator())->getId();

    viewportToContents(0, y, dummy, temp);

    // Gets the row where the distance is now.
    dstRow = (INT32) ((double) temp / ((double) getInternalBaseElementSize() * scf_y));
    dstRow = QMIN(dstRow, dreamsdb->layoutWaterfallGetNumRows() - 1);
    dstRow = QMAX(dstRow, 0);
    dstClockId = (* draldb->getClockDomainFreqIterator())->getId();

    CYCLE source(draldb->getClockDomain(srcClockId));
    CYCLE dest(draldb->getClockDomain(dstClockId));

    toCycle((INT32) pressed_mx, srcRow, false, &source);
    toCycle((INT32) mx, dstRow, false, &dest);

    INT32 src_x;
    INT32 dst_x;

    src_x = toCol(&source) - (INT32) ((double) contentsX() / scf_x);
    dst_x = toCol(&dest) - (INT32) ((double) contentsX() / scf_x);

    // Compute the x comp of vector.
    INT32 vx = dst_x - src_x;
    INT32 mod = abs(vx);

    // Minimum size check.
    if(mod < 4)
    {
        p.end();
        return;
    }

    // Central line.
    vx = (INT32) ((double) vx * scf_x);
    src_x = (INT32) ((double) src_x * scf_x);
    dst_x = (INT32) ((double) dst_x * scf_x);

    p.drawLine(src_x, y, dst_x, y);

    // Cota marks.
    INT32 vheight = visibleHeight();
    p.drawLine(src_x, 0, src_x, vheight);
    p.drawLine(dst_x, 0, dst_x, vheight);

    // Put the current measure.
    QString str;

    // If is in the same clock domain.
    if(srcClockId == dstClockId)
    {
        CYCLE cyc;

        // Gets the absolut difference in cycles.
        if(dest > source)
        {
            cyc = dest - source;
        }
        else
        {
            cyc = source - dest;
        }

        str = "Distance: " + QString::number(cyc.cycle);

        // Shows divisions if necessary.
        if(cyc.division != 0)
        {
            str += ".5";
        }

        str += " @ " + source.clock->getFormattedFreq(CLOCK_GHZ);
        drawDstRubberDistance(&p, &str, true, y, vx);
    }
    else
    {
        INT64 lcm = abs(dest.toLCMCycles() - source.toLCMCycles());
        lcm = lcm >> 1;
        // Paints the distance in source cycles.
        float cycles = (float) lcm / (float) source.clock->getLCMRatio();
        str = "Distance: " + QString::number(cycles, 'f', 2) + " @ " + source.clock->getFormattedFreq(CLOCK_GHZ);
        drawDstRubberDistance(&p, &str, true, y, vx);

        // Paints the distance in dest cycles.
        cycles = (float) lcm / (float) dest.clock->getLCMRatio();
        str = "Distance: " + QString::number(cycles, 'f', 2) + " @ " + dest.clock->getFormattedFreq(CLOCK_GHZ);
        drawDstRubberDistance(&p, &str, false, y, vx);
    }
    p.end();
}

void
WaterfallDAvtView::drawDstRubberDistance(QPainter * p, QString * str, bool high, INT32 y, INT32 vx)
{
    QFontMetrics fm = p->fontMetrics();
    QRect tbr = fm.boundingRect(* str);

    INT32 px, py;

    px = pressed_snaped_x;

    if(high)
    {
        py = y - (tbr.height() << 1);
    }
    else
    {
        py = y + (tbr.height() << 1);
    }
    if(vx > 0)
    {
        if(vx > (tbr.width() + 20))
        {
            px += vx >> 1;
            px -= tbr.width() >> 1;
        }
        else
        {
            px += 10;
        }
        // Draws the text.
        p->drawText(px, py, * str);
    }
    else if(vx < 0)
    {
        if(abs(vx) > (tbr.width() + 20))
        {
            px += vx >> 1;
            px += tbr.width() >> 1;
        }
        else
        {
            px -= 10;
        }
        p->translate((double) -tbr.width(), 0);
        // Draws the text.
        p->drawText(px, py, * str);

        // Must undo the translation because might use this painter twice.
        p->translate((double) tbr.width(), 0);
    }
    else
    {
        Q_ASSERT(false);
    }

}

void
WaterfallDAvtView::applyMagicWand (int mx,int my)
{
    //printf ("magic wind called on (%d,%d)\n",mx,my);fflush(stdout);
    popup_col = mx;
    popup_row = my;
}

void
WaterfallDAvtView::showDistance(int pressed_mx, int released_mx)
{
}

void
WaterfallDAvtView::keyReleaseEvent(QKeyEvent* e)
{
    CYCLE initialCycle;
    INT32 initialColumn;
    int y_scroll;

    // check for some keys
    switch (e->key())
    {
        case Key_Left:
             scrollBy(-cube_side,0);
             break;

        case Key_Right:
             scrollBy(cube_side,0);
             break;

        case Key_Home:
             // go to the initial cycle of the selected row
             dreamsdb->layoutWaterfallComputeRangeForRow(selectedRow);
             initialCycle = dreamsdb->layoutWaterfallGetCurrentRowMinCycle();
             initialColumn = toCol(&initialCycle);
             y_scroll = floorf(double(selectedRow*cube_side) * scf_y) - contentsY();
             scrollBy(floorf(double(initialColumn) * scf_x) - contentsX(), y_scroll);
             break;

        case Key_End:
             setContentsPos(contentsWidth(),contentsY());
             break;

        case Key_Up:
             scrollBy(0,-cube_side);
             break;

        case Key_Down:
             scrollBy(0,cube_side);
             break;

        case Key_Prior:
             scrollBy(-visibleWidth(),0);
             break;

        case Key_Next:
             scrollBy(visibleWidth(),0);
             break;
    }
}

QString 
WaterfallDAvtView::getTipString(bool eventOnly)
{
    int col = current_mx;
    int row = current_my;

    QString s = QString::null;
    ItemHandler hnd;
    CLOCK_ID clockId = (* draldb->getClockDomainFreqIterator())->getId();
    CYCLE cycle(draldb->getClockDomain(clockId));
    toCycle(current_mx, row, false, &cycle);
    cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;
    INT64 ps = (totalPs * current_mx) / totalPixels;
    row = row / getInternalBaseElementSize();

    switch(currentPointerType)
    {
        case POINTER_SELECT:
            ExpressionList *tmplist;

            if(dreamsdb->layoutWaterfallHasValidData(row, ps, &hnd, &tmplist))
            {
                if(hnd.isValidItemHandler() && hnd.isValidEventHandler())
                {
                    s = "Event info for item " + QString::number(hnd.getItemId()) + " in picosecond " + QString::number(ps) + " \n";
                    DralGraphNode * node;
                    NodeSlot * slot;
                    switch(hnd.getEventType())
                    {
                        case IDB_MOVE_ITEM:
                            s += "Type: Move Item\n";
                            s += "Edge: " + draldb->getEdge(hnd.getEventEdgeId())->getName() + "\n";
                            s += "Cycle: " + hnd.getEventCycle().toString(CYCLE_MASK_NUM | CYCLE_MASK_DIV | CYCLE_MASK_CLK);
                            break;

                        case IDB_ENTER_NODE:
                            s += "Type: Enter Node\n";
                            node = draldb->getNode(hnd.getEventNodeId());
                            slot = node->getLayout();
                            s += "Node: " + node->getName() + "\n";
                            s += "Slot: " + slot->linearSlot(hnd.getEventPosition()).toString() + "\n";
                            s += "Cycle: " + hnd.getEventCycle().toString(CYCLE_MASK_NUM | CYCLE_MASK_DIV | CYCLE_MASK_CLK);
                            break;

                        case IDB_EXIT_NODE:
                            s += "Type: Exit Node\n";
                            node = draldb->getNode(hnd.getEventNodeId());
                            slot = node->getLayout();
                            s += "Node: " + node->getName() + "\n";
                            s += "Slot: " + slot->linearSlot(hnd.getEventPosition()).toString() + "\n";
                            s += "Cycle: " + hnd.getEventCycle().toString(CYCLE_MASK_NUM | CYCLE_MASK_DIV | CYCLE_MASK_CLK);
                            break;

                        default:
                            s += "Type: Unknown\n";
                            break;
                    }
                }
            }
            else
            {
                if(!eventOnly)
                {
                    s = getRowInfo(row);
                }
            }
            break;

        case POINTER_ZOOMING: 
        case POINTER_ZOOMING_NWIN:
        case POINTER_MWAND:
            s = cycle.toString(CYCLE_MASK_ALL);
            s = s + " Row " + QString::number(current_my);
            break;

        case POINTER_DISTANCE: 
        case POINTER_SHADING:
            s = cycle.toString(CYCLE_MASK_ALL);
            break;
        
        default:
            s = QString::null;
    }
    return s;
}

TipWaterfallDreams::TipWaterfallDreams( QWidget * parent )
    : QToolTip( parent )
{
    setWakeUpDelay(1500);
}

void 
TipWaterfallDreams::maybeTip( const QPoint &pos )
{
    if(!parentWidget()->inherits("WaterfallDAvtView"))
    {
        //printf ("TipFloorplanDreams::maybeTip no right parent, getting out\n");
        //hide();
        return;
    }

    WaterfallDAvtView *asv = (WaterfallDAvtView*)parentWidget();    
    if (asv->currentPointerType>=POINTER_DRAWLINE) return;
    if (asv->currentPointerType==POINTER_MWAND) return;
    if (asv->currentPointerType==POINTER_PANNING) return;
    
    QRect r(pos.x()-1, pos.y()-1, pos.x()+1, pos.y()+1);
    QString s = asv->getTipString(true);

    if(s != QString::null)
        tip(r, s);
    else
        hide();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void 
WaterfallDAvtView::annotationCopy()
{
    // does just a copy of the current selected guy
    if (currentAnnItem)
    {
        if (copyAnnItem) delete copyAnnItem;
        copyAnnItem = currentAnnItem->clone();
    }
    else
    {
        copyAnnItem = NULL;
    }
}

void 
WaterfallDAvtView::annotationPaste()
{
    if (copyAnnItem)
    {
        if (currentAnnItem)
        {
            currentAnnItem->setSelected(false);
            annCtrl->repaintAnnItem(currentAnnItem,scf_x,scf_y);
        }
        currentAnnItem = annCtrl->annotationPaste(copyAnnItem);
        copyAnnItem=currentAnnItem->clone();
    }
}

void 
WaterfallDAvtView::annotationRemove()
{
    if (currentAnnItem) 
    {
        annCtrl->annotationRemove(currentAnnItem);
        currentAnnItem=NULL;
        pressAnnItem=NULL;
    }
}

void 
WaterfallDAvtView::cloneAnnotationCtrl(AnnotationCtrl* parent)    
{
    annCtrl->clone(parent);
}

void 
WaterfallDAvtView::showCycle(CYCLE cycle)
{
    INT32 col;

    currentCycle.fromPs(cycle.toPs());

    col = currentCycle.cycle - draldb->getFirstEffectiveCycle(currentCycle.clock->getId()).cycle;
    if((col >= draldb->getNumCycles(currentCycle.clock->getId()).cycle) || (col < 0) )
    {
        qApp->beep();
        return;
    }

    shadeColumn(col, true);

    if(dreamsdb->layoutWaterfallGetUsesDivisions())
    {
        col = col << 1;
    }

    INT32 cw = (INT32) ((double) visibleWidth() / getInternalBaseElementSize() / scf_x);
    INT32 cx = (INT32) ((double) contentsX() / getInternalBaseElementSize() / scf_x);

    // If is already visible dont move!
    if((col >= cx) && (col <= (cx + cw)))
    {
        return;
    }

    INT32 x = (INT32) floor((double) col * getInternalBaseElementSize() * scf_x);
    setContentsPos(x - (visibleWidth() / 2), contentsY());
}

void 
WaterfallDAvtView::setSelectedRow(int row)
{
    int tmp=selectedRow;
    double dy;
    double dh=((double)cube_side*scf_y);

    //cerr << "WaterfallDAvtView::setSelectedRow called w/selRow " << selectedRow << " row " << row << endl;
    selectedRow=-1;
    // clean old selection
    if (tmp>=0)
    {
        dy=((double)(tmp*cube_side))*scf_y;
        //cerr << "WaterfallDAvtView::setSelectedRow cleaning old row " << tmp << " dy " << dy << endl;
        repaintContents(contentsX(),(int)dy+myVerticalOffset-4,visibleWidth()+8,(int)dh+4,true);
    }
    selectedRow=row;
    dy=((double)(row*cube_side))*scf_y;
    repaintContents(contentsX(),(int)dy+myVerticalOffset-4,visibleWidth()+8,(int)dh+4,true);
    //cerr << "WaterfallDAvtView::setSelectedRow redrawing for row " << row << " dy " << dy << endl;
}
   
/*
void 
WaterfallDAvtView::updateScallingFactor (double sx, double sy)
{
    // limit zoom in to 3, more than than looks ungly and is useless
    double x=QMIN(3.0,sx);
    double y=QMIN(3.0,sy);
    DAvtView::updateScallingFactor(x,y); 
}
*/

void 
WaterfallDAvtView::raiseMovementSignal(double x, double y)
{
    if(x != 0.0)
    {
        INT64 ps = (totalPs * (INT64) x) / totalPixels;

        emit cycleChanged(static_cast<Q_INT64>(ps));
    }
    if(y != 0.0)
    {
        emit itemChanged(y);
    }
}

/*
 * Converts a position in the layout view into a cycle.
 *
 * @param col is the column within the canvas.
 * @param row is the row within the matrix.
 * @param inval sets the behavior in case that the cycle is in a
 *        division that is not shown in the row: inval the cycle
 *        or not.
 * @param cycle is the return value.
 *
 * @return void.
 */
void
WaterfallDAvtView::toCycle(INT32 col, INT32 row, bool inval, CYCLE * cycle) const
{
    double ratio = static_cast<double>(cycle->clock->getFastestRatio()) * static_cast<double>(getInternalBaseElementSize());
    cycle->cycle = static_cast<INT32>(floor(static_cast<double>(col) / ratio));
    cycle->division = 0;

    if(dreamsdb->layoutWaterfallGetUsesDivisions())
    {
        cycle->division = cycle->cycle & 1;
        cycle->cycle = cycle->cycle >> 1;
    }
}

/*
 * Converts a cycle in a position in the layout view.
 *
 * @return the col related to the cycle.
 */
INT32
WaterfallDAvtView::toCol(CYCLE * cycle) const
{
    INT32 col;
    double ratio = static_cast<double>(cycle->clock->getFastestRatio()) * static_cast<double>(getInternalBaseElementSize());

    if(dreamsdb->layoutWaterfallGetUsesDivisions())
    {
        col = cycle->cycle << 1;
        col += cycle->division;
    }
    else
    {
        col = cycle->cycle;
    }
    col = static_cast<INT32>(static_cast<double>(col) * ratio);
    return col;
}

void
WaterfallDAvtView::switchRebel(bool value)
{
    contextMenu->setItemChecked(rebel_id, value);

    if(value)
    {
        myMDI->setIcon(IconFactory::getInstance()->image32);
        myMDI->setIconText("Locked");
        setFrameStyle( QFrame::Panel | QFrame::Raised );
        setLineWidth(style().pixelMetric(QStyle::PM_DefaultFrameWidth, this) );
    }        
    else
    {
        myMDI->setIcon(IconFactory::getInstance()->image33);
        myMDI->setIconText("Unlocked");
        setFrameStyle( QFrame::Box | QFrame::Raised );
        setLineWidth(3);
    }
}

void
WaterfallDAvtView::drawShadingColumns(int x1,int x2, int y1, int y2, QPainter* painter)
{
    //printf ("drawShadingColumns called (%d,%d)->(%d,%d)\n",x1,y1,x2,y2);

    int mcol0,mcol1,mcol;
    int sz;
    if(dreamsdb->layoutWaterfallGetUsesDivisions())
    {
        sz = (int) QMAX(1.0, getInternalBaseElementSize() << 1);
    }
    else
    {
        sz = (int) QMAX(1.0, getInternalBaseElementSize());
    }
    int height = y2 - y1 + 1;
    painter->setPen(shaderPen);
    painter->setBrush(shaderBrush);

    mcol0 = x1 / sz;
    mcol1 = x2 / sz;
    mcol = mcol0;

    while (mcol<=mcol1)
    {
        if(getShadeFlag(mcol))
        {
            painter->drawRect(mcol * sz, y1, sz, height);
        }    
        ++mcol;
    }
}

/*
 * Draws all the selected columns.
 *
 * @return void.
 */
void
WaterfallDAvtView::shadeColumn(INT32 col, bool shade)
{
    cshadows->shadeColumn(col, shade);

    if(dreamsdb->layoutWaterfallGetUsesDivisions())
    {
        repaintContents(
            (INT32) ((double) col * scf_x * (double) (getInternalBaseElementSize() << 1) - (2.0 * scf_x)),
            0,
            (INT32) (scf_x * (double) (getInternalBaseElementSize() << 1) + (4.0 * scf_x)),
            (INT32) (scf_y * (double) realContentsHeight),
            false);
    }
    else
    {
        repaintContents(
            (INT32) ((double) col * scf_x * (double) getInternalBaseElementSize() - (2.0 * scf_x)),
            0,
            (INT32) (scf_x * (double) getInternalBaseElementSize() + (4.0 * scf_x)),
            (INT32) (scf_y * (double) realContentsHeight),
            false);
    }
}

/*
 * Switches the state of the shadowing for a given cycle.
 *
 * @return void.
 */
void
WaterfallDAvtView::switchShadeFlag(INT32 col)
{
    INT32 myCol;
    if(dreamsdb->layoutWaterfallGetUsesDivisions())
    {
        myCol = col / (getInternalBaseElementSize() << 1);
    }
    else
    {
        myCol = col / getInternalBaseElementSize();
    }
    shadeColumn(myCol, !getShadeFlag(myCol));
    showLastShadded(myCol);
    if(mdiLocked)
    {
        emit toggledShadowCol(myCol, getShadeFlag(myCol));
    }
}

void
WaterfallDAvtView::clearShadedColumnsNoPropagate()
{
    if(cshadows == NULL)
    {
        return;
    }
    cshadows->clear();

    repaintContents(false);
}

void 
WaterfallDAvtView::clearShadedColumns()
{
    if(cshadows != NULL)
    {
        cshadows->clear();
    }

    repaintContents(false);

    // propagate is appropiated 
    if(mdiLocked)
    {
        emit clearAllShadedCols();
    }
}

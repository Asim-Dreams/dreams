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
  * @file ResourceDAvtView.cpp
  */

// General includes.
#include <iostream>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/ResourceDAvtView.h"
#include "dialogs/event/EventDialogImpl.h"
#include "dialogs/find/FindDialogImplResource.h"
#include "dialogs/tag/TagDialogImpl.h"
#include "dockwin/ancestor/AncestorDockWin.h"
#include "dockwin/disassembly/DisDockWinResource.h"
#include "dockwin/pushout/PushoutDockWin.h"
#include "dockwin/tag/TagDockWin.h"
#include "layout/resource/LayoutResourceCache.h"
#include "mdi/MDIDreamsWindow.h"
#include "Dreams.h"
#include "ShowTags.h"
#include "FavoriteTags.h"
#include "TagSelector.h"

// Qt includes.
#include <qstringlist.h>
#include <qtooltip.h>
#include <qaccel.h>

// Externs.
extern QWidget * focusMDI;

ResourceDAvtView::ResourceDAvtView(DreamsDB * _dreamsdb, LayoutResourceCache * _cache, QWidget * parent, QMainWindow * mdi, QMainWindow * main, INT32 numcycles, INT32 numevents)
    : DAvtView(parent, main, numcycles * cube_side, numevents * cube_side)
{
    myMDI = mdi; 
    mdiLocked = true;

    // filter out some events
    horizontalScrollBar()->installEventFilter(this);
    verticalScrollBar()->installEventFilter(this);
    viewport()->installEventFilter(this);
    /*
    if ( parent!=NULL )
    {
        parent->installEventFilter( this );
    }
    */

    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
    cache = _cache;

    numCycles = numcycles;
    numEvents = numevents;
    annCtrl->setBaseSize(getInternalBaseElementSize());
    gridSize = (int) ceil((double) getInternalBaseElementSize() * 10.0);
    //annCtrl->setGridSize(gridSize);
    annCtrl->setGridSize(getInternalBaseElementSize()); // tmp hack

    initShadeFlags();

    // create tag-favorite class
    ft = FavoriteTags::getInstance();
    currentTagSelector = ft->getList().first();

    // HighLight manager
    hlmgr = new HighLightMgrResource(dreamsdb, cache, cube_side, this);
    connect(hlmgr, SIGNAL(highlightChanged(int)), mainWin, SLOT(highlightChanged(int)));
    connect(hlmgr, SIGNAL(purgeLastHighlight()), mainWin, SLOT(purgeLastHighlight()));
    connect(hlmgr, SIGNAL(unhighlightCriteriaSignal(TAG_ID, UINT64)), mainWin, SLOT(unhighlightCriteria(TAG_ID, UINT64)));
    connect(hlmgr, SIGNAL(contentsSelectedEvent(TAG_ID, UINT64, bool, HighLightType, bool)), mainWin, SLOT(contentsSelectedEvent(TAG_ID, UINT64, bool, HighLightType, bool)));

    htype = HLT_DEFAULT;

    // by default we see both highlighted and non-highlighted guys
    vHEvents  = true;
    vNHEvents = true;

    //force_dis_refresh = true;

    // I add a popup menu to ease some common operations
    addContextMenu();
    rc_pressed_x = -1;
    rc_pressed_y = -1;
    rc_released_x = -1;
    rc_released_y = -1;

    shaderBrush = QBrush(prefMgr->getShadingColor());

    bgColor = prefMgr->getBackgroundColor();

    cursorCycle.cycle = -1;

    /*
    escAccel = new QAccel( this );
    escAccel->connectItem( escAccel->insertItem(Key_Escape),this,SLOT(escKey()) );
    escapeHitted = false;
    */

    tip = new TipResourceDreams(this);

    // by default all docked windows are on
    disWinEnabled = true;
    ancWinEnabled = true;
    pushWinEnabled = true;

    // Create item window
    disWin = new DisDockWinResource(dreamsdb, cache, myMDI, main, this);
    initDisWindow();

    createAncestorWindow();
    createPushoutWindow();

    // Create the find dialog
    fndDialog = new FindDialogImplResource(dreamsdb, cache, this, main, "findDialog", true);

    copyAnnItem = NULL;

    // Sets the currentCycle as the first cycle of the fastest clock.
    currentCycle = draldb->getFirstEffectiveCycle((* draldb->getClockDomainFreqIterator())->getId());

    totalPs = 0;
    totalPixels = 0;
}

/*
 * Destructor of the class. Destroys all the allocated objects.
 *
 * @return destroys the object.
 */
ResourceDAvtView::~ResourceDAvtView()
{
    delete contextMenu;
    delete ancWin;
    delete tip;
    clearShadeFlags();
}

void 
ResourceDAvtView::setMDINumber(int v) 
{
    DAvtView::setMDINumber(v);
    // propagate to the caption of the docked windows
    if(v > 1)
    {
        QString version = QString::number(v) + ")";
        if(disWin)
        {
            disWin->setCaption(QString("Item Window (" + version));
        }
        if(ancWin)
        {
            ancWin->setCaption(QString("Relationship Window (" + version));
        }
    }
}

void
ResourceDAvtView::setRealContentsSize()
{
    numCycles = dreamsdb->getNumCycles((* draldb->getClockDomainFreqIterator())->getId()).cycle;
    numEvents = cache->getNumRows();

    CYCLE last = draldb->getLastCycle();
    if(dreamsdb->layoutResourceGetUsesDivisions())
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
    totalPs = last.toPs() - draldb->getFirstEffectiveCycle(last.clock->getId()).toPs();

    // If divisions are used, the size is doubled.
    if(dreamsdb->layoutResourceGetUsesDivisions())
    {
        totalPixels = numCycles * cube_side * 2;
    }
    else
    {
        totalPixels = numCycles * cube_side;
    }

    AScrollView::setRealContentsSize(totalPixels, numEvents * cube_side);

    initShadeFlags();
    updateCloseMenuState();
}

void
ResourceDAvtView::reset()
{
    numCycles = 0;
    numEvents = 0;

    AScrollView::setRealContentsSize(0, 0);

    // docked windows
    ancWin->reset();

    // clean-up highlight stuff
    hlmgr->purgeAll();
    updateCloseMenuState();

    // annotation tools
    annCtrl->reset();

    // do a forced repaint
    repaintContents(false);
    setPointerType(POINTER_SELECT);
}

// --------------------------------------------------------------------
// --------------------------------------------------------------------
void
ResourceDAvtView::refreshFavoriteTagList()
{
    ft->refresh();
    static_cast<MDIDreamsWindow*>(myMDI)->refreshFavoriteTagList();
}

// --------------------------------------------------------------------
// --------------------------------------------------------------------
void 
ResourceDAvtView::addContextMenu()
{
    contextMenu = new QPopupMenu(this);

    contextMenuRowTools = new QPopupMenu(contextMenu);
    contextMenuEventTools = new QPopupMenu(contextMenu);
    contextMenuSyncTools = new QPopupMenu(contextMenu);

    // Row tools.
    contextMenu->insertItem("&Row Tools", contextMenuRowTools);
    show_row_occ_id = contextMenuRowTools->insertItem("Show &Occupancy", this, SLOT(do_showOccupancy()));
    show_row_info_id = contextMenuRowTools->insertItem("Show &Info", this, SLOT(do_showRowInfo()));
    if(((Dreams *) mainWin)->params.DEVMode())
    {
        contextMenuRowTools->insertItem("Show &TrackId", this, SLOT(do_showRowTrackId()));
    }

    // Item tools.
    contextMenu->insertItem("&Event Tools", contextMenuEventTools);
    rel_scan_id = contextMenuEventTools->insertItem("Relationship &Scan", this, SLOT(do_ancScan()));
    show_color_id = contextMenuEventTools->insertItem("Show &Coloring Info", this, SLOT(do_showColInfo()));
    show_all_events_id = contextMenuEventTools->insertItem("Show All &Events", this, SLOT(do_showAllEvents()));
    show_all_tags_id = contextMenuEventTools->insertItem("Show All &Tags", this, SLOT(do_showAllTags()));

    // Synchronization tools.
    contextMenu->insertItem("&Sync Tools", contextMenuSyncTools);
    contextMenuSyncTools->insertItem("Sync To This &Cycle", this, SLOT(do_syncToCycle()));
    contextMenuSyncTools->insertItem("Sync To This Cycle (same &trace)", this, SLOT(do_syncToCycleTrace()));
    sync_to_item_id = contextMenuSyncTools->insertItem("Sync To This &Item", this, SLOT(do_syncToItem()));

    // Free window.
    contextMenu->insertSeparator();
    rebel_id = contextMenu->insertItem("&Free Window", this, SLOT(do_switchRebel()));
    // By defaul MDI is NOT rebel.
    contextMenu->setItemChecked(rebel_id, false);
}

void
ResourceDAvtView::do_switchRebel()
{
    mdiLocked = !mdiLocked;
    contextMenu->setItemChecked(rebel_id, !mdiLocked);
    if(mdiLocked)
    {
        myMDI->setIcon(IconFactory::getInstance()->image32);
        myMDI->setIconText("Locked");
        setFrameStyle(QFrame::Panel | QFrame::Raised);
        setLineWidth(style().pixelMetric(QStyle::PM_DefaultFrameWidth, this));
    }
    else
    {
        myMDI->setIcon(IconFactory::getInstance()->image33);
        myMDI->setIconText("Unlocked");
        setFrameStyle(QFrame::Box | QFrame::Raised);
        setLineWidth(3);

        // We hide the current cursor.
        cursorCycle.cycle = -1;
    }
}

void
ResourceDAvtView::do_ancScan()
{
    //printf ("do_ancScan called with col=%d, row=%d\n",popup_col,popup_row);
    CLOCK_ID clockId;

    clockId = dreamsdb->layoutResourceGetRowClockId(popup_row, cache);

    if(dreamsdb->layoutResourceGetValidData(popup_row, popup_cycle, cache))
    {
        Dreams * tdr = (Dreams *) mainWin;
        ItemHandler hnd;

        dreamsdb->layoutResourceGetRowItemInside(popup_row, popup_cycle, &hnd, cache);
        if(hnd.isValidItemHandler())
        {
            ancWin->initTrackingWidth(&hnd, popup_cycle, popup_row);
            tdr->enableMenuOptions(numCycles > 1);
        }
        //tdr->ensureRScanDWinVisible();
    }
}

void
ResourceDAvtView::do_showColInfo()
{
    CLOCK_ID clockId;
    CYCLE cycle;

    clockId = dreamsdb->layoutResourceGetRowClockId(popup_row, cache);
    cycle.clock = draldb->getClockDomain(clockId);
    toCycle(current_mx, popup_row, true, &cycle);
    cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

    QString report = dreamsdb->layoutResourceGetRowCSLInfo(popup_row, cycle, cache) + "\n";
    QMessageBox::information(this, "Event Coloring Description", report, QMessageBox::Ok);
}

void
ResourceDAvtView::do_showRowInfo()
{
    QString report = getRowInfo(popup_row, current_mx);
    QMessageBox::information(this, "Row Detailed Description", report, QMessageBox::Ok);
}

void
ResourceDAvtView::do_showOccupancy()
{
    CLOCK_ID clockId = dreamsdb->layoutResourceGetRowClockId(popup_row, cache);
    CYCLE cycle = dreamsdb->getNumCycles(clockId);
    RowType type = dreamsdb->layoutResourceGetRowType(popup_row, cache);
    INT32 count = 0;

    QString report;
    report = "Cycles: " + QString::number(cycle.cycle) + "\n";
    report += "Events in: ";
    if(dreamsdb->layoutResourceGetRowShowHigh(popup_row, cache))
    {
        report += "high phase";
        count++;
    }
    if(dreamsdb->layoutResourceGetRowShowLow(popup_row, cache))
    {
        if(count == 0)
        {
            report += " and low phase";
        }
        else
        {
            report += "low phase";
        }
        count++;
    }

    report += "\n";

    UINT32 base_row = dreamsdb->layoutResourceGetRowBaseRow(popup_row, cache);
    UINT32 temp_row = base_row;
    UINT32 row_count = 0;
    UINT32 total_occ = 0;

    while((temp_row < cache->getNumRows()) && (dreamsdb->layoutResourceGetRowBaseRow(temp_row, cache) == base_row))
    {
        UINT32 occ = dreamsdb->layoutResourceGetRowOccupancy(temp_row, cache);
        total_occ += occ;

        if(type == SimpleEdgeRow)
        {
            report += "Lane ";
        }
        else
        {
            report += "Slot ";
        }

        QString str_occ = QString::number(occ);
        UINT32 fills = 6 - str_occ.length();

        for(UINT32 i = 0; i < fills; i++)
        {
            str_occ = "  " + str_occ;
        }

        report += QString::number(row_count) + " occupancy:\t" + str_occ + "\t(" + QString::number(((float) occ / (float) (cycle.cycle * count)), 'f', 2) + " Events/Cycle)\n";
        temp_row++;
        row_count++;
    }

    QString str_occ = QString::number(total_occ);
    UINT32 fills = 6 - str_occ.length();

    for(UINT32 i = 0; i < fills; i++)
    {
        str_occ = "  " + str_occ;
    }

    report += "Total occupancy:\t\t" + str_occ + "\t(" + QString::number(((float) total_occ / (float) (cycle.cycle * count)), 'f', 2) + " Events/Cycle)\n";
    QString header;

    if(type == SimpleEdgeRow)
    {
        header = "Occupancy for edge " + draldb->getEdge(dreamsdb->layoutResourceGetRowDralId(popup_row, cache))->getName();
    }
    else if(type == SimpleNodeRow)
    {
        header = "Occupancy for node " + draldb->getNode(dreamsdb->layoutResourceGetRowDralId(popup_row, cache))->getName();
    }

    QMessageBox::information(this, header, report, QMessageBox::Ok);
}

void
ResourceDAvtView::do_showRowTrackId()
{
    QString report = "The track id for this row is: " + QString::number(dreamsdb->layoutResourceGetRowTrackId(popup_row, cache));
    QMessageBox::information(this, "Row TrackId", report, QMessageBox::Ok);
}

void
ResourceDAvtView::do_syncToCycle()
{
    emit syncToCycle(popup_cycle_sync);
}

void
ResourceDAvtView::do_syncToCycleTrace()
{
    emit syncToCycleTrace(popup_cycle_sync);
}

void
ResourceDAvtView::do_syncToItem()
{
    QString tagName;
    ItemHandler hnd;
    DralGraphEdge * edge;
    DRAL_ID edgeid;
    TAG_ID tagId;
    TagValueType type;
    TagValueBase base;
    CYCLE when;
    UINT64 value;
    bool ok;

    dreamsdb->layoutResourceGetRowItemInside(popup_row, popup_cycle, &hnd, cache);
    Q_ASSERT(hnd.isValidItemHandler());

    edgeid = dreamsdb->layoutResourceGetRowDralId(popup_row, cache);
    edge = draldb->getEdge(edgeid);
    tagName = ((Dreams *) mainWin)->getItemSyncTagSelector()->currentText();

    // Gets if the tag id is defined.
    tagId = draldb->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        QMessageBox::warning(this, "Sync error", "The sync item tag is not defined.", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    // Only support for integer types.
    // TODO: change it to string and type and check that all the
    // traces have the same definition for the tag.
    if(draldb->getTagValueType(tagId) != TAG_INTEGER_TYPE)
    {
        QMessageBox::warning(this, "Sync error", "The sync item tag must be an integer.", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    ok = hnd.getTagById(tagId, &value, &type, &base, &when, popup_cycle);
    // Gets the value for the item.
    if(!ok)
    {
        QMessageBox::warning(this, "Sync error", "The selected item has no value defined for the sync item tag.", QMessageBox::Ok, QMessageBox::NoButton);
    }
    else
    {
        // Highlighting, shading, etc.
        shadeColumn(popup_cycle, true);
        getHighLightMgr()->contentsSelectEvent(0, popup_row, popup_cycle, true, currentTagSelector, HLT_DEFAULT, true);
        disWin->alignWith(0, popup_row, popup_cycle);
        repaintContents(false);

        // We must use the tag name and edge name and not the DRAL_ID and TAG_ID because
        // in other traces their values might be different, but their names must be the
        // same (the function that handles this signal must check that their exist, otherwise
        // raise an error).
        emit syncToItem(edge->getName(), edge->getSourceNodeName(), edge->getDestinationNodeName(), tagName, value);
    }
}

QString
ResourceDAvtView::getRowInfo(INT32 selrow, INT32 selcol)
{
    CLOCK_ID clockId;
    CYCLE cycle;

    clockId = dreamsdb->layoutResourceGetRowClockId(selrow, cache);
    cycle.clock = draldb->getClockDomain(clockId);
    toCycle(selcol, selrow, true, &cycle);
    cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

    return dreamsdb->layoutResourceGetRowInfo(selrow, cycle, cache) + "\n";
}

void
ResourceDAvtView::do_showAllTags()
{
    if(dreamsdb->layoutResourceGetValidData(popup_row, popup_cycle, cache))
    {
        TRACK_ID trackId;
        ItemHandler hnd;

        switch(dreamsdb->layoutResourceGetRowType(popup_row, cache))
        {
            case SimpleNodeRow:
                trackId = dreamsdb->layoutResourceGetRowTrackId(popup_row, cache);

                if(trackId != TRACK_ID_INVALID)
                {
                    TagDialogImpl td(draldb, trackId, popup_cycle, this, "td", true);
                    td.exec();
                }
                break;

            case SimpleEdgeRow:
                dreamsdb->layoutResourceGetRowItemInside(popup_row, popup_cycle, &hnd, cache);

                if(hnd.isValidItemHandler())
                {
                    TagDialogImpl td(draldb, &hnd, popup_cycle, this, "td", true);
                    td.exec();
                }
                break;

            default:
                return;
        }
    }
}

void
ResourceDAvtView::do_showAllEvents()
{
    ItemHandler hnd;

    dreamsdb->layoutResourceGetRowItemInside(popup_row, popup_cycle, &hnd, cache);

    if(hnd.isValidItemHandler())
    {
        EventDialogImpl ed(draldb, &hnd, popup_cycle, this, "ed", true);
        ed.exec();
    }
}

void 
ResourceDAvtView::rightClickPressEvent(QMouseEvent * e)
{
    rc_pressed_x = e->x();
    rc_pressed_y = e->y();
}

void 
ResourceDAvtView::rightClickReleaseEvent(QMouseEvent * e)
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
        wy = ((double) cy / scf_y);

        popup_row = (INT32) (wy / (double) getInternalBaseElementSize());
        popup_row = QMAX(popup_row, 0);

        // Coherency check.
        if(popup_row >= numEvents)
        {
            return;
        }

        clockId = dreamsdb->layoutResourceGetRowClockId(popup_row, cache);
        popup_cycle.clock = draldb->getClockDomain(clockId);
        popup_cycle_sync.clock = draldb->getClockDomain(clockId);
        toCycle((INT32) wx, popup_row, true, &popup_cycle);
        toCycle((INT32) wx, popup_row, false, &popup_cycle_sync);
        popup_cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;
        popup_cycle_sync.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

        if((dreamsdb->layoutResourceGetRowType(popup_row, cache) == SimpleNodeRow) || (dreamsdb->layoutResourceGetRowType(popup_row, cache) == SimpleEdgeRow))
        {
            contextMenuRowTools->setEnabled(true);
        }
        else
        {
            contextMenuRowTools->setEnabled(false);
        }

        // check if there is something there
        bool ena = dreamsdb->layoutResourceGetValidData(popup_row, popup_cycle, cache);

        if(!ena)
        {
            contextMenuEventTools->setEnabled(false);
            contextMenuSyncTools->setItemEnabled(sync_to_item_id, false);
        }
        else
        {
            contextMenuEventTools->setEnabled(true);

            if(dreamsdb->layoutResourceGetRowType(popup_row, cache) == SimpleEdgeRow)
            {
                contextMenuEventTools->setItemEnabled(rel_scan_id, true);
                contextMenuEventTools->setItemEnabled(show_all_events_id, true);
                contextMenuSyncTools->setItemEnabled(sync_to_item_id, true);
            }
            else
            {
                contextMenuEventTools->setItemEnabled(rel_scan_id, false);
                contextMenuEventTools->setItemEnabled(show_all_events_id, false);
                contextMenuSyncTools->setItemEnabled(sync_to_item_id, false);
            }

            contextMenuEventTools->setItemEnabled(show_all_tags_id, true);
        }
        contextMenu->exec(QCursor::pos());
    }
}

void
ResourceDAvtView::drawCursorColumn(QPainter * p, int x1, int x2, int y1, int y2)
{
    // Checks if valid cursor cycle.
    if(cursorCycle.cycle < 0)
    {
        return;
    }

    INT32 height;

    // Gets the bounds.
    height = y2 - y1 + 1;

    // Sets the pen and brush.
    p->setPen(shaderPen);
    p->setBrush(brushCursor);

    INT32 col;

    // Clock translation.
    col = toCol(&cursorCycle);

    // Checks if inside the bounds.
    if((x1 <= col) && (col <= x2))
    {
        INT32 size;

        if(dreamsdb->layoutResourceGetUsesDivisions())
        {
            size = (INT32) ((float) getInternalBaseElementSize() * cursorCycle.clock->getFastestRatio() * 2.0f);
        }
        else
        {
            size = (INT32) ((float) getInternalBaseElementSize() * cursorCycle.clock->getFastestRatio());
        }
        p->drawRect(col, y1, size, height);
    }
}

bool
ResourceDAvtView::drawWorldCoordinatesContents(QPainter * p, int ncx, int ncy, int ncw, int nch)
{
    // compute affected area with the CUBE_SIDE...
    INT32 x1 = ncx - (ncx % cube_side);
    INT32 x2 = ncx + ncw + 2 * (ncx % cube_side);
    INT32 y1 = ncy - (ncy % cube_side);
    INT32 y2 = ncy + nch + 2 * (ncy % cube_side);

    x1 -= 10;
    x2 += 10;
    y1 -= 10;
    y2 += 10;

    x1 = QMAX(x1, 0);
    y1 = QMAX(y1, 0);
    x2 = QMIN(x2, realContentsWidth - 1);
    y2 = QMIN(y2, realContentsHeight - 1);

    // cursor shading
    drawCursorColumn(p, x1, x2, y1, y2);

    // Put the black pen and go ahead.
    p->setPen(QPen(QColor(0, 0, 0), cube_border_size));
    p->setBrush(NoBrush);

    bool ssx = ((1.0 / (double) getInternalBaseElementSize()) > scf_x);
    bool ssy = ((1.0 / (double) getInternalBaseElementSize()) > scf_y);
    
    double dtx1 = (double) x1 * scf_x;
    double dtx2= (double) (x1 + getInternalBaseElementSize()) * scf_x;
    bool nfsx = dtx2 - dtx1 < 2.0;

    double dty1 = (double) y1 * scf_y;
    double dty2 = (double)(y1 + getInternalBaseElementSize()) * scf_y;
    bool nfsy = dty2 - dty1 < 4.0;
    
    if(ssx && ssy)
    {
        dreamsdb->layoutResourceDrawSubSamplingScale(x1, x2, y1, y2, scf_x, scf_y, p, cache, vNHEvents);
    }
    else if(nfsx && nfsy)
    {
        dreamsdb->layoutResourceDrawNoFullScale(x1, x2, y1, y2, p, cache, vNHEvents);
    }
    else
    {
        dreamsdb->layoutResourceDrawFullScale(x1, x2, y1, y2, p, this, cache, vNHEvents);
    }

    // check for highlighted guys
    drawHighlights(p, x1, x2, y1, y2, vHEvents);

    return true;
}

void 
ResourceDAvtView::drawHighlights(QPainter* p, int x1, int x2, int y1, int y2, bool visible)
{
    // limit y1/y2
    //if (y1>=numEvents) y1 = numEvents-1;
    //if (y2>=numEvents) y2 = numEvents-1;

    // warning highlight update must be over the whole visible area
    double cx = contentsX();
    double cy = contentsY();
    double cw = visibleWidth();
    double ch = visibleHeight();

    // transform to AGT world coordinates
    double cwx = (cx / scf_x);
    double cwy = (cy / scf_y);
    double cww = (cw / scf_x);
    double cwh = (ch / scf_y);

    // rounding
    int rx1 = (int) floor(cwx);
    int rx2 = (int) floor(cwx + cww - 1.0);
    int ry1 = (int) ceil(cwy);
    int ry2 = (int) ceil(cwy + cwh - 1.0);

    // divide by element-size...
    int mx1 = (int) floor ((double) rx1 / getInternalBaseElementSize());
    int mx2 = (int) ceil ((double) rx2 / getInternalBaseElementSize());
    int my1 = (int) floor ((double) ry1 / getInternalBaseElementSize());
    int my2 = (int) ceil ((double) ry2 / getInternalBaseElementSize());

    // apply first effective cycle
    //mx1 += draldb->getFirstEffectiveCycle();
    //mx2 += draldb->getFirstEffectiveCycle();

    hlmgr->updateViewRange(mx1, mx2, my1, my2);

    // refresh can be done in the dirty area only
    hlmgr->drawHighlights(p, x1, x2, y1, y2, visible);
}

void
ResourceDAvtView::updateRowBar(double x, double y)
{
    if(focusMDI == myMDI)
    {
        QString rowdesc;
        CLOCK_ID clockId;
        const ClockDomainEntry * clock;
        INT32 row;

        y = QMIN(y, realContentsHeight - 1.0);
        y = QMAX(y, 0.0);
        row = (INT32) (y / (double) getInternalBaseElementSize());

        // Coherency check.
        if(row >= numEvents)
        {
            return;
        }

        CYCLE cycle;

        clockId = dreamsdb->layoutResourceGetRowClockId(row, cache);
        cycle.clock = draldb->getClockDomain(clockId);
        toCycle((INT32) x, row, true, &cycle);
        cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

        rowdesc = dreamsdb->layoutResourceGetRowDescription(row, cache, RV_ROW_DESC_ALL);

        emit row_msg_changed(rowdesc.leftJustify(30, ' ', false));
    }
}

void
ResourceDAvtView::mouseWorldDoubleClickEvent(QMouseEvent * e, double x, double y)
{
    /** @todo This function became deprecated, remove !? */
}

void
ResourceDAvtView::mouseWorldMoveEvent(QMouseEvent * e, double x, double y)
{
    INT32 col;
    INT32 row;
    TRACK_ID trackId;
    CLOCK_ID clockId;
    const ClockDomainEntry * clock;

    // Invoke parent implementation.
    AScrollView::mouseWorldMoveEvent(e, x, y);

    y = QMIN(y, realContentsHeight - 1.0);
    y = QMAX(y, 0.0);
    row = (INT32) (y / (double) getInternalBaseElementSize());

    if(row >= numEvents)
    {
        return;
    }

    trackId = dreamsdb->layoutResourceGetRowTrackId(row, cache);

    // Checks cases where no row exists.
    if(trackId == TRACK_ID_INVALID)
    {
        return;
    }

    CYCLE cycle;

    clockId = dreamsdb->layoutResourceGetRowClockId(row, cache);
    cycle.clock = draldb->getClockDomain(clockId);
    toCycle((INT32) x, row, true, &cycle);
    cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

    // and now update the tagwindow if needed
    // look for the event on the matrix:
    if(static_cast<MDIDreamsWindow *>(myMDI)->getEnabledTagWindow() && !(e->state() & AltButton))
    {
        if(dreamsdb->layoutResourceGetValidData(row, cycle, cache))
        {
            RowType rtype = dreamsdb->layoutResourceGetRowType(row, cache);
            ItemHandler hnd;

            switch(rtype)
            {
                case SimpleEdgeRow:
                    dreamsdb->layoutResourceGetRowItemInside(row, cycle, &hnd, cache);

                    if(hnd.isValidItemHandler())
                    {
                        static_cast<MDIDreamsWindow *>(myMDI)->getTagWindow()->updateItemTag(&hnd, cycle);
                    }
                    break;

                case SimpleNodeRow:
                    static_cast<MDIDreamsWindow *>(myMDI)->getTagWindow()->updateTrackTag(trackId, cycle);
                    break;

                default:
                    Q_ASSERT(false);
            }
        }
        else
        {
            static_cast<MDIDreamsWindow *>(myMDI)->getTagWindow()->reset();
        }
    }
}

void
ResourceDAvtView::contentsSelectEvent(INT32 col, INT32 row, bool adding, bool shifted)
{
    bool needRep;
    INT32 myRow;
    CLOCK_ID clockId;
    const ClockDomainEntry * clock;

    myRow = row / getInternalBaseElementSize();
    myRow = QMAX(myRow, 0);
    myRow = QMIN(myRow, numEvents - 1);

    CYCLE cycle;

    clockId = dreamsdb->layoutResourceGetRowClockId(myRow, cache);
    cycle.clock = draldb->getClockDomain(clockId);
    toCycle((INT32) col, myRow, true, &cycle);
    cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

    if(dreamsdb->layoutResourceGetValidData(myRow, cycle, cache))
    {
        needRep = hlmgr->contentsSelectEvent(0, myRow, cycle, adding, currentTagSelector, htype, false);

        // check shift key (synchro with Item window)
        if(!shifted && disWinEnabled)
        {
            disWin->alignWith(0, myRow, cycle);
        }
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
}

/**
 * Method to display a cursor in the "current cycle" column
 * XXX
 * This support is still under development, given that we plan
 * to add this support to the AGT library
 */
void
ResourceDAvtView::showCursor(CYCLE cycle, bool moveToCursor)
{
    currentCycle.fromPs(cycle.toPs());
    cursorCycle = cycle;

    if(moveToCursor)
    {
        // Gets the internal col.
        cycle.cycle -= draldb->getFirstEffectiveCycle(cycle.clock->getId()).cycle;
        INT32 x = toCol(&cycle);
        //INT32 x = (INT32) floor((double) col * (double) QMAX(1, getInternalBaseElementSize()) * scf_x * cycle.clock->getFastestRatio());
        setContentsPos(x - (visibleWidth() / 2), contentsY());
    }

    repaintContents(false);
}

void 
ResourceDAvtView::showCycle(CYCLE cycle)
{
    CLOCK_ID clockId;
    bool found;

    currentCycle.fromPs(cycle.toPs());

    found = false;

    // Checks that the cycle is from the same trace.
    for(clockId = 0; clockId < draldb->getNumClocks(); clockId++)
    {
        if(cycle.clock == draldb->getClockDomain(clockId))
        {
            found = true;
            break;
        }
    }

    if(!found)
    {
        // The cycle comes from another trace, lets try to find a similar clock.
        found = false;

        // Checks that the cycle is from the same trace.
        for(clockId = 0; clockId < draldb->getNumClocks(); clockId++)
        {
            if(cycle.clock->getFreq() == draldb->getClockDomain(clockId)->getFreq())
            {
                found = true;
                break;
            }
        }

        if(found)
        {
            // A similar clock has been found, so we use it.
            cycle.clock = draldb->getClockDomain(clockId);
            cycle.fromPs(currentCycle.toPs());
        }
        else
        {
            // We use the fastest clock as no similar clock has been found.
            cycle = currentCycle;
        }
    }

    INT32 col = currentCycle.cycle - draldb->getFirstEffectiveCycle(currentCycle.clock->getId()).cycle;
    if((col >= draldb->getNumCycles(currentCycle.clock->getId()).cycle) || (col < 0) )
    {
        qApp->beep();
        return;
    }

    goToColumn(col);
    shadeColumn(cycle, true);
}

/*
 * Moves the resource view to the specified edge and to the first
 * event that has tagName with value tagValue. If the edge is not
 * shown (or defined) or if the tag is not defined a warning msg
 * will be shown.
 *
 * @return void.
 */
void
ResourceDAvtView::showItem(QString edgeName, QString fromNodeName, QString toNodeName, QString tagName, UINT64 tagValue)
{
    DralGraphEdge * edge;   ///< Requested edge.
    const rowList * rowl; ///< List of rows that have the edge edgeName.
    DRAL_ID edgeId;       ///< Dral id of the edge for this trace.
    TAG_ID tagId;         ///< Tag id of the tagName for this trace.

    // Tries to map the tag name to tag id.
    tagId = draldb->getTagId(tagName);
    if(tagId == TAG_ID_INVALID)
    {
        QMessageBox::warning(this, "Sync error", "The requested tag " + tagName + " is not defined in the trace " + dreamsdb->getTraceName() + ".", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    // Tries to find the edge in this trace.
    edge = draldb->findEdgeByNameFromTo(edgeName, fromNodeName, toNodeName);
    if(edge == NULL)
    {
        QMessageBox::warning(this, "Sync error", "The requested edge (" + edgeName + ") doesn't exist in trace " + dreamsdb->getTraceName() + ".", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    // Tries to find the edge in the resource view.
    dreamsdb->layoutResourceGetRowsWithDralId(edge->getEdgeId(), SimpleEdgeRow, &rowl);
    if(rowl->size() == 0)
    {
        QMessageBox::warning(this, "Sync error", "The requested edge (" + edgeName + ") is not shown in the resource view of the trace " + dreamsdb->getTraceName() + ".", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    // Now we must find the first item that matches tag and value.
    LayoutResourceFindOptions env;       ///< Environment settings that define what rows we want to get. Default by now.
    LayoutResourceFindDataSet entries;   ///< List of entries in the search.
    LayoutResourceFindDataIterator it;   ///< Iterator of the entries.
    UINT32 * rows;                 ///< List of rows in a vector.
    rowList::const_iterator rowit; ///< Iterator of the row list.
    UINT32 i;                      ///< Simple int iterator.

    // TODO: implement a new get find data that only gets rows from
    // a given list (actually the rowl list).
    dreamsdb->layoutResourceGetFindData(&entries, draldb->getFirstCycle(), &env);
    rows = new UINT32[rowl->size()];

    // First erases all the entries that are not from the rows of the edge.
    // Copies the list of rows to a vector for faster lookups.
    rowit = rowl->begin();
    i = 0;
    while(rowit != rowl->end())
    {
        rows[i] = * rowit;
        ++rowit;
        i++;
    }

    // Now performs the loop that checks what rows will be kept and
    // what discarted.
    it = entries.begin();
    while(it != entries.end())
    {
        bool erase = true;
        // Checks that the row is one of the wanted ones.
        for(i = 0; i < rowl->size(); i++)
        {
            if((* it)->getRow() == rows[i])
            {
                erase = false;
                break;
            }
        }

        if(erase)
        {
            entries.erase(it);
        }

        ++it;
    }

    delete [] rows;

    // Now that we got the list with the rows that we want, we can start
    // the loop until we find something (or nothing :D).

    LayoutResourceFindDataEntry * entry; ///< Working entry.
    ItemHandler hnd;               ///< Handler that will have the items inside the rows.
    TagValueType tgType;           ///< The type of the tag.
    TagValueBase tgBase;           ///< The base.
    UINT64 tgValue;                ///< Value of the item tag.
    CYCLE tgWhen;                  ///< When the tag was set.
    bool found;                    ///< Stores if we found something.
    bool ok;                       ///< Temp boolean.

    found = false;

    while(!entries.empty())
    {
        // Gets the first entry, which is the actual candidate as is the row
        // with a non-visited item that has the lowest cycle or in tie case
        // the smallest non-visited row.
        entry = * entries.begin();

        // Gets the item inside.
        ok = draldb->lookForItemId(&hnd, entry->getItemId());
        Q_ASSERT(ok);

        // Gets the value.
        ok = hnd.getTagById(tagId, &tgValue, &tgType, &tgBase, &tgWhen, draldb->getLastCycle());

        // Checks if match.
        if(ok)
        {
            // Integer match.
            if(tgValue == tagValue)
            {
                found = true;
                break;
            }
        }

        // Removes the entry from the set.
        entries.erase(entries.begin());

        // Moves the handler to the next item.
        entry->skipToNextItem();

        // If still valid is reinserted to the set.
        if(entry->isValidDataEntry())
        {
            entries.insert(entry);
        }
    }

    // We've ended the loop, see if something found.
    if(found)
    {
        // Entry is pointing to the "winner", so just use its values.
        UINT32 row = entry->getRow();

        // First of all, we want to map from physical rows to virtual rows.
        if(!dreamsdb->layoutResourceGetRowIsShown(row, cache))
        {
            // We need to be check that the destination row is shown.
            // If changed we don't need to request a repaint, because
            // is done later.
            dreamsdb->layoutResourceShowRow(row, cache);
            dreamsdb->layoutResourceComputeLayoutCache(cache);
            foldingChanged();
            getHighLightMgr()->reset();
        }

        row = dreamsdb->layoutResourceGetRowVirtMap(row, cache);

        // Now move to the column and row.
        CYCLE cyc;
        CYCLE foundCyc;

        foundCyc.clock = draldb->getFirstEffectiveCycle(entry->getClockId()).clock;
        foundCyc.fromLCMCycles(entry->getCycle());

        cyc.clock = draldb->getClockDomain(entry->getClockId());
        cyc.cycle = foundCyc.cycle - draldb->getFirstEffectiveCycle(entry->getClockId()).cycle;

        // Go there!
        goToColumn((INT32) ((float) cyc.cycle * cyc.clock->getFastestRatio()));
        goToRow(row);
        shadeColumn(cyc, true);

        // Highlighting.
        getHighLightMgr()->contentsSelectEvent(0, row, foundCyc, true, currentTagSelector, HLT_DEFAULT, true);
        repaintContents(false);
        cache->getFoldWidget()->repaint();
        disWin->alignWith(0, row, foundCyc);
    }
    else
    {
        QMessageBox::warning(this, "Sync error", "The requested tag " + tagName + " with value " + QString::number(tagValue) + " is not found in the edge " + edgeName + " of the trace " + dreamsdb->getTraceName() + ".", QMessageBox::Ok, QMessageBox::NoButton);
    }
}

void
ResourceDAvtView::updateCloseMenuState()
{
    //printf ("updateCloseMenuState() called with nc=%d\n",numCycles);
    Dreams* tdr = (Dreams*)mainWin;
    tdr->enableMenuOptions(numCycles>1);
}

void
ResourceDAvtView::drawDstRubber(int x, int y, int mx)
{
    QPainter p;
    p.begin(viewport());
    p.setRasterOp(NotROP);
    p.setPen(QPen(color0, 1));
    p.setBrush(NoBrush);
    p.setFont(QFont("Helvetica", (qApp->font()).pointSize()));

    INT32 srcRow;
    INT32 dstRow;
    INT32 dummy;
    INT32 temp;
    CLOCK_ID srcClockId;
    CLOCK_ID dstClockId;

    // Gets the row where the distance was pressed.
    srcRow = pressed_my / getInternalBaseElementSize();
    srcRow = QMIN(srcRow, numEvents - 1);
    srcRow = QMAX(srcRow, 0);
    srcClockId = dreamsdb->layoutResourceGetRowClockId(srcRow, cache);

    viewportToContents(0, y, dummy, temp);

    // Gets the row where the distance is now.
    dstRow = (INT32) ((double) temp / ((double) getInternalBaseElementSize() * scf_y));
    dstRow = QMIN(dstRow, numEvents - 1);
    dstRow = QMAX(dstRow, 0);
    dstClockId = dreamsdb->layoutResourceGetRowClockId(dstRow, cache);

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
ResourceDAvtView::drawDstRubberDistance(QPainter * p, QString * str, bool high, INT32 y, INT32 vx)
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
ResourceDAvtView::clearShadedColumnsNoPropagate()
{
    for(UINT32 i = 0; i < myShadows.size(); i++)
    {
        myShadows[i]->clear();
    }

    repaintContents(false);
}

void 
ResourceDAvtView::clearShadedColumns()
{
    for(UINT32 i = 0; i < myShadows.size(); i++)
    {
        myShadows[i]->clear();
    }

    repaintContents(false);

    // propagate is appropiated 
    if(mdiLocked)
    {
        emit clearAllShadedCols();
    }
}

void
ResourceDAvtView::setAncestorTrackingTag(QString str)
{
    TAG_ID tagId = draldb->getTagId(str);
    if(tagId == TAG_ID_INVALID)
    {
        qApp->beep();
        return;
    }
    if(ancWin != NULL)
    {
        ancWin->setTrackingTag(tagId);
    }
}

void
ResourceDAvtView::applyMagicWand(int mx, int my)
{
    popup_row = (INT32) (my / (double) getInternalBaseElementSize());
    popup_row = QMAX(popup_row, 0);

    // Coherency check.
    if(popup_row >= numEvents)
    {
        return;
    }

    CLOCK_ID clockId = dreamsdb->layoutResourceGetRowClockId(popup_row, cache);
    popup_cycle.clock = draldb->getClockDomain(clockId);
    toCycle(mx, popup_row, true, &popup_cycle);
    popup_cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

    do_ancScan();
}

void
ResourceDAvtView::updateCycleBar(double x)
{
    if(focusMDI == myMDI)
    {
        CYCLE cycle;
        TRACK_ID trackId;
        CLOCK_ID clockId;
        const ClockDomainEntry * clock;

        x = QMIN(x, realContentsWidth - 1.0);
        x = QMAX(x, 0.0);

        INT32 myRow;
        myRow = current_my / getInternalBaseElementSize();
        myRow = QMIN(myRow, numEvents - 1);
        myRow = QMAX(myRow, 0);
        trackId = dreamsdb->layoutResourceGetRowTrackId(myRow, cache);

        if(trackId != TRACK_ID_INVALID)
        {
            clockId = draldb->getClockId(trackId);
        }
        else
        {
            clockId = 0;
        }

        cycle.clock = draldb->getClockDomain(clockId);
        toCycle((INT32) x, myRow, false, &cycle);
        cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

        emit cycle_msg_changed(cycle.toString(CYCLE_MASK_ALL) + " ");
    }
}

void
ResourceDAvtView::showDistance(int pressed_mx, int released_mx)
{
    if(focusMDI == myMDI)
    {
        INT32 srcRow;
        INT32 dstRow;
        CLOCK_ID srcClockId;
        CLOCK_ID dstClockId;

        // Gets the row where the distance was pressed.
        srcRow = pressed_my / getInternalBaseElementSize();
        srcRow = QMIN(srcRow, numEvents - 1);
        srcRow = QMAX(srcRow, 0);
        srcClockId = dreamsdb->layoutResourceGetRowClockId(srcRow, cache);

        // Gets the row where the distance is now.
        dstRow = current_my / getInternalBaseElementSize();
        dstRow = QMIN(dstRow, numEvents - 1);
        dstRow = QMAX(dstRow, 0);
        dstClockId = dreamsdb->layoutResourceGetRowClockId(dstRow, cache);

        CYCLE source;
        CYCLE dest;
        CYCLE cyc;

        source.clock = draldb->getClockDomain(srcClockId);
        cyc.clock = source.clock;
        dest.clock = draldb->getClockDomain(dstClockId);

        toCycle((INT32) pressed_mx, srcRow, false, &source);
        toCycle((INT32) released_mx, dstRow, false, &dest);

        cyc.fromLCMCycles(abs(dest.toLCMCycles() - source.toLCMCycles()));

        emit status_msg_changed(
            source.toString(CYCLE_MASK_NUM | CYCLE_MASK_DIV | CYCLE_MASK_CLK) + " -> " +
            dest.toString(CYCLE_MASK_NUM | CYCLE_MASK_DIV | CYCLE_MASK_CLK) + " : " +
            QString::number(cyc.toPs()) + " ps "
        );
    }
}

void
ResourceDAvtView::keyReleaseEvent(QKeyEvent* e)
{
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
             setContentsPos(0,contentsY());
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

void
ResourceDAvtView::updateTagWindow(ItemHandler * hnd)
{
    static_cast<MDIDreamsWindow*>(myMDI)->getTagWindow()->updateItemTag(hnd, draldb->getLastCycle());
}

QString 
ResourceDAvtView::getTipString(bool eventOnly)
{
    QString s = QString::null;
    TRACK_ID trackId;
    CLOCK_ID clockId;
    const ClockDomainEntry * clock;
    CYCLE cycle;
    INT32 row = current_my / getInternalBaseElementSize();
    row = QMIN(row, numEvents - 1);
    row = QMAX(row, 0);

    switch(currentPointerType)
    {
        case POINTER_SELECT:
            if(row < numEvents)
            {
                trackId = dreamsdb->layoutResourceGetRowTrackId(row, cache);
                clockId = dreamsdb->layoutResourceGetRowClockId(row, cache);

                cycle.clock = draldb->getClockDomain(clockId);
                toCycle(current_mx, row, true, &cycle);
                cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

                if(dreamsdb->layoutResourceGetValidData(row, cycle, cache))
                {
                    RowType rtype = dreamsdb->layoutResourceGetRowType(row, cache);
                    ItemHandler hnd;

                    switch(rtype)
                    {
                        case SimpleEdgeRow:
                            dreamsdb->layoutResourceGetRowItemInside(row, cycle, &hnd, cache);

                            if(hnd.isValidItemHandler())
                            {
                                s = ShowTags::getItemTags(draldb, &hnd, cycle);
                            }
                            break;

                        case SimpleNodeRow:
                            if(trackId != TRACK_ID_INVALID)
                            {
                                // TODO: now only shows the tags of the division 0.
                                s = ShowTags::getTrackTags(draldb, trackId, cycle);
                            }
                            break;
                    
                        default:
                            Q_ASSERT(false);
                    }
                }
                else
                {
                    if(!eventOnly)
                    {
                        s = getRowInfo(row, current_mx);
                    }
                }
            }
            break;

        case POINTER_ZOOMING: 
        case POINTER_ZOOMING_NWIN:
        case POINTER_MWAND:
            trackId = dreamsdb->layoutResourceGetRowTrackId(row, cache);
            clockId = dreamsdb->layoutResourceGetRowClockId(row, cache);

            cycle.clock = draldb->getClockDomain(clockId);
            toCycle(current_mx, row, false, &cycle);
            cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

            s = cycle.toString(CYCLE_MASK_ALL);
            s = s + " Row " + QString::number(row);
            break;

        case POINTER_DISTANCE: 
        case POINTER_SHADING: 
            trackId = dreamsdb->layoutResourceGetRowTrackId(row, cache);
            clockId = dreamsdb->layoutResourceGetRowClockId(row, cache);

            cycle.clock = draldb->getClockDomain(clockId);
            toCycle(current_mx, row, false, &cycle);
            cycle.cycle += draldb->getFirstEffectiveCycle(clockId).cycle;

            s = cycle.toString(CYCLE_MASK_ALL);
            break;
        
        default:
            s = QString::null;
    }

    return s;
}

TipResourceDreams::TipResourceDreams(QWidget * parent)
    : QToolTip(parent)
{
    setWakeUpDelay(1500);
}

void 
TipResourceDreams::maybeTip(const QPoint & pos)
{
    if(!parentWidget()->inherits("ResourceDAvtView"))
    {
        return;
    }

    ResourceDAvtView *asv = (ResourceDAvtView*)parentWidget();    
    if(asv->currentPointerType != POINTER_SELECT)
    {
        return;
    }
    
    QRect r(pos.x()-1,pos.y()-1,pos.x()+1,pos.y()+1);
    QString s = asv->getTipString(true);

    if(s != QString::null)
    {
        tip(r, s);
    }
    else
    {
        hide();
    }
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
ResourceDAvtView::createAncestorWindow()
{
    // create the Ancestor Docked Window
    ancWin = new AncestorDockWin(dreamsdb, cache, prefMgr, annCtrl, myMDI, this, hlmgr);
    Q_ASSERT(ancWin != NULL);
    if(prefMgr->getShowRelationshipWindow())
    {
        ancWin->show();
        setEnabledAncestorWindow(true);
    }
    else
    {
        ancWin->hide();
        setEnabledAncestorWindow(false);
    }
    connect(ancWin, SIGNAL(visibilityChanged(bool)), mainWin, SLOT(ancWinVisibilityChanged(bool)));
    connect(ancWin, SIGNAL(placeChanged(QDockWindow::Place)), mainWin, SLOT(ancWinPlaceChanged(QDockWindow::Place)));
    connect(ancWin, SIGNAL(orientationChanged(Orientation)), mainWin, SLOT(ancWinOrientationChanged(Orientation)));
}

void
ResourceDAvtView::createPushoutWindow()
{
    // create the Ancestor Docked Window
    pushWin = new PushoutDockWin(dreamsdb, cache, hlmgr, prefMgr, myMDI, this);
    if(prefMgr->getShowPushoutWindow())
    {
        pushWin->show();
        setEnabledPushoutWindow(true);
    }
    else
    {
        pushWin->hide();
        setEnabledPushoutWindow(false);
    }
    connect(pushWin, SIGNAL(visibilityChanged(bool)), mainWin, SLOT(pushWinVisibilityChanged(bool)));
    connect(pushWin, SIGNAL(placeChanged(QDockWindow::Place)), mainWin, SLOT(pushWinPlaceChanged(QDockWindow::Place)));
    connect(pushWin, SIGNAL(orientationChanged(Orientation)), mainWin, SLOT(pushWinOrientationChanged(Orientation)));
}

void 
ResourceDAvtView::annotationCopy()
{
    // does just a copy of the current selected guy
    if(currentAnnItem)
    {
        if(copyAnnItem)
        {
            delete copyAnnItem;
        }
        copyAnnItem = currentAnnItem->clone();
    }
    else
    {
        copyAnnItem = NULL;
    }
}

void
ResourceDAvtView::annotationPaste()
{
    if(copyAnnItem)
    {
        if(currentAnnItem)
        {
            currentAnnItem->setSelected(false);
            annCtrl->repaintAnnItem(currentAnnItem, scf_x, scf_y);
        }
        currentAnnItem = annCtrl->annotationPaste(copyAnnItem);
        copyAnnItem=currentAnnItem->clone();
    }
}

void
ResourceDAvtView::annotationRemove()
{
    if(currentAnnItem) 
    {
        annCtrl->annotationRemove(currentAnnItem);
        currentAnnItem=NULL;
        pressAnnItem=NULL;
    }
}

void
ResourceDAvtView::cloneAnnotationCtrl(AnnotationCtrl * parent)
{
    annCtrl->clone(parent);
}

/*
void
ResourceDAvtView::escKey()
{
    //printf ("esc hitted\n");fflush(stdout);
    escapeHitted = true;
}

bool
ResourceDAvtView::eventFilter ( QObject * watched, QEvent * e )
{
    if (!redrawing) return AScrollView::eventFilter( watched, e );
    //printf ("eventFilter called while redrawing!\n");

    if ( (watched==horizontalScrollBar())||(watched==verticalScrollBar()) )
    {
        //printf (">>>scrollbars..., type=%d\n",e->type());fflush(stdout);
        bool c1 = e->type() == QEvent::MouseButtonPress;
        bool c2 = e->type() == QEvent::MouseButtonRelease;
        bool c3 = e->type() == QEvent::MouseButtonDblClick;
        bool c4 = e->type() == QEvent::MouseMove;

        return c1||c2||c3||c4;
    }
    else if (watched==viewport())
    {
        //printf (">>>viewport..., type=%d\n",e->type());fflush(stdout);
        return !((e->type()==QEvent::MouseButtonPress)||(e->type()==QEvent::MouseButtonRelease));
    }
    else if (watched==myParent)
    {
        //printf (">>>main..., type=%d\n",e->type());fflush(stdout);
        if (
            (e->type()==QEvent::AccelAvailable) ||
            (e->type()==QEvent::AccelOverride) ||
            (e->type()==QEvent::KeyPress) ||
            (e->type()==QEvent::KeyRelease)||
            (e->type()==QEvent::Destroy)||
            (e->type()==QEvent::Hide)||
            (e->type()==QEvent::Close)||
            (e->type()==QEvent::Quit)||
            (e->type()==QEvent::WindowDeactivate)
            ) return false;

        return TRUE; // eat event
    }
    else
    {
        Q_ASSERT(false);
    }
    return false;
}
*/

void
ResourceDAvtView::raiseMovementSignal(double x, double y)
{
    if(y != 0.0)
    {
        emit resourceChanged(y);
    }
    if(x != 0.0)
    {
        INT64 ps = (totalPs * (INT64) x) / totalPixels;

        emit cycleChanged(static_cast<Q_INT64>(ps));
    }
}

/*
 * Moves the view to the desired column in fastest ratio domain.
 *
 * @return void.
 */
void
ResourceDAvtView::goToColumn(INT32 column)
{
    INT32 cw = (INT32) ((double) visibleWidth() / getInternalBaseElementSize() / scf_x);
    INT32 cx = (INT32) ((double) contentsX() / getInternalBaseElementSize() / scf_x);

    if(dreamsdb->layoutResourceGetUsesDivisions())
    {
        column = column << 1;
    }

    // If is already visible dont move!
    if((column >= cx) && (column <= (cx + cw)))
    {
        return;
    }

    INT32 x = (INT32) floor((double) column * getInternalBaseElementSize() * scf_x);
    setContentsPos(x - (visibleWidth() / 2), contentsY());
}

/*
 * Moves the view to the desired row.
 *
 * @return void.
 */
void
ResourceDAvtView::goToRow(INT32 row)
{
    INT32 ch = (INT32) ((double) visibleHeight() / (double) getInternalBaseElementSize() / scf_y);
    INT32 cy = (INT32) ((double) contentsY() / (double) getInternalBaseElementSize() / scf_y);

    // If is already visible dont move!
    if((row >= cy) && (row <= (cy + ch)))
    {
        return;
    }

    INT32 y = (INT32) floor((double) row * getInternalBaseElementSize() * scf_y);
    setContentsPos(contentsX(), y - (visibleHeight() / 2));
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
ResourceDAvtView::toCycle(INT32 col, INT32 row, bool inval, CYCLE * cycle) const
{
    double ratio = static_cast<double>(cycle->clock->getFastestRatio()) * static_cast<double>(getInternalBaseElementSize());
    cycle->cycle = static_cast<INT32>(floor(static_cast<double>(col) / ratio));
    cycle->division = 0;

    if(dreamsdb->layoutResourceGetUsesDivisions())
    {
        if(dreamsdb->layoutResourceGetRowShowExpanded(row, cache))
        {
            // Just shows one phase, but all the cycle.
            if(dreamsdb->layoutResourceGetRowShowHigh(row, cache))
            {
                // If shows the high phase, the division is high.
                cycle->division = 0;
            }
            else
            {
                // If shows the low phase, the division is low.
                cycle->division = 1;
            }
            cycle->cycle = cycle->cycle >> 1;
        }
        else if(inval)
        {
            // Shows normal phases, let's see if the row has defined the actual phase.
            if(cycle->cycle & 1)
            {
                // We are in a low phase...
                if(dreamsdb->layoutResourceGetRowShowLow(row, cache))
                {
                    // As the row shows low phases, we set the cycle correctly.
                    cycle->cycle = cycle->cycle >> 1;
                    cycle->division = 1;
                }
                else
                {
                    // As the row doesn't show low phases, we are in a invalid cycle.
                    cycle->cycle = -1;
                    cycle->division = 1;
                }
            }
            else
            {
                // We are in a high phase...
                if(dreamsdb->layoutResourceGetRowShowHigh(row, cache))
                {
                    // As the row shows high phases, we set the cycle correctly.
                    cycle->cycle = cycle->cycle >> 1;
                    cycle->division = 0;
                }
                else
                {
                    // As the row doesn't show high phases, we are in a invalid cycle.
                    cycle->cycle = -1;
                    cycle->division = 0;
                }
            }
        }
        else
        {
            cycle->division = cycle->cycle & 1;
            cycle->cycle = cycle->cycle >> 1;
        }
    }
}

/*
 * Converts a cycle in a position in the layout view.
 *
 * @return the col related to the cycle.
 */
INT32
ResourceDAvtView::toCol(CYCLE * cycle) const
{
    INT32 col;
    double ratio = static_cast<double>(cycle->clock->getFastestRatio()) * static_cast<double>(getInternalBaseElementSize());

    if(dreamsdb->layoutResourceGetUsesDivisions())
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

/*
 * Inits the variables needed to paint the shading columns.
 *
 * @return void.
 */
void
ResourceDAvtView::initShadeFlags()
{
    // Clears all the previous elements.
    clearShadeFlags();

    // Adds a new column shadows for each clock domain.
    for(UINT32 i = 0; i < draldb->getNumClocks(); i++)
    {
        // Uses the number of cycles of the domain.
        myShadows.push_back(new ColumnShadows(draldb->getNumCycles(i).cycle));
    }
}

/*
 * Inits the variables needed to paint the shading columns.
 *
 * @return void.
 */
void
ResourceDAvtView::clearShadeFlags()
{
    // Deletes all the pointers.
    for(UINT32 i = 0; i < myShadows.size(); i++)
    {
        delete myShadows[i];
    }

    // Clears the vector.
    myShadows.clear();
}

/*
 * Draws all the selected columns.
 *
 * @return void.
 */
void
ResourceDAvtView::drawShadingColumns(int x1, int x2, int y1, int y2, QPainter * p)
{
    // All code to paint just the shadow columns from the domain
    // of the row (one row just have shadowed the cycles of its domain)
    /*printf ("drawShadingColumns called (%d,%d)->(%d,%d)\n",x1,y1,x2,y2);

    INT32 row1;
    INT32 row2;

    row1 = y1 / getInternalBaseElementSize();
    row1--;
    row1 = QMAX(row1, 0);
    row2 = y2 / getInternalBaseElementSize();
    row2++;
    row2 = QMIN(row2, numEvents - 1);

    p->setBrush(shaderBrush);
    p->setPen(shaderPen);

    // For all the rows.
    while(row1 <= row2)
    {
        const ClockDomainEntry * clock; ///< The clock domain of the actual row.
        ColumnShadows * shadows;        ///< The shadows columns for this domain.
        CLOCK_ID clockId;               ///< Clock id of this row.
        float ratio;                    ///< The size of the rectangles of this row.
        INT32 yPos;                     ///< Y position where to draw the rectangle.
        INT32 cycle1;                   ///< Initial cycle.
        INT32 cycle2;                   ///< End cycle.
        INT32 numRows;                  ///< Number of consecutive rows with the same clock domain.

        // Skips the line if blank row.
        if(dreamsdb->layoutResourceGetRowType(row1, cache) == SimpleBlankRow)
        {
            row1++;
            continue;
        }

        // Gets the clock, shadows and cycles that might be shadowed.
        clockId = dreamsdb->layoutResourceGetRowClockId(row1, cache);
        clock = draldb->getClockDomain(clockId);
        if(dreamsdb->layoutResourceGetUsesDivisions())
        {
            ratio = (float) getInternalBaseElementSize() * clock->getFastestRatio() * 2.0f;
        }
        else
        {
            ratio = (float) getInternalBaseElementSize() * clock->getFastestRatio();
        }
        shadows = myShadows[clockId];
        yPos = row1 * getInternalBaseElementSize();

        cycle1 = (INT32) ((float) x1 / ratio);
        cycle1--;
        cycle1 = QMAX(cycle1, 0);
        cycle2 = (INT32) ((float) x2 / ratio);
        cycle2++;
        cycle2 = QMIN(cycle2, draldb->getNumCycles(clockId).cycle - 1);

        numRows = 1;
        row1++;
        // Gets all the consecutive rows that have the same clock domain.
        while((row1 <= row2) && (dreamsdb->layoutResourceGetRowType(row1, cache) != SimpleBlankRow) && (dreamsdb->layoutResourceGetRowClockId(row1, cache) == clockId))
        {
            numRows++;
            row1++;
        }

        // For all the cycles of this row.
        while(cycle1 <= cycle2)
        {
            // Checks if the cycle is shaded.
            if(shadows->getShadeFlag(cycle1))
            {
                INT32 xPos = (INT32) ((float) cycle1 * ratio);
                p->drawRect(xPos, yPos, (INT32) ratio, numRows * getInternalBaseElementSize());
            }
            cycle1++;
        }
    }*/
    INT32 row1;
    INT32 row2;

    row1 = y1 / getInternalBaseElementSize();
    row1--;
    row1 = QMAX(row1, 0);
    row2 = y2 / getInternalBaseElementSize();
    row2++;
    row2 = QMIN(row2, numEvents - 1);

    p->setBrush(shaderBrush);
    p->setPen(shaderPen);

    for(CLOCK_ID clockId = 0; clockId < draldb->getNumClocks(); clockId++)
    {
        const ClockDomainEntry * clock; ///< The clock domain of the actual row.
        ColumnShadows * shadows;        ///< The shadows columns for this domain.
        float ratio;                    ///< The size of the rectangles of this row.
        INT32 yPos;                     ///< Y position where to draw the rectangle.
        INT32 cycle1;                   ///< Initial cycle.
        INT32 cycle2;                   ///< End cycle.

        // Gets the clock, shadows and cycles that might be shadowed.
        clock = draldb->getClockDomain(clockId);
        if(dreamsdb->layoutResourceGetUsesDivisions())
        {
            ratio = (float) getInternalBaseElementSize() * clock->getFastestRatio() * 2.0f;
        }
        else
        {
            ratio = (float) getInternalBaseElementSize() * clock->getFastestRatio();
        }
        shadows = myShadows[clockId];
        yPos = row1 * getInternalBaseElementSize();

        cycle1 = (INT32) ((float) x1 / ratio);
        cycle1--;
        cycle1 = QMAX(cycle1, 0);
        cycle2 = (INT32) ((float) x2 / ratio);
        cycle2++;
        cycle2 = QMIN(cycle2, draldb->getNumCycles(clockId).cycle - 1);

        // For all the cycles of this row.
        while(cycle1 <= cycle2)
        {
            // Checks if the cycle is shaded.
            if(shadows->getShadeFlag(cycle1))
            {
                INT32 xPos = (INT32) ((float) cycle1 * ratio);
                p->drawRect(xPos, yPos, (INT32) ratio, (row2 - row1) * getInternalBaseElementSize());
            }
            cycle1++;
        }
    }
}

/*
 * Draws all the selected columns.
 *
 * @return void.
 */
void
ResourceDAvtView::copyColumnShadows(AScrollView * parent)
{
}

/*
 * Draws all the selected columns.
 *
 * @return void.
 */
void
ResourceDAvtView::shadeColumn(INT32 col, bool shade)
{
    CLOCK_ID clockId;
    INT32 myRow;
    const ClockDomainEntry * clock;

    // Gets the clock id of the pressed row.
    myRow = pressed_my / getInternalBaseElementSize();
    myRow = QMIN(myRow, numEvents - 1);
    clockId = dreamsdb->layoutResourceGetRowClockId(myRow, cache);
    clock = draldb->getClockDomain(clockId);

    myShadows[clockId]->shadeColumn(col, shade);

    double ratio;

    if(dreamsdb->layoutResourceGetUsesDivisions())
    {
        ratio = (double) clock->getFastestRatio() * 2.0;
    }
    else
    {
        ratio = (double) clock->getFastestRatio();
    }
    repaintContents(
        (INT32) ((double) col * ratio * scf_x * (double) getInternalBaseElementSize() - (2.0 * scf_x)),
        0,
        (INT32) (scf_x * ratio * (double) getInternalBaseElementSize() + (4.0 * scf_x)),
        (INT32) (scf_y * (double) realContentsHeight),
        false);
}

/*
 * Draws all the selected columns.
 *
 * @return void.
 */
void
ResourceDAvtView::shadeColumn(CYCLE cycle, bool shade)
{
    CLOCK_ID clockId;
    const ClockDomainEntry * clock;

    // Gets the clock id of the pressed row.
    clockId = cycle.clock->getId();
    clock = cycle.clock;

    myShadows[clockId]->shadeColumn(cycle.cycle, shade);

    double ratio;

    if(dreamsdb->layoutResourceGetUsesDivisions())
    {
        ratio = (double) clock->getFastestRatio() * 2.0;
    }
    else
    {
        ratio = (double) clock->getFastestRatio();
    }
    repaintContents(
        (INT32) ((double) cycle.cycle * ratio * scf_x * (double) getInternalBaseElementSize() - (2.0 * scf_x)),
        0,
        (INT32) (scf_x * ratio * (double) getInternalBaseElementSize() + (4.0 * scf_x)),
        (INT32) (scf_y * (double) realContentsHeight),
        false);
}

/*
 * Switches the state of the shadowing for a given cycle.
 *
 * @return void.
 */
void
ResourceDAvtView::switchShadeFlag(INT32 col)
{
    CLOCK_ID clockId;
    INT32 myCol;
    INT32 myRow;

    // Gets the clock id of the pressed row.
    myRow = pressed_my / getInternalBaseElementSize();
    myRow = QMIN(myRow, numEvents - 1);

    clockId = dreamsdb->layoutResourceGetRowClockId(myRow, cache);
    if(dreamsdb->layoutResourceGetUsesDivisions())
    {
        myCol = (INT32) ((float) col / ((float) getInternalBaseElementSize() * draldb->getClockDomain(clockId)->getFastestRatio() * 2.0f));
    }
    else
    {
        myCol = (INT32) ((float) col / ((float) getInternalBaseElementSize() * draldb->getClockDomain(clockId)->getFastestRatio()));
    }
    // Inverts the selection.
    shadeColumn(myCol, !myShadows[clockId]->getShadeFlag(myCol));
    if(mdiLocked)
    {
        emit toggledShadowCol(myCol, myShadows[clockId]->getShadeFlag(myCol));
    }
}

/*
 * Repaints all the contents when the folding has changed.
 *
 * @return void.
 */
void
ResourceDAvtView::foldingChanged()
{
    // Also recomputes the size of the ascroll view.
    numEvents = cache->getNumRows();
    AScrollView::setRealContentsSize(totalPixels, numEvents * cube_side);
    // This is needed in order to recompute the highlighted events.
    hlmgr->reset();

    // All the beziers are removed.
    /*QPtrDictIterator<AnnotationItem> it = annCtrl->getIterator();

    while(it.current() != NULL)
    {
        AnnotationAutoBezierArrow * bezier = dynamic_cast<AnnotationAutoBezierArrow *>(it.current());
        ++it;
        if(bezier)
        {
            annCtrl->annotationRemove(bezier);
        }
    }*/
    ancWin->reset();
    repaintContents(true);
}

/*
 * Returns the necessary information to paint the folding.
 *
 * @return void.
 */
void
ResourceDAvtView::getFoldInfo(UINT32 * offset, double * scfy) const
{
    // Gets the vertical scroll bar offset.
    * offset = verticalScrollBar()->value();
    * scfy = scf_y;
}

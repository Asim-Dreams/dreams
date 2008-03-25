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
  * @file FloorplanDAvtView.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/FloorplanDAvtView.h"
#include "dialogs/find/FindDialogImplFloorplan.h"
#include "dialogs/tag/TagDialogImpl.h"
#include "dockwin/disassembly/DisDockWinFloorplan.h"
#include "dockwin/tag/TagDockWin.h"
#include "mdi/MDIFloorplanWindow.h"
#include "Dreams.h"
#include "ShowTags.h"
#include "FavoriteTags.h"
#include "DreamsDB.h"

// Qt includes.
#include <qstringlist.h>
#include <qtooltip.h>
#include <qaccel.h>

// Externs.
extern QWidget * focusMDI;

FloorplanDAvtView::FloorplanDAvtView(DreamsDB * _dreamsdb, QWidget * parent, QMainWindow * mdi, QMainWindow * main)
    : DAvtView(parent, main, cube_side, cube_side)
{
    //printf ("starting 2dview constructor...\n");
    myMDI = mdi;
    mdiLocked = true;
    
    // filter out some events
    horizontalScrollBar()->installEventFilter(this);
    verticalScrollBar()->installEventFilter(this);
    viewport()->installEventFilter(this);
    
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();

    AScrollView::setRealContentsSize((int)(dreamsdb->layoutFloorplanGetWidth()), (int)(dreamsdb->layoutFloorplanGetHeight()));
    updateCloseMenuState();

    annCtrl->setBaseSize(getBaseElementSize());
    gridSize = (int)ceil(getBaseElementSize() * 10.0);
    //annCtrl->setGridSize(gridSize);
    annCtrl->setGridSize(getBaseElementSize()); // tmp hack

    initShadeFlags();

    currentCycle = draldb->getFirstEffectiveCycle(0);

    // create tag-favorite class
    ft = FavoriteTags::getInstance();
    currentTagSelector = ft->getList().first();

    // HighLight manager
    hlmgr = new HighLightMgrFloorplan(dreamsdb, cube_side);
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

    tip = new TipFloorplanDreams(this);

    // Create item window
    disWin = new DisDockWinFloorplan(dreamsdb, myMDI, main, this);
    initDisWindow();
    
    // Create the find dialog
    DAvtView::fndDialog = new FindDialogImplFloorplan(dreamsdb, this, main, "findDialog",true);

    copyAnnItem = NULL;
}

FloorplanDAvtView::~FloorplanDAvtView()
{
    if(tip != NULL) delete tip;
    if(contextMenu != NULL) delete contextMenu;
}

void 
FloorplanDAvtView::contentsSelectEvent(INT32 col, INT32 row, bool adding, bool shifted)
{
    bool needRep;
    if(hasContentsOn(col, row))
    {
        needRep = static_cast<HighLightMgrFloorplan *>(hlmgr)->contentsSelectEvent(col, row, currentCycle, adding, currentTagSelector, htype);

        // check shift key (synchro with Item window)
        if(!shifted && disWinEnabled) 
        {
            disWin->alignWith(0, row, currentCycle);
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
        repaintContents(false);
    }
}

void
FloorplanDAvtView::reset()
{
    numCycles=0;
    numEvents=0;

    AScrollView::setRealContentsSize(0,0);

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


/**
 * Places the animation in the cycle given as parameter. 
 * This method emits a signal notifying the change of current 
 * cycle to be attended by the rest of the Dreams window.
 * By the moment we update the change by each movement, so always is 1
 *
 * @param New execution cycle
 **/
void
FloorplanDAvtView::showCycle(CYCLE cycle)
{
    CLOCK_ID clockId;
    bool found;

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
            // A similar clock has been found, just use it.
            currentCycle.clock = draldb->getClockDomain(clockId);
        }

        currentCycle.fromPs(cycle.toPs());
    }
    else
    {
        // Changes to the domain as the clock belongs to this trace.
        currentCycle = cycle;
    }

    updateCycleBar(0.0);
    repaintContents(false);
}

// -----------------------------------------------------------------------------------
// Context menu methods
// -----------------------------------------------------------------------------------
void 
FloorplanDAvtView::addContextMenu()
{
    contextMenu = new QPopupMenu(this);
    contextMenuSlotTools = new QPopupMenu(contextMenu);
    contextMenuPlayTools = new QPopupMenu(contextMenu);
    contextMenuSyncTools = new QPopupMenu(contextMenu);

    // Add the slot tools menu.
    contextMenu->insertItem("S&lot Tools", contextMenuSlotTools);
    show_slot_info_id = contextMenuSlotTools->insertItem("&Show Slot Info", this, SLOT(do_showSlotInfo()));
    launch_watch_id = contextMenuSlotTools->insertItem("&Launch Watch Window", this, SLOT(do_launchWatchWindow()));

    // Add the play tools menu.
    contextMenu->insertItem("&Play Tools", contextMenuPlayTools);
    Dreams *twoDMain = dynamic_cast<Dreams *>(mainWin);
    Q_ASSERT(twoDMain);
    rew_id   = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image42), "&Rewind"       , twoDMain->getPtbRewind(), SLOT(animateClick()));
    stepb_id = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image43), "Step &Backward", twoDMain->getPtbStepBackward(), SLOT(animateClick()));
    play_id  = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image44), "&Play"         , twoDMain->getPtbPlay(), SLOT(animateClick()));
    stop_id  = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image45), "&Stop"         , twoDMain->getPtbStop(), SLOT(animateClick()));
    stepf_id = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image46), "Step For&ward" , twoDMain->getPtbStepForward(), SLOT(animateClick()));
    forw_id  = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image47), "F&orward"      , twoDMain->getPtbForward(), SLOT(animateClick()));

    // Synchronization tools.
    contextMenu->insertItem("&Sync Tools", contextMenuSyncTools);
    contextMenuSyncTools->insertItem("Sync To This &Cycle", this, SLOT(do_syncToCycle()));
    contextMenuSyncTools->insertItem("Sync To This Cycle (same &trace)", this, SLOT(do_syncToCycleTrace()));

    // Free window.
    contextMenu->insertSeparator();
    rebel_id = contextMenu->insertItem("&Free Window", this, SLOT(do_switchRebel()));
    // by default MDI is NOT rebel 
    contextMenu->setItemChecked(rebel_id, false);
}

void 
FloorplanDAvtView::rewind()
{
    contextMenu->setItemEnabled(rew_id, false); 
    contextMenu->setItemEnabled(play_id, true); 
    contextMenu->setItemEnabled(forw_id, true); 
}

void 
FloorplanDAvtView::play()
{
    contextMenu->setItemEnabled(rew_id, true); 
    contextMenu->setItemEnabled(play_id, false); 
    contextMenu->setItemEnabled(forw_id, true); 
}

void 
FloorplanDAvtView::stop()
{
    contextMenu->setItemEnabled(rew_id, true); 
    contextMenu->setItemEnabled(play_id, true); 
    contextMenu->setItemEnabled(forw_id, true); 
}

void 
FloorplanDAvtView::forward()
{
    contextMenu->setItemEnabled(rew_id, true); 
    contextMenu->setItemEnabled(play_id, true); 
    contextMenu->setItemEnabled(forw_id, false); 
}

void
FloorplanDAvtView::do_switchRebel()
{
    mdiLocked=!mdiLocked;
    contextMenu->setItemChecked(rebel_id,!mdiLocked);
    if (mdiLocked)
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
FloorplanDAvtView::do_showSlotInfo()
{
    TRACK_ID trackId = dreamsdb->layoutFloorplanGetTrackIdForCell(popup_col, popup_row);
    if(trackId != TRACK_ID_INVALID)
    {
        TagDialogImpl td(draldb, trackId, currentCycle, this, "td", true);
        td.exec();
    }
}

void
FloorplanDAvtView::do_launchWatchWindow()
{
    Dreams * main = dynamic_cast<Dreams *>(mainWin);
    Q_ASSERT(main);

    MDIDreamsWindow * new_win = (MDIDreamsWindow *) main->newWatchDView(dreamsdb, dreamsdb->layoutFloorplanGetNodeNameAt(popup_col, popup_row));
    new_win->showCycle(currentCycle);
}

void
FloorplanDAvtView::do_syncToCycle()
{
    emit syncToCycle(currentCycle);
}

void
FloorplanDAvtView::do_syncToCycleTrace()
{
    emit syncToCycleTrace(currentCycle);
}

QString
FloorplanDAvtView::getRowInfo(INT32 selrow)
{
    QString report("");
    return report;
}

void 
FloorplanDAvtView::rightClickPressEvent(QMouseEvent* e)
{
    rc_pressed_x = e->x();
    rc_pressed_y = e->y();
}

void 
FloorplanDAvtView::rightClickReleaseEvent(QMouseEvent* e)
{
    rc_released_x = e->x();
    rc_released_y = e->y();

    bool c1 = abs(rc_pressed_x-rc_released_x)<3;
    bool c2 = abs(rc_pressed_y-rc_released_y)<3;
    if (c1&&c2)
    {
        int cx,cy;
        double wx,wy;
        viewportToContents (rc_released_x,rc_released_y,cx,cy);
        wx = ((double)cx/scf_x);
        wy = ((double)cy/scf_y);
        popup_col = (int)(wx/getBaseElementSize());
        popup_row = (int)(wy/getBaseElementSize());

        bool bEnabled = dreamsdb->layoutFloorplanHasValidData(popup_col, popup_row, currentCycle); 
        contextMenu->setItemEnabled(show_slot_info_id, bEnabled);
        QString node_name = dreamsdb->layoutFloorplanGetNodeNameAt(popup_col, popup_row);
        bEnabled = (dreamsdb->layoutFloorplanGetNodeNameAt(popup_col, popup_row) != QString::null);
        if(bEnabled)
        {
            WatchWindow * ww = dreamsdb->layoutWatchGetWindow(node_name);
            bEnabled = (ww != NULL);
        }
        contextMenu->setItemEnabled(launch_watch_id, bEnabled);

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
FloorplanDAvtView::drawWorldCoordinatesContents(QPainter* p, int ncx, int ncy, int ncw, int nch)
{
    bool ok = true;

    // compute affected area with the CUBE_SIDE...
    // FIXME
    // Actually all the Floorplan geometry is not really based on the CUBE_SIDE size, given that
    // the user can specify the geometry of the cell's.
    int x1 = ncx - (ncx % cube_side);
    int x2 = ncx + ncw + 2 * (ncx % cube_side);
    int y1 = ncy - (ncy % cube_side);
    int y2 = ncy + nch + 2 * (ncy % cube_side);

    x1 -= 10;
    x2 += 10;
    y1 -= 10;
    y2 += 10;

    x1 = QMAX(x1, 0);
    y1 = QMAX(y1, 0);
    x2 = QMIN(x2, realContentsWidth - 1);
    y2 = QMIN(y2, realContentsHeight - 1);

    // put the black pen and go ahead
    p->setPen(QPen(QColor(0,0,0), cube_border_size));
    p->setBrush(NoBrush);

    qlToDraw.clear();
    dreamsdb->layoutFloorplanGetRectanglesAt(x1, y1, x2, y2, qlToDraw);

    // TODO
    // Implement the rest of view posibilities. By now we only show one scale view... sig!
    ok = drawFullScale(p, qlToDraw);

    return ok;
}

void 
FloorplanDAvtView::updateCycleBar(double x)
{
    if(focusMDI == myMDI)
    {
        emit cycle_msg_changed(currentCycle.toString(CYCLE_MASK_ALL) + " ");
    }
}

/**
 * update the tag window with the ItemHandler selected by the user
 * TODO
 * Implement something :)
 *
 * @param hnd trackId of the selected item
 */
void 
FloorplanDAvtView::updateTagWindow(ItemHandler * hnd)
{
    static_cast<MDIDreamsWindow*>(myMDI)->getTagWindow()->updateItemTag(hnd, draldb->getLastCycle());
}

void
FloorplanDAvtView::updateRowBar(double x, double y)
{
    if(focusMDI == myMDI)
    {
        QString rowdesc = dreamsdb->layoutFloorplanGetCellAt(current_x, current_y);;
        if (rowdesc.isNull())
            rowdesc = "";

        emit row_msg_changed(rowdesc.leftJustify(30,' ',false));
    }
}

void
FloorplanDAvtView::mouseWorldDoubleClickEvent (QMouseEvent* e, double x, double y)
{
    /** @todo This function became deprecated, remove !? */
}

/**
 * Respond to the mouse movement across the viewport.
 * Update the Tag window if it is enabled and visible
 */
void
FloorplanDAvtView::mouseWorldMoveEvent(QMouseEvent* e, double x, double y)
{
    current_x = static_cast<int>(x);
    current_y = static_cast<int>(y);

    if(current_x < 0) current_x = 0;
    if(current_y < 0) current_y = 0;

    // invoke parent implementation
    AScrollView::mouseWorldMoveEvent(e, x, y);

    // and now update the tagwindow if needed
    // look for the event on the matrix:
    if(static_cast<MDIDreamsWindow*>(myMDI)->getEnabledTagWindow() && !(e->state() & AltButton))
    {
        TRACK_ID trackId = dreamsdb->layoutFloorplanGetTrackIdForCell(current_x, current_y);
        //cerr << "FloorplanDAvtView::mouseWorldMoveEvent: trackId = " << trackId << endl;
        if(trackId != TRACK_ID_INVALID)
        {
            ItemHandler handler;
            UINT16 clockId = draldb->getClockId(trackId);
            CYCLE cycle(draldb->getClockDomain(clockId));

            cycle.fromLCMCycles(currentCycle.toLCMCycles());

            if(draldb->getItemInSlot(trackId, cycle, &handler))
            {
                //cerr << "FloorplanDAvtView::mouseWorldMoveEvent: itemId = " << handler.getItemId() << endl;
                static_cast<MDIDreamsWindow*>(myMDI)->getTagWindow()->updateItemTag(&handler, cycle);
            }
        }
        else
        {
            static_cast<MDIDreamsWindow*>(myMDI)->getTagWindow()->reset();
        }
    }
}

void
FloorplanDAvtView::updateCloseMenuState()
{
    //printf ("updateCloseMenuState() called with nc=%d\n",numCycles);
    Dreams* tdr = (Dreams*)mainWin;
    tdr->enableAncestorMenu(false);
    tdr->enableHighlightSubMenu(false);
}

void
FloorplanDAvtView::drawDstRubber(int x, int y, int mx)
{
}

void
FloorplanDAvtView::applyMagicWand (int mx,int my)
{
    //printf ("magic wind called on (%d,%d)\n",mx,my);fflush(stdout);
    popup_col = mx;
    popup_row = my;
}

void
FloorplanDAvtView::showDistance(int pressed_mx, int released_mx)
{
}

void
FloorplanDAvtView::keyReleaseEvent(QKeyEvent* e)
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

QString 
FloorplanDAvtView::getTipString(bool eventOnly)
{
    QString s = QString::null;
    
    /* TODO 
     * Decide what to show in the different tooltips
     */
    switch (currentPointerType)
    {
        case POINTER_SELECT:
        case POINTER_ZOOMING: 
        case POINTER_ZOOMING_NWIN:
        {
            s = dreamsdb->layoutFloorplanGetCellAt(current_x, current_y);
            TRACK_ID trackId = dreamsdb->layoutFloorplanGetTrackIdForCell(current_x, current_y);
            //cerr << "FloorplanDAvtView::getTipString: trackId = " << trackId << endl;
            if(trackId != TRACK_ID_INVALID)
            {
                ItemHandler handler;
                CLOCK_ID clockId = draldb->getClockId(trackId);
                CYCLE cycle(draldb->getClockDomain(clockId));

                cycle.fromLCMCycles(currentCycle.toLCMCycles());

                if(draldb->getItemInSlot(trackId, cycle, &handler))
                {
                    s += "\n---------------\n";
                    s += ShowTags::getItemTags(draldb, &handler, cycle);
                }
            }
            break;
        }


    //    case POINTER_MWAND:
    //    s = "Cycle " + QString::number((UINT32)(current_mx+myDB->getFirstEffectiveCycle()));
    //    s = s + " Row " + QString::number(current_my);
    //    break;

    //    case POINTER_DISTANCE: 
    //    case POINTER_SHADING: 
    //    s = "Cycle " + QString::number((UINT32)(current_mx+myDB->getFirstEffectiveCycle()));
    //    break;
    //    

        default:
            s = QString::null;
    };

    return s;
}




TipFloorplanDreams::TipFloorplanDreams( QWidget * parent )
    : QToolTip( parent )
{
    setWakeUpDelay(1500);
}

void 
TipFloorplanDreams::maybeTip( const QPoint &pos )
{
    //printf ("TipFloorplanDreams::maybeTip called\n");
    if(!parentWidget()->inherits("FloorplanDAvtView"))
    {
        //printf ("TipFloorplanDreams::maybeTip no right parent, getting out\n");
        //hide();
        return;
    }

    FloorplanDAvtView *asv = (FloorplanDAvtView*)parentWidget();    
    /*
    if (asv->currentPointerType>=POINTER_DRAWLINE) return;
    if (asv->currentPointerType==POINTER_MWAND) return;
    if (asv->currentPointerType==POINTER_PANNING) return;
    */
    if (asv->currentPointerType != POINTER_SELECT) return;
    
    QRect r(pos.x()-1, pos.y()-1, pos.x()+1, pos.y()+1);
    QString s = asv->getTipString(true);

    //if (s!=QString::null) printf ("TipFloorplanDreams::maybeTip got null string\n");
    if(s != QString::null)
        tip(r, s);
    else
        hide();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void 
FloorplanDAvtView::annotationCopy()
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
FloorplanDAvtView::annotationPaste()
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
FloorplanDAvtView::annotationRemove()
{
    if (currentAnnItem) 
    {
        annCtrl->annotationRemove(currentAnnItem);
        currentAnnItem=NULL;
        pressAnnItem=NULL;
    }
}

void 
FloorplanDAvtView::cloneAnnotationCtrl(AnnotationCtrl* parent)    
{
    annCtrl->clone(parent);
}

/*
 * Draws all the selected columns.
 *
 * @return void.
 */
void
FloorplanDAvtView::drawShadingColumns(int x1, int x2, int y1, int y2, QPainter * p)
{
}

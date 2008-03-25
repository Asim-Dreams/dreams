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

/*
 * @file MDIResourceWindowFold.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/ResourceDAvtView.h"
#include "avt/ResourceDAvtView.h"
#include "layout/resource/LayoutResourceCache.h"
#include "mdi/MDIResourceWindowFold.h"
#include "DreamsDB.h"
#include "PreferenceMgr.h"

// Qt includes.
#include <qevent.h>

// Some internal defines.
#define SCALE_THRESHOLD (float) 0.6f
#define FOLD_WIDGET_WIDTH (UINT32) 16

/*
 * Creates a new two dreams folding widget.
 *
 * @return the new object.
 */
MDIResourceWindowFold::MDIResourceWindowFold(DreamsDB * _dreamsdb, LayoutResourceCache * _cache, QWidget * parent)
    : QFrame(parent)
{
    // Gets the parameters.
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();
    cache = _cache;

    // Sets the style of the widget.
    setFrameStyle(QFrame::NoFrame);
    setMouseTracking(true);
    setMinimumWidth(FOLD_WIDGET_WIDTH);

    // Sets some fields.
    bgcolor = PreferenceMgr::getInstance()->getBackgroundColor();
    linecolor = QColor(0, 0, 0);
    tip = NULL;
    lastTipRow = (UINT32) -1;
    row_mask = (RV_ROW_DESC_NAME | RV_ROW_DESC_FROM | RV_ROW_DESC_TO | RV_ROW_DESC_LANE | RV_ROW_DESC_LEVEL);

    addContextMenu();
}

/*
 * Destructor of the class.
 *
 * @return the new object.
 */
MDIResourceWindowFold::~MDIResourceWindowFold()
{
    delete contextMenu;
}

/*
 * Paints the folding content.
 *
 * @return void.
 */
void
MDIResourceWindowFold::drawContents(QPainter * p)
{
    UINT32 offset;
    double dscfy;
    float scfy;

    davt->getFoldInfo(&offset, &dscfy);

    lastScfy = (float) dscfy;
    lastOffset = offset;

    scfy = lastScfy;
    offset = (INT32) ((float) offset / scfy);

    p->setPen(linecolor);
    p->fillRect(rect(), bgcolor);

    // From this threshold no grouping info is shown.
    if(scfy < SCALE_THRESHOLD)
    {
        return;
    }

    QFont font;

    font = p->font();
    QFont new_font = font;
    new_font.setPointSize((INT32) (10 * scfy));
    p->setFont(new_font);

    INT32 firstRow = offset / CUBE_SIDE;
    INT32 myOffset = offset % CUBE_SIDE;
    INT32 lastRow = firstRow + (height() / (INT32) ((float) CUBE_SIDE * scfy)) + 1;
    lastRow = QMIN(lastRow, (INT32) cache->getNumRows());

    myOffset = -myOffset;

    for(INT32 i = firstRow; i < lastRow; i++)
    {
        switch(cache->foldCache[i])
        {
            case ResourceViewFoldUnfold:
                drawUnfoldItem(p,  myOffset, scfy);
                break;

            case ResourceViewFoldFold:
                drawFoldItem(p,  myOffset, scfy);
                break;

            case ResourceViewFoldLine:
                drawLineItem(p, myOffset, scfy);
                break;

            case ResourceViewFoldLineAndEnd:
                drawLineAndEndItem(p, myOffset, scfy);
                break;

            case ResourceViewFoldEnd:
                drawEndItem(p, myOffset, scfy);
                break;

            case ResourceViewFoldNothing:
                break;
        }

        p->drawText(2 + CUBE_SIDE, (UINT32) ((myOffset + CUBE_SIDE) * scfy), dreamsdb->layoutResourceGetRowDescription(i, cache, row_mask));
        myOffset += CUBE_SIDE;
    }

    p->setFont(font);
}

/*
 * Paints the unfolding item.
 *
 * @return void.
 */
void
MDIResourceWindowFold::drawUnfoldItem(QPainter * p, INT32 pos, float scfy)
{
    pos = (INT32) ((pos + (CUBE_SIDE >> 1) + 1) * scfy) - 4;
    p->drawRect(2, pos, 9, 9);
    p->drawLine(4, pos + 4, 8, pos + 4);
}

/*
 * Paints the folding item.
 *
 * @return void.
 */
void
MDIResourceWindowFold::drawFoldItem(QPainter * p, INT32 pos, float scfy)
{
    pos = (INT32) ((pos + (CUBE_SIDE >> 1) + 1) * scfy) - 4;
    p->drawRect(2, pos, 9, 9);
    p->drawLine(4, pos + 4, 8, pos + 4);
    p->drawLine(6, pos + 2, 6, pos + 6);
}

/*
 * Paints the line item.
 *
 * @return void.
 */
void
MDIResourceWindowFold::drawLineItem(QPainter * p, INT32 pos, float scfy)
{
    p->drawLine(1 + (CUBE_SIDE >> 1), (INT32) (pos * scfy), 1 + (CUBE_SIDE >> 1), (INT32) ((pos + CUBE_SIDE) * scfy));
}

/*
 * Paints a line and end of group item.
 *
 * @return void.
 */
void
MDIResourceWindowFold::drawLineAndEndItem(QPainter * p, INT32 pos, float scfy)
{
    p->drawLine(1 + (CUBE_SIDE >> 1), (INT32) (pos * scfy), 1 + (CUBE_SIDE >> 1), (INT32) ((pos + CUBE_SIDE) * scfy));
    p->drawLine(1 + (CUBE_SIDE >> 1), (INT32) ((pos + (CUBE_SIDE >> 1)) * scfy), CUBE_SIDE - 2, (INT32) ((pos + (CUBE_SIDE >> 1)) * scfy));
}

/*
 * Paints the end of group item.
 *
 * @return void.
 */
void
MDIResourceWindowFold::drawEndItem(QPainter * p, INT32 pos, float scfy)
{
    p->drawLine(1 + (CUBE_SIDE >> 1), (INT32) (pos * scfy), 1 + (CUBE_SIDE >> 1), (INT32) ((pos + (CUBE_SIDE >> 1)) * scfy));
    p->drawLine(1 + (CUBE_SIDE >> 1), (INT32) ((pos + (CUBE_SIDE >> 1)) * scfy), CUBE_SIDE - 2, (INT32) ((pos + (CUBE_SIDE >> 1)) * scfy));
}

/*
 * Returns the size of the fold.
 *
 * @return void.
 */
QSize
MDIResourceWindowFold::sizeHint() const
{
    return QSize(FOLD_WIDGET_WIDTH, 4000);
}

/*
 * Function to handle mouse events.
 *
 * @return void.
 */
void
MDIResourceWindowFold::mouseReleaseEvent(QMouseEvent * e)
{
    if(e->button() != LeftButton)
    {
        contextMenu->exec(QCursor::pos());
        repaint();
        return;
    }

    // Ignored if out of threshold.
    if(lastScfy < SCALE_THRESHOLD)
    {
        return;
    }

    // Prevents from weird cases.
    if(e->y() < 0)
    {
        return;
    }

    // Gets the row where the event happened.
    UINT32 row = posToRow(e->y());

    // Looks if the selected row is a group row.
    if(dreamsdb->layoutResourceGetRowIsGroup(row, cache))
    {
        // Changes the state of the row and recomputes the layout cache.
        dreamsdb->layoutResourceSwitchRowGroup(row, cache);
        dreamsdb->layoutResourceComputeLayoutCache(cache);
        davt->foldingChanged();
        repaint();
    }
}

/*
 * Handles the mouse moves over the widget.
 *
 * @return void.
 */
void
MDIResourceWindowFold::mouseMoveEvent(QMouseEvent * e)
{
    // Ignored if out of threshold.
    if(lastScfy < SCALE_THRESHOLD)
    {
        return;
    }

    // Prevents from weird cases.
    if(e->y() < 0)
    {
        return;
    }

    // Gets the row where the event happened.
    UINT32 row = posToRow(e->y());

    // Looks if the selected row is a group row.
    if(dreamsdb->layoutResourceGetRowIsGroup(row, cache))
    {
        if((row != lastTipRow) || (tip == NULL))
        {
            // We are in a new group, so we create a new tip.
            cleanTip();
            lastTipRow = row;

            tip = new AGTTipLabel(NULL, dreamsdb->layoutResourceGetRowComposedName(row, cache));
            tip->show();
            tip->raise();
        }

        tip->move(mapToGlobal(QPoint(e->x() + 10, e->y())));
    }
    else
    {
        cleanTip();
        lastTipRow = (UINT32) -1;
    }
}

/*
 * Actions done when the focus is gone.
 *
 * @return void.
 */
void
MDIResourceWindowFold::leaveEvent(QEvent * e)
{
    cleanTip();
}

/*
 * Sets the ResourceDAvtView of the folding.
 *
 * @return void.
 */
void
MDIResourceWindowFold::setTwoDAvtView(ResourceDAvtView * v)
{
    davt = v;
}

/*
 * Slot called when the resource of its two davt changes. Needs repaint.
 *
 * @return void.
 */
void
MDIResourceWindowFold::resourceChanged(double)
{
    repaint();
}

/*
 * Returns the row mapped to the position pos.
 *
 * @return the row.
 */
UINT32
MDIResourceWindowFold::posToRow(UINT32 pos) const
{
    UINT32 row;

    row = (UINT32) floorf((float) (pos + lastOffset) / ((float) CUBE_SIDE * lastScfy));
    row = QMIN(row, dreamsdb->layoutResourceGetNumPhyisicalRows() - 1);
    return row;
}

/*
 * Clears the tip.
 *
 * @return void.
 */
void
MDIResourceWindowFold::cleanTip()
{
    if(tip != NULL)
    {
        tip->hide();
        delete tip;
        tip = NULL;
    }
}

/*
 * Adds the context menu.
 *
 * @return void.
 */
void
MDIResourceWindowFold::addContextMenu()
{
    contextMenu = new QPopupMenu(this);
    contextMenuShow = new QPopupMenu(contextMenu);
    contextMenuSetup = new QPopupMenu(contextMenu);
    contextMenuSetupNames = new QPopupMenu(contextMenu);
    contextMenuSetupOccupancy = new QPopupMenu(contextMenu);

    // Show menu.
    contextMenu->insertItem("&Show", contextMenuShow);
    show_name_id = contextMenuShow->insertItem("Show &Names", this, SLOT(do_showNames()));
    show_occupancy_id = contextMenuShow->insertItem("Show &Occupancy", this, SLOT(do_showOccupancy()));

    contextMenuShow->setItemChecked(show_name_id, true);
    contextMenuShow->setItemChecked(show_occupancy_id, false);

    // Setup menu.
    contextMenu->insertItem("&Setup", contextMenuSetup);

    // Row name setup menu
    contextMenuSetup->insertItem("Setup &Names", contextMenuSetupNames);
    setup_name_number_id = contextMenuSetupNames->insertItem("Show Row &Number", this, SLOT(do_showRowNumber()));
    setup_name_type_id = contextMenuSetupNames->insertItem("Show Row &Type", this, SLOT(do_showRowType()));
    setup_name_name_id = contextMenuSetupNames->insertItem("Show Row &Name", this, SLOT(do_showRowName()));
    setup_name_from_id = contextMenuSetupNames->insertItem("Show Row &From", this, SLOT(do_showRowFrom()));
    setup_name_to_id = contextMenuSetupNames->insertItem("Show Row &To", this, SLOT(do_showRowTo()));
    setup_name_lane_id = contextMenuSetupNames->insertItem("Show Row &Lane", this, SLOT(do_showRowLane()));
    setup_name_level_id = contextMenuSetupNames->insertItem("Show Row L&evel", this, SLOT(do_showRowLevel()));

    contextMenuSetupNames->setItemChecked(setup_name_number_id, false);
    contextMenuSetupNames->setItemChecked(setup_name_type_id, false);
    contextMenuSetupNames->setItemChecked(setup_name_name_id, true);
    contextMenuSetupNames->setItemChecked(setup_name_from_id, true);
    contextMenuSetupNames->setItemChecked(setup_name_to_id, true);
    contextMenuSetupNames->setItemChecked(setup_name_lane_id, true);
    contextMenuSetupNames->setItemChecked(setup_name_level_id, true);

    // Row occupancy setup menu
    contextMenuSetup->insertItem("Setup &Occupancy", contextMenuSetupOccupancy);

    // By defaul MDI is NOT rebel.
    //contextMenu->setItemChecked(rebel_id, false);
}

/*
 * Shows the names.
 *
 * @return void.
 */
void
MDIResourceWindowFold::do_showNames()
{
    if(contextMenuSetupNames->isItemChecked(show_name_id) == true)
    {
        return;
    }
    contextMenuShow->setItemChecked(show_name_id, true);
    contextMenuShow->setItemChecked(show_occupancy_id, false);
}

/*
 * Shows the occupancy.
 *
 * @return void.
 */
void
MDIResourceWindowFold::do_showOccupancy()
{
    if(contextMenuSetupNames->isItemChecked(show_occupancy_id) == true)
    {
        return;
    }
    contextMenuShow->setItemChecked(show_name_id, false);
    contextMenuShow->setItemChecked(show_occupancy_id, true);
}

/*
 * Shows the row number (for name behaviour).
 *
 * @return void.
 */
void
MDIResourceWindowFold::do_showRowNumber()
{
    bool state = contextMenuSetupNames->isItemChecked(setup_name_number_id);
    contextMenuSetupNames->setItemChecked(setup_name_number_id, !state);
    if(state)
    {
        // Clears the mask
        row_mask = row_mask & ~RV_ROW_DESC_NUM;
    }
    else
    {
        // Sets the mask
        row_mask = row_mask | RV_ROW_DESC_NUM;
    }
}

/*
 * Shows the row type (for name behaviour).
 *
 * @return void.
 */
void
MDIResourceWindowFold::do_showRowType()
{
    bool state = contextMenuSetupNames->isItemChecked(setup_name_type_id);
    contextMenuSetupNames->setItemChecked(setup_name_type_id, !state);
    if(state)
    {
        // Clears the mask
        row_mask = row_mask & ~RV_ROW_DESC_TYPE;
    }
    else
    {
        // Sets the mask
        row_mask = row_mask | RV_ROW_DESC_TYPE;
    }
}

/*
 * Shows the row name (for name behaviour).
 *
 * @return void.
 */
void
MDIResourceWindowFold::do_showRowName()
{
    bool state = contextMenuSetupNames->isItemChecked(setup_name_name_id);
    contextMenuSetupNames->setItemChecked(setup_name_name_id, !state);
    if(state)
    {
        // Clears the mask
        row_mask = row_mask & ~RV_ROW_DESC_NAME;
    }
    else
    {
        // Sets the mask
        row_mask = row_mask | RV_ROW_DESC_NAME;
    }
}

/*
 * Shows the row from (for name behaviour).
 *
 * @return void.
 */
void
MDIResourceWindowFold::do_showRowFrom()
{
    bool state = contextMenuSetupNames->isItemChecked(setup_name_from_id);
    contextMenuSetupNames->setItemChecked(setup_name_from_id, !state);
    if(state)
    {
        // Clears the mask
        row_mask = row_mask & ~RV_ROW_DESC_FROM;
    }
    else
    {
        // Sets the mask
        row_mask = row_mask | RV_ROW_DESC_FROM;
    }
}

/*
 * Shows the row to (for name behaviour).
 *
 * @return void.
 */
void
MDIResourceWindowFold::do_showRowTo()
{
    bool state = contextMenuSetupNames->isItemChecked(setup_name_to_id);
    contextMenuSetupNames->setItemChecked(setup_name_to_id, !state);
    if(state)
    {
        // Clears the mask
        row_mask = row_mask & ~RV_ROW_DESC_TO;
    }
    else
    {
        // Sets the mask
        row_mask = row_mask | RV_ROW_DESC_TO;
    }
}

/*
 * Shows the row lane (for name behaviour).
 *
 * @return void.
 */
void
MDIResourceWindowFold::do_showRowLane()
{
    bool state = contextMenuSetupNames->isItemChecked(setup_name_lane_id);
    contextMenuSetupNames->setItemChecked(setup_name_lane_id, !state);
    if(state)
    {
        // Clears the mask
        row_mask = row_mask & ~RV_ROW_DESC_LANE;
    }
    else
    {
        // Sets the mask
        row_mask = row_mask | RV_ROW_DESC_LANE;
    }
}

/*
 * Shows the row level (for name behaviour).
 *
 * @return void.
 */
void
MDIResourceWindowFold::do_showRowLevel()
{
    bool state = contextMenuSetupNames->isItemChecked(setup_name_level_id);
    contextMenuSetupNames->setItemChecked(setup_name_level_id, !state);
    if(state)
    {
        // Clears the mask
        row_mask = row_mask & ~RV_ROW_DESC_LEVEL;
    }
    else
    {
        // Sets the mask
        row_mask = row_mask | RV_ROW_DESC_LEVEL;
    }
}

/*
 * Copyright (C) 2006 Intel Corporation
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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/DAvtView.h"
#include "dDB/DralDB.h"
#include "dockwin/disassembly/DisDockWinFloorplan.h"
#include "mdi/MDIFloorplanWindow.h"
#include "DreamsDB.h"

/*
 * Creator of the class. Forwards the call to its super class.
 *
 * @return new object.
 */
DisDockWinFloorplan::DisDockWinFloorplan(DreamsDB * dreamsdb, QMainWindow * mdi, QMainWindow * mw, DAvtView * asv)
    : DisDockWin(dreamsdb, mdi, mw, asv)
{
}

void
DisDockWinFloorplan::doSync(bool adding, bool moving)
{
    bool searchOk=true;

    QValueList<DisEntry>* itemList = currentTab->getItemList();
    INT32 selRow = currentTab->getSelected();
    if(selRow >= (INT32)itemList->count())
    {
        return;
    }

    CYCLE tgwhen;
    TagValueType tgvtype;
    TagValueBase tgbase;
    UINT64 tagValue;

    ItemHandler hnd (((*itemList)[selRow]).handler);
    Q_ASSERT(hnd.isValidItemHandler());

    hnd.rewindToFirstTag();
    hnd.nextTag();
    Q_ASSERT(hnd.getTagId() == draldb->getNewItemTagId());

    CYCLE foundCycle = hnd.getTagCycle();

    INT32 foundX = 0;
    INT32 foundY = 0;
    if(!dreamsdb->layoutFloorplanGetFirstCellAppearenceWithTagValue(draldb->getItemIdTagId(), hnd.getItemId(), foundCycle, foundX, foundY))
    {
        QMessageBox::critical(this, "Scanning Error",
        "Unable to find any Event related with this ItemID",
        QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }

    QString currentTagSelector = myAvt->getCurrentTagSelector();
    if(!hnd.getTagByName(currentTagSelector, &tagValue, &tgvtype, &tgbase, &tgwhen, foundCycle))
    {
        QMessageBox::warning (NULL,"Scanning error","Unable to find TAG "+currentTagSelector+
        " on ItemID "+QString::number(hnd.getItemId())+".",
        QMessageBox::Ok ,QMessageBox::NoButton);
        return;
    }
    else
    {
        TAG_ID tagId = hnd.getTagId();
        (myAvt->getHighLightMgr())->contentsSelectEvent(tagId, tagValue, true, HLT_DEFAULT, true, true);

        // go there!
        // Show cycle also repaints
        static_cast<MDIFloorplanWindow *>(myAvt->getMDI())->showCycle(foundCycle);
        myAvt->goToPoint(foundX, foundY);
        myAvt->showCycle(foundCycle);

        // synch other tabs
        if (prefMgr->getKeepSynchTabs())
        {
            DisDockWin::alignWith(hnd);
        }
    }
}

void
DisDockWinFloorplan::alignWith(INT32 col, INT32 row, CYCLE cycle)
{
    TRACK_ID trackId = dreamsdb->layoutFloorplanGetTrackIdForCell(col, row);
    if(trackId == TRACK_ID_INVALID)
    {
        return;
    }

    ItemHandler hnd;
    UINT16 clockId = draldb->getClockId(trackId);

    if(!draldb->getItemInSlot(trackId, cycle, &hnd))
    {
        return;
    }

    DisDockWin::alignWith(hnd);
}



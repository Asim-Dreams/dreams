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
#include "avt/ResourceDAvtView.h"
#include "dockwin/disassembly/DisDockWinResource.h"

// Qt includes.
#include <qpopupmenu.h>

/*
 * Creator of the class.
 *
 * @return the new object.
 */
DisDockWinResource::DisDockWinResource(DreamsDB * dreamsdb, LayoutResourceCache * _cache, QMainWindow * mdi, QMainWindow * mw, DAvtView * asv)
    : DisDockWin(dreamsdb, mdi, mw, asv)
{
    cache = _cache;
}


/*
 * Destroys the object.
 *
 * @return void.
 */
DisDockWinResource::~DisDockWinResource()
{
}

void
DisDockWinResource::doSync(bool adding, bool moving)
{
    bool searchOk=true;

    QValueList<DisEntry>* itemList = currentTab->getItemList();
    INT32 selRow = currentTab->getSelected();

    if(selRow >= (INT32) itemList->count())
    {
        return;
    }

    CYCLE cycle;
    UINT32 row = (UINT32) -1;

    ItemHandler hnd (((*itemList)[selRow]).handler);
    Q_ASSERT(hnd.isValidItemHandler());
    INT32 itemId = hnd.getItemId();

    // Highlights the selection.
    (myAvt->getHighLightMgr())->contentsSelectEvent(&hnd, adding, myAvt->getCurrentTagSelector(), HLT_DEFAULT, true, draldb->getLastCycle());

    if(moving)
    {
        searchOk = dreamsdb->layoutResourceFindItemFirstAppearence(itemId, draldb->getFirstCycle(), row, cycle);

        if(!searchOk)
        {
            QMessageBox::critical(this, "Scanning Error", "Unable to find any Event related with this ItemID", QMessageBox::Ok,QMessageBox::NoButton, QMessageBox::NoButton);
        }
        else
        {
            CLOCK_ID clockId = dreamsdb->layoutResourceGetRowClockId(row, cache);
            CYCLE temp;

            // Gets the cycles in the fastest clock domain.
            temp.clock = * draldb->getClockDomainFreqIterator();
            temp.fromLCMCycles(cycle.toLCMCycles());
            temp.cycle -= draldb->getFirstEffectiveCycle(temp.clock->getId()).cycle; 

            // Use it to go to the correct column.
            static_cast<ResourceDAvtView *>(myAvt)->goToColumn(temp.cycle);
            static_cast<ResourceDAvtView *>(myAvt)->goToRow(row);

            cycle.cycle -= draldb->getFirstEffectiveCycle(clockId).cycle;
            ((ResourceDAvtView *) myAvt)->shadeColumn(cycle, true);
            cache->getFoldWidget()->repaint();

            // synch other tabs
            if(prefMgr->getKeepSynchTabs())
            {
                DisDockWin::alignWith(hnd);
            }
        }
    }

    myAvt->repaintContents(false);
}

void
DisDockWinResource::alignWith(INT32 col, INT32 row, CYCLE cycle)
{
    if(dreamsdb->layoutResourceGetRowType(row, cache) != SimpleEdgeRow)
    {
        return;
    }

    // 1) get Item Id at this col/row
    ItemHandler hnd;

    dreamsdb->layoutResourceGetRowItemInside(row, cycle, &hnd, cache);

    if(!hnd.isValidItemHandler())
    {
        //printf("neg. heap index in col=%d,row=%d\n",col,row);
        return;
    }

    DisDockWin::alignWith(hnd);
}

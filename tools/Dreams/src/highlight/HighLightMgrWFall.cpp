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
 * @file HighLightMgrWFall.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "DreamsDB.h"
#include "HighLightMgrWFall.h"

// Qt includes.
#include <qmessagebox.h>

bool
HighLightMgrWFall::contentsSelectEvent(INT32 col, INT32 row, CYCLE cycle, bool adding, QString currentTagSelector, HighLightType htype, bool noErase)
{
    currentCycle = cycle;

    // tag heap entry for clicked event:
    ItemHandler hnd;
    ITEM_ID itemid = dreamsdb->layoutWaterfallGetItemIdForRow(row);
    draldb->lookForItemId(&hnd, itemid);
    Q_ASSERT(hnd.isValidItemHandler());

    /*// check if <col,row> is already highlighted
    INT32 stackIdx = getMatchingCriteria(&hnd);

    if(stackIdx >= 0)
    {
        if(noErase)
        {
            return false;
        }
        else
        {
             // in this case we un-highlight such criteria
            purgeLayer(stackIdx);
            return true;
        }
    }*/

    return HighLightMgr::contentsSelectEvent(&hnd, adding, currentTagSelector, htype, noErase, currentCycle);
}

bool 
HighLightMgrWFall::getHighlightColor(ItemHandler * hnd, CYCLE cycle, QPen & penHighlight)
{
    HLCriteriaStack::Iterator iter = criteriaStack->begin();
    HLCriteriaStack::Iterator iter_end = criteriaStack->end();
    UINT64 tgvalue;
    CYCLE tgwhen;
    TagValueType tgvtype;
    TagValueBase tgbase;

    // This function should be called only if at least one criteria
    // is set.
    bool bOk = hnd->getTagById((* iter).tagId, &tgvalue, &tgvtype, &tgbase, &tgwhen, cycle);

    if(bOk)
    {
        do
        {
            if((* iter).tagValue == tgvalue)
            {
                penHighlight = hpens[(INT32) (* iter).type - 1];
                return true;
            }

            iter++;
        }
        while(iter != iter_end);
    }

    return false;
}

INT32 
HighLightMgrWFall::getMatchingCriteria(ItemHandler *hnd)
{
    INT32 nPosition = 0;
    bool bFound = false;
    HLCriteriaStack::Iterator iter = criteriaStack->begin();
    HLCriteriaStack::Iterator iter_end = criteriaStack->end();
    UINT64 tgvalue;
    CYCLE tgwhen;
    TagValueType tgvtype;
    TagValueBase tgbase;
    bool bOk = hnd->getTagById((*iter).tagId, &tgvalue, &tgvtype, &tgbase, &tgwhen, currentCycle);

    if(bOk)
    {
        while(!bFound && (iter != iter_end))
        {
            bFound = ((*iter).tagValue == tgvalue);
            nPosition++;
            iter++;
        }
    }

    nPosition = bFound ? nPosition - 1 : -1;
    return nPosition;
}

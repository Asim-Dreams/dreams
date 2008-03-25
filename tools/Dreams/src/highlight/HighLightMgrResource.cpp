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

/**
 * @file HighLightMgrResource.cpp
 */

// General includes.
#include <iostream>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "DreamsDB.h"
#include "HighLightMgrResource.h"
#include "DrawingTools.h"

// Qt includes.
#include <qmessagebox.h>

// Asim includes.
#include "asim/AScrollView.h"

void 
HighLightMgrResource::drawHighlights(QPainter* p, int x1, int x2, int y1, int y2, bool visible)
{
    // divide by element-size...
    x1 = (int) floor((double) x1 / (double) cube_side);
    x2 = (int) ceil((double) x2 / (double) cube_side);
    y1 = (int) floor((double) y1 / (double) cube_side);
    y2 = (int) ceil((double) y2 / (double) cube_side);

    // iterate through criteria list
    HLCriteriaStack::iterator it;
    for(it = criteriaStack->begin(); it != criteriaStack->end(); ++it)
    {
        HighLightCriteria criteria = *it;
        HLightList * eventList = criteria.eventList;

        // iterate through the highlight list
        HLightListIterator hl_iter;
        for(hl_iter = eventList->begin(); hl_iter != eventList->end(); ++hl_iter)
        {
            // unpack the stored data
            HighLightListItem item;
            INT32 col;
            INT32 row;
            INT32 first_cycle;
            HighLightType htype;
            const ClockDomainEntry * clock;

            item = * hl_iter;
            row = item.row;
            htype = item.highlight_type;
            clock = item.cycle.clock;
            first_cycle = draldb->getFirstEffectiveCycle(clock->getId()).cycle;

            if(dreamsdb->layoutResourceGetUsesDivisions())
            {
                col = (INT32) ((item.cycle.cycle - first_cycle) * clock->getFastestRatio() * 2.0f);
            }
            else
            {
                col = (INT32) ((item.cycle.cycle - first_cycle) * clock->getFastestRatio());
            }

            if(isInner(x1, x2, y1, y2, col, row))
            {
                if(dreamsdb->layoutResourceGetValidData(row, item.cycle, cache))
                {
                    ColorShapeItem csh = dreamsdb->layoutResourceGetColorShapeLetter(row, item.cycle, cache);
                    float size_ratio;

                    if(dreamsdb->layoutResourceGetUsesDivisions())
                    {
                        // We are using divisions, so the base column is multiplied by two.
                        col = (item.cycle.cycle - first_cycle) * 2;
                        if(dreamsdb->layoutResourceGetRowShowExpanded(row, cache))
                        {
                            // If the row is expanded, we show it doubled.
                            size_ratio = clock->getFastestRatio() * 2.0f;
                        }
                        else
                        {
                            // Normal size.
                            size_ratio = clock->getFastestRatio();
                            // If is in the low division, move 1 cube.
                            if(item.cycle.division == 1)
                            {
                                col++;
                            }
                        }
                    }
                    else
                    {
                        // Normal show.
                        col = item.cycle.cycle - first_cycle;
                        size_ratio = clock->getFastestRatio();
                    }

                    if(visible)
                    {
                        p->setBrush(QBrush(AColorPalette::getColor(csh.getColorIdx())));
                        selectPen(p, htype);
                        //DrawingTools::drawShape(col * cube_side, row * cube_side, p, csh.getShape(), cube_side, cube_side);
                        DrawingTools::drawShape((INT32) ((float) (col * cube_side) * clock->getFastestRatio()), row * cube_side, p, 
                                csh.getShape(), QChar(csh.getLetter()), AColorPalette::getColor(csh.getLetterColorIdx()),
                                (INT32) ((float) cube_side * size_ratio), cube_side, asv);
                    }
                    else
                    {
                        p->eraseRect((INT32) ((float) (col * cube_side) * clock->getFastestRatio()), row * cube_side, (INT32) ((float) cube_side * size_ratio), cube_side);
                    }
                }
            }
        }        // inner loop
    }            // outer loop
}

bool
HighLightMgrResource::contentsSelectEvent(INT32 col, INT32 row, CYCLE cycle, bool adding, QString currentTagSelector, HighLightType htype, bool noErase)
{
    /// @TODO review this
    if(dreamsdb->layoutResourceGetRowType(row, cache) != SimpleEdgeRow)
    {
        return false;
    }

    // Checks that something is in this cycle in this row.
    if(!dreamsdb->layoutResourceGetValidData(row, cycle, cache))
    {
        return false;
    }


    /*// Check if <col,row> is already highlighted.
    int stackIdx = lookForEvent(cycle, row);

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

    // Tag heap entry for clicked event:
    ItemHandler hnd;
    TRACK_ID trackId;
    CLOCK_ID clockId;

    trackId = dreamsdb->layoutResourceGetRowTrackId(row, cache);
    clockId = draldb->getClockId(trackId);

    dreamsdb->layoutResourceGetRowItemInside(row, cycle, &hnd, cache);

    return HighLightMgr::contentsSelectEvent(&hnd, adding, currentTagSelector, htype, noErase, cycle);
}

void  
HighLightMgrResource::updateViewRange(int x1, int x2, int y1, int y2)
{
    if(criteriaStack->count() <= 0)
    {
        return;
    }

    y2 = QMIN(y2, (INT32) cache->getNumRows() - 1);

    //printf ("updateViewRange over: (%d,%d)->(%d,%d) \n",x1,y1,x2,y2);

    bool c1 = (last_x1 > x1);
    bool c2 = (last_x2 < x2);
    bool c3 = (last_y1 > y1);
    bool c4 = (last_y2 < y2);
    if(!(c1 || c2 || c3 || c4))
    {
        return;
    }

    last_x1 = x1;
    last_x2 = x2;
    last_y1 = y1;
    last_y2 = y2;

    int i, j;

    // The logic cycles are divided by 2.
    if(dreamsdb->layoutResourceGetUsesDivisions())
    {
        x1 = x1 >> 1;
        x2 = x2 >> 1;
    }

    // iterate through criteria list
    HLCriteriaStack::iterator it;
    for(it = criteriaStack->begin(); it != criteriaStack->end(); ++it)
    {
        HighLightCriteria criteria = * it;
        criteria.eventList->clear();

        for(j = y1; j <= y2; j++)
        {
            UINT16 clockId;
            const ClockDomainEntry * clock;
            CYCLE begin;
            CYCLE end;
            TRACK_ID trackId;

            clockId = dreamsdb->layoutResourceGetRowClockId(j, cache);
            trackId = dreamsdb->layoutResourceGetRowTrackId(j, cache);

            // Only updates state for usefull rows.
            if(trackId == TRACK_ID_INVALID)
            {
                continue;
            }

            clock = draldb->getClockDomain(clockId);

            begin = draldb->getFirstEffectiveCycle(clockId);
            begin.cycle += (INT32) floorf((float) x1 / clock->getFastestRatio());
            end = draldb->getFirstEffectiveCycle(clockId);
            end.cycle += (INT32) ceilf((float) x2 / clock->getFastestRatio());

            // Updates the items along all the view.
            while(begin.cycle <= end.cycle)
            {
                // We want to find if in the current <row, col> we have the value associated
                ItemHandler hnd;
                if(dreamsdb->layoutResourceGetRowShowHigh(j, cache))
                {
                    begin.division = 0;
                    dreamsdb->layoutResourceGetRowItemInside(j, begin, &hnd, cache);

                    if(hnd.isValidItemHandler())
                    {
                        UINT64 tgValue;
                        CYCLE tgWhen;
                        TagValueType tgvType;
                        TagValueBase tgBase;

                        bool bTagFound = hnd.getTagById(criteria.tagId, &tgValue, &tgvType, &tgBase, &tgWhen, begin);
                        if(bTagFound && (criteria.tagValue == tgValue))
                        {
                            addHighLight(begin, j, criteria.type, criteria.eventList, &hnd);
                        }
                    }
                }
                
                if(dreamsdb->layoutResourceGetRowShowLow(j, cache))
                {
                    begin.division = 1;
                    dreamsdb->layoutResourceGetRowItemInside(j, begin, &hnd, cache);

                    if(hnd.isValidItemHandler())
                    {
                        UINT64 tgValue;
                        CYCLE tgWhen;
                        TagValueType tgvType;
                        TagValueBase tgBase;

                        bool bTagFound = hnd.getTagById(criteria.tagId, &tgValue, &tgvType, &tgBase, &tgWhen, begin);
                        if(bTagFound && (criteria.tagValue == tgValue))
                        {
                            addHighLight(begin, j, criteria.type, criteria.eventList, &hnd);
                        }
                    }
                }
                begin.cycle++;
            }
        }
    }
}

// ==================================================
// Copyright (C) 2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

/**
  * @file HighLightMgrFloorplan.h
  */

#ifndef _HIGHLIGHTFLOORPLAN_H
#define _HIGHLIGHTFLOORPLAN_H

// Dreams includes.
#include "DreamsDefs.h"
#include "HighLightMgr.h"
#include "dDB/DralDB.h"

/**
 * This class wraps all the functionality needed to manage highlighting and
 * highlighting criteria stack for the Floorplan view.
 *
 * @version 0.1
 * @date started at 2004-06-30
 * @author Santi Galan
 */
class HighLightMgrFloorplan : public HighLightMgr
{
    Q_OBJECT

  private:
    CYCLE currentCycle;

  public:  
    HighLightMgrFloorplan(DreamsDB * dreamsdb, INT32 cside)
        : HighLightMgr(dreamsdb, cside)
    {
    }

    HighLightMgrFloorplan(HighLightMgr * parent)
        : HighLightMgr(parent)
    {
    }
    
    bool contentsSelectEvent(INT32 col, INT32 row, CYCLE cycle, bool adding, QString currentTagSelector, HighLightType htype, bool noErase = false);

    bool getHighlightColor(ItemHandler * hnd, CYCLE cycle, QPen & penHighlight);
    INT32 getMatchingCriteria(ItemHandler * hnd);

  private:
};

#endif /* _HIGHLIGHTFLOORPLAN_H */

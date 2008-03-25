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
  * @file FindDialogImplFloorplan.h
  */

#ifndef __FINDDIALOGIMPLFLOORPLAN_H__
#define __FINDDIALOGIMPLFLOORPLAN_H__

#include "DreamsDefs.h"
#include "dialogs/find/FindDialogImpl.h"
#include "layout/floorplan/LayoutFloorplanFind.h"

// Class forwarding.
class FloorplanDAvtView;

/**
  * Short desc.
  * Long  desc.
  *
  * @author Santiago Galan
  * @date started at 2004-07-14
  * @version 0.1
  */
class FindDialogImplFloorplan : public FindDialogImpl 
{
    public:
        FindDialogImplFloorplan(DreamsDB * dreamsdb, FloorplanDAvtView * avt, QWidget * parent = 0, const char * name = 0, bool modal = FALSE, WFlags fl = 0);

    protected:
        //void doFind(QString str, TAG_ID tagId, TagValueType tgtype, CYCLE startCycle, INT32 startRow, bool cs, bool em, bool re);
        void doFind(QString str, TAG_ID tagId, TagValueType tgtype, CYCLE startCycle, bool resetSearch = false);

    protected:
        LayoutFloorplanFindDataSet entries; ///< List of current entries in the search.
        INT32 lastFoundCellX;
        INT32 lastFoundCellY;
} ;

#endif /* __FINDDIALOGIMPLFLOORPLAN_H__ */

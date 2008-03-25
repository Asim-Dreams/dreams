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
  * @file FindDialogImplResource.h
  */

#ifndef __FINDDIALOGIMPLRESOURCE_H__
#define __FINDDIALOGIMPLRESOURCE_H__

// Dreams includes.
#include "dialogs/find/FindDialogImpl.h"
#include "layout/resource/LayoutResourceFind.h"
#include "DreamsDefs.h"

// Class forwarding.
class ResourceDAvtView;
class LayoutResourceCache;

/*
 * Short desc.
 * Long  desc.
 *
 * @author Santiago Galan
 * @date started at 2004-07-14
 * @version 0.1
 */
class FindDialogImplResource : public FindDialogImpl 
{
    public:
        FindDialogImplResource(DreamsDB * dreamsdb, LayoutResourceCache * _cache, ResourceDAvtView * avt, QWidget * parent = 0, const char * name = 0, bool modal = FALSE, WFlags fl = 0);

    protected:
        void doFind(QString str, TAG_ID tagId, TagValueType tgtype, CYCLE startCycle, bool resetSearch = false);
        void found(LayoutResourceFindDataEntry * entry);

    protected:
        LayoutResourceFindDataSet entries; ///< List of current entries in the search.
        LayoutResourceCache * cache;       ///< Cache of the 2D view.
        UINT32 row;                  ///< Row where the event is found.
        bool eventFound;             ///< True when a match is found.
} ;

#endif // __FINDDIALOGIMPLRESOURCE_H__

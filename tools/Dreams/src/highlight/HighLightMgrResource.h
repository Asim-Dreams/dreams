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
  * @file HighLightMgrResource.h
  */

#ifndef _HIGHLIGHTRESOURCE_H
#define _HIGHLIGHTRESOURCE_H

// Dreams includes.
#include "DreamsDefs.h"
#include "HighLightMgr.h"

// Class forwarding.
class AScrollView;
class LayoutResourceCache;

/**
 * This class wraps all the functionality needed to manage highlighting and
 * highlighting criteria stack for the resource view.
 *
 * @version 0.1
 * @date started at 2004-06-30
 * @author Santi Galan
 */
class HighLightMgrResource : public HighLightMgr
{
  Q_OBJECT

  public:
    HighLightMgrResource(DreamsDB * dreamsdb, LayoutResourceCache * _cache, INT32 cside, AScrollView * _asv) : HighLightMgr(dreamsdb, cside)
    {
        asv = _asv;
        cache = _cache;
    }
    
    void updateViewRange(int x1, int x2, int y1, int y2);
    void drawHighlights(QPainter * p, int x1, int x2, int y1, int y2, bool visible);
    bool contentsSelectEvent(INT32 col, INT32 row, CYCLE cycle, bool adding, QString currentTagSelector, HighLightType htype, bool noErase = false);

  private:
    AScrollView * asv;
    LayoutResourceCache * cache;
} ;

#endif /* _HIGHLIGHTRESOURCE_H */

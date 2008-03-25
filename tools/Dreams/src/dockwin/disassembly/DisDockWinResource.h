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
  * @file DisDockWinResource.h
  */

#ifndef _DISDOCKWINRESOURCE_H
#define _DISDOCKWINRESOURCE_H

// Dreams includes.
#include "DreamsDefs.h"
#include "DisDockWin.h"

// Class forwarding.
class LayoutResourceCache;
class QPopupMenu;

/**
 * Docked window to display disassembly tags of visible events (Item id range).
 * Specific implementation for Resource view.
 * Put long explanation here
 * @version 0.1
 * @date started at 2004-07-08
 * @author Santi Galan
 */
class DisDockWinResource : public DisDockWin 
{
    public:
        DisDockWinResource(DreamsDB * dreamsdb, LayoutResourceCache * _cache, QMainWindow * mdi, QMainWindow * mw, DAvtView * asv);
        ~DisDockWinResource();

        void alignWith(INT32 col, INT32 row, CYCLE cycle);
        void doSync(bool adding, bool moving);

    private:
        LayoutResourceCache * cache; ///< Fold state of the associated resource view.
} ;

#endif /* _DISDOCKWINRESOURCE_H */

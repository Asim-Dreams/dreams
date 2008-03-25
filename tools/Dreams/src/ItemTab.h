// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
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
  * @file ItemTab.h
  */

#ifndef _ITEMTAB_H
#define _ITEMTAB_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "DreamsDefs.h"

// Qt includes.
#include <qvaluevector.h>
#include <qptrlist.h>

// Class forwarding.
class TabSpec;
class DreamsDB;

/**
  * This class holds the item selections for tabs according the the parsed data in TabSpec 
  *
  * @author Federico Ardanaz
  * @date started at 2004-11-03
  * @version 0.1
  */
class ItemTab 
{
    public:
        ItemTab(TabSpec* tspec);
        ~ItemTab();
        void reset();

        inline QValueVector<INT32>* getItemList() { return itemList; }
        inline TabSpec* getTabSpec() { return tabSpec; }

        static void computeItemLists(DreamsDB * dreamsdb);
    private:
        QValueVector<INT32>* itemList;
        TabSpec*             tabSpec;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

typedef QPtrList<ItemTab> ItemTabList;

#endif

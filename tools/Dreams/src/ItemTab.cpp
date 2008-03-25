/*
 * Copyright (C) 2004-2006 Intel Corporation
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
  * @file ItemTab.cpp
  */

// General includes.
#include <iostream>
#include <map>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "ItemTab.h"
#include "TabSpec.h"
#include "DreamsDB.h"

ItemTab::ItemTab(TabSpec* tspec)
{
    //cerr << "ItemTab created for tspec " << tspec->getName() << endl;
    itemList = new QValueVector<INT32>();
    tabSpec=tspec;
}

ItemTab::~ItemTab()
{
    delete itemList;
}
        
void 
ItemTab::reset()
{
}

void
ItemTab::computeItemLists(DreamsDB * dreamsdb)
{
    DralDB * draldb = dreamsdb->getDralDB();

    //cerr << "computeItemLists called" << endl;

    // empty db?
    if(dreamsdb->getNumCycles(0).cycle <= 0)
    {
        //printf("Empty db!, giving up\n");fflush(stdout);
        return;
    }

    TabSpecList *tabspeclist = dreamsdb->getTabSpecList();
    ItemTabList *itablist = dreamsdb->getItemTabList();

    if(tabspeclist->isEmpty())
    {
        return;
    }

    ItemHandler hnd;
    draldb->getFirstItem(&hnd);

    UINT64 tgvalue = 0;
    INT32 tabcount = (INT32) tabspeclist->count();
    bool selected;
    multimap<UINT64, ITEM_ID> * sort_maps;
    
    // build ItemTab objects
    for(INT32 tab = 0; tab < tabcount; tab++)
    {
        TabSpec * tspec = tabspeclist->at(tab);
        ItemTab * itab = new ItemTab(tspec);
        itablist->append(itab);
    }

    sort_maps = new multimap<UINT64, ITEM_ID>[tabcount];

    // WARNING do not change loop order, this way we scan tag Heap only once and improve cache performance.
    while(hnd.isValidItemHandler())
    {
        // loop over the tabs
        for(INT32 tab = 0; tab < tabcount; tab++)
        {
            selected=true;

            TabSpec* tspec = tabspeclist->at(tab);
            ItemTab* itab = itablist->at(tab);

            // check withtag
            if(tspec->getWithTagId() != TAG_ID_INVALID)
            {
                selected = selected && hnd.hasTag(tspec->getWithTagId());
            }

            // check select/split cases
            if(tspec->getSelectTagId() != TAG_ID_INVALID)
            {
                UINT64 matchingValue=0;
                if(tspec->isSplit())
                {
                    //matchingValue = tspec->getSplitByValue();
                }
                else
                {
                    matchingValue = tspec->getMatchingValue();
                }
                bool hasTag = hnd.hasTag(tspec->getSelectTagId());
                if(hasTag)
                {
                    tgvalue = hnd.getTagValue();
                }
                selected = selected && hasTag && (matchingValue == tgvalue);
            }

            if(selected)
            {
                if(tspec->getSortById() != TAG_ID_INVALID)
                {
                    // If a sort is used, sort the item using the map.
                    UINT64 matchingValue = (UINT64) -1;
                    if(hnd.hasTag(tspec->getSortById()))
                    {
                        matchingValue = hnd.getTagValue();
                    }
                    pair<UINT64, ITEM_ID> p;
                    p.first = matchingValue;
                    p.second = hnd.getItemId();
                    sort_maps[tab].insert(p);
                }
                else
                {
                    // Else stores it directly to the item list.
                    itab->getItemList()->append(hnd.getItemId());
                }
            }
        }

        // step until the next "Item Id"
        hnd.nextItem();
    }

    // Gets the tabs with order.
    for(INT32 tab = 0; tab < tabcount; tab++)
    {
        TabSpec* tspec = tabspeclist->at(tab);
        ItemTab* itab = itablist->at(tab);

        // If the tab has a sort.
        if(tspec->getSortById() != TAG_ID_INVALID)
        {
            multimap<UINT64, ITEM_ID>::iterator it;
            it = sort_maps[tab].begin();

            // TODO: add a new specifier in the item window to choose
            // if we want separators between different values.
            if(true)
            {
                ITEM_ID lastId = it->first;
                // Iterates through all the entries.
                while(it != sort_maps[tab].end())
                {
                    // When the value changes, adds a separator.
                    if(it->first != lastId)
                    {
                        itab->getItemList()->append(ITEM_ID_INVALID);
                        lastId = it->first;
                    }
                    itab->getItemList()->append(it->second);
                    ++it;
                }
            }
            else
            {
                // Iterates through all the entries.
                while(it != sort_maps[tab].end())
                {
                    itab->getItemList()->append(it->second);
                    ++it;
                }
            }
        }
    }

    delete [] sort_maps;

    return;
}

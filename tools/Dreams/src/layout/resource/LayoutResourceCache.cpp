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
 * @file LayoutResourceCache.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "layout/resource/LayoutResourceCache.h"
#include "DreamsDB.h"

/*
 * Creator of the class. Allocates space for the maximum possible size
 * and creates a cache with all the groups expanded.
 *
 * @return new object.
 */
LayoutResourceCache::LayoutResourceCache(UINT32 _numRows, UINT32 numGroups)
{
    numRows = _numRows;
    // Allocates space for the maximum size. Just one allocation all the layout lifetime.
    virToPhy = new LayoutResourceMapEntry[numRows];
    phyToVir = new UINT32[numRows];
    foldCache = new ResourceViewFoldEnum[numRows];
    groupExpansion = new bool[numGroups];
    numVirtualRows = 0;

    // All are expanded by default.
    for(UINT32 i = 0; i < numGroups; i++)
    {
        groupExpansion[i] = true;
    }
}

/*
 * Destructor of the class. Deletes the allocated cache.
 *
 * @return destroys the object.
 */
LayoutResourceCache::~LayoutResourceCache()
{
    delete [] virToPhy;
    delete [] phyToVir;
    delete [] foldCache;
    delete [] groupExpansion;
}

/*
 * Creator of the class.
 *
 * @return new object.
 */
LayoutResourceMapEntry::LayoutResourceMapEntry()
{
}

/*
 * Destructor of the class.
 *
 * @return new object.
 */
LayoutResourceMapEntry::~LayoutResourceMapEntry()
{
}

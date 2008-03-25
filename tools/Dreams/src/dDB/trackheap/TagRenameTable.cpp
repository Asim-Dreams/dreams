// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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
  * @file TagRenameTable.cpp
  */

// Dreams includes.
#include "dDB/trackheap/TagRenameTable.h"

/*
 * Creator of the class. Allocates space for the renaming.
 *
 * @return the new object.
 */
TagRenameTable::TagRenameTable(INT32 numTags)
{
    renaming = (TAG_ID *) malloc(numTags * sizeof(TAG_ID));
    limit = numTags;
    nextIndex = 0;
}

/*
 * Destructor of the class. Frees the requested space.
 *
 * @return delete the object.
 */
TagRenameTable::~TagRenameTable()
{
    free(renaming);
}

/*
 * Renames the given tag. If the rename was already done the old
 * rename is returned. Otherwise the tag is renamed with the next
 * available index.
 *
 * @return the tag renaming.
 */
TAG_ID
TagRenameTable::rename(TAG_ID index)
{
    // Tries to find the index along the entries.
    for(TAG_ID i = 0; i < nextIndex; i++)
    {
        if(renaming[i] == index)
        {
            // If found, returns the value.
            return i;
        }
    }

    // We need to allocate a new entry for the index.
    if(nextIndex == limit)
    {
        // No more space, so we alloc more.
        limit = limit + QMAX(1, limit >> 2);
        renaming = (TAG_ID *) realloc(renaming, limit * sizeof(TAG_ID));
    }

    // We still have space in the vector, so just append it.
    renaming[nextIndex] = index;
    return nextIndex++;
}

/*
 * This function returns if the tag index is already renamed.
 * The state of the renaming is not changed.
 *
 * @return if the index tag is renamed.
 */
bool
TagRenameTable::isRenamed(TAG_ID index) const
{
    // Tries to find the index along the entries.
    for(TAG_ID i = 0; i < nextIndex; i++)
    {
        if(renaming[i] == index)
        {
            return true;
        }
    }

    return false;
}

/*
 * Gets a set of the renamed tags in the actual rename table.
 *
 * @return the renamed tags.
 */
set<TAG_ID>
TagRenameTable::getRenamedTags() const
{
    set<TAG_ID> ret;

    // Runs through all the tag list.
    for(TAG_ID i = 0; i < nextIndex; i++)
    {
        ret.insert(renaming[i]);
    }

    return ret;
}

/*
 * Gets the object size.
 *
 * @return the object size.
 */
INT64
TagRenameTable::getObjSize() const
{
    return (sizeof(TagRenameTable) + (limit * sizeof(TAG_ID)));
}

/*
 * Returns the usage description of this object.
 *
 * @return the usage description.
 */
QString
TagRenameTable::getUsageDescription() const
{
    QString ret;

    ret = "\t\t\tTag rename table with " + QString::number(limit) + " entries. Total size is " + QString::number(getObjSize()) + "\n";
    return ret;
}

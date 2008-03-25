/*****************************************************************************
*
* @brief TagDictionary code.
*
* @author Oscar Rosell
*
*  Copyright (C) 2003-2006 Intel Corporation
*  
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*  
*
*****************************************************************************/

#include "tagDictionary.h"
#include <assert.h>

unsigned char TagDictionary::addEntry(TagDescriptor entry)
{
    return dictionary_.addEntry(entry);
}

TagDescriptor TagDictionary::getEntry(unsigned char idx)
{
    return dictionary_.getEntry(idx);
}

unsigned char TagDictionary::getEntryIndex(const QString& str, bool* found)
{
    return dictionary_.getEntryIndex(TagDescriptor(str,TagDescriptor::NULL_TAG_TYPE), found);
}

unsigned char TagDictionary::getEntryIndex(const TagDescriptor& entry, bool* found)
{
    return dictionary_.getEntryIndex(entry, found);
}

MyDictionary<TagDescriptor,unsigned char> TagDictionary::dictionary_;


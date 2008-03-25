/*
 * Copyright (C) 2003-2006 Intel Corporation
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
  * @file
  * @brief
  */
  
#ifndef _TAG_ITEM_DEF_
#define _TAG_ITEM_DEF_

#include "asim/DralDB.h"

typedef struct
{
    // 64 bits
    UINT64 tagId    : 12;
    UINT64 isLast   :  1;
    UINT64 isMutable:  1;
    UINT64 isSOV    :  1;
    UINT64 dkey     : 20;
    UINT64 cycitm   : 29;
} TagItem2D;

/** @typdef TagAEVector
  * @brief
  */
typedef AEVector<TagItem2D,32000,2000> TagAEVector2D; // max ~ 32.10^6 entries


#endif


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
  * @file DRALTag.h
  */

#ifndef _DRALTAG_H
#define _DRALTAG_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/aux/FValueVector.h"
#include "dDB/aux/AEVector.h"

/*
 * @define TAG_ID
 *
 * Defines the type of data that represents a tag identifier.
 */
#define TAG_ID UINT16
#define TAG_ID_INVALID (TAG_ID) -1

/*
 * @define TRACK_ID
 *
 * Defines the type of data that represents a track identifier.
 */
#define TRACK_ID UINT32
#define TRACK_ID_INVALID (TRACK_ID) -1
#define TRACK_ID_LINEAR_NUM_DIM_ERROR (TRACK_ID) -2
#define TRACK_ID_LINEAR_OOB_DIM_ERROR (TRACK_ID) -3
#define TRACK_ID_FIRST_INVALID (TRACK_ID) -3

/*
 * @define DRAL_ID
 *
 * Defines the type of data that represents a dral identifier.
 */
#define DRAL_ID UINT16
#define DRAL_ID_INVALID (DRAL_ID) -1

/*
 * @define CLOCK_ID
 *
 * Defines the type of data that represents a clock identifier.
 */
#define CLOCK_ID UINT16
#define CLOCK_ID_INVALID (CLOCK_ID) -1

/*
 * @define ITEM_ID
 *
 * Defines the type of data that represents an item.
 */
#define ITEM_ID UINT32
#define ITEM_ID_INVALID (ITEM_ID) -1

/* @typedef TagIDList
 * @brief
 * Dynamic vector of tag id's.
 */
typedef AEVector<TAG_ID, 32, 128> TagIDList;

/*
 * @typedef enum TagSizeStorage
 *
 * Defines the different internal sizes of the tags.
 */
typedef enum
{
    TAG_SIZE_64_BITS, ///< Values of 64 bits will be stored using a 20 bit dictionary. Basically used to store integers and strings after being converted to an integer.
    TAG_SIZE_31_BITS, ///< Values of 31 bits will be stored. Basically used to store the items that move from one slot to another.
    TAG_SIZE_8_BITS   ///< Values of 8 bits will be stored.
} TagSizeStorage;

/*
 * @typedef enum TagVecEncodingType
 *
 * Defines the different types of tag vectors.
 */
typedef enum
{
    TVE_VALUE_31,           // for 31 bit values (moveitems, enter/exitnodes, ...)
    TVE_VALUE_31_DENSE,
    TVE_VALUE_31_SHORT,
    TVE_VALUE_64,            // for 64 bit values (node tags, ...)
    TVE_VALUE_64_DENSE,
    TVE_VALUE_64_NF,
    TVE_VALUE_64_NF_DENSE,
    TVE_VALUE_8,            // for 8 bit values (colors, ...)
    TVE_VALUE_8_DENSE,
    TVE_VALUE_8_NF,
    TVE_VALUE_8_NF_DENSE
} TagVecEncodingType;

/** @typdef SOVList
  * @brief
  * Set of values is a vector of unsigned integers.
  */
typedef FValueVector<UINT64> SOVList;

#endif

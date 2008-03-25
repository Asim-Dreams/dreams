// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file DefaultSize.h
  * @brief Default size for some fundamental structures
  */

#ifndef __DEFAULT_SIZES_H
#define __DEFAULT_SIZES_H

// -------------------------------------------------
// EventItem definitions
// -------------------------------------------------

#define EITEM_MAX_COLORSHAPE_ITEMS 256              ///< Number of ColorShape variants by row
#define EITEM_NON_INIT_COLORSHAPE  255              ///< ColorShape after creation (without init)
#define EITEM_MAX_COLORSHAPE_INDEX 254              ///< Maximum valid ColorShape value

#define EITEM_NON_INIT_EVENT_ITEM  8388607         ///< ElemItem after creation (without init)
#define EITEM_DELTA_OVERFLOW_ITEM  8388606         ///< ElemItem which was not diff. encoded
#define EITEM_MAX_EVENT_ITEM_DELTA 8388605         ///< Maximum valid itemIdDelta value

// -------------------------------------------------
// ColDescriptor definitions
// -------------------------------------------------
#define COLDESC_SZ_FLAG_VECTOR 496                 ///< 496 bytes means at most 3968 events per column
#define COLDESC_MAX_EVENTS     3968

// -------------------------------------------------
// TagDescVector definitions
// -------------------------------------------------
#define TAGDESCVEC_DEFAULT_SIZE 1000

// -------------------------------------------------
// StrTable definitions
// -------------------------------------------------
#define STRTBL_DEFAULT_SIZE     10000

// -------------------------------------------------
// AColorPalette definitions
// -------------------------------------------------
#define ACPALETTE_NUMCOLORS 65536

// -------------------------------------------------
// AColorPalette definitions
// -------------------------------------------------
#define LIVEDB_DEFAULT_MAXITEMCNT 100000

#endif



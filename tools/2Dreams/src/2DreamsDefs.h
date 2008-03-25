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
  * @file 2DreamsDefs.h
  * @brief Globally used definitions like version, ADF tags, etc.
  */

#ifndef _TWODREAMSDEFS_H
#define _TWODREAMSDEFS_H

// version and so on
#define DREAMS2_MAJOR_VERSION 2
#define DREAMS2_MINOR_VERSION 0
#define DREAMS2_BUILD_VERSION 0
#define DREAMS2_STR_MAJOR_VERSION "2"
#define DREAMS2_STR_MINOR_VERSION "0"
#define DREAMS2_STR_BUILD_VERSION "pre14"

#define HELP_INDEX "index.htm"
#define HELP_SERVER "http://bssad.intel.com"
#define HELP_REMOTE_PATH "/Software/2Dreams/help/"

// 2Dreams Core
/** @def used as a "canonical" item id string */
#define ITEMID_STR_TAG "ITEMID"
/** @def used as a "canonical" cycle id string */
#define CYCLEID_STR_TAG "CYCLEID"

#define AGEPURGE_STR_TAG "_AGEPURGED"
#define EOFPURGE_STR_TAG "_EOFPURGED"

#define CUBE_SIZE   8
#define CUBE_BORDER 1
#define CUBE_SIDE (CUBE_SIZE+CUBE_BORDER)

#define DREAMS2_SESSION_MAGIC_NUMBER 0xfedeecdf
#define DREAMS2_EMBEDDED_ADF_MAGIC_NUMBER 0x0adf

// for node tags
#define TGNODE_REFCOLDISTANCE 128
#define TGNODE_REFCOLDISTANCE_MASKOUT 0xff80U
#define TGNODE_REFCOLDISTANCE_MOD     0x007fU

#define TGNODE_MAXDELTA_TGID   255
#define TGNODE_NONDELTA_VALUE  8388607L
#define TGNODE_MAXDELTA_VALUE  8388606L
#define TGNODE_MINDELTA_VALUE -8388608L

#endif


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
  * @file DreamsDefs.h
  * @brief Globally used definitions like version, ADF tags, etc.
  */

#ifndef _TWODREAMSDEFS_H
#define _TWODREAMSDEFS_H

#include <assert.h>

// version and so on
#define DREAMS_MAJOR_VERSION 2
#define DREAMS_MINOR_VERSION 5
#define DREAMS_BUILD_VERSION 0
#define DREAMS_STR_MAJOR_VERSION "2"
#define DREAMS_STR_MINOR_VERSION "5"
#define DREAMS_STR_BUILD_VERSION "0"

#define HELP_INDEX "index.htm"
#define HELP_SERVER "http://bssad.intel.com"
#define HELP_REMOTE_PATH "/Software/2Dreams/help/"

// Dreams Core
/** @def used as a "canonical" item id string */
#define ITEMID_STR_TAG "ITEMID"

#define CUBE_SIZE   10
#define CUBE_BORDER 1
#define CUBE_SIDE (CUBE_SIZE+CUBE_BORDER)

#define CUBE_SIZE_BIG   15
#define CUBE_BORDER_BIG 1
#define CUBE_SIDE_BIG (CUBE_SIZE_BIG+CUBE_BORDER_BIG)

#define DREAMS2_SESSION_MAGIC_NUMBER      0xfedeecdf
#define DREAMS2_EMBEDDED_ADF_MAGIC_NUMBER 0x0adf
#define DREAMS2_SETENV_MAGIC_NUMBER 0x05e7

// Clocking base interval
#define BASE_INTERVAL 1000

// redefine the Q_ASSERT to produce an abort() so
// that you can debug with gdb
// this is only effective if this .h is included BEFORE
// the any QT .h and we have the QT debug mode  and fatal falgs on...
#if !defined(Q_ASSERT) && defined(QT_DEBUG) && defined(QT_FATAL_ASSERT) 
#      define Q_ASSERT(x) assert(x)
#endif


#endif

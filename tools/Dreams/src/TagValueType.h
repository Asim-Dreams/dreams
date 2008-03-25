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
  * @file TagValueType.h
  */

#ifndef _TAGVALUETYPE_H
#define _TAGVALUETYPE_H

// Qt includes.
#include <qstring.h>

/*
 * @brief
 * This is a small enumeration for the different types of values
 * a tag can hold on.
 *
 * @version 0.1
 * @date started at 2002-04-01
 * @author Federico Ardanaz
 */
typedef enum
{
    TAG_UNKNOWN_TYPE,
    TAG_STRING_TYPE,
    TAG_INTEGER_TYPE, ///< 64 bits UINT64
    TAG_FP_TYPE,      ///< 64 bits IEEE double
    TAG_SOV_TYPE      ///< list of values (INT64)
} TagValueType;

/*
 * @brief
 * This is a small enumeration for the different value bases.
 *
 * @version 0.1
 * @date started at 2004-12-01
 * @author Guillem Sole
 */
typedef enum
{
    TAG_BASE_BINARY,
    TAG_BASE_OCTAL,
    TAG_BASE_DECIMAL,
    TAG_BASE_HEXA
} TagValueBase;

/*
 * @typedef
 * @brief This is a small enumeration for the different types of values a tag can hold on.
 *
 * @version 0.1
 * @date started at 2002-04-01
 * @author Federico Ardanaz
 */
class TagValueToolkit
{
    public:
        static bool parseValueType(QString str, TagValueType* result);
        static QString toString(TagValueType value);
        static bool parseValueBase(QString str, TagValueBase * base);
};

#endif

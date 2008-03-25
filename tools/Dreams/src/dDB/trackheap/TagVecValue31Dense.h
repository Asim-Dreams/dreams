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
  * @file TagVecValue31Dense.h
  */

#ifndef _DRALDB_TAG_VEC_VALUE_31_DENSE_H
#define _DRALDB_TAG_VEC_VALUE_31_DENSE_H

// Dreams includes.
#include "dDB/trackheap/TagVecValue31.h"

/**
  * @brief
  * This class is a tag vector that stores 31 bits.
  *
  * @description
  * This implementation of the index tag vector uses 31 bits to
  * store the item id and 12 for the offset.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class TagVecValue31Dense : public TagVecValue31
{
    public:
        TagVecValue31Dense(TagVecValue31 * original);
        virtual ~TagVecValue31Dense();

        virtual void reset(INT32 bcycle);

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        inline bool addTagValue(INT32 cycle, UINT64 value);
        inline bool undefineTagValue(INT32 cycle);

        inline TagVecEncodingType getType();
        void dumpCycleVector();

        inline ZipObject * compressYourSelf(CYCLE cycle, bool last = false);
};

/*
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecValue31Dense::getType()
{
    return TVE_VALUE_31_DENSE;
}

/**
  * Object already compressed.
  *
  * @return itself.
  */
ZipObject *
TagVecValue31Dense::compressYourSelf(CYCLE cycle, bool last)
{
    return this;
}

/*
 * Adds a new tag in the vector.
 *
 * @return false.
 */
bool
TagVecValue31Dense::addTagValue(INT32 cycle, UINT64 value)
{
    return false;
}

/*
 * Adds a new tag in the vector.
 *
 * @return false.
 */
bool
TagVecValue31Dense::undefineTagValue(INT32 cycle)
{
    return false;
}

#endif

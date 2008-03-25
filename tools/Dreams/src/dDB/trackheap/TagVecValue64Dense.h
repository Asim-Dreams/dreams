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
  * @file TagVecValue64Dense.h
  */

#ifndef _DRALDB_TAG_VEC_VALUE_64_DENSE_H
#define _DRALDB_TAG_VEC_VALUE_64_DENSE_H

// Dreams includes.
#include "dDB/trackheap/TagVecValue64.h"

/**
  * @brief
  * This class is a tag vector that uses a dictionary.
  *
  * @description
  * This is an implementation that just allocates the necessary
  * space for the entries that it stores. Has the same functions
  * than the non dense version. All the write functions are
  * disabled.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class TagVecValue64Dense : public TagVecValue64
{
    public:
        TagVecValue64Dense(TagVecValue64 * original);
        virtual ~TagVecValue64Dense();

        virtual void reset(INT32 bcycle);

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        inline bool addTagValue(INT32 cycle, UINT64 value);
        inline bool undefineTagValue(INT32 cycle);

        inline TagVecEncodingType getType();
        void dumpCycleVector();

        inline ZipObject * compressYourSelf(CYCLE cycle, bool last = false);
};

/**
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecValue64Dense::getType()
{
    return TVE_VALUE_64_DENSE;
}

/*
 * Object already compressed.
 *
 * @return itself.
 */
ZipObject *
TagVecValue64Dense::compressYourSelf(CYCLE cycle, bool last)
{
    return this;
}

/*
 * Adds a new tag in the vector.
 *
 * @return false.
 */
bool
TagVecValue64Dense::addTagValue(INT32 cycle, UINT64 value)
{
    return false;
}

/*
 * Undefines the last tag value.
 *
 * @return false.
 */
bool
TagVecValue64Dense::undefineTagValue(INT32 cycle)
{
    return false;
}

#endif

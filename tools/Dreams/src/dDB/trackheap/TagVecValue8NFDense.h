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
  * @file TagVecValue8NFDense.h
  */

#ifndef _DRALDB_TAG_VEC_VALUE_8_NF_DENSE_H
#define _DRALDB_TAG_VEC_VALUE_8_NF_DENSE_H

// Dreams includes.
#include "dDB/trackheap/TagVecValue8NF.h"

/**
  * @brief
  * This class is a dense tag vector that stores 8 bits values.
  *
  * @description
  * This implementation just alloc the necessary space to store
  * all the values. When a value is requested for one cycle, the
  * function only returns match if the set of the value is in the
  * same cycle.
  *
  * @author Guillem Sole
  * @date started at 2004-10-27
  * @version 0.1
  */
class TagVecValue8NFDense : public TagVecValue8NF
{
    public:
        TagVecValue8NFDense(TagVecValue8NF * original);
        virtual ~TagVecValue8NFDense();

        virtual void reset(INT32 bcycle);

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        inline bool addTagValue(INT32 cycle, UINT64 value);
        inline bool addTagValue(INT32 cycle, QString value);
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
TagVecValue8NFDense::getType()
{
    return TVE_VALUE_8_NF_DENSE;
}

/*
 * Object already compressed.
 *
 * @return itself.
 */
ZipObject *
TagVecValue8NFDense::compressYourSelf(CYCLE cycle, bool last)
{
    return this;
}

/*
 * Adds a new tag in the vector.
 *
 * @return false.
 */
bool
TagVecValue8NFDense::addTagValue(INT32 cycle, UINT64 value)
{
    return false;
}

/*
 * Adds a new tag in the vector.
 *
 * @return false.
 */
bool
TagVecValue8NFDense::addTagValue(INT32 cycle, QString value)
{
    return false;
}

/*
 * Undefines the last value in the vector.
 *
 * @return true if everything is ok.
 */
bool
TagVecValue8NFDense::undefineTagValue(INT32 cycle)
{
    return false;
}

#endif

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
  * @file TagVecValue31Short.h
  */

#ifndef _DRALDB_TAG_VEC_VALUE_31_SHORT_H
#define _DRALDB_TAG_VEC_VALUE_31_SHORT_H

// Dreams includes.
#include "dDB/trackheap/TagVecValue31.h"

/*
 * @typedef TagVecValue31ShortNode
 * @brief
 * Entries of a tag vector item index.
 */
typedef struct
{
    UINT32 value       : 19; ///< Item id of the entry.
    UINT32 cycleOffset : 12; ///< Offset of the entry.
    UINT32 defined     : 1;  ///< If the value is defined or not.
} TagVecValue31ShortNode;

/*
 * @brief
 * This class is a tag vector that stores 20 bits.
 *
 * @description
 * This implementation of the index tag vector just uses 20 bits
 * to store the item id and 12 for the offset. Only 20 bits are
 * needed because is guaranteed that the maximum value enter is
 * never greater than 2^20.
 *
 * @author Federico Ardanaz
 * @date started at 2002-07-15
 * @version 0.1
 */
class TagVecValue31Short : public TagVec
{
    public:
        TagVecValue31Short(TagVecValue31 * original);
        virtual ~TagVecValue31Short();

        virtual void reset(INT32 bcycle);

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        TagReturn getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle);

        inline bool addTagValue(INT32 cycle, UINT64 value);
        inline bool undefineTagValue(INT32 cycle);

        inline TagVecEncodingType getType();
        void dumpCycleVector();

        inline ZipObject * compressYourSelf(CYCLE cycle, bool last = false);

    private:
        bool skipToNextCycleWithChange(InternalTagHandler * hnd);
        void getActualValue(InternalTagHandler * hnd);

        friend class TagVecValue31ShortHandler;

    protected:
        TagVecValue31ShortNode * valvec; ///< Array of indexes.
        INT16 nextEntry;                 ///< Position of the next entry.
        INT32 baseCycle;                 ///< Base cycle of the vector.
};

/*
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecValue31Short::getType()
{
    return TVE_VALUE_31_SHORT;
}

/*
 * Object already compressed.
 *
 * @return itself.
 */
ZipObject *
TagVecValue31Short::compressYourSelf(CYCLE cycle, bool last)
{
    return this;
}

/*
 * Adds a new tag in the vector.
 *
 * @return false.
 */
bool
TagVecValue31Short::addTagValue(INT32 cycle, UINT64 value)
{
    return false;
}

/*
 * Adds a new tag in the vector.
 *
 * @return false.
 */
bool
TagVecValue31Short::undefineTagValue(INT32 cycle)
{
    return false;
}

#endif

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
  * @file TagVecValue31.h
  */

#ifndef _DRALDB_TAG_VEC_VALUE_31_H
#define _DRALDB_TAG_VEC_VALUE_31_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/trackheap/TagVec.h"

#define TAG_VEC_VALUE_31_MIN_DENSITY 0.75

/*
 * @typedef TagVecValue31Node
 * @brief
 * nextEntry of a tag vector item index.
 */
typedef struct
{
    UINT16 cycleOffset : 12; ///< Offset inside the chunk.
    UINT16 defined     : 1;  ///< Mark if the value is defined or not.
    UINT32 value;            ///< Item id of the entry.
} TagVecValue31Node;

/*
 * @brief
 * This class is a tag vector that stores 31 bits.
 *
 * @description
 * This implementation of the tag vector is used to store elements
 * of 31 bits. An element matches only if is requested in the exact
 * cycle that when it was set.
 *
 * @author Federico Ardanaz
 * @date started at 2002-07-15
 * @version 0.1
 */
class TagVecValue31 : public TagVec
{
    public:
        TagVecValue31(INT32 bcycle, INT32 maxEntries = CYCLE_CHUNK_SIZE);
        virtual ~TagVecValue31();

        virtual void reset(INT32 bcycle);

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        TagReturn getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle);

        virtual inline bool addTagValue(INT32 cycle, UINT64 value);
        virtual inline bool undefineTagValue(INT32 cycle);

        virtual inline TagVecEncodingType getType();
        virtual void dumpCycleVector();

        virtual ZipObject * compressYourSelf(CYCLE cycle, bool last = false);

    protected:
        bool skipToNextCycleWithChange(InternalTagHandler * hnd);
        void getActualValue(InternalTagHandler * hnd);

        friend class TagVecValue31Dense;
        friend class TagVecValue31Short;
        friend class InternalTagHandler;

    protected:
        TagVecValue31Node * valvec; ///< Fixed size 4096 nextEntry.
        INT32 baseCycle;            ///< Base cycle of the vector.
        INT16 nextEntry;            ///< Number of nextEntry entered.
        UINT32 maxValue;            ///< Maximum value entered.
};

/*
 * Adds a new tag in the vector.
 *
 * @return true if everything is ok.
 */
bool
TagVecValue31::addTagValue(INT32 cycle, UINT64 value)
{
    INT16 offset; ///< Offset inside the vector for cycle.

    offset = cycle - baseCycle;
    Q_ASSERT(offset < CYCLE_CHUNK_SIZE);
    Q_ASSERT((nextEntry <= 0) || (offset >= valvec[nextEntry - 1].cycleOffset));

    // Updates statistics.
    maxValue = QMAX(maxValue, value);

    // If the set is in the same cyce just overwrites the information.
    if((nextEntry > 0) && (valvec[nextEntry - 1].cycleOffset == offset))
    {
        valvec[nextEntry - 1].value = (UINT32) value;
        valvec[nextEntry - 1].defined = true;
    }
    // If the value changes creates a new entry.
    // Just adds a new value if is different from the previous.
/*    else if(
        (nextEntry <= 0) ||
        !valvec[nextEntry - 1].defined ||
        ((UINT32) value != valvec[nextEntry - 1].value)
    )*/
    else
    {
        Q_ASSERT(nextEntry < CYCLE_CHUNK_SIZE);
        valvec[nextEntry].value = (UINT32) value;
        valvec[nextEntry].cycleOffset = offset;
        valvec[nextEntry].defined = true;
        nextEntry++;
    }
    return true;
}

/*
 * Undefines the value.
 *
 * @return true if everything is ok.
 */
bool
TagVecValue31::undefineTagValue(INT32 cycle)
{
    INT16 offset; ///< Offset inside the vector for cycle.

    offset = cycle - baseCycle;
    Q_ASSERT(offset < CYCLE_CHUNK_SIZE);
    Q_ASSERT((nextEntry <= 0) || (offset >= valvec[nextEntry - 1].cycleOffset));

    // If the set is in the same cyce just overwrites the information.
    if((nextEntry > 0) && (valvec[nextEntry - 1].cycleOffset == offset))
    {
        valvec[nextEntry - 1].defined = false;
    }
    // If the value changes creates a new entry.
    else if((nextEntry <= 0) || valvec[nextEntry - 1].defined)
    {
        Q_ASSERT(nextEntry < CYCLE_CHUNK_SIZE);
        valvec[nextEntry].cycleOffset = offset;
        valvec[nextEntry].defined = false;
        nextEntry++;
    }
    return true;
}

/*
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecValue31::getType()
{
    return TVE_VALUE_31;
}

#endif

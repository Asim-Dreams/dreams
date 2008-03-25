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
  * @file TagVecValue8NF.h
  */

#ifndef _DRALDB_TAG_VEC_VALUE_8_NF_H
#define _DRALDB_TAG_VEC_VALUE_8_NF_H

// Dreams includes.
#include "dDB/trackheap/TagVecValue8.h"

/**
  * @brief
  * This class is a tag vector that stores values of 8 bits.
  *
  * @description
  * The information is not compressed in this class. Only values
  * in the same cycle match.
  *
  * @author Guillem Sole
  * @date started at 2004-10-27
  * @version 0.1
  */
class TagVecValue8NF : public TagVec
{
    public:
        TagVecValue8NF(INT32 bcycle, INT32 maxEntries = CYCLE_CHUNK_SIZE);
        virtual ~TagVecValue8NF();

        virtual void reset(INT32 bcycle);

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        TagReturn getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle);

        virtual inline bool addTagValue(INT32 cycle, UINT64 value);
        virtual inline bool undefineTagValue(INT32 cycle);

        virtual INT32 getOccupancy(bool defined, INT32 lastCycle) const;

        inline TagVecEncodingType getType();
        void dumpCycleVector();

        ZipObject * compressYourSelf(CYCLE cycle, bool last = false);

    protected:
        bool skipToNextCycleWithChange(InternalTagHandler * hnd);
        void getActualValue(InternalTagHandler * hnd);

        friend class TagVecValue8NFDense;
        friend class InternalTagHandler;

    protected:
        TagVecValue8Node * valvec; ///< Pointer to the entries.
        INT16 nextEntry;           ///< Position of the next value.
        INT32 baseCycle;           ///< Cycle where the data starts.
};

/*
 * Adds a new tag in the vector.
 *
 * @return true if everything is ok.
 */
bool
TagVecValue8NF::addTagValue(INT32 cycle, UINT64 value)
{
    UINT32 offset; ///< Offset inside the vector for cycle.

    offset = cycle - baseCycle;

    Q_ASSERT(offset < CYCLE_CHUNK_SIZE);

    // Initial case.
    if(nextEntry == 0)
    {
        // Inserts the value.
        valvec[nextEntry].value = (UINT8) value;
        valvec[nextEntry].cycleOffset = (UINT16) offset;
        valvec[nextEntry].defined = true;
        nextEntry++;
    }
    // If is overwritting a value.
    else if(offset == valvec[nextEntry - 1].cycleOffset)
    {
        // Overwrites the key of the next entry.
        valvec[nextEntry - 1].value = (UINT8) value;
        valvec[nextEntry - 1].defined = true;
    }
    // Just adds a new value.
    else
    {
        Q_ASSERT(nextEntry < CYCLE_CHUNK_SIZE);

        // Inserts the value.
        valvec[nextEntry].value = (UINT8) value;
        valvec[nextEntry].cycleOffset = (UINT16) offset;
        valvec[nextEntry].defined = true;
        nextEntry++;
    }
    return true;
}

/*
 * Undefines the last value in the vector.
 *
 * @return true if everything is ok.
 */
bool
TagVecValue8NF::undefineTagValue(INT32 cycle)
{
    UINT32 offset; ///< Offset inside the vector for cycle.

    offset = cycle - baseCycle;

    Q_ASSERT(offset < CYCLE_CHUNK_SIZE);

    // Set to undefined if was set.
    if((nextEntry > 0) && (offset == valvec[nextEntry - 1].cycleOffset))
    {
        // Erases the entry.
        nextEntry--;
    }
    return true;
}

/*
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecValue8NF::getType()
{
    return TVE_VALUE_8_NF;
}

#endif

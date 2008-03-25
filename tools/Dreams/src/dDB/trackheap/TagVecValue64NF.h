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
  * @file TagVecValue64NF.h
  */

#ifndef _DRALDB_TAG_VEC_VALUE_64_NF_H
#define _DRALDB_TAG_VEC_VALUE_64_NF_H

// Dreams includes.
#include "dDB/trackheap/TagVecValue64.h"

/**
  * @brief
  * This class is a tag vector that uses a dictionary.
  *
  * @description
  * This implementation uses the Dict2064 class to map the keys
  * and values of the entries. The information is not compressed
  * in this class. Only values in the same cycle match.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class TagVecValue64NF : public TagVec
{
    public:
        TagVecValue64NF(INT32 bcycle, Dict2064 * dict, CLOCK_ID _clock_id, INT32 maxEntries = CYCLE_CHUNK_SIZE);
        virtual ~TagVecValue64NF();

        virtual void reset(INT32 bcycle);
        void setNewDict(Dict2064 * _dict);

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        TagReturn getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle);

        virtual inline bool addTagValue(INT32 cycle, UINT64 value);
        virtual inline bool undefineTagValue(INT32 cycle);

        inline TagVecEncodingType getType();
        void dumpCycleVector();

        ZipObject * compressYourSelf(CYCLE cycle, bool last = false);

    protected:
        bool skipToNextCycleWithChange(InternalTagHandler * hnd);
        void getActualValue(InternalTagHandler * hnd);

        friend class InternalTagHandler;
        friend class TagVecValue64NFDense;

    protected:
        TagVecValue64Node * valvec; ///< Pointer to the entries.
        INT16 nextEntry;            ///< Position of the next value.
        INT32 baseCycle;            ///< Cycle where the data starts.
        Dict2064 * dict;            ///< Pointer to the dictionary.
        CLOCK_ID clock_id;          ///< Clock id of this tag vector.
};

/*
 * Adds a new tag in the vector.
 *
 * @return true if everything is ok.
 */
bool
TagVecValue64NF::addTagValue(INT32 cycle, UINT64 value)
{
    UINT32 offset; ///< Offset inside the vector for cycle.
    INT32 dkey;    ///< Dictionary key for the value.

    offset = cycle - baseCycle;
    dkey = dict->getKeyFor(value, clock_id);

    Q_ASSERT(offset < CYCLE_CHUNK_SIZE);

    // Initial case.
    if(nextEntry == 0)
    {
        // Inserts the value.
        valvec[nextEntry].key = (UINT32) dkey;
        valvec[nextEntry].cycleOffset = (UINT32) offset;
        nextEntry++;
    }
    // If is overwritting a value.
    else if(offset == valvec[nextEntry - 1].cycleOffset)
    {
        // Overwrites the key of the next entry.
        valvec[nextEntry - 1].key = dkey;
    }
    // Just adds a new value.
    else
    {
        Q_ASSERT(nextEntry < CYCLE_CHUNK_SIZE);

        // Inserts the value.
        valvec[nextEntry].key = (UINT32) dkey;
        valvec[nextEntry].cycleOffset = (UINT32) offset;
        nextEntry++;
    }
    return true;
}

/*
 * Undefines the tag value.
 *
 * @return true if everything is ok.
 */
bool
TagVecValue64NF::undefineTagValue(INT32 cycle)
{
    UINT32 offset; ///< Offset inside the vector for cycle.

    offset = cycle - baseCycle;

    Q_ASSERT(offset < CYCLE_CHUNK_SIZE);

    // If is overwritting a value.
    if((nextEntry > 0) && (offset == valvec[nextEntry - 1].cycleOffset))
    {
        // Deletes the entry.
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
TagVecValue64NF::getType()
{
    return TVE_VALUE_64_NF;
}

#endif

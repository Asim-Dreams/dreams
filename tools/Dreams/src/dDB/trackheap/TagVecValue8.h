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

/*
 * @file TagVecValue8.h
 */

#ifndef _DRALDB_TAG_VEC_VALUE_8_H
#define _DRALDB_TAG_VEC_VALUE_8_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/trackheap/TagVec.h"

#define TAG_VEC_VALUE_8_MIN_DENSITY 0.75

/*
 * @typedef TagVecValue8Node
 * @brief
 * Entries of a tag vector dictionary.
 */
typedef struct
{
    UINT16 cycleOffset : 12; ///< Offset with the base cycle.
    UINT16 defined     :  1; ///< True if the value is defined.
    UINT8 value;             ///< Value stored.
} TagVecValue8Node;

// Forward reference of dense version.
class TagVecValue8Dense;

/*
 * @brief
 * This class is a tag vector that stores 8 bit values.
 *
 * @description
 * This vector can store up to 4096 different values of 8 bits. The
 * class behaves as a forwarding vector (when a value is set, this
 * value remains until another value is set). This class can be
 * compressed to a dense version that allocates less space.
 *
 * @author Guillem Sole
 * @date started at 2004-10-27
 * @version 0.1
 */
class TagVecValue8 : public TagVec
{
    public:
        TagVecValue8(INT32 bcycle, INT32 maxEntries = CYCLE_CHUNK_SIZE);
        virtual ~TagVecValue8();

        virtual void reset(INT32 bcycle);

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        TagReturn getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle);

        virtual inline bool addTagValue(INT32 cycle, UINT64 value);
        virtual inline bool undefineTagValue(INT32 cycle);

        virtual INT32 getOccupancy(bool defined, INT32 lastCycle) const;

        virtual inline TagVecEncodingType getType();
        virtual void dumpCycleVector();

        virtual ZipObject * compressYourSelf(CYCLE cycle, bool last = false);

    protected:
        bool skipToNextCycleWithChange(InternalTagHandler * hnd);
        void getActualValue(InternalTagHandler * hnd);

        friend class InternalTagHandler;
        friend class TagVecValue8Dense;

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
TagVecValue8::addTagValue(INT32 cycle, UINT64 value)
{
    INT16 offset; ///< Offset inside the vector for cycle.

    offset = cycle - baseCycle;

    Q_ASSERT(offset < CYCLE_CHUNK_SIZE);
    Q_ASSERT((nextEntry <= 0) || (offset >= valvec[nextEntry - 1].cycleOffset));

    // If the set is in the same cycle just overwrites the information.
    if((nextEntry > 0) && (valvec[nextEntry - 1].cycleOffset == offset))
    {
        valvec[nextEntry - 1].value = (UINT32) value;
        valvec[nextEntry - 1].defined = true;
    }
    // TagIdVec checks if something has changed.
/*    // If the value changes creates a new entry.
    else if
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
TagVecValue8::undefineTagValue(INT32 cycle)
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

/**
 * Returns the type of vector.
 *
 * @return the type.
 */
TagVecEncodingType
TagVecValue8::getType()
{
    return TVE_VALUE_8;
}

#endif

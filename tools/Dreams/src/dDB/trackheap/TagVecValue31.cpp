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
  * @file TagVecValue31.cpp
  */

// General includes.
#include <stdio.h>

// Dreams includes.
#include "dDB/taghandler/TagHandler.h"
#include "dDB/trackheap/TagVecValue31.h"
#include "dDB/trackheap/TagVecValue31Dense.h"
#include "dDB/trackheap/TagVecValue31Short.h"

/**
 * Creator of this class. Sets to 0 the array.
 *
 * @return new object.
 */
TagVecValue31::TagVecValue31(INT32 bcycle, INT32 maxEntries)
{
    baseCycle = bcycle;
    nextEntry = 0;
    maxValue = 0;
    valvec = new TagVecValue31Node[maxEntries];
}

/*
 * Destructor of this class. Nothing is done.
 *
 * @return destroys the object.
 */
TagVecValue31::~TagVecValue31()
{
    delete [] valvec;
}

/*
 * Resets the state of the class.
 *
 * @return void.
 */
void
TagVecValue31::reset(INT32 bcycle)
{
    baseCycle = bcycle;
    nextEntry = 0;
    maxValue = 0;
}

/**
 * Gets the value of this tag in the cycle cycle.
 *
 * @return true if the value has been found.
 */
TagReturn
TagVecValue31::getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle)
{
    UINT16 offset; ///< Internal offset for the cycle.
    INT16 lower;   ///< Lowest element that is in the valid range of the binary search.
    INT16 upper;   ///< Highest element that is in the valid range of the binary search.
    INT16 middle;  ///< Actual element that is being evaluated.

    // Gets the internal offset for the requested cycle.
    offset = cycle - baseCycle;

    // Checks if a value is defined inside this vector.
    if(valvec[0].cycleOffset > offset)
    {
        return TAG_NOT_FOUND;
    }

    // Initialization for the dicotomic search.
    // At least one entry, so in case that only one entry was inserted,
    // i will point to this entry the first cycle.
    lower = 0;
    upper = nextEntry - 1;

    // Performas a binary search along all the valid entries of the vector.
    while(upper >= lower)
    {
        // Computes the actual element.
        middle = (lower + upper) >> 1;

        // Checks if we've found the element:
        // The end condition is reached when the actual element has an offset
        // equal or lower than the cycle and when the following element is outside
        // the vector or when its offset is bigger than the cycle.
        if(
            (valvec[middle].cycleOffset <= offset) && 
            (((middle + 1) == nextEntry) || (valvec[middle + 1].cycleOffset > offset))
        )
        {
            // Checks if we're in an undefined part.
            if(!valvec[middle].defined)
            {
                return TAG_UNDEFINED;
            }

            // Computes the cycle when the computed value was set and gets the value.
            * value = valvec[middle].value;

            // Checks if the caller wants to know the cycle.
            if(atcycle != NULL)
            {
                * atcycle = valvec[middle].cycleOffset + baseCycle;
            }

            // We've found something for sure.
            return TAG_FOUND;
        }

        // Checks which direction we must go.
        if(valvec[middle].cycleOffset < offset)
        {
            lower = middle + 1;
        }
        else
        {
            upper = middle - 1;
        }
    }

    // Shouldn't get here.
    Q_ASSERT(false);

    // Avoid a warning...
    return TAG_NOT_FOUND;
}

/**
  * Compresses the vector to a dense vector.
  *
  * @return the compressed vector.
  */
ZipObject *
TagVecValue31::compressYourSelf(CYCLE cycle, bool last)
{
    if(last || (cycle.cycle >= (baseCycle + CYCLE_CHUNK_SIZE - 1)))
    {
        double density = ((double) nextEntry / (double) CYCLE_CHUNK_SIZE);

        // Only is compressed if is a low densed vector.
        if(density < TAG_VEC_VALUE_31_MIN_DENSITY)
        {
            // If the max item id can't be stored in 20 bits creates a normal dense item index vector.
            if(maxValue >= 524288)
            {
                return new TagVecValue31Dense(this);
            }
            // Creates a short item index vector.
            else
            {
                return new TagVecValue31Short(this);
            }
        }
    }
    return this;
}

/**
  * Dumps the content of the vector.
  *
  * @return void.
  */
void
TagVecValue31::dumpCycleVector()
{
    INT32 entry;
    UINT32 act_value;
    UINT16 act_offset;
    bool act_defined;

    act_defined = false;
    act_value = 0;
    act_offset = 0;
    entry = 0;

    printf("Dumping cycle vector base cycle = %d. Encoding type is TVE_VALUE_31\n", baseCycle);
    for(UINT32 i = 0; i < CYCLE_CHUNK_SIZE; i++)
    {
        if((entry < nextEntry) && (valvec[entry].cycleOffset == i))
        {
            act_value = valvec[entry].value;
            act_offset = valvec[entry].cycleOffset + baseCycle;
            act_defined = valvec[entry].defined;
            entry++;
        }

        if(act_defined)
        {
            printf("<%d,%u> ", act_offset, act_value);
        }
    }
    printf("\n");
}

/*
 * Computes the object size and returns the value
 *
 * @return the object size.
 */
INT64
TagVecValue31::getObjSize() const
{
    INT64 size;

    size = sizeof(TagVecValue31) + (CYCLE_CHUNK_SIZE * sizeof(TagVecValue31Node));
    return size;
}

/*
 * Returns the usage description. Unimplemented.
 *
 * @return empty string.
 */
QString
TagVecValue31::getUsageDescription() const
{
    return "";
}

/*
 * Moves the handler to the next value with change.
 *
 * @return if a different value has been found.
 */
bool
TagVecValue31::skipToNextCycleWithChange(InternalTagHandler * hnd)
{
    hnd->act_entry++;

    // We achieve this code if 
    if(hnd->act_entry < nextEntry)
    {
        hnd->act_defined = valvec[hnd->act_entry].defined;
        hnd->act_value = (UINT64) valvec[hnd->act_entry].value;
        hnd->act_cycle.cycle = valvec[hnd->act_entry].cycleOffset + baseCycle;
        return true;
    }

    // No change found.
    return false;
}

/*
 * Gets the value for the actual cycle where the handler is pointing.
 *
 * @return void.
 */
void
TagVecValue31::getActualValue(InternalTagHandler * hnd)
{
    UINT16 offset = (hnd->act_cycle.cycle & (CYCLE_CHUNK_SIZE - 1));

    while((hnd->act_entry < (nextEntry - 1)) && (valvec[hnd->act_entry + 1].cycleOffset <= offset))
    {
        hnd->act_entry++;
        hnd->act_defined = valvec[hnd->act_entry].defined;
        hnd->act_value = (UINT64) valvec[hnd->act_entry].value;
    }
}

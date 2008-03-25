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
  * @file TagVecValue8.cpp
  */

// General includes.
#include <stdio.h>

// Dreams includes.
#include "dDB/taghandler/TagHandler.h"
#include "dDB/trackheap/TagVecValue8.h"
#include "dDB/trackheap/TagVecValue8Dense.h"

/*
 * Creator of this class. Gets the static instances.
 *
 * @return new object.
 */
TagVecValue8::TagVecValue8(INT32 bcycle, INT32 maxEntries)
{
    baseCycle = bcycle;
    nextEntry = 0;
    valvec = new TagVecValue8Node[maxEntries];
}

/*
 * Destructor of this class. Nothing is done.
 *
 * @return destroys the object.
 */
TagVecValue8::~TagVecValue8()
{
    delete [] valvec;
}

/*
 * Resets the state of the vector.
 *
 * @return void.
 */
void
TagVecValue8::reset(INT32 bcycle)
{
    baseCycle = bcycle;
    nextEntry = 0;
}

/*
 * Gets the value of this tag in the cycle cycle.
 *
 * @return true if the value has been found.
 */
TagReturn
TagVecValue8::getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle)
{
    UINT32 offset; ///< Internal offset for the cycle.
    UINT32 ccycle; ///< Cycle when the returned value was set.
    UINT32 lower;  ///< Lowest element that is in the valid range of the binary search.
    UINT32 upper;  ///< Highest element that is in the valid range of the binary search.
    UINT32 middle; ///< Actual element that is being evaluated.

    // Gets the internal offset for the requested cycle.
    offset = cycle - baseCycle;

    // Checks if a value is defined inside this vector.
    if(valvec[0].cycleOffset > (UINT32) offset)
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
            (((middle + 1) == (UINT32) nextEntry) || (valvec[middle + 1].cycleOffset > offset))
        )
        {

            if(!valvec[middle].defined)
            {
                return TAG_UNDEFINED;
            }

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

/*
 * Compresses the vector to a dense vector.
 *
 * @return the compressed vector.
 */
ZipObject *
TagVecValue8::compressYourSelf(CYCLE cycle, bool last)
{
    ZipObject * result = this;

    if(last || (cycle.cycle >= (baseCycle + CYCLE_CHUNK_SIZE - 1)))
    {
        double density = ((double) nextEntry / (double) CYCLE_CHUNK_SIZE);

        // Only is compressed if is a low densed vector.
        if(density < TAG_VEC_VALUE_8_MIN_DENSITY)
        {
            result = new TagVecValue8Dense(this);
        }
    }
    return result;
}

/*
 * Gets the number of cycles that a value is defined.
 *
 * @return the occupancy.
 */
INT32
TagVecValue8::getOccupancy(bool defined, INT32 lastCycle) const
{
    INT32 occ;

    // If no entry.
    if(nextEntry == 0)
    {
        // If defined, all the cycles are defined.
        if(defined)
        {
            return lastCycle;
        }
        // None cycle is occupied.
        else
        {
            return 0;
        }
    }

    // The first cycles must have the same state than the previous chunk.
    if(defined)
    {
        occ = valvec[0].cycleOffset;
    }
    else
    {
        occ = 0;
    }

    // Adds the normal values.
    for(INT16 i = 0; i < nextEntry - 1; i++)
    {
        // If defined, adds the cycles until the next change.
        if(valvec[i].defined)
        {
            occ += valvec[i + 1].cycleOffset - valvec[i].cycleOffset;
        }
    }

    // If defined the last entry, adds cycles until the end of the chunk.
    if(valvec[nextEntry - 1].defined)
    {
        occ += lastCycle - valvec[nextEntry - 1].cycleOffset;
    }

    return occ;
}

/*
 * Dumps the content of the vector.
 *
 * @return void.
 */
void
TagVecValue8::dumpCycleVector()
{
    printf("Dumping cycle vector base cycle = %d. Encoding type is TVE_VALUE_8\n", baseCycle);
    printf("TVE_VALUE_8 efficiency = %g \n", ((double) nextEntry / (double) CYCLE_CHUNK_SIZE * 100.0));
    for(INT16 i = 0; i < nextEntry; i++)
    {
        printf("<%d,%u> ", baseCycle + valvec[i].cycleOffset, valvec[i].value);
    }
    printf("\n");
}

/*
 * Computes the object size and returns the value
 *
 * @return the object size.
 */
INT64
TagVecValue8::getObjSize() const
{
    INT64 size;

    size = sizeof(TagVecValue8) + (CYCLE_CHUNK_SIZE * sizeof(TagVecValue8Node));
    return size;
}

/*
 * Returns the usage description. Unimplemented.
 *
 * @return empty string.
 */
QString
TagVecValue8::getUsageDescription() const
{
    return "";
}

/*
 * Moves the handler to the next value with change.
 *
 * @return if a different value has been found.
 */
bool
TagVecValue8::skipToNextCycleWithChange(InternalTagHandler * hnd)
{
    hnd->act_entry++;

    // We achieve this code if 
    if(hnd->act_entry < nextEntry)
    {
        hnd->act_defined = valvec[hnd->act_entry].defined;
        hnd->act_value = valvec[hnd->act_entry].value;
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
TagVecValue8::getActualValue(InternalTagHandler * hnd)
{
    UINT16 offset = (hnd->act_cycle.cycle & (CYCLE_CHUNK_SIZE - 1));

    while((hnd->act_entry < (nextEntry - 1)) && (valvec[hnd->act_entry + 1].cycleOffset <= offset))
    {
        hnd->act_entry++;
        hnd->act_defined = valvec[hnd->act_entry].defined;
        hnd->act_value = valvec[hnd->act_entry].value;
    }
}

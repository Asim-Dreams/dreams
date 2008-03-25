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
  * @file TagVecValue31Short.cpp
  */

// General includes.
#include <stdio.h>

// Dreams includes.
#include "dDB/taghandler/TagHandler.h"
#include "dDB/trackheap/TagVecValue31Short.h"

/*
 * Creator of this class. Just allocates space for the entries.
 *
 * @return new object.
 */
TagVecValue31Short::TagVecValue31Short(TagVecValue31 * original)
{
    UINT32 pos = 0;

    baseCycle = original->baseCycle;
    nextEntry = original->nextEntry;

    // Allocates space just for the number of entries.
    valvec = new TagVecValue31ShortNode[nextEntry];

    // Copies all the entries.
    for(INT16 i = 0; i < nextEntry; i++)
    {
        valvec[i].defined = original->valvec[i].defined;
        valvec[i].value = original->valvec[i].value;
        valvec[i].cycleOffset = original->valvec[i].cycleOffset;
    }
}

/*
 * Destructor of this class. Deletes the array that was allocated.
 *
 * @return destroys the object.
 */
TagVecValue31Short::~TagVecValue31Short()
{
    delete [] valvec;
}

/*
 * Can't be called.
 *
 * @return void.
 */
void
TagVecValue31Short::reset(INT32 bcycle)
{
    Q_ASSERT(false);
}

/**
 * Gets the value (item id) in the cycle cycle.
 *
 * @return true if the value has been found.
 */
TagReturn
TagVecValue31Short::getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle)
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
  * Dumps the content of the vector.
  *
  * @return void.
  */
void
TagVecValue31Short::dumpCycleVector()
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
TagVecValue31Short::getObjSize() const
{
    INT64 size;

    size = sizeof(TagVecValue31Short) * (nextEntry * sizeof(TagVecValue31ShortNode));
    return size;
}

/*
 * Returns the usage description. Unimplemented.
 *
 * @return empty string.
 */
QString
TagVecValue31Short::getUsageDescription() const
{
    return "";
}

/*
 * Moves the handler to the next value with change.
 *
 * @return if a different value has been found.
 */
bool
TagVecValue31Short::skipToNextCycleWithChange(InternalTagHandler * hnd)
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
TagVecValue31Short::getActualValue(InternalTagHandler * hnd)
{
    UINT16 offset = (hnd->act_cycle.cycle & (CYCLE_CHUNK_SIZE - 1));

    while((hnd->act_entry < (nextEntry - 1)) && (valvec[hnd->act_entry + 1].cycleOffset <= offset))
    {
        hnd->act_entry++;
        hnd->act_defined = valvec[hnd->act_entry].defined;
        hnd->act_value = (UINT64) valvec[hnd->act_entry].value;
    }
}

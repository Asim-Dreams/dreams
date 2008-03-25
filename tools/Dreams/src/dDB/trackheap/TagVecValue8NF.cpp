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
  * @file TagVecValue8NF.cpp
  */

// General includes.
#include <stdio.h>

// Dreams includes.
#include "dDB/taghandler/TagHandler.h"
#include "dDB/trackheap/TagVecValue8NF.h"
#include "dDB/trackheap/TagVecValue8NFDense.h"

/*
 * Creator of this class.
 *
 * @return new object.
 */
TagVecValue8NF::TagVecValue8NF(INT32 bcycle, INT32 maxEntries)
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
TagVecValue8NF::~TagVecValue8NF()
{
    delete [] valvec;
}

/*
 * Resets the state of the vector.
 *
 * @return void.
 */
void
TagVecValue8NF::reset(INT32 bcycle)
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
TagVecValue8NF::getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle)
{
    UINT32 offset; ///< Internal offset for the cycle.
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
        if(valvec[middle].cycleOffset == offset)
        {
            * value = (UINT64) valvec[middle].value;

            // Checks if the caller wants to know the cycle.
            if(atcycle != NULL)
            {
                * atcycle = cycle;
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

    // Nothing found.
    return TAG_NOT_FOUND;
}

/*
 * Compresses the vector to a dense vector.
 *
 * @return the compressed vector.
 */
ZipObject *
TagVecValue8NF::compressYourSelf(CYCLE cycle, bool last)
{
    ZipObject * result = this;

    if(last || (cycle.cycle >= (baseCycle + CYCLE_CHUNK_SIZE - 1)))
    {
        double density = ((double) nextEntry / (double) CYCLE_CHUNK_SIZE);

        // Only is compressed if is a low densed vector.
        if(density < TAG_VEC_VALUE_8_MIN_DENSITY)
        {
            result = new TagVecValue8NFDense(this);
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
TagVecValue8NF::getOccupancy(bool defined, INT32 lastCycle) const
{
    INT32 occ;

    occ = 0;
    for(INT16 i = 0; i < nextEntry; i++)
    {
        occ += valvec[i].defined;
    }
    return occ;
}

/*
 * Dumps the content of the vector.
 *
 * @return void.
 */
void
TagVecValue8NF::dumpCycleVector()
{
    printf("Dumping cycle vector base cycle = %d. Encoding type is TVE_VALUE_8_NF\n", baseCycle);
    printf("TVE_VALUE_8_NF efficiency = %g \n", ((double) nextEntry / (double) CYCLE_CHUNK_SIZE * 100.0));
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
TagVecValue8NF::getObjSize() const
{
    INT64 size;

    size = sizeof(TagVecValue8NF) + (CYCLE_CHUNK_SIZE * sizeof(TagVecValue8Node));
    return size;
}

/*
 * Returns the usage description. Unimplemented.
 *
 * @return empty string.
 */
QString
TagVecValue8NF::getUsageDescription() const
{
    return "";
}

/*
 * Moves the handler to the next value with change.
 *
 * @return if a different value has been found.
 */
bool
TagVecValue8NF::skipToNextCycleWithChange(InternalTagHandler * hnd)
{
    if(hnd->act_entry == -1)
    {
        // As we know that the tag handler calls first the getActualValue we have two cases:
        // - No entry in the offset 0: then act_entry remains in -1 and the act_defined is set to false.
        //   If the previous defined was true, it will be detected by the tag handler, so always the
        //   defined value will be false.
        // - Entry in the offset 0: the getActualValue will set move act_entry to 0, so we don't care
        //   about this case.

        // We move to the next defined entry.
        hnd->act_entry = 0;
        hnd->act_defined = true;
        hnd->act_cycle.cycle = valvec[0].cycleOffset + baseCycle;
        hnd->act_value = valvec[0].value;
        return true;
    }

    // Checks if we found a value in the last search.
    if(valvec[hnd->act_entry].cycleOffset == (UINT32) (hnd->act_cycle.cycle & (CYCLE_CHUNK_SIZE - 1)))
    {
        INT32 last_cycle = (hnd->act_cycle.cycle & (CYCLE_CHUNK_SIZE - 1)) - 1;

        // We try to find an undefine.
        while(hnd->act_entry < nextEntry)
        {
            // Checks if the entries are consecutive.
            if(valvec[hnd->act_entry].cycleOffset == (UINT32) (last_cycle + 1))
            {
                UINT64 temp_value = valvec[hnd->act_entry].value;
                // Checks if the values are different.
                if(temp_value != hnd->act_value)
                {
                    hnd->act_value = temp_value;
                    hnd->act_cycle.cycle = valvec[hnd->act_entry].cycleOffset + baseCycle;
                    return true;
                }
            }
            else
            {
                // We've found a non consecutive value. We must undefine.
                hnd->act_defined = false;
                hnd->act_entry--;
                hnd->act_cycle.cycle = valvec[hnd->act_entry].cycleOffset + baseCycle + 1;
                return true;
            }
            last_cycle++;
            hnd->act_entry++;
        }

        hnd->act_entry--;

        // If the last entry is in the last cycle, then we haven't found a value.
        if(valvec[hnd->act_entry].cycleOffset == (CYCLE_CHUNK_SIZE - 1))
        {
            return false;
        }

        // We have found an undefined after the last entry.
        hnd->act_defined = false;
        hnd->act_cycle.cycle = valvec[hnd->act_entry].cycleOffset + baseCycle + 1;
        return true;
    }
    else
    {
        hnd->act_entry++;
        if(hnd->act_entry >= nextEntry)
        {
            return false;
        }
        hnd->act_defined = true;
        hnd->act_value = valvec[hnd->act_entry].value;
        hnd->act_cycle.cycle = valvec[hnd->act_entry].cycleOffset + baseCycle;
        return true;
    }
}

/*
 * Gets the value for the actual cycle where the handler is pointing.
 *
 * @return void.
 */
void
TagVecValue8NF::getActualValue(InternalTagHandler * hnd)
{
    UINT16 offset = (hnd->act_cycle.cycle & (CYCLE_CHUNK_SIZE - 1));

    while((hnd->act_entry < (nextEntry - 1)) && (valvec[hnd->act_entry + 1].cycleOffset < offset))
    {
        hnd->act_entry++;
    }

    if((hnd->act_entry < (nextEntry - 1)) && (valvec[hnd->act_entry + 1].cycleOffset == offset))
    {
        hnd->act_entry++;
        hnd->act_defined = true;
        hnd->act_value = valvec[hnd->act_entry].value;
    }
    else
    {
        hnd->act_defined = false;
    }
}

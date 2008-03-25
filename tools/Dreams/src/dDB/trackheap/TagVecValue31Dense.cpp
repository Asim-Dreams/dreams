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
#include "dDB/trackheap/TagVecValue31Dense.h"

/**
 * Creator of this class. Just allocates space for the entries.
 *
 * @return new object.
 */
TagVecValue31Dense::TagVecValue31Dense(TagVecValue31 * original)
    : TagVecValue31(original->baseCycle, original->nextEntry)
{
    nextEntry = original->nextEntry;

    // Copies the values.
    memcpy(valvec, original->valvec, sizeof(TagVecValue31Node) * nextEntry);
}

/**
 * Destructor of this class.
 *
 * @return destroys the object.
 */
TagVecValue31Dense::~TagVecValue31Dense()
{
}

/*
 * Can't be called.
 *
 * @return void.
 */
void
TagVecValue31Dense::reset(INT32 bcycle)
{
    Q_ASSERT(false);
}

/*
 * Dumps the content of the vector.
 *
 * @return void.
 */
void
TagVecValue31Dense::dumpCycleVector()
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
TagVecValue31Dense::getObjSize() const
{
    INT64 size;

    size = sizeof(TagVecValue31Dense) + (nextEntry * sizeof(TagVecValue31Node));
    return size;
}

/*
 * Returns the usage description. Unimplemented.
 *
 * @return empty string.
 */
QString
TagVecValue31Dense::getUsageDescription() const
{
    return "";
}

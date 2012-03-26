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
  * @file TagVecValue64Dense.cpp
  */

// Dreams includes.
#include "dDB/taghandler/TagHandler.h"
#include "dDB/trackheap/TagVecValue64Dense.h"

#include <stdio.h>

/*
 * Creator of this class. Gets the static instances and allocates
 * space for the entries.
 *
 * @return new object.
 */
TagVecValue64Dense::TagVecValue64Dense(TagVecValue64 * original)
    : TagVecValue64(original->baseCycle, original->dict, original->clock_id, original->nextEntry)
{
    nextEntry = original->nextEntry;

    // Copies the values.
    memcpy(valvec, original->valvec, sizeof(TagVecValue64Node) * nextEntry);
}

/*
 * Destructor of this class.
 *
 * @return destroys the object.
 */
TagVecValue64Dense::~TagVecValue64Dense()
{
}

/*
 * Can't be called.
 *
 * @return void.
 */
void
TagVecValue64Dense::reset(INT32 bcycle)
{
    Q_ASSERT(false);
}

/**
  * Dumps the content of the vector.
  *
  * @return void.
  */
void
TagVecValue64Dense::dumpCycleVector()
{
    printf("Dumping cycle vector base cycle = %d. Encoding type is TVE_VALUE_64_DENSE\n", baseCycle);
    for(INT16 i = 0; i < nextEntry; i++)
    {
        printf("<%d,%llu> ", baseCycle + valvec[i].cycleOffset, dict->getValueByCycle(valvec[i].key, valvec[i].cycleOffset + baseCycle, clock_id));
    }
    printf("\n");
}

/*
 * Computes the object size and returns the value
 *
 * @return the object size.
 */
INT64
TagVecValue64Dense::getObjSize() const
{
    INT64 size;

    size = sizeof(TagVecValue64Dense) + (nextEntry * sizeof(TagVecValue64Node));
    return size;
}

/*
 * Returns the usage description. Unimplemented.
 *
 * @return empty string.
 */
QString
TagVecValue64Dense::getUsageDescription() const
{
    return "";
}

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
  * @file TagVecValue8Dense.cpp
  */

// General includes.
#include <stdio.h>

// Dreams includes.
#include "dDB/taghandler/TagHandler.h"
#include "dDB/trackheap/TagVecValue8Dense.h"

/*
 * Creator of this class. Gets the static instances and allocates
 * space for the entries.
 *
 * @return new object.
 */
TagVecValue8Dense::TagVecValue8Dense(TagVecValue8 * original)
    : TagVecValue8(original->baseCycle, original->nextEntry)
{
    nextEntry = original->nextEntry;

    // Copies the values.
    memcpy(valvec, original->valvec, sizeof(TagVecValue8Node) * nextEntry);
}

/*
 * Destructor of this class.
 *
 * @return destroys the object.
 */
TagVecValue8Dense::~TagVecValue8Dense()
{
}

/*
 * Can't be called.
 *
 * @return void.
 */
void
TagVecValue8Dense::reset(INT32 bcycle)
{
    Q_ASSERT(false);
}

/*
 * Dumps the content of the vector.
 *
 * @return void.
 */
void
TagVecValue8Dense::dumpCycleVector()
{
    printf("Dumping cycle vector base cycle = %d. Encoding type is TVE_VALUE_8_DENSE\n", baseCycle);
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
TagVecValue8Dense::getObjSize() const
{
    INT64 size;

    size = sizeof(TagVecValue8Dense) + (nextEntry * sizeof(TagVecValue8Node));
    return size;
}

/*
 * Returns the usage description. Unimplemented.
 *
 * @return empty string.
 */
QString
TagVecValue8Dense::getUsageDescription() const
{
    return "";
}

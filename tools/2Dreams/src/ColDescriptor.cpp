/*
 * Copyright (C) 2003-2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

/**
  * @file ColDescriptor.cpp
  */

#include <string.h>

#include "ColDescriptor.h"

ColDescriptor* ColDescriptor::lastThis = NULL;

INT32 ColDescriptor::lastRow = -1;
INT32 ColDescriptor::lastComputedValue = -1;
UINT8 ColDescriptor::popcnt[256];

void
ColDescriptor::computeNumEvents()
{

    INT32 current = 0;
    INT32 acum = 0;
    for (int i=0;i<COLDESC_SZ_FLAG_VECTOR;i++)
    {
        acum+=popcnt[flagVector[i]];
    }
    //printf ("\t\t\t\t\tColDescriptor:: number of computed events = %d\n",acum);
    if (acum==0) { return; }

    numEvents = acum;
    events = new EventItem[numEvents];
    Q_ASSERT(events!=NULL);

    // flag which indicates that the computation was done...
    computedNumEvents = 1;

    // stats
    //eventCountAccum += numEvents;
}

// static initialize popCount table
void
ColDescriptor::initPopCnt()
{
    for (int i =0;i<256;i++)
    {
        ColDescriptor::popcnt[i] = popCount((UINT8)i);
    }
}

UINT8
ColDescriptor::popCount(UINT8 value)
{
    UINT8 cnt=0;
    for (int i=0;i<8;i++)
    {
        if ((value >> (7-i)) & 1) ++cnt;
    }
    return cnt;
}



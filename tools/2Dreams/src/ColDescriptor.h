// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file ColDescriptor.h
  */

#ifndef _COLDESCRIPTOR_H
#define _COLDESCRIPTOR_H

#include "asim/DralDB.h"

// 2Dreams
#include "EventItem.h"
#include "DefaultSizes.h"

/**
  * This class holds the information needed to each column in the event matrix.
  * ColDescriptor cannot inherit from AMemObj 'cause it's size critical. We
  * implement AMemObj API anyway so ColDescVector can query us in the standard way.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class ColDescriptor
{
    public:

        // -----
        inline ColDescriptor();
        inline ~ColDescriptor();
        // -----------------------------------------------

        void computeNumEvents();
        inline INT32 getNumEvents() const;

        inline void addItemEventData(INT32 row, INT32 csidx,UINT32 item_id);
        inline void addTrackEventData(INT32 row, INT32 csidx);

        inline bool isUsed(INT32 row);
        inline bool hasValidData(INT32 row);
        inline void setUsed(INT32 row);

        inline INT32 getColorShapeIdx(INT32 row);

        // convenience methods to speed-up look-ups.
        inline INT32 getMinItemId() const;
        inline INT32 getMaxItemId() const;

        // I implement this in the same wat than AMemObj,
        // we avoid inheritance due to size restrictions
        inline INT64 getObjSize() const;


    protected:
        inline void setColorShapeIdx (INT32 row, INT32 idx);
        inline INT32 computeSegPos(INT32 row);

    private:
        UINT16 numEvents        : 15;     ///< at most 32768 events by segment
        UINT16 computedNumEvents:  1;     ///< checks whether the number of events has been computed or not
        EventItem* events;                ///< the array of color/shape values

        // A flag vector for used/unused events in a given column
        // The max number of events is 8*SZ_FLAG_VECTOR
        UBYTE flagVector[COLDESC_SZ_FLAG_VECTOR];

        // to optimize some scanning operations we keep a
        // precomputed set of values
        UINT32 minItemId;
        UINT32 maxItemId;

    public:
        static UINT8 popCount(UINT8 value);
        static void initPopCnt();

    private:
        static ColDescriptor* lastThis;
        static INT32 lastRow;
        static INT32 lastComputedValue;
        static UINT8 popcnt[256];
};


// -----------------------------------------------
// -----------------------------------------------
// Inlined functions
// -----------------------------------------------
// -----------------------------------------------

ColDescriptor::ColDescriptor()
{
    bzero((void*)flagVector,(size_t)(COLDESC_SZ_FLAG_VECTOR*sizeof(UBYTE)));
    events = NULL;
    numEvents = 0;
    computedNumEvents = 0;
    minItemId=0xffffffff;
    maxItemId=0;
}

ColDescriptor::~ColDescriptor()
{
    if (events!=NULL) delete [] events;
    events = NULL;
}

bool
ColDescriptor::isUsed(INT32 row)
{
    if (row<0) { return (false); }
    if (row>=COLDESC_MAX_EVENTS) { return (false); }
    return ((computedNumEvents!=0) && ((flagVector[row>>3]>>(7-(row&0x0007)) & 1) ) );
}

void
ColDescriptor::setUsed(INT32 row)
{
    Q_ASSERT(row<COLDESC_MAX_EVENTS);
    flagVector[row>>3] |= (1 << (7-(row&0x0007)));
}

void
ColDescriptor::setColorShapeIdx(INT32 row, INT32 idx)
{
    // find out the event position in the segment
    Q_ASSERT (idx>=0);
    //Q_ASSERT (row<COLDESC_MAX_EVENTS);

    INT32 spos = computeSegPos(row);
    Q_ASSERT(spos<(INT32)numEvents);
    events[spos].setShapeColor((UINT32)idx);
}

INT32
ColDescriptor::getColorShapeIdx(INT32 row)
{
    //printf ("\t\t\t\t ColDesc::getColorShapeIdx called on row = %d\n",row);

    if (row>=COLDESC_MAX_EVENTS) return (-1);
    if (row<0) return (-1);

    // find out the event position in the segment
    INT32 spos = computeSegPos(row);
    Q_ASSERT(spos<(INT32)numEvents);
    return (INT32) events[spos].getShapeColor();
}

INT32
ColDescriptor::computeSegPos(INT32 row)
{
    //Q_ASSERT (row<COLDESC_MAX_EVENTS);

    // try to reuse last computation
    if ( (this==lastThis)&&(lastRow==row) )
    {
        return lastComputedValue;
    }

    // I need to count the number of '1' untill row positon
    // in the flagVector;

    INT32 acum = 0;
    INT32 bytepos = row >> 3;
    INT32 byte=0;
    while (byte<bytepos)
    {
        acum += popcnt[flagVector[byte]];
        ++byte;
    }

    INT32 current = row - (row&0x0007);
    while  (current < row)
    {
        acum += ( flagVector[current >> 3]>>(7-(current&0x0007)) & 1 ) ;
        ++current;
    }

    lastThis = this;
    lastRow = row;
    lastComputedValue = acum;

    return (acum);
}

INT64
ColDescriptor::getObjSize() const
{
    INT64 result = sizeof(this);
    result += (sizeof(EventItem)*numEvents) ;
    return (result);
}

INT32
ColDescriptor::getMinItemId() const
{ return minItemId; }

INT32
ColDescriptor::getMaxItemId() const
{ return maxItemId; }

void
ColDescriptor::addItemEventData(INT32 row, INT32 csidx, UINT32 item_id)
{
    Q_ASSERT(isUsed(row));
    // and the color/shape combination
    setColorShapeIdx(row,csidx);
    // update min/max counters
    minItemId = QMIN(minItemId,item_id);
    maxItemId = QMAX(maxItemId,item_id);
}

void
ColDescriptor::addTrackEventData(INT32 row, INT32 csidx)
{
    Q_ASSERT(isUsed(row));
    // and the color/shape combination
    setColorShapeIdx(row,csidx);
}

bool
ColDescriptor::hasValidData(INT32 row)
{
    if (!isUsed(row)) { return false; }
    // find out the event position in the segment
    INT32 spos = computeSegPos(row);
    Q_ASSERT(spos<(INT32)numEvents);
    return ( events[spos].getShapeColor() != EITEM_NON_INIT_COLORSHAPE );
}

#endif


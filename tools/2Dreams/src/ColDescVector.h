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
  * @file ColDescVector.h
  */

#ifndef _COLDESCVECTOR_H
#define _COLDESCVECTOR_H

//#include <stdio.h>

#include <qmessagebox.h>

#include "asim/DralDB.h"

#include "2DreamsDefs.h"
#include "ColDescriptor.h"

typedef AEVector<ColDescriptor,8192,125> ColDescAEVector; 

/**
  * This class holds a vector for the Event Matrix Column Descriptors.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class ColDescVector : public AMemObj, public StatObj
{
    public:
        // ---- AMemObj Interface methods
        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;
        // -----------------------------------------------

        ColDescVector();
        virtual ~ColDescVector();

        inline INT32  getNumCycles ();
        inline bool   incNumCycles (INT32 offset);

        inline bool isUsed (INT32 col, INT32 row);
        inline bool hasValidData (INT32 col, INT32 row);
        inline void setUsed (INT32 col, INT32 row);
        inline void computeNumEvents(INT32 col);

        inline void addItemEventData(INT32 col, INT32 row, INT32 csidx,UINT32 item_id);
        inline void addTrackEventData(INT32 col, INT32 row, INT32 csidx);

        inline INT32 getColorShapeIdx(INT32 col, INT32 row);

        inline INT32 getMinItemId(INT32 col);
        inline INT32 getMaxItemId(INT32 col);

        void reset();

    private:
        INT32 nextEntry;
        ColDescAEVector* aecolvector;

    private:
        static LogMgr* log;
};

INT32
ColDescVector::getNumCycles()
{ return nextEntry; }

bool
ColDescVector::incNumCycles (INT32 offset)
{
    if (offset<1)
    {
        log->addLog("Invalid Cycle sequence, check .drl file!");
        return false;
    }
    nextEntry += offset;
    return (true);
}


bool
ColDescVector::isUsed (INT32 col, INT32 row)
{
    if (col<0) return false;
    if (row<0) return false;

    if (col<nextEntry)
        return (*aecolvector)[col].isUsed(row);
    else
        return (false);
}

bool
ColDescVector::hasValidData (INT32 col, INT32 row)
{
    if (col<0) return false;
    if (row<0) return false;

    if (col<nextEntry)
        return (*aecolvector)[col].hasValidData(row);
    else
        return (false);
}

void
ColDescVector::setUsed (INT32 col, INT32 row)
{
    Q_ASSERT(row>=0);
    Q_ASSERT(col>=0);
    Q_ASSERT(col<nextEntry);
    (*aecolvector)[col].setUsed(row);
}

void
ColDescVector::addItemEventData(INT32 col, INT32 row, INT32 csidx,UINT32 item_id)
{
    //printf ("ColDescVector::addItemEventData called on col=%d,row=%d\n",col,row);fflush(stdout);
    Q_ASSERT(col>=0);
    Q_ASSERT(col<nextEntry);
    Q_ASSERT(row>=0);
    Q_ASSERT(csidx>=0);
    (*aecolvector)[col].addItemEventData(row,csidx,item_id);
}

void
ColDescVector::addTrackEventData(INT32 col, INT32 row, INT32 csidx)
{
    //printf ("ColDescVector::addItemEventData called on col=%d,row=%d\n",col,row);fflush(stdout);
    Q_ASSERT(col>=0);
    Q_ASSERT(col<nextEntry);
    Q_ASSERT(row>=0);
    Q_ASSERT(csidx>=0);
    (*aecolvector)[col].addTrackEventData(row,csidx);
}

void
ColDescVector::computeNumEvents(INT32 col)
{
    Q_ASSERT(col<nextEntry);
    Q_ASSERT(col>=0);
    (*aecolvector)[col].computeNumEvents();
}

INT32
ColDescVector::getColorShapeIdx(INT32 col, INT32 row)
{
    Q_ASSERT(col>=0);
    Q_ASSERT(col<nextEntry);
    Q_ASSERT(row>=0);
    return (*aecolvector)[col].getColorShapeIdx(row);
}

INT32
ColDescVector::getMinItemId(INT32 col)
{
    Q_ASSERT(col<nextEntry);
    Q_ASSERT(col>=0);
    return (*aecolvector)[col].getMinItemId();
}

INT32
ColDescVector::getMaxItemId(INT32 col)
{
    Q_ASSERT(col<nextEntry);
    Q_ASSERT(col>=0);
    return (*aecolvector)[col].getMaxItemId();
}
#endif



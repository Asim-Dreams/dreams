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
  * @file TagVec.h
  */

#ifndef _DRALDB_TAGVEC_H
#define _DRALDB_TAGVEC_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/DRALTag.h"
#include "dDB/aux/ZipObject.h"
#include "dDB/aux/AMemObj.h"

// Qt includes.
#include <qstring.h>

typedef enum
{
    TAG_FOUND,     ///< Value set.
    TAG_UNDEFINED, ///< Value undefined.
    TAG_NOT_FOUND  ///< No value found, search in another vector.
} TagReturn;

// Class forwarding.
class InternalTagHandler;

/**
  * @brief
  * Abstract class to hide encoding formats.
  *
  * @description
  * This class is an interface of different vector implementations.
  * Each implementation is used to know the value of a tag in a
  * precise moment. Each version differs from each other by the way
  * this matching is done : only matches if the tag was set in the
  * same cycle that is requested, if was set before, ...
  *
  * @version 0.1
  * @date started at 2003-05-27
  * @author Federico Ardanaz
  */
class TagVec : public AMemObj, public ZipObject
{
    public:
        virtual ~TagVec() { };

        virtual void reset(INT32 bcycle) = 0;

        virtual INT64 getObjSize() const = 0;
        virtual QString getUsageDescription() const = 0;

        inline virtual TagReturn getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle);
        inline virtual TagReturn getTagValue(INT32 cycle, SOVList ** value, INT32 * atcycle);

        inline virtual bool addTagValue(INT32 cycle, UINT64 value);
        inline virtual bool addTagValue(INT32 cycle, SOVList * value);
        inline virtual bool undefineTagValue(INT32 cycle);

        inline virtual INT32 getOccupancy(bool defined, INT32 lastCycle) const;

        virtual void dumpCycleVector() = 0;
        virtual TagVecEncodingType getType() = 0;

        friend class InternalTagHandler;

    private:
        virtual bool skipToNextCycleWithChange(InternalTagHandler * hnd) = 0;
        virtual void getActualValue(InternalTagHandler * hnd) = 0;
};

/**
 * Gets the value in the cycle cycle.
 *
 * @return always false.
 */
TagReturn
TagVec::getTagValue(INT32 cycle, UINT64 * value, INT32 * atcycle)
{
    return TAG_NOT_FOUND;
}

/**
 * Gets the value in the cycle cycle.
 *
 * @return always false.
 */
TagReturn
TagVec::getTagValue(INT32 cycle, SOVList ** value, INT32 * atcycle)
{
    return TAG_NOT_FOUND;
}

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVec::addTagValue(INT32 cycle, UINT64 value)
{
    return false;
}

/**
 * Adds a new tag in the vector.
 *
 * @return always false.
 */
bool
TagVec::addTagValue(INT32 cycle, SOVList * value)
{
    return false;
}

/*
 * Undefines the tag.
 *
 * @return always false.
 */
bool
TagVec::undefineTagValue(INT32 cycle)
{
    return false;
}

/*
 * Gets the number of cycles that a value is defined.
 *
 * @return the occupancy.
 */
INT32
TagVec::getOccupancy(bool defined, INT32 lastCycle) const
{
    return 0;
}

#endif

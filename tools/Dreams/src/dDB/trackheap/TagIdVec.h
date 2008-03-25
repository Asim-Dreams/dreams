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
  * @file TagIdVec.h
  */

#ifndef _DRALDB_TAGIDVEC_H
#define _DRALDB_TAGIDVEC_H

// General includes.
#include <vector>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/aux/AEVector.h"
#include "dDB/aux/ZipObject.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/trackheap/TagVec.h"

#define TAGIDVECNODE_AEPAGESIZE 4
#define TAGIDVECNODE_AENUMPAGES 64
#define TAGIDVECNODE_MAXCHUNKS (TAGIDVECNODE_AEPAGESIZE * TAGIDVECNODE_AENUMPAGES)
#define TAGIDVECNODE_MAXCYCLES (TAGIDVECNODE_MAXCHUNKS * CYCLE_CHUNK_SIZE)

// Class forwarding.
class Dict2064;
class TagVecValue64;
class TagVecValue64NF;
class TagVecValue31;
class TagVecValue8;
class TagVecValue8NF;

/*
 * @brief
 * Class that stores the tracks of all the tags of a node or item.
 *
 * @description
 * This class has an array of 4096 entries that track the values
 * of the tags of an element. Each entry is a tag vector that
 * holds the values for this tag.
 *
 * @author Federico Ardanaz
 * @date started at 2002-07-15
 * @version 0.1
 */

/* @typedef TagCycleVector
 * @brief
 * Array that holds all the pointers to tag vectors for a given
 * tag. 8 * 32 * 4096 = 1 million cycles.
 */
typedef AEVector<TagVec *, TAGIDVECNODE_AEPAGESIZE, TAGIDVECNODE_AENUMPAGES> TagCycleVector;

class TagIdVecNode : public AMemObj, public ZipObject
{
    public:
        TagIdVecNode();
        virtual ~TagIdVecNode();

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        // ---- ZibObject Interface methods
        ZipObject * compressYourSelf(CYCLE cycle, bool last = false);
        // -----------------------------------------------

        inline void setFwd(bool value);
        inline void setSize(TagSizeStorage value);

        bool getTagValue(CYCLE cycle, UINT64 * value, CYCLE * atcycle) const;
        bool getTagValue(CYCLE cycle, SOVList ** value, CYCLE * atcycle) const;
        bool getTagValuePhase(CYCLE cycle, UINT64 * value, CYCLE * atcycle) const;
        bool getTagValuePhase(CYCLE cycle, SOVList ** value, CYCLE * atcycle) const;

        UINT32 getOccupancy() const;

        inline bool addTagValue(CYCLE cycle, UINT64 value);
        inline bool addTagValue(CYCLE cycle, SOVList * value);
        inline bool undefineTagValue(CYCLE cycle);

        inline bool hasData() const;
        inline bool hasHighData() const;
        inline bool hasLowData() const;

        void checkCycleChunk(INT32 cycleChunk, CLOCK_ID clock_id, TagCycleVector ** cycleVecParam);

        void dumpTagIdVector();

        static void purgeAllocatedVectors();

    protected:
        bool internalGetTagValue(const TagCycleVector * cycleVec, INT32 cycle, UINT64 * value, INT32 * atcycle) const;
        UINT32 internalGetOccupancy(const TagCycleVector * cycleVec) const;

    protected:
        TagCycleVector * cycleVecHigh;   ///< Pointer to the tag vectors with high values.
        TagCycleVector * cycleVecLow;    ///< Pointer to the tag vectors with low values.
        UINT32 firstCycle          : 20; ///< First cycle with contents.
        UINT32 lastCompressedChunk :  9; ///< Last compressed chunk of the tag.
        UINT32 dummy               :  3; ///< Dummy space
        UINT32 lastCycle           : 20; ///< Last cycle with contens.
        UINT32 fwd                 :  1; ///< A value is forwarded when search.
        UINT32 size                :  4; ///< Size of the tag values.
        UINT32 lastValueUndefined  :  1; ///< True if the last value is undefined.
        UINT64 lastValue;                ///< Last entered value. Used to compress same values entries.
        Dict2064 * dict;                 ///< Pointer to the dictionary.

    private:
        // This lists stores vectors that had been freed. Basically they
        // are stored to avoid freeeing and allocating vectors of the same
        // type when a chunk is compressed.
        static vector<TagVecValue64 *> listTagVecValue64;
        static vector<TagVecValue64NF *> listTagVecValue64NF;
        static vector<TagVecValue31 *> listTagVecValue31;
        static vector<TagVecValue8 *> listTagVecValue8;
        static vector<TagVecValue8NF *> listTagVecValue8NF;

        friend class TrackIDNode;
        friend class InternalTagHandler;
} ;

/**
 * Adds a value to the tag in the cycle cycle. Uses the cycle to
 * know which is the tag vector where the value must be stored.
 *
 * @return true if everything is ok.
 */
bool
TagIdVecNode::addTagValue(CYCLE cycle, UINT64 value)
{
    // We must add the value when no data is still entered, or when
    // the last value entered was an undefine or when the last value
    // is different than the actual value.
    if(lastValueUndefined || (lastValue != value))
    {
        lastCycle = QMAX(cycle.cycle, (INT32) lastCycle);
        firstCycle = QMIN((INT32) firstCycle, cycle.cycle);
        INT32 cycleChunk = cycle.cycle >> CYCLE_OFFSET_BITS;

        lastValueUndefined = false;
        lastValue = value;

        // check if there is a vector allocated for that...
        if(cycle.division == 0)
        {
            checkCycleChunk(cycleChunk, cycle.clock->getId(), &cycleVecHigh);
            return (* cycleVecHigh)[cycleChunk]->addTagValue(cycle.cycle, value);
        }
        else
        {
            checkCycleChunk(cycleChunk, cycle.clock->getId(), &cycleVecLow);
            return (* cycleVecLow)[cycleChunk]->addTagValue(cycle.cycle, value);
        }
    }
    return true;
}

/**
 * Adds a value to the tag in the cycle cycle. Uses the cycle to
 * know which is the tag vector where the value must be stored.
 *
 * @return true if everything is ok.
 */
bool
TagIdVecNode::addTagValue(CYCLE cycle, SOVList * value)
{
    // TODO: implement and think how to store with few bits the value...
    // maybe reuse the value as a pointer to an allocated space with
    // the SOV.
    return false;
/*    // We must add the value when the last value entered was an
    // undefine or when the last value is different than the actual value.
    if(lastValueUndefined || (lastValue != value))
    {
        lastCycle = QMAX(cycle.cycle, lastCycle);
        firstCycle = QMIN(firstCycle, cycle.cycle);
        INT32 cycleChunk = cycle.cycle >> CYCLE_OFFSET_BITS;

        lastValueUndefined = false;
        // check if there is a vector allocated for that...
        if(cycle.division == 0)
        {
            checkCycleChunk(cycleChunk, cycle.clock->getId(), &cycleVecHigh);
            return (* cycleVecHigh)[cycleChunk]->addTagValue(cycle.cycle, value);
        }
        else
        {
            checkCycleChunk(cycleChunk, cycle.clock->getId(), &cycleVecLow);
            return (* cycleVecLow)[cycleChunk]->addTagValue(cycle.cycle, value);
        }
    }*/
}

/**
 * Undefines the tag in the cycle cycle. Uses the cycle to
 * know which is the tag vector where the value must be stored.
 *
 * @return true if everything is ok.
 */
bool
TagIdVecNode::undefineTagValue(CYCLE cycle)
{
    // We undefine something when the last value was not an undefine.
    if(!lastValueUndefined)
    {
        lastCycle = QMAX(cycle.cycle, (INT32) lastCycle);
        firstCycle = QMIN((INT32) firstCycle, cycle.cycle);
        INT32 cycleChunk = cycle.cycle >> CYCLE_OFFSET_BITS;

        lastValueUndefined = true;
        // check if there is a vector allocated for that...
        if(cycle.division == 0)
        {
            checkCycleChunk(cycleChunk, cycle.clock->getId(), &cycleVecHigh);
            return (* cycleVecHigh)[cycleChunk]->undefineTagValue(cycle.cycle);
        }
        else
        {
            checkCycleChunk(cycleChunk, cycle.clock->getId(), &cycleVecLow);
            return (* cycleVecLow)[cycleChunk]->undefineTagValue(cycle.cycle);
        }
    }
    return true;
}

/*
 * Checks if the array with the values has already been allocated.
 *
 * @return if the array has been allocated.
 */
bool
TagIdVecNode::hasData() const
{
    return (hasLowData() || hasHighData());
}

/*
 * Checks if the array with the values has already been allocated
 * for high values.
 *
 * @return if the array has been allocated.
 */
bool
TagIdVecNode::hasHighData() const
{
    return (cycleVecHigh != NULL);
}

/*
 * Checks if the array with the values has already been allocated
 * for low values.
 *
 * @return if the array has been allocated.
 */
bool
TagIdVecNode::hasLowData() const
{
    return (cycleVecLow != NULL);
}

/**
 * Sets to forward the behaviour of the vector.
 *
 * @return void.
 */
void
TagIdVecNode::setFwd(bool value)
{
    // you cannot change behavior AFTER infomation has been put on the vector.
    Q_ASSERT((cycleVecHigh == NULL) && (cycleVecLow == NULL));
    fwd = (UINT8) value;
}

/**
 * Sets to dictionary the behaviour of the vector.
 *
 * @return void.
 */
void
TagIdVecNode::setSize(TagSizeStorage value)
{
    // you cannot change behavior AFTER infomation has been put on the vector.
    Q_ASSERT((cycleVecHigh == NULL) && (cycleVecLow == NULL));
    size = (UINT8) value;
}

#endif

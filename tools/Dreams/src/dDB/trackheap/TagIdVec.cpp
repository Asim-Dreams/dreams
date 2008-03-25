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
  * @file TagIdVec.cpp
  */

// Dreams includes.
#include "dDB/trackheap/TagIdVec.h"
#include "dDB/trackheap/TagVecValue64.h"
#include "dDB/trackheap/TagVecValue64NF.h"
#include "dDB/trackheap/TagVecValue31.h"
#include "dDB/trackheap/TagVecValue8.h"
#include "dDB/trackheap/TagVecValue8NF.h"

vector<TagVecValue64 *> TagIdVecNode::listTagVecValue64;
vector<TagVecValue64NF *> TagIdVecNode::listTagVecValue64NF;
vector<TagVecValue31 *> TagIdVecNode::listTagVecValue31;
vector<TagVecValue8 *> TagIdVecNode::listTagVecValue8;
vector<TagVecValue8NF *> TagIdVecNode::listTagVecValue8NF;

/*
 * Creator of this class. Gets the static instances and sets the
 * default values to the fields of the class.
 *
 * @return new object.
 */
TagIdVecNode::TagIdVecNode()
{
    dict = NULL;
    firstCycle = 1048575;
    lastCycle = 0;
    fwd = false;
    size = TAG_SIZE_64_BITS;
    cycleVecHigh = NULL;
    cycleVecLow = NULL;
    lastValueUndefined = true;
    lastValue = 0;
    lastCompressedChunk = 0;
}

/*
 * Destructor of this class. Frees the array if was allocated.
 *
 * @return destroys the object.
 */
TagIdVecNode::~TagIdVecNode()
{
    INT32 lastChunk = lastCycle >> CYCLE_OFFSET_BITS;

    if(cycleVecHigh != NULL)
    {
        for(INT32 i = 0; i <= lastChunk; i++)
        {
            if(cycleVecHigh->hasElement(i) && ((* cycleVecHigh)[i] != NULL))
            {
                delete (* cycleVecHigh)[i];
            }
        }

        delete cycleVecHigh;
    }

    if(cycleVecLow != NULL)
    {
        for(INT32 i = 0; i <= lastChunk; i++)
        {
            if(cycleVecLow->hasElement(i) && ((* cycleVecLow)[i] != NULL))
            {
                delete (* cycleVecLow)[i];
            }
        }

        delete cycleVecLow;
    }
}

/*
 * Gets the tag value in the cycle cycle.
 *
 * @return bool if a value is found.
 */
bool
TagIdVecNode::getTagValue(CYCLE cycle, UINT64 * value, CYCLE * atcycle) const
{
    INT32 cyc;        ///< Cycle when the value is set.
    INT8 division;    ///< Division where the tag is found.
    bool ok;          ///< If a value is found.

    division = 0;

    // If forwarding, then we must look at both vectors.
    if(fwd)
    {
        UINT64 value1, value2; ///< Values returned from the high and low searches.
        INT32 cyc1, cyc2;      ///< Cycles returned from the high and low searches.
        bool ok1, ok2;         ///< Something found returned from the high and low searches.

        // High phase access.
        ok1 = internalGetTagValue(cycleVecHigh, cycle.cycle, &value1, &cyc1);

        // Decrements if we are in a high phase, to maintain coherency: the last low value must be searched
        // in cycle.cycle - 1.
        cycle.cycle += cycle.division - 1;

        // Low phase access.
        ok2 = internalGetTagValue(cycleVecLow, cycle.cycle, &value2, &cyc2);

        // Sets the ok result.
        ok = ok1 || ok2;

        // Gets the correct value.
        if(ok1 && !ok2)
        {
            // Only have a value in the high phase.
            * value = value1;
            cyc = cyc1;
            division = 0;
        }
        else if(!ok1 && ok2)
        {
            // Only have a value in the low phase.
            * value = value2;
            cyc = cyc2;
            division = 1;
        }
        else if(ok1 && ok2)
        {
            // A value in both phases, we must get the older one.
            // If equal cycles, we get the low phase value.
            if(cyc1 > cyc2)
            {
                * value = value1;
                cyc = cyc1;
                division = 0;
            }
            else
            {
                * value = value2;
                cyc = cyc2;
                division = 1;
            }
        }
    }
    else
    {
        if(cycle.division == 0)
        {
            ok = internalGetTagValue(cycleVecHigh, cycle.cycle, value, &cyc);
        }
        else
        {
            ok = internalGetTagValue(cycleVecLow, cycle.cycle, value, &cyc);
        }
    }

    // Sets the atcycle if exists.
    if(ok && (atcycle != NULL))
    {
        atcycle->clock = cycle.clock;
        atcycle->division = division;
        atcycle->cycle = cyc;
    }

    return ok;
}

/*
 * Gets the tag value in the cycle cycle.
 *
 * @return bool if a value is found.
 */
bool
TagIdVecNode::getTagValue(CYCLE cycle, SOVList ** value, CYCLE * atcycle) const
{
    /// @todo implement this
    return false;
}

/*
 * Gets the tag value in the cycle cycle.
 *
 * @return bool if a value is found.
 */
bool
TagIdVecNode::getTagValuePhase(CYCLE cycle, UINT64 * value, CYCLE * atcycle) const
{
    INT32 cyc; ///< Cycle when the value is set.
    bool ok;   ///< If a value is found.

    if(cycle.division == 0)
    {
        ok = internalGetTagValue(cycleVecHigh, cycle.cycle, value, &cyc);
    }
    else
    {
        ok = internalGetTagValue(cycleVecLow, cycle.cycle, value, &cyc);
    }

    // Sets the atcycle if exists.
    if(ok && (atcycle != NULL))
    {
        atcycle->clock = cycle.clock;
        atcycle->division = cycle.division;
        atcycle->cycle = cyc;
    }

    return ok;
}

/*
 * Gets the tag value in the cycle cycle.
 *
 * @return bool if a value is found.
 */
bool
TagIdVecNode::getTagValuePhase(CYCLE cycle, SOVList ** value, CYCLE * atcycle) const
{
    /// @todo implement this
    return false;
}

/*
 * Gets the tag value in the cycle cycle.
 *
 * @return bool if a value is found.
 */
bool
TagIdVecNode::internalGetTagValue(const TagCycleVector * cycleVec, INT32 cycle, UINT64 * value, INT32 * atcycle) const
{
    TagReturn hit;    ///< Stores when we've found a value.
    INT32 cycleChunk; ///< Stores the actual chunk.
    INT32 minChunk;   ///< Stores the threshold chunk.

    hit = TAG_NOT_FOUND;

    if(cycleVec == NULL)
    {
        return false;
    }

    if(cycle < (INT32) firstCycle)
    {
        return false;
    }
    if(cycle > (INT32) lastCycle)
    {
        if(!fwd)
        {
            return false;
        }
        else
        {
            cycle = lastCycle;
        }
    }

    cycleChunk = cycle >> CYCLE_OFFSET_BITS;

    if(cycleVec->hasElement(cycleChunk) && ((* cycleVec)[cycleChunk] != NULL))
    {
        hit = (* cycleVec)[cycleChunk]->getTagValue(cycle, value, atcycle);
        if(hit == TAG_FOUND)
        {
            return true;
        }
        else if((hit == TAG_UNDEFINED) || !fwd)
        {
            return false;
        }
    }
    else if(!fwd)
    {
        return false;
    }

    cycleChunk = cycle >> CYCLE_OFFSET_BITS;
    // If the value was not fnd in the "natural" chunk look back.
    minChunk = firstCycle >> CYCLE_OFFSET_BITS;

    cycleChunk--;

    while((hit == TAG_NOT_FOUND) && (cycleChunk >= minChunk))
    {
        if(cycleVec->hasElement(cycleChunk) && ((* cycleVec)[cycleChunk] != NULL))
        {
            hit = (* cycleVec)[cycleChunk]->getTagValue(cycle, value, atcycle);
        }
        cycleChunk--;
    }

    return (hit == TAG_FOUND);
}

/**
 * Dumps the content of the vector.
 *
 * @return void.
 */
void
TagIdVecNode::dumpTagIdVector()
{
    if(!hasData())
    {
        return;
    }

    INT32 firstChunk = firstCycle >> CYCLE_OFFSET_BITS;
    INT32 lastChunk = lastCycle >> CYCLE_OFFSET_BITS;

    if(cycleVecHigh != NULL)
    {
        for(INT32 i = firstChunk; i <= lastChunk; i++)
        {
            if(cycleVecHigh->hasElement(i) && ((*cycleVecHigh)[i] != NULL))
            {
                (*cycleVecHigh)[i]->dumpCycleVector();
            }
        }
    }
    if(cycleVecLow != NULL)
    {
        for(INT32 i = firstChunk; i <= lastChunk; i++)
        {
            if(cycleVecLow->hasElement(i) && ((*cycleVecLow)[i] != NULL))
            {
                (*cycleVecLow)[i]->dumpCycleVector();
            }
        }
    }
}

/*
 * Gets the occupancy for the given tag.
 *
 * @return the occupancy.
 */
UINT32
TagIdVecNode::getOccupancy() const
{
    UINT32 occ = 0;

    if(cycleVecHigh != NULL)
    {
        occ += internalGetOccupancy(cycleVecHigh);
    }
    if(cycleVecLow != NULL)
    {
        occ += internalGetOccupancy(cycleVecLow);
    }
    return occ;
}

UINT32
TagIdVecNode::internalGetOccupancy(const TagCycleVector * cycleVec) const
{
    UINT32 occ = 0;
    INT32 currEndCycle = 4095;
    INT32 lastChunk = lastCycle >> CYCLE_OFFSET_BITS;
    UINT64 dummyValue;
    INT32 dummyAtCycle;
    bool defined = false;

    for(INT32 i = 0; i <= lastChunk; i++)
    {
        if(cycleVec->hasElement(i) && ((* cycleVec)[i] != NULL))
        {
            if(i != lastChunk)
            {
                occ += (* cycleVec)[i]->getOccupancy(defined, CYCLE_CHUNK_SIZE);
            }
            else
            {
                occ += (* cycleVec)[i]->getOccupancy(defined, lastCycle & CYCLE_OFFSET_MASK);
            }
            TagReturn ret = (* cycleVec)[i]->getTagValue(currEndCycle, &dummyValue, &dummyAtCycle);
            if(ret == TAG_FOUND)
            {
                defined = true;
            }
            else if(ret == TAG_UNDEFINED)
            {
                defined = false;
            }
        }
        currEndCycle += CYCLE_CHUNK_SIZE;
    }
    return occ;
}

/*
* Compresses the vector.
*
* @return the compressed vector.
*/
ZipObject *
TagIdVecNode::compressYourSelf(CYCLE cycle, bool last)
{
    if(!hasData())
    {
        return this;
    }

    INT32 lastChunk = cycle.cycle >> CYCLE_OFFSET_BITS;

    if(lastChunk >= TAGIDVECNODE_MAXCHUNKS)
    {
        lastChunk = TAGIDVECNODE_MAXCHUNKS - 1;
    } 

    if(cycleVecHigh != NULL)
    {
        for(INT32 i = lastCompressedChunk; i <= lastChunk; i++)
        {
            if(cycleVecHigh->hasElement(i) && ((* cycleVecHigh)[i] != NULL))
            {
                TagVec * newvec = (TagVec *) ((* cycleVecHigh)[i]->compressYourSelf(cycle, last));
                if((* cycleVecHigh)[i] != newvec)
                {
                    switch(size)
                    {
                        case TAG_SIZE_64_BITS:
                            if(fwd)
                            {
                                listTagVecValue64.push_back((TagVecValue64 *) (* cycleVecHigh)[i]);
                            }
                            else
                            {
                                listTagVecValue64NF.push_back((TagVecValue64NF *) (* cycleVecHigh)[i]);
                            }
                            break;

                        case TAG_SIZE_31_BITS:
                            if(fwd)
                            {
                                listTagVecValue31.push_back((TagVecValue31 *) (* cycleVecHigh)[i]);
                            }
                            else
                            {
                                Q_ASSERT(false);
                            }
                            break;

                        case TAG_SIZE_8_BITS:
                            if(fwd)
                            {
                                listTagVecValue8.push_back((TagVecValue8 *) (* cycleVecHigh)[i]);
                            }
                            else
                            {
                                listTagVecValue8NF.push_back((TagVecValue8NF *) (* cycleVecHigh)[i]);
                            }
                            break;

                        default:
                            // Sanity check.
                            Q_ASSERT(false);
                            break;
                    }

                    (* cycleVecHigh)[i] = newvec;
                }
            }
        }
    }

    if(cycleVecLow != NULL)
    {
        for(INT32 i = lastCompressedChunk; i <= lastChunk; i++)
        {
            if(cycleVecLow->hasElement(i) && ((* cycleVecLow)[i] != NULL))
            {
                TagVec * newvec = (TagVec *) ((* cycleVecLow)[i]->compressYourSelf(cycle, last));
                if((* cycleVecLow)[i] != newvec)
                {
                    switch(size)
                    {
                        case TAG_SIZE_64_BITS:
                            if(fwd)
                            {
                                listTagVecValue64.push_back((TagVecValue64 *) (* cycleVecLow)[i]);
                            }
                            else
                            {
                                listTagVecValue64NF.push_back((TagVecValue64NF *) (* cycleVecLow)[i]);
                            }
                            break;

                        case TAG_SIZE_31_BITS:
                            if(fwd)
                            {
                                listTagVecValue31.push_back((TagVecValue31 *) (* cycleVecLow)[i]);
                            }
                            else
                            {
                                Q_ASSERT(false);
                            }
                            break;

                        case TAG_SIZE_8_BITS:
                            if(fwd)
                            {
                                listTagVecValue8.push_back((TagVecValue8 *) (* cycleVecLow)[i]);
                            }
                            else
                            {
                                listTagVecValue8NF.push_back((TagVecValue8NF *) (* cycleVecLow)[i]);
                            }
                            break;

                        default:
                            // Sanity check.
                            Q_ASSERT(false);
                            break;
                    }

                    (* cycleVecLow)[i] = newvec;
                }
            }
        }
    }

    lastCompressedChunk = lastChunk + 1;
    return this;
}

/**
 * Checks that a cycle chunk is allocated. First looks if the
 * array has been allocated. Then looks if the chunk is allocated.
 * In negative case is allocated and all the pointers are set to
 * NULL. Finally allocates the correct type of tag vector.
 *
 * @return void.
 */
void
TagIdVecNode::checkCycleChunk(INT32 cycleChunk, CLOCK_ID clock_id, TagCycleVector ** cycleVecParam)
{
    bool defined;
    UINT64 value;
    TagCycleVector * cycleVec;

    // first time we use this tagId?
    if((* cycleVecParam) == NULL)
    {
        * cycleVecParam = new TagCycleVector();
    }

    cycleVec = * cycleVecParam;

    // since TagCycleVector is a vector of pointers I need to
    // make sure I initialize it with NULLs but, at the same time
    // I'd like to avoid initialize (and therefore allocate)
    // uneeded pages on the AEVector so:

    // No allocated page?
    if(!(cycleVec->hasElement(cycleChunk)))
    {
        // Allocate it.
        cycleVec->allocateElement(cycleChunk);

        // Clear the AE page to avoid dangling/trash pointers.
        INT32 aepage = cycleChunk / TAGIDVECNODE_AEPAGESIZE;
        INT32 aepageElem0 = aepage * TAGIDVECNODE_AEPAGESIZE;  // improve this with a mask
        for(UINT32 i = 0; i < TAGIDVECNODE_AEPAGESIZE; i++)
        {
            (* cycleVec)[aepageElem0 + i] = NULL;
        }
    }

    if((* cycleVec)[cycleChunk] == NULL)
    {
        // Get some TagVec implementation.
        switch(size)
        {
            case TAG_SIZE_64_BITS:
                if(fwd)
                {
                    if(!listTagVecValue64.empty())
                    {
                        TagVecValue64 * tag = listTagVecValue64.back();
                        listTagVecValue64.pop_back();
                        tag->reset(cycleChunk * CYCLE_CHUNK_SIZE);
                        tag->setNewDict(dict);
                        (* cycleVec)[cycleChunk] = tag;
                    }
                    else
                    {
                        (* cycleVec)[cycleChunk] = new TagVecValue64(cycleChunk * CYCLE_CHUNK_SIZE, dict, clock_id);
                    }
                }
                else
                {
                    if(!listTagVecValue64NF.empty())
                    {
                        TagVecValue64NF * tag = listTagVecValue64NF.back();
                        listTagVecValue64NF.pop_back();
                        tag->reset(cycleChunk * CYCLE_CHUNK_SIZE);
                        tag->setNewDict(dict);
                        (* cycleVec)[cycleChunk] = tag;
                    }
                    else
                    {
                        (* cycleVec)[cycleChunk] = new TagVecValue64NF(cycleChunk * CYCLE_CHUNK_SIZE, dict, clock_id);
                    }
                }
                break;

            case TAG_SIZE_31_BITS:
                if(fwd)
                {
                    if(!listTagVecValue31.empty())
                    {
                        TagVecValue31 * tag = listTagVecValue31.back();
                        listTagVecValue31.pop_back();
                        tag->reset(cycleChunk * CYCLE_CHUNK_SIZE);
                        (* cycleVec)[cycleChunk] = tag;
                    }
                    else
                    {
                        (* cycleVec)[cycleChunk] = new TagVecValue31(cycleChunk * CYCLE_CHUNK_SIZE);
                    }
                }
                else
                {
                    // TODO: implement this class.
                    Q_ASSERT(false);
                    //(*cycleVec)[cycleChunk] = new TagVecValue31NF(cycleChunk * CYCLE_CHUNK_SIZE);
                }
                break;

            case TAG_SIZE_8_BITS:
                if(fwd)
                {
                    if(!listTagVecValue8.empty())
                    {
                        TagVecValue8 * tag = listTagVecValue8.back();
                        listTagVecValue8.pop_back();
                        tag->reset(cycleChunk * CYCLE_CHUNK_SIZE);
                        (* cycleVec)[cycleChunk] = tag;
                    }
                    else
                    {
                        (* cycleVec)[cycleChunk] = new TagVecValue8(cycleChunk * CYCLE_CHUNK_SIZE);
                    }
                }
                else
                {
                    if(!listTagVecValue8NF.empty())
                    {
                        TagVecValue8NF * tag = listTagVecValue8NF.back();
                        listTagVecValue8NF.pop_back();
                        tag->reset(cycleChunk * CYCLE_CHUNK_SIZE);
                        (* cycleVec)[cycleChunk] = tag;
                    }
                    else
                    {
                        (* cycleVec)[cycleChunk] = new TagVecValue8NF(cycleChunk * CYCLE_CHUNK_SIZE);
                    }
                }
                break;

            default:
                // Sanity check.
                Q_ASSERT(false);
                break;
        }
    }
}

/*
 * Purges all the allocated vectors that hasn't yet been deleted.
 *
 * @return void.
 */
void
TagIdVecNode::purgeAllocatedVectors()
{
    {
        vector<TagVecValue64 *>::iterator it;

        it = listTagVecValue64.begin();
        while(it != listTagVecValue64.end())
        {
            delete (* it);
            ++it;
        }
        listTagVecValue64.clear();
    }
    {
        vector<TagVecValue64NF *>::iterator it;

        it = listTagVecValue64NF.begin();
        while(it != listTagVecValue64NF.end())
        {
            delete (* it);
            ++it;
        }
        listTagVecValue64NF.clear();
    }
    {
        vector<TagVecValue31 *>::iterator it;

        it = listTagVecValue31.begin();
        while(it != listTagVecValue31.end())
        {
            delete (* it);
            ++it;
        }
        listTagVecValue31.clear();
    }
    {
        vector<TagVecValue8 *>::iterator it;

        it = listTagVecValue8.begin();
        while(it != listTagVecValue8.end())
        {
            delete (* it);
            ++it;
        }
        listTagVecValue8.clear();
    }
    {
        vector<TagVecValue8NF *>::iterator it;

        it = listTagVecValue8NF.begin();
        while(it != listTagVecValue8NF.end())
        {
            delete (* it);
            ++it;
        }
        listTagVecValue8NF.clear();
    }
}

INT64
TagIdVecNode::getObjSize() const
{
    INT64 size;

    size = sizeof(TagIdVecNode);

    // Adds all the tag vectors.
    if(cycleVecHigh != NULL)
    {
        size += cycleVecHigh->getObjSize() - sizeof(cycleVecHigh);

        INT32 firstChunk = firstCycle >> CYCLE_OFFSET_BITS;
        INT32 lastChunk = lastCycle >> CYCLE_OFFSET_BITS;

        // check incoherence conditions (typically produced by corrupted drl files)
        if(lastChunk >= TAGIDVECNODE_MAXCHUNKS)
        {
            lastChunk = TAGIDVECNODE_MAXCHUNKS - 1;
        } 

        INT32 lim = (lastChunk / TAGIDVECNODE_AEPAGESIZE) + 1;
        lim = lim * TAGIDVECNODE_AEPAGESIZE;
        for(INT32 i = 0; i < lim; i++)
        {
            if(cycleVecHigh->hasElement(i))
            {
                if((* cycleVecHigh)[i] != NULL)
                {
                    size += (* cycleVecHigh)[i]->getObjSize();
                }
            }
        }
    }
    // Adds all the tag vectors.
    if(cycleVecLow != NULL)
    {
        size += cycleVecLow->getObjSize() - sizeof(cycleVecLow);

        INT32 firstChunk = firstCycle >> CYCLE_OFFSET_BITS;
        INT32 lastChunk = lastCycle >> CYCLE_OFFSET_BITS;

        // check incoherence conditions (typically produced by corrupted drl files)
        if(lastChunk >= TAGIDVECNODE_MAXCHUNKS)
        {
            lastChunk = TAGIDVECNODE_MAXCHUNKS - 1;
        } 

        INT32 lim = (lastChunk / TAGIDVECNODE_AEPAGESIZE) + 1;
        lim = lim * TAGIDVECNODE_AEPAGESIZE;
        for(INT32 i = 0; i < lim; i++)
        {
            if(cycleVecLow->hasElement(i))
            {
                if((* cycleVecLow)[i] != NULL)
                {
                    size += (* cycleVecLow)[i]->getObjSize();
                }
            }
        }
    }
    return size;
}

QString
TagIdVecNode::getUsageDescription() const
{
    QString ret;
    INT64 size;

    ret += QString("\t\t\tThis TagIdVecNode object occupies ") + QString::number(getObjSize()) + QString(" bytes.\n");
    if(cycleVecHigh != NULL)
    {
        ret += QString("\t\t\tDescription of the different tagvec high elements (") + QString::number(cycleVecHigh->getNumSegments() * cycleVecHigh->getSegmentSize()) + QString(" elements allocated with a total size of ") + QString::number(cycleVecHigh->getObjSize()) + QString(" bytes).\n");
    }
    if(cycleVecLow != NULL)
    {
        ret += QString("\t\t\tDescription of the different tagvec high elements (") + QString::number(cycleVecLow->getNumSegments() * cycleVecLow->getSegmentSize()) + QString(" elements allocated with a total size of ") + QString::number(cycleVecLow->getObjSize()) + QString(" bytes).\n");
    }

    return ret;
}

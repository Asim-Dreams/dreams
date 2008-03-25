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
  * @file TrackVec.h
  */

#ifndef _DRALDB_TRACKVEC_H
#define _DRALDB_TRACKVEC_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/TagDescVector.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/GVector.h"
#include "dDB/aux/ZipObject.h"
#include "dDB/trackheap/TrackHeapDef.h"
#include "dDB/trackheap/TagIdVec.h"
#include "dDB/trackheap/TagRenameTable.h"

/*
 * @brief
 * This class holds pointers to the different tags for a track.
 *
 * @description
 * The class extends a growing vector that dynamically allocates
 * more space as needed. The new allocated tags are set to NULL to
 * maintain coherency.
 *
 * @version 0.1
 * @date started at 2004-10-14
 * @author Guillem Sole
 */
class TagIdVector : public GVector<TagIdVecNode *>
{
    protected:

        /*
         * This function is called to set the default value of the new
         * slots created.
         *
         * @return the default value.
         */
        TagIdVecNode *
        defaultValue() const
        {
            return (TagIdVecNode *) NULL;
        }

    public:

        /*
         * Creator of this class. Forwards the function.
         *
         * @return the new object.
         */
        TagIdVector(UINT32 init_size)
            : GVector<TagIdVecNode *>(init_size)
        {
            resetChunk(0, init_size);
        }
} ;

/**
  * @brief
  * This class tracks all the nodes and items requested.
  *
  * @description
  * All the elements of the database that the client want to
  * be tracked are stored in this vector. Each element has another
  * vector with the tracking of all of each tags.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class TrackIDNode : public AMemObj, public ZipObject
{
    public:
        TrackIDNode();
        virtual ~TrackIDNode();

        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;

        ZipObject * compressYourSelf(CYCLE cycle, bool last = false);

        inline bool getTagValue(TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle) const;
        inline bool getTagValue(TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle) const;
        inline bool getTagValuePhase(TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle) const;
        inline bool getTagValuePhase(TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle) const;

        inline bool addTagValue(TAG_ID tagId, CYCLE cycle, UINT64 value);
        inline bool addTagValue(TAG_ID tagId, CYCLE cycle, SOVList * value);
        inline bool undefineTagValue(TAG_ID tagId, CYCLE cycle);

        inline UINT32 getTagOccupancy(TAG_ID tagId) const;

        TagIDList * getKnownTagIDs() const;
        inline CLOCK_ID getClockId() const;

        void dumpRegression();

        inline void compressTag(TAG_ID tagId, CYCLE cycle, bool last);

    private:
        TagRenameTable * trt;       ///< Table that renames the sparse tag id to a collapsed tag id.
        TagIdVector tgIdVector;     ///< Array to do the tracking of all the tags.
        UINT16 clockId;             ///< Clock domain of the node.
        Dict2064 * dict;            ///< Pointer to the dictionary.
        TagDescVector * tagdescvec; ///< Pointer to the tag description pointer.

        friend class TrackHeap;
};

/** @typedef TrackIDVector
  * @brief
  * Array that holds all the elements that are tracked in the
  * database. A maximum of 256k elements allowed.
  */
typedef AEVector<TrackIDNode, 1024, 256> TrackIDVector;

/*
 * Gets the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is found.
 */
bool
TrackIDNode::getTagValue(TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle) const
{
    TagIdVecNode * tag;

    // If the tag is not already renamed, then no value is defined.
    if(!trt->isRenamed(tagId))
    {
        return false;
    }

    // Gets the renamed tag.
    tag = tgIdVector.constAccess(trt->rename(tagId));

    // If the tag is not allocated, then no value is found.
    if(tag == NULL)
    {
        return false;
    }
    return tag->getTagValue(cycle, value, atcycle);
}

/*
 * Gets the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is found.
 */
bool
TrackIDNode::getTagValue(TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle) const
{
    TagIdVecNode * tag; ///< Pointer to the current tag.

    // If the tag is not already renamed, then no value is defined.
    if(!trt->isRenamed(tagId))
    {
        return false;
    }

    // Gets the renamed tag.
    tag = tgIdVector.constAccess(trt->rename(tagId));

    // If the tag is not allocated, then no value is found.
    if(tag == NULL)
    {
        return false;
    }
    return tag->getTagValue(cycle, value, atcycle);
}

/*
 * Gets the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is found.
 */
bool
TrackIDNode::getTagValuePhase(TAG_ID tagId, CYCLE cycle, UINT64 * value, CYCLE * atcycle) const
{
    TagIdVecNode * tag;

    // If the tag is not already renamed, then no value is defined.
    if(!trt->isRenamed(tagId))
    {
        return false;
    }

    // Gets the renamed tag.
    tag = tgIdVector.constAccess(trt->rename(tagId));

    // If the tag is not allocated, then no value is found.
    if(tag == NULL)
    {
        return false;
    }
    return tag->getTagValuePhase(cycle, value, atcycle);
}

/*
 * Gets the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is found.
 */
bool
TrackIDNode::getTagValuePhase(TAG_ID tagId, CYCLE cycle, SOVList ** value, CYCLE * atcycle) const
{
    TagIdVecNode * tag; ///< Pointer to the current tag.

    // If the tag is not already renamed, then no value is defined.
    if(!trt->isRenamed(tagId))
    {
        return false;
    }

    // Gets the renamed tag.
    tag = tgIdVector.constAccess(trt->rename(tagId));

    // If the tag is not allocated, then no value is found.
    if(tag == NULL)
    {
        return false;
    }
    return tag->getTagValuePhase(cycle, value, atcycle);
}

/**
 * Adds the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is added correctly.
 */
bool
TrackIDNode::addTagValue(TAG_ID tagId, CYCLE cycle, UINT64 value)
{
    UINT16 tag_ren;     ///< The renamed tag value.
    TagIdVecNode * tag; ///< Pointer to the current tag.

    tag_ren = trt->rename(tagId);
    tag = tgIdVector[tag_ren];

    // If the tag was not allocated.
    if(tag == NULL)
    {
        // Allocates a new one.
        tag = new TagIdVecNode();
        tgIdVector[tag_ren] = tag;

        // Copies some needed variables.
        tag->dict = dict;

        // Sets the behaviour of the tag.
        tag->setFwd(tagdescvec->getTagForwarding(tagId));
        tag->setSize(tagdescvec->getTagSize(tagId));
    }
    return tag->addTagValue(cycle, value);
}

/**
 * Adds the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is added correctly.
 */
bool
TrackIDNode::addTagValue(TAG_ID tagId, CYCLE cycle, SOVList * value)
{
    UINT16 tag_ren;     ///< The renamed tag value.
    TagIdVecNode * tag; ///< Pointer to the current tag.

    tag_ren = trt->rename(tagId);
    tag = tgIdVector[tag_ren];

    // If the tag was not allocated.
    if(tag == NULL)
    {
        // Allocates a new one.
        tag = new TagIdVecNode();
        tgIdVector[tag_ren] = tag;

        // Copies some needed variables.
        tag->dict = dict;

        // Sets the behaviour of the tag.
        tag->setFwd(tagdescvec->getTagForwarding(tagId));
        tag->setSize(tagdescvec->getTagSize(tagId));
    }
    return tag->addTagValue(cycle, value);
}

/**
 * Undefines the value of the tag tagId in the cycle cycle.
 *
 * @return true if a value is added correctly.
 */
bool
TrackIDNode::undefineTagValue(TAG_ID tagId, CYCLE cycle)
{
    UINT16 tag_ren;     ///< The renamed tag value.
    TagIdVecNode * tag; ///< Pointer to the current tag.

    tag_ren = trt->rename(tagId);
    tag = tgIdVector[tag_ren];

    // If the tag was not allocated.
    if(tag == NULL)
    {
        // Allocates a new one.
        tag = new TagIdVecNode();
        tgIdVector[tag_ren] = tag;

        // Copies some needed variables.
        tag->dict = dict;

        // Sets the behaviour of the tag.
        tag->setFwd(tagdescvec->getTagForwarding(tagId));
        tag->setSize(tagdescvec->getTagSize(tagId));
    }
    return tag->undefineTagValue(cycle);
}

/*
 * Gets the occupancy for the given tag.
 *
 * @return the occupancy.
 */
UINT32
TrackIDNode::getTagOccupancy(TAG_ID tagId) const
{
    TagIdVecNode * tag;

    // If the tag is not already renamed, then no value is defined.
    if(!trt->isRenamed(tagId))
    {
        return false;
    }

    // Gets the renamed tag.
    tag = tgIdVector.constAccess(trt->rename(tagId));

    // If the tag is not allocated, then no value is found.
    if(tag == NULL)
    {
        return 0;
    }
    return tag->getOccupancy();
}

/*
 * Returns the clock domain of this track.
 *
 * @return the clock id.
 */
CLOCK_ID
TrackIDNode::getClockId() const
{
    return clockId;
}

/*
 * Compresses a tag.
 *
 * @return void.
 */
void
TrackIDNode::compressTag(TAG_ID tagId, CYCLE cycle, bool last)
{
    if(trt->isRenamed(tagId))
    {
        TagIdVecNode * tag; ///< Pointer to the current tag.

        tag = tgIdVector.constAccess(trt->rename(tagId));
        if(tag != NULL)
        {
            tag->compressYourSelf(cycle, last);
        }
    }
}

#endif

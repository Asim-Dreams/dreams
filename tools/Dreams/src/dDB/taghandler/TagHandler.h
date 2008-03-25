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
  * @file TagHandler.h
  * @brief
  */

#ifndef _DRALDB_TAGHANDLER_H
#define _DRALDB_TAGHANDLER_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/ClockDomainMgr.h"

// Class forwarding.
class TagVec;
class TagIdVecNode;

/*
 * @brief
 * This class is an iterator to access sequentially or with 
 * positive increments to the values of a tag.
 *
 * @description
 * As a tag can be stored in two tag vectors, this interface
 * hides the use of one or two tag vectors. Two implementations
 * exist: one with just a tag vector, that just forwards the
 * calls and another with two tag vectors, that must sort them
 * after each call to maintain coherency.
 *
 * @author Guillem Sole
 * @date started at 2005-01-28
 * @version 0.1
 */
class TagHandler
{
    public:
        virtual TRACK_ID getTrackId() const = 0;
        virtual TAG_ID getTagId() const = 0;

        virtual UINT64 getTagValue() const = 0;
        virtual CYCLE getTagCycle() const = 0;
        virtual bool getTagDefined() const = 0;

        virtual bool isValidHandler() const = 0;

        virtual void nextCycle() = 0;
        virtual void skipToNextCycleWithChange() = 0;
        virtual void rewindToFirstCycle() = 0;

        virtual void prevCycle() = 0;
        virtual void skipToPrevCycleWithChange() = 0;
        virtual void forwardToLastCycle() = 0;

        virtual void skipToCycle(CYCLE cycle) = 0;
        virtual void skipToCycleDefined(CYCLE cycle) = 0;
} ;

/*
 * @brief
 * This class is an iterator to access sequentially or with 
 * positive increments to the values of a tag.
 *
 * @description
 * This is a very buggy class because depends on the whole track
 * database. If any of these classes is changed, this class must
 * be changed to in order to avoid buggy conditions. 
 *
 * @author Guillem Sole
 * @date started at 2004-07-30
 * @version 0.1
 */
class InternalTagHandler : public TagHandler
{
    private:
        InternalTagHandler(const ClockDomainEntry * clock, INT8 division);
        virtual ~InternalTagHandler();

    public:
        inline TRACK_ID getTrackId() const;
        inline TAG_ID getTagId() const;

        inline UINT64 getTagValue() const;
        inline CYCLE getTagCycle() const;
        inline bool getTagDefined() const;

        inline bool isValidHandler() const;

        void nextCycle();
        void skipToNextCycleWithChange();
        void rewindToFirstCycle();

        void prevCycle();
        void skipToPrevCycleWithChange();
        void forwardToLastCycle();

        void skipToCycle(CYCLE cycle);
        void skipToCycleDefined(CYCLE cycle);

    protected:
        void init(TagIdVecNode * _tag, INT32 _base_cycle, INT8 _division);
        inline void invalid();

        friend class TrackHeap;
        friend class DualTagHandler;
        friend class TagVecValue64;
        friend class TagVecValue64Dense;
        friend class TagVecValue64NF;
        friend class TagVecValue64NFDense;
        friend class TagVecValue31;
        friend class TagVecValue31Dense;
        friend class TagVecValue31Short;
        friend class TagVecValue8;
        friend class TagVecValue8Dense;
        friend class TagVecValue8NF;
        friend class TagVecValue8NFDense;

    protected:
        TRACK_ID track_id;   ///< Track id where the handler is pointing.
        TAG_ID tag_id;       ///< Tag id where the handler is pointing.
        UINT16 chunk;        ///< Actual tag vector chunk.
        INT32 base_cycle;    ///< Base cycle of the tag handler.
        CYCLE act_cycle;     ///< Cycle where the handler is pointing.
        INT16 act_entry;     ///< Actual entry of the tag vecor.
        UINT64 act_value;    ///< The actual value.
        bool act_defined;    ///< If the actual value is defined or not.
        bool is_fwd;         ///< True if this tag is a forwarding tag.
        INT8 division;       ///< Working division of the tag.
        TagIdVecNode * tag;  ///< Pointer to the tag vector node where the values are stored.
        TagVec * vec;        ///< Pointer to the actual tag vector.
} ;

/*
 * @brief
 * This class is an iterator to access sequentially or with 
 * positive increments to the values of a tag. This class is
 * used in tags that has values in both the high and low phases.
 *
 * @description
 * As a tag can be set in both the high and low phase, and the
 * values are stored in two tag vectors different, we need this
 * class to hide this access to both vectors. DualTagHandler has
 * a pointer to two InternalTagHandlers and another one pointing
 * to the actual of them (the one that has the value with the
 * smallest cycle). When a command that changes the cycle is called,
 * this is forwarded to the actual tag and then the order of the
 * two tags is reevaluated after this.
 *
 * @author Guillem Sole
 * @date started at 2004-07-30
 * @version 0.1
 */
class DualTagHandler : public TagHandler
{
    private:
        DualTagHandler(const ClockDomainEntry * clock);
        virtual ~DualTagHandler();

    public:
        inline TRACK_ID getTrackId() const;
        inline TAG_ID getTagId() const;

        inline UINT64 getTagValue() const;
        inline CYCLE getTagCycle() const;
        inline bool getTagDefined() const;

        inline bool isValidHandler() const;

        void nextCycle();
        void skipToNextCycleWithChange();
        void rewindToFirstCycle();

        void prevCycle();
        void skipToPrevCycleWithChange();
        void forwardToLastCycle();

        void skipToCycle(CYCLE cycle);
        void skipToCycleDefined(CYCLE cycle);

    protected:
        friend class TrackHeap;

    protected:
        InternalTagHandler * tagHigh; ///< Tag handler with the high values.
        InternalTagHandler * tagLow;  ///< Tag handler with the low values.
        InternalTagHandler * actual;  ///< Tag handler with the actual value.
} ;

/*
 * Gets the track id of the handler.
 *
 * @return the track id.
 */
TRACK_ID
InternalTagHandler::getTrackId() const
{
    return track_id;
}

/*
 * Gets the tag id of the handler.
 *
 * @return the tag id.
 */
TAG_ID
InternalTagHandler::getTagId() const
{
    return tag_id;
}

/*
 * Gets the actual value.
 *
 * @return the value.
 */
UINT64
InternalTagHandler::getTagValue() const
{
    return act_value;
}

/*
 * Gets the cycle of the handler.
 *
 * @return the cycle.
 */
CYCLE
InternalTagHandler::getTagCycle() const
{
    CYCLE ret = act_cycle;

    ret.cycle += base_cycle;
    return ret;
}

/*
 * Gets if the actual value is defined.
 *
 * @return if the actual value is defined.
 */
bool
InternalTagHandler::getTagDefined() const
{
    return act_defined;
}

/*
 * Returns if the handler is a valid handler.
 *
 * @return if is valid.
 */
bool
InternalTagHandler::isValidHandler() const
{
    return (tag != NULL);
}

/*
 * Invalidates the handler.
 *
 * @return void.
 */
void
InternalTagHandler::invalid()
{
    tag = NULL;
    vec = NULL;
    act_cycle.cycle = BIGGEST_CYCLE - base_cycle;
    act_defined = false;
    is_fwd = false;
    chunk = (UINT16) -1;
}

/*
 * Gets the track id of the handler.
 *
 * @return the track id.
 */
TRACK_ID
DualTagHandler::getTrackId() const
{
    return actual->getTrackId();
}

/*
 * Gets the tag id of the handler.
 *
 * @return the tag id.
 */
TAG_ID
DualTagHandler::getTagId() const
{
    return actual->getTagId();
}

/*
 * Gets the actual value.
 *
 * @return the value.
 */
UINT64
DualTagHandler::getTagValue() const
{
    return actual->getTagValue();
}

/*
 * Gets the cycle of the handler.
 *
 * @return the cycle.
 */
CYCLE
DualTagHandler::getTagCycle() const
{
    return actual->getTagCycle();
}

/*
 * Gets if the actual value is defined.
 *
 * @return if the actual value is defined.
 */
bool
DualTagHandler::getTagDefined() const
{
    return actual->getTagDefined();
}

/*
 * Returns if the handler is a valid handler.
 *
 * @return if is valid.
 */
bool
DualTagHandler::isValidHandler() const
{
    return actual->isValidHandler();
}

#endif

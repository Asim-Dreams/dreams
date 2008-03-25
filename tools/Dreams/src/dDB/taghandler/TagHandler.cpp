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
  * @file TagHandler.cpp
  * @brief
  */

// Dreams includes.
#include "dDB/taghandler/TagHandler.h"
#include "dDB/trackheap/TrackHeap.h"

/**
 * Creator of this class.
 *
 * @return new object.
 */
InternalTagHandler::InternalTagHandler(const ClockDomainEntry * clock, INT8 division)
{
    act_cycle.clock = clock;
    act_cycle.division = division;
    invalid();
}

/*
 * Destructor of this class.
 *
 * @return destroys the object.
 */
InternalTagHandler::~InternalTagHandler()
{
}

/*
 * Initializes the fields of the handler.
 *
 * @return void.
 */
void
InternalTagHandler::init(TagIdVecNode * _tag, INT32 _base_cycle, INT8 _division)
{
    tag = _tag;
    division = _division;
    if(tag != NULL)
    {
        is_fwd = tag->fwd;
    }
    base_cycle = _base_cycle;
}

/*
 * Moves the handler to the next cycle.
 *
 * @return void.
 */
void
InternalTagHandler::nextCycle()
{
    Q_ASSERT(act_cycle.cycle != (BIGGEST_CYCLE - base_cycle));

    // Increments the cycle.
    act_cycle.cycle++;

    // Checks if we've changed of chunk.
    if((act_cycle.cycle & (CYCLE_CHUNK_SIZE - 1)) == 0)
    {
        // Increments to the next chunk.
        chunk++;

        if(division == 0)
        {
            // Checks if the actual chunk has a tag vector defined (and hence at least one value).
            if((tag != NULL) && (tag->cycleVecHigh != NULL) && tag->cycleVecHigh->hasElement(chunk) && (tag->cycleVecHigh->at(chunk) != NULL))
            {
                // Gets the handler of the type of vector.
                vec = tag->cycleVecHigh->at(chunk);

                // Updates the actual element.
                act_entry = -1;
                vec->getActualValue(this);
            }
            else
            {
                vec = NULL;

                // If the tag values are not forwarded, the value is changed to undefined, else its value
                // renmains unchanged.
                if(!is_fwd)
                {
                    act_defined = false;
                }
            }
        }
        else
        {
            // Checks if the actual chunk has a tag vector defined (and hence at least one value).
            if((tag != NULL) && (tag->cycleVecLow != NULL) && tag->cycleVecLow->hasElement(chunk) && (tag->cycleVecLow->at(chunk) != NULL))
            {
                // Gets the handler of the type of vector.
                vec = tag->cycleVecLow->at(chunk);

                // Updates the actual element.
                act_entry = -1;
                vec->getActualValue(this);
            }
            else
            {
                vec = NULL;

                // If the tag values are not forwarded, the value is changed to undefined, else its value
                // renmains unchanged.
                if(!is_fwd)
                {
                    act_defined = false;
                }
            }
        }
    }
    // Only recomputes in case that we are not in a simple tag vec handler.
    else if(vec != NULL)
    {
        // Gets the actual entry.
        vec->getActualValue(this);
    }
}

/*
 * Moves the handler to the next cycle where the value has changed
 * or is undefined.
 *
 * @return void.
 */
void
InternalTagHandler::skipToNextCycleWithChange()
{
    // If we have a valid vector.
    if(vec != NULL)
    {
        // Tries to find another value in the actual vector.
        if(vec->skipToNextCycleWithChange(this))
        {
            // We already have the next cycle with change.
            return;
        }
    }
    // If no value is defined at all.
    else
    {
        // Marks the cycle and the chunk to an out of bounds position.
        if(
               (tag == NULL)
            || ((division == 0) && (tag->cycleVecHigh == NULL))
            || ((division == 1) && (tag->cycleVecLow == NULL))
        )
        {
            act_cycle.cycle = BIGGEST_CYCLE - base_cycle;
            return;
        }
    }

    chunk++;

    // Store the last value to detect a change.
    UINT64 last_value = act_value;
    bool last_defined = act_defined;
    UINT16 last_chunk = (tag->lastCycle >> CYCLE_OFFSET_BITS) + 1;

    // If forwarding the next change is the next not null vector with change.
    while(chunk <= last_chunk)
    {
        act_cycle.cycle = chunk << CYCLE_OFFSET_BITS;
        act_entry = -1;

        if(division == 0)
        {
            // Checks if a chunk is defined.
            if(tag->cycleVecHigh->hasElement(chunk) && (tag->cycleVecHigh->at(chunk) != NULL))
            {
                // Gets the first entry of the vector.
                vec = tag->cycleVecHigh->at(chunk);
                vec->getActualValue(this);

                // Checks if the first value is different.
                if((act_value != last_value) || (act_defined != last_defined))
                {
                    // If different we've done it.
                    return;
                }
                // Tries to find a change inside the actual vector.
                else if(vec->skipToNextCycleWithChange(this))
                {
                    // If found a change we've finished.
                    return;
                }
            }
            // If the tag is not forwarded and we have no vector if the last value was
            // defined, then we must return a not defined.
            else if(!is_fwd && last_defined)
            {
                act_defined = false;
                return;
            }
        }
        else
        {
            // Checks if a chunk is defined.
            if(tag->cycleVecLow->hasElement(chunk) && (tag->cycleVecLow->at(chunk) != NULL))
            {
                // Gets the first entry of the vector.
                vec = tag->cycleVecLow->at(chunk);
                vec->getActualValue(this);

                // Checks if the first value is different.
                if((act_value != last_value) || (act_defined != last_defined))
                {
                    // If different we've done it.
                    return;
                }
                // Tries to find a change inside the actual vector.
                else if(vec->skipToNextCycleWithChange(this))
                {
                    // If found a change we've finished.
                    return;
                }
            }
            // If the tag is not forwarded and we have no vector if the last value was
            // defined, then we must return a not defined.
            else if(!is_fwd && last_defined)
            {
                act_defined = false;
                return;
            }
        }

        // Try the next chunk.
        chunk++;
    }

    // Nothing found, so we have no vector and point to an out of bounds position.
    vec = NULL;
    act_cycle.cycle = BIGGEST_CYCLE - base_cycle;
}

/*
 * Points the handler to the first cycle.
 *
 * @return void.
 */
void
InternalTagHandler::rewindToFirstCycle()
{
    // At the beginning points to the first cycle (0).
    act_cycle.cycle = 0;
    act_entry = -1;
    chunk = 0;
    act_defined = false;

    // Checks if the first chunk has a tag vector defined (and hence at least one value).
    if(division == 0)
    {
        if((tag != NULL) && (tag->cycleVecHigh != NULL) && tag->cycleVecHigh->hasElement(0) && (tag->cycleVecHigh->at(0) != NULL))
        {
            // Gets the first tag vector.
            vec = tag->cycleVecHigh->at(0);
            vec->getActualValue(this);
        }
    }
    else
    {
        if((tag != NULL) && (tag->cycleVecLow != NULL) && tag->cycleVecLow->hasElement(0) && (tag->cycleVecLow->at(0) != NULL))
        {
            // Gets the first tag vector.
            vec = tag->cycleVecLow->at(0);
            vec->getActualValue(this);
        }
    }
}

/*
 * Moves the handler to the previous cycle.
 *
 * @return void.
 */
void
InternalTagHandler::prevCycle()
{
/*    Q_ASSERT(act_cycle.cycle != (BIGGEST_CYCLE - base_cycle));
    Q_ASSERT(act_cycle.cycle != 0);

    // Decrements the cycle.
    act_cycle.cycle--;

    // Checks if we've changed of chunk.
    if((act_cycle.cycle & (CYCLE_CHUNK_SIZE - 1)) == 0)
    {
        // Decrements to the previous chunk.
        chunk--;

        if(division == 0)
        {
            // Checks if the actual chunk has a tag vector defined (and hence at least one value).
            if((tag != NULL) && (tag->cycleVecHigh != NULL) && tag->cycleVecHigh->hasElement(chunk) && (tag->cycleVecHigh->at(chunk) != NULL))
            {
                // Gets the handler of the type of vector.
                vec = tag->cycleVecHigh->at(chunk);

                // Updates the actual element.
                act_entry = -1;
                vec->getActualValue(this);
            }
            else
            {
                vec = NULL;

                // If the tag values are not forwarded, the value is changed to undefined, else its value
                // renmains unchanged.
                if(!is_fwd)
                {
                    act_defined = false;
                }
            }
        }
        else
        {
            // Checks if the actual chunk has a tag vector defined (and hence at least one value).
            if((tag != NULL) && (tag->cycleVecLow != NULL) && tag->cycleVecLow->hasElement(chunk) && (tag->cycleVecLow->at(chunk) != NULL))
            {
                // Gets the handler of the type of vector.
                vec = tag->cycleVecLow->at(chunk);

                // Updates the actual element.
                act_entry = -1;
                vec->getActualValue(this);
            }
            else
            {
                vec = NULL;

                // If the tag values are not forwarded, the value is changed to undefined, else its value
                // renmains unchanged.
                if(!is_fwd)
                {
                    act_defined = false;
                }
            }
        }
    }
    // Only recomputes in case that we are not in a simple tag vec handler.
    else if(vec != NULL)
    {
        // Gets the actual entry.
        vec->getActualValue(this);
    }*/
}

/*
 * Moves the handler to the previous cycle where the value has changed
 * or is undefined.
 *
 * @return void.
 */
void
InternalTagHandler::skipToPrevCycleWithChange()
{
/*    // If we have a valid vector.
    if(vec != NULL)
    {
        // Tries to find another value in the actual vector.
        if(vec->skipToNextCycleWithChange(this))
        {
            // We already have the next cycle with change.
            return;
        }
    }
    // If no value is defined at all.
    else
    {
        // Marks the cycle and the chunk to an out of bounds position.
        if(tag == NULL)
        {
            act_cycle.cycle = BIGGEST_CYCLE - base_cycle;
            return;
        }
        if((division == 0) && (tag->cycleVecHigh == NULL))
        {
            act_cycle.cycle = BIGGEST_CYCLE - base_cycle;
            return;
        }
        if((division == 1) && (tag->cycleVecLow == NULL))
        {
            act_cycle.cycle = BIGGEST_CYCLE - base_cycle;
            return;
        }
    }

    chunk++;

    // Store the last value to detect a change.
    UINT64 last_value = act_value;
    bool last_defined = act_defined;
    UINT16 last_chunk = (tag->lastCycle >> CYCLE_OFFSET_BITS) + 1;

    // If forwarding the next change is the next not null vector with change.
    while(chunk <= last_chunk)
    {
        act_cycle.cycle = chunk << CYCLE_OFFSET_BITS;
        act_entry = -1;

        if(division == 0)
        {
            // Checks if a chunk is defined.
            if(tag->cycleVecHigh->hasElement(chunk) && (tag->cycleVecHigh->at(chunk) != NULL))
            {
                // Gets the first entry of the vector.
                vec = tag->cycleVecHigh->at(chunk);
                vec->getActualValue(this);

                // Checks if the first value is different.
                if((act_value != last_value) || (act_defined != last_defined))
                {
                    // If different we've done it.
                    return;
                }
                // Tries to find a change inside the actual vector.
                else if(vec->skipToNextCycleWithChange(this))
                {
                    // If found a change we've finished.
                    return;
                }
            }
            // If the tag is not forwarded and we have no vector if the last value was
            // defined, then we must return a not defined.
            else if(!is_fwd && last_defined)
            {
                act_defined = false;
                return;
            }
        }
        else
        {
            // Checks if a chunk is defined.
            if(tag->cycleVecLow->hasElement(chunk) && (tag->cycleVecLow->at(chunk) != NULL))
            {
                // Gets the first entry of the vector.
                vec = tag->cycleVecLow->at(chunk);
                vec->getActualValue(this);

                // Checks if the first value is different.
                if((act_value != last_value) || (act_defined != last_defined))
                {
                    // If different we've done it.
                    return;
                }
                // Tries to find a change inside the actual vector.
                else if(vec->skipToNextCycleWithChange(this))
                {
                    // If found a change we've finished.
                    return;
                }
            }
            // If the tag is not forwarded and we have no vector if the last value was
            // defined, then we must return a not defined.
            else if(!is_fwd && last_defined)
            {
                act_defined = false;
                return;
            }
        }

        // Try the next chunk.
        chunk++;
    }

    // Nothing found, so we have no vector and point to an out of bounds position.
    vec = NULL;
    act_cycle.cycle = BIGGEST_CYCLE - base_cycle;*/
}

/*
 * Points the handler to the last cycle.
 *
 * @return void.
 */
void
InternalTagHandler::forwardToLastCycle()
{
/*    // At the beginning points to the first cycle (0).
    act_cycle.cycle = 0;
    act_entry = -1;
    chunk = 0;
    act_defined = false;

    // Checks if the first chunk has a tag vector defined (and hence at least one value).
    if(division == 0)
    {
        if((tag != NULL) && (tag->cycleVecHigh != NULL) && tag->cycleVecHigh->hasElement(0) && (tag->cycleVecHigh->at(0) != NULL))
        {
            // Gets the first tag vector.
            vec = tag->cycleVecHigh->at(0);
            vec->getActualValue(this);
        }
    }
    else
    {
        if((tag != NULL) && (tag->cycleVecLow != NULL) && tag->cycleVecLow->hasElement(0) && (tag->cycleVecLow->at(0) != NULL))
        {
            // Gets the first tag vector.
            vec = tag->cycleVecLow->at(0);
            vec->getActualValue(this);
        }
    }*/
}

/*
 * Moves the handler to the cycle
 *
 * @return void.
 */
void
InternalTagHandler::skipToCycle(CYCLE cycle)
{
    cycle.cycle -= base_cycle;

    act_entry = -1;
    if((act_cycle.division == 0) && (cycle.division == 1))
    {
        // If this handler is a high handler and the cycle is in a low phase,
        // we must move to the next cycle.
        cycle.cycle++;
    }
    chunk = cycle.cycle >> CYCLE_OFFSET_BITS;
    act_cycle.cycle = cycle.cycle;
    act_defined = false;

    if(
           (tag == NULL)
        || ((division == 0) && (tag->cycleVecHigh == NULL))
        || ((division == 1) && (tag->cycleVecLow == NULL))
    )
    {
        // Marks the cycle and the chunk to an out of bounds position.
        act_cycle.cycle = BIGGEST_CYCLE - base_cycle;
        return;
    }

    UINT16 last_chunk = (tag->lastCycle >> CYCLE_OFFSET_BITS) + 1;
    vec = NULL;

    if(division == 0)
    {
        // Checks if a chunk is defined.
        if(tag->cycleVecHigh->hasElement(chunk))
        {
            // Gets the first entry of the vector.
            vec = tag->cycleVecHigh->at(chunk);
        }
    }
    else
    {
        // Checks if a chunk is defined.
        if(tag->cycleVecLow->hasElement(chunk))
        {
            // Gets the first entry of the vector.
            vec = tag->cycleVecLow->at(chunk);
        }
    }

    vec->getActualValue(this);
}

/*
 * Moves the handler to the first cycle where a value is set after
 * the cycle cycle.
 *
 * @return void.
 */
void
InternalTagHandler::skipToCycleDefined(CYCLE cycle)
{
    cycle.cycle -= base_cycle;

    act_entry = -1;
    if((act_cycle.division == 0) && (cycle.division == 1))
    {
        // If this handler is a high handler and the cycle is in a low phase,
        // we must move to the next cycle.
        cycle.cycle++;
    }
    chunk = cycle.cycle >> CYCLE_OFFSET_BITS;
    act_cycle.cycle = chunk << CYCLE_OFFSET_BITS;
    act_defined = false;

    if(
           (tag == NULL)
        || ((division == 0) && (tag->cycleVecHigh == NULL))
        || ((division == 1) && (tag->cycleVecLow == NULL))
    )
    {
        // Marks the cycle and the chunk to an out of bounds position.
        act_cycle.cycle = BIGGEST_CYCLE - base_cycle;
        return;
    }

    UINT16 last_chunk = (tag->lastCycle >> CYCLE_OFFSET_BITS) + 1;

    // If forwarding the next change is the next not null vector with change.
    while(chunk <= last_chunk)
    {
        if(division == 0)
        {
            // Checks if a chunk is defined.
            if(tag->cycleVecHigh->hasElement(chunk) && (tag->cycleVecHigh->at(chunk) != NULL))
            {
                // Gets the first entry of the vector.
                vec = tag->cycleVecHigh->at(chunk);
                vec->getActualValue(this);
                break;
            }
        }
        else
        {
            // Checks if a chunk is defined.
            if(tag->cycleVecLow->hasElement(chunk) && (tag->cycleVecLow->at(chunk) != NULL))
            {
                // Gets the first entry of the vector.
                vec = tag->cycleVecLow->at(chunk);
                vec->getActualValue(this);
                break;
            }
        }

        // Try the next chunk.
        chunk++;
        act_cycle.cycle += CYCLE_CHUNK_SIZE;
    }

    // Skips entries until cycle.
    while((!act_defined || (act_cycle.cycle < cycle.cycle)) && ((act_cycle.cycle + base_cycle) != BIGGEST_CYCLE))
    {
        skipToNextCycleWithChange();
    }
}

/**
 * Creator of this class.
 *
 * @return new object.
 */
DualTagHandler::DualTagHandler(const ClockDomainEntry * clock)
{
    tagHigh = new InternalTagHandler(clock, 0);
    tagLow = new InternalTagHandler(clock, 1);
    actual = NULL;
}

/*
 * Destructor of this class.
 *
 * @return destroys the object.
 */
DualTagHandler::~DualTagHandler()
{
    delete tagHigh;
    delete tagLow;
}

/*
 * Moves the handler to the next cycle.
 *
 * @return void.
 */
void
DualTagHandler::nextCycle()
{
    Q_ASSERT(false);
}

/*
 * Moves the handler to the next cycle where the value has changed
 * or is undefined.
 *
 * @return void.
 */
void
DualTagHandler::skipToNextCycleWithChange()
{
    // Skips the two handlers to the desired cycle.
    actual->skipToNextCycleWithChange();

    // Gets the one with lowest cycle.
    if(tagHigh->getTagCycle().cycle <= tagLow->getTagCycle().cycle)
    {
        actual = tagHigh;
    }
    else
    {
        actual = tagLow;
    }
}

/*
 * Points the handler to the first cycle.
 *
 * @return void.
 */
void
DualTagHandler::rewindToFirstCycle()
{
    tagHigh->rewindToFirstCycle();
    tagLow->rewindToFirstCycle();

    if(tagHigh->getTagDefined())
    {
        // If high is defined in cycle 0, then is the actual.
        actual = tagHigh;

        if(!tagLow->getTagDefined())
        {
            // If the low tag is not defined, moves it to the next value.
            tagLow->skipToNextCycleWithChange();
        }
    }
    else
    {
        if(!tagLow->getTagDefined())
        {
            // If the low tag is not defined, moves it to the next value and
            // gets the high as the actual.
            actual = tagHigh;
            tagLow->skipToNextCycleWithChange();
        }
        else
        {
            // If the low tag is defined and the high not, gets the low as the
            // actual and advances the high.
            actual = tagLow;
            tagHigh->skipToNextCycleWithChange();
        }
    }
}

/*
 * Moves the handler to the previous cycle.
 *
 * @return void.
 */
void
DualTagHandler::prevCycle()
{
    Q_ASSERT(false);
}

/*
 * Moves the handler to the prev cycle where the value has changed
 * or is undefined.
 *
 * @return void.
 */
void
DualTagHandler::skipToPrevCycleWithChange()
{
/*    // Skips the two handlers to the desired cycle.
    actual->skipToNextCycleWithChange();

    // Gets the one with lowest cycle.
    if(tagHigh->getTagCycle().cycle <= tagLow->getTagCycle().cycle)
    {
        actual = tagHigh;
    }
    else
    {
        actual = tagLow;
    }*/
}

/*
 * Points the handler to the last cycle.
 *
 * @return void.
 */
void
DualTagHandler::forwardToLastCycle()
{
/*    tagHigh->rewindToFirstCycle();
    tagLow->rewindToFirstCycle();

    if(tagHigh->getTagDefined())
    {
        // If high is defined in cycle 0, then is the actual.
        actual = tagHigh;

        if(!tagLow->getTagDefined())
        {
            // If the low tag is not defined, moves it to the next value.
            tagLow->skipToNextCycleWithChange();
        }
    }
    else
    {
        if(!tagLow->getTagDefined())
        {
            // If the low tag is not defined, moves it to the next value and
            // gets the high as the actual.
            actual = tagHigh;
            tagLow->skipToNextCycleWithChange();
        }
        else
        {
            // If the low tag is defined and the high not, gets the low as the
            // actual and advances the high.
            actual = tagLow;
            tagHigh->skipToNextCycleWithChange();
        }
    }*/
}

/*
 * Moves the handler to the cycle
 *
 * @return void.
 */
void
DualTagHandler::skipToCycle(CYCLE cycle)
{
    // Skips the two handlers to the desired cycle.
    tagHigh->skipToCycle(cycle);
    tagLow->skipToCycle(cycle);
    actual = tagHigh;
}

/*
 * Moves the handler to the first cycle where a value is set after
 * the cycle cycle.
 *
 * @return void.
 */
void
DualTagHandler::skipToCycleDefined(CYCLE cycle)
{
    // Skips the two handlers to the desired cycle.
    tagHigh->skipToCycleDefined(cycle);
    tagLow->skipToCycleDefined(cycle);

    // Gets the one with lowest cycle.
    if(tagHigh->getTagCycle().cycle <= tagLow->getTagCycle().cycle)
    {
        actual = tagHigh;
    }
    else
    {
        actual = tagLow;
    }
}

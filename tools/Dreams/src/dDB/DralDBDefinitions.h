// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
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

/*
 * @file DralDBDefinitions.h
 * @brief Sizes and definition of fundamental structures
 */

#ifndef _DRALDBDEFINITIONS_H_
#define _DRALDBDEFINITIONS_H_

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"

// Qt includes.
#include <qstring.h>

#define DRALDB_MAJOR_VERSION 0
#define DRALDB_MINOR_VERSION 7
#define DRALDB_STR_MAJOR_VERSION "0"
#define DRALDB_STR_MINOR_VERSION "7"

// -------------------------------------------------
// TagDescVector definitions
// -------------------------------------------------
#define TAGDESCVEC_SIZE 4096

// -------------------------------------------------
// StrTable definitions
// -------------------------------------------------
#define STRTBL_DEFAULT_SIZE     10000

// -------------------------------------------------
// Cycle bitfield-related definitions
// -------------------------------------------------
#define CYCLE_CHUNK_SIZE 4096
#define CYCLE_OFFSET_BITS 12
#define CYCLE_OFFSET_MASK 0x00000FFFU

// -------------------------------------------------
// DralGraph definitions
// -------------------------------------------------
/**
  * @def DEFAULT_DGN_SIZE
  * @brief This is default size used for hashed list of Dral Nodes.
  * Bear in mind that this MUST be a prime number.
  */
#define DEFAULT_DBGN_SIZE 2111

/**
  * @def DEFAULT_DGE_SIZE
  * @brief This is default size used for hashed list of Dral Edges.
  * Bear in mind that this MUST be a prime number.
  */
#define DEFAULT_DBGE_SIZE 99991


#define CACHED_ITEMIDS 10000

// -------------------------------------------------
// Global definitions
// -------------------------------------------------

/** @def used as a "canonical" item id string */
#define ITEMID_STR_TAG "ITEMID"

/** @def used as a "canonical" item inside a slot string */
#define SLOTITEM_STR_TAG "SLOT_ITEM"

/** @def used as a "canonical" item creation string */
#define NEWITEM_STR_TAG "NEW_ITEM"

/** @def used as a "canonical" item destruction string */
#define DELITEM_STR_TAG "DEL_ITEM"

/*
 * @struct TagValueEntry
 *
 * Represents the values for an item or a slot during a determined
 * cycle.
 */
struct TagValueEntry
{
    bool * valids;
    UINT64 * values;

    TagValueEntry() : valids(NULL), values(NULL) { }
} ;

/*
 * @brief
 * This class represents the capacity of a node with a defined
 * layout.
 * WARNING!!! WARNING!!! WARNING!!!
 * Be careful when dealing with this class: when the object is
 * destroyed it automatically deletes the vector. Don't have
 * various elements pointing to the same allocated array of
 * integers!!!
 */
class NodeSlot
{
    public:
        inline NodeSlot();
        inline NodeSlot(const NodeSlot& slot);
        inline NodeSlot(UINT16 numDim);
        inline NodeSlot(UINT16 numDim, UINT32 * dim);
        inline ~NodeSlot();

        inline void operator=(const NodeSlot& orig);
        inline bool operator==(const NodeSlot& cmp) const;

        inline void clearSlot();

        inline INT32 linearPosition(NodeSlot * base) const;
        inline NodeSlot linearSlot(UINT32 position) const;
        inline static INT32 linearPosition(UINT16 dimensions, UINT32 * capacities, NodeSlot * base);

        inline QString dump() const;
        inline QString toString() const;

    private:
        inline void copyVector(UINT16 numDim, UINT32 * dim);

    public:
        UINT16 dimensions;    ///< Number dimensions of the slot.
        UINT32 * capacities;  ///< Capacity of each dimension.
        UINT32 totalCapacity; ///< Total capacity of the slot.
} ;

/*
 * Creator of this class. Sets default values.
 *
 * @return new object.
 */
NodeSlot::NodeSlot()
{
    capacities = NULL;
    dimensions = 0;
    totalCapacity = 0;
}

/*
 * Copy creator. Makes a copy of a previous NodeSlot.
 *
 * @return new object.
 */
NodeSlot::NodeSlot(const NodeSlot& slot)
{
    copyVector(slot.dimensions, slot.capacities);
}

/*
 * Creator with dimension parameter
 *
 * @return new object.
 */
NodeSlot::NodeSlot(UINT16 numDim)
{
    dimensions = numDim;
    capacities = new UINT32[numDim];
    clearSlot();
}

/*
 * Creator with vector parameter
 *
 * @return new object.
 */
NodeSlot::NodeSlot(UINT16 numDim, UINT32 * dim)
{
    copyVector(numDim, dim);
}

/*
 * Destructor of this class. Frees the vector if necessary.
 *
 * @return destroys the object.
 */
NodeSlot::~NodeSlot()
{
    if(capacities != NULL)
    {
        delete [] capacities;
    }
}

/*
 * Assignation operator of this class.
 *
 * @return void.
 */
void
NodeSlot::operator=(const NodeSlot& orig)
{
    if(capacities != NULL)
    {
        delete [] capacities;
    }
    copyVector(orig.dimensions, orig.capacities);
}

/*
 * Comparison of two node slots.
 *
 * @return if the two node slots are equal.
 */
bool
NodeSlot::operator==(const NodeSlot& cmp) const
{
    if(dimensions != cmp.dimensions)
    {
        return false;
    }
    else
    {
        // Checks that all the dimensions have the same size.
        for(UINT16 i = 0; i < dimensions; i++)
        {
            if(capacities[i] != cmp.capacities[i])
            {
                return false;
            }
        }
        return true;
    }
}

/*
 * Clears the content of the slot.
 *
 * @return void.
 */
void
NodeSlot::clearSlot()
{
    totalCapacity = 0;
    for(UINT16 i = 0; i < dimensions; i++)
    {
        capacities[i] = 0;
    }
}

/*
 * Computes the linear position using the dimensions defined
 * with another NodeSlot.
 *
 * @return the linear position.
 */
INT32
NodeSlot::linearPosition(NodeSlot * base) const
{
    return linearPosition(dimensions, capacities, base);
}

/*
 * Returns the slot that matchs with the linear position param
 * using the base node slot as the dimensions definition.
 *
 * @return the slot.
 */
NodeSlot
NodeSlot::linearSlot(UINT32 position) const
{
    UINT32 disp;          ///< Accumulated displacement.
    NodeSlot ret(* this); ///< Creates a slot with the same number of dimensions.

    disp = 1;

    for(UINT16 i = 0; i < dimensions; i++)
    {
        disp *= capacities[i];
        ret.capacities[i] = position % disp;
        position -= ret.capacities[i];
    }

    return ret;
}

/*
 * Computes the linear position using the dimensions defined
 * with another NodeSlot. The same version but static.
 *
 * @return the linear position.
 */
INT32
NodeSlot::linearPosition(UINT16 dimensions, UINT32 * capacities, NodeSlot * base)
{
    INT32 position; ///< Used to count the linear position.
    INT32 disp;     ///< Accumulated displacement.

    if(base->dimensions != dimensions)
    {
        return TRACK_ID_LINEAR_NUM_DIM_ERROR;
    }

    disp = 1;
    position = 0;

    for(INT16 i = dimensions - 1; i >= 0; i--)
    {
        if(capacities[i] >= base->capacities[i])
        {
            return TRACK_ID_LINEAR_OOB_DIM_ERROR;
        }
        position += disp * capacities[i];
        disp *= base->capacities[i];
    }

    return position;
}

/*
 * Dumps the content of the node slot.
 *
 * @return a string with the dump.
 */
QString
NodeSlot::dump() const
{
    QString ret = "";

    ret = "Dim " + QString::number(dimensions);
    if(dimensions > 0)
    {
        ret += ": ";
        for(UINT16 i = 0; i < dimensions - 1; i++)
        {
            ret += QString::number(capacities[i]) + ", ";
        }
        ret += QString::number(capacities[dimensions - 1]);
    }
    return ret;
}

/*
 * Dumps the content of the node slot.
 *
 * @return a string with the dump.
 */
QString
NodeSlot::toString() const
{
    QString ret = "{";

    if(dimensions > 0)
    {
        for(UINT16 i = 0; i < dimensions - 1; i++)
        {
            ret += QString::number(capacities[i]) + ", ";
        }
        ret += QString::number(capacities[dimensions - 1]);
    }
    ret += "}";
    return ret;
}

/*
 * Copies a integer vector to the NodeSlot.
 *
 * @return void.
 */
void
NodeSlot::copyVector(UINT16 numDim, UINT32 * dim)
{
    dimensions = numDim;
    totalCapacity = 0;
    if(dimensions != 0)
    {
        capacities = new UINT32[dimensions];
        totalCapacity = 1;
        for(UINT32 i = 0; i < dimensions; i++)
        {
            capacities[i] = dim[i];
            totalCapacity *= dim[i];
        }
    }
    else
    {
        capacities = NULL;
    }
}

#endif

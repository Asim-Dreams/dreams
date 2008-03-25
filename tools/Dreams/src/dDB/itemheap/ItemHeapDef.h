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
  * @file ItemTagDef.h
  */

#ifndef _DRALDB_TAGITEMDEF_H_
#define _DRALDB_TAGITEMDEF_H_

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/aux/AEVector.h"

// Some defines...
// Item vector size.
#define ITEM_CHUNK_SIZE 32768
#define ITEM_NUM_CHUNKS 8192
#define ITEM_MAX_ENTRIES ITEM_CHUNK_SIZE * ITEM_NUM_CHUNKS

// Event vector size.
#define EVENT_CHUNK_SIZE 32768
#define EVENT_NUM_CHUNKS 8192
#define EVENT_MAX_ENTRIES EVENT_CHUNK_SIZE * EVENT_NUM_CHUNKS

// Number of events per chunk.
#define EVENTS_PER_CHUNK 4

// Values of special chunk links.
#define ITEM_OPEN 0
#define ITEM_UNUSED 1

// Defines some thresholds.
#define ITEM_MAX_CHUNK_OFFSET (UINT32) 0x0FFFFFFF
#define ITEM_MAX_POSITION (UINT32) 0x0007FFF
#define NODE_MAX_VALUE (UINT32) 0x0003FFF
#define MAX_TAG_ID (INT32) 0x000003FF
#define ITEM_MAX_EVENT (UINT64) 0x000000001FFFFFFFULL

/* @typdef enum ItemHeapEventType
 * @brief
 * Enumeration with all the diferent events that can be stored
 * in the item database.
 * WARNING!!!!!!
 * Some parts of the code suppose that all the types of event
 * (move item, enter/exit nodes and events) are sequentially
 * enumerated, so be carefully if the enumaration is changed.
 * WARNING!!!!!!
 * Don't change the order of the bit-fields of the different
 * types of Item DB nodes because they're not used in a union
 * fashion and a change in the order might change the expected
 * results.
 */
typedef enum
{
    IDB_TAG,  ///< A tag.
    IDB_EVENT ///< An event.
} ItemHeapEventType;

/* @typdef enum ItemHeapEventType
 * @brief
 * Different type of events. 3 bits needed.
 */
typedef enum
{
    IDB_MOVE_ITEM,     ///< The item has moved throughout an edge.
    IDB_ENTER_NODE,    ///< The item has entered into a node.
    IDB_EXIT_NODE,     ///< The item has left a node.
    IDB_GEN_EVENT,     ///< New general event (PT support).
    IDB_EN_EX_NODE_ESC ///< Enter/Exit node case where we need more bits to code all the information. An extra node is used to code the information.
} ItemHeapEventSubType;

/* @typdef ItemHeapTagNode
 * @brief
 * Contains all the information necessary to know what tag is
 * holding, which value has, which type of tag is and when the
 * value was set. 64 bits.
 */
typedef struct
{
    UINT64 link_occ  :  7; ///< Bits that are used in the ItemTagHeapChunk struct to link chunks and store their occupancy.
    UINT64 type      :  1; ///< Has the type of event. Must be 0.
    UINT64 clockId   :  3; ///< Has the clock id of the tag.
    UINT64 cycle     : 20; ///< Cycle when the tag was set.
    UINT64 division  :  1; ///< Division of the cycle when the tag was set.
    UINT64 tagId     : 10; ///< Id of the tag.
    UINT64 dkey      : 20; ///< Dictionary key value of the tag.
    UINT64 isMutable :  1; ///< Is a mutable tag.
    UINT64 isSOV     :  1; ///< Is a set of values tag.
} ItemHeapTagNode;

/* @typdef ItemHeapMoveNode
 * @brief
 * Contains all the information necessary to store move items and
 * enter or exit node. In case that more than 18 bits are needed to
 * code the Enter/Exit slot, the type is marked as escaped and uses
 * the next chunk to store the slot or edge position. 64 bits.
 */
typedef struct
{
    UINT64 link_occ  :  7; ///< Bits that are used in the ItemTagHeapChunk struct to link chunks and store their occupancy.
    UINT64 type      :  1; ///< Has the type of event. Must be 1.
    UINT64 clockId   :  3; ///< Has the clock id of the event.
    UINT64 cycle     : 20; ///< Cycle when the event happened.
    UINT64 division  :  1; ///< Division of the cycle when the event happened.
    UINT64 eventType :  3; ///< Has the type of event.
    UINT64 node_edge : 14; ///< Id of the node or edge where the move.
    UINT64 pos       : 15; ///< Position of the edge or linear position of the slot where has entered or exited.
} ItemHeapMoveNode;

/* @typdef ItemHeapEscapeNode
 * @brief
 * Just has the base fields and the position of the movement.
 */
typedef struct
{
    UINT64 link_occ  :  7; ///< Bits that are used in the ItemTagHeapChunk struct to link chunks and store their occupancy.
    UINT64 type      :  1; ///< Has the type of event. Must be 1.
    UINT64 clockId   :  3; ///< Has the clock id of the event.
    UINT64 cycle     : 20; ///< Cycle when the event happened.
    UINT64 division  :  1; ///< Division of the cycle when the event happened.
    UINT64 eventType :  3; ///< Has the type of event.
    UINT64 pos       : 29; ///< Position of the edge or linear position of the slot where has entered or exited.
} ItemHeapEscapeNode;

/* @typdef ItemHeapEventNode
 * @brief
 * Contains general events that have been set to the node.
 */
typedef struct
{
    UINT64 link_occ  :  7; ///< Bits that are used in the ItemTagHeapChunk struct to link chunks and store their occupancy.
    UINT64 type      :  1; ///< Has the type of event. Must be 1.
    UINT64 clockId   :  3; ///< Has the clock id of the event.
    UINT64 cycle     : 20; ///< Cycle when the event happened.
    UINT64 division  :  1; ///< Division of the cycle when the event happened.
    UINT64 eventType :  3; ///< Has the type of event.
    UINT64 event_id  : 29; ///< Id of the general event.
} ItemHeapEventNode;

/* @typdef union ItemHeapNode
 * @brief
 * Has a union of all the previous items.
 */
typedef union
{
    ItemHeapTagNode    tag;   ///< Tag node.
    ItemHeapMoveNode   move;  ///< Movement node.
    ItemHeapEscapeNode esc;   ///< Escape node.
    ItemHeapEventNode  event; ///< Event node.
} ItemHeapNode;

/*
 * @brief
 * Chunk of four nodes of an item.
 *
 * @description
 * Uses the link and occupancy bits of each ItemHeapNode to know
 * the pointer to the next chunk, and which is the first free slot
 * of the chunk. For this pourpose uses 25 bits for the next
 * chunk and 3 bits for the first free slot. This last field
 * uses the 3 high bits of the first node of the chunk to store
 * its information and the next chunk pointer uses all the
 * other bits of the other 3 tags and the 4 lower bits of the
 * first tag. To allow reductions in the number of bits used to
 * point to the next chunk, this information is used as an relative
 * positive offset to the next pointer instead as an absolut
 * offset.
 *
 * -----------------------------------------------------------------
 * | Occ  |                   Offset to next chunk                 |
 * -----------------------------------------------------------------
 * | 6 5 4 3 2 1 0 | 6 5 4 3 2 1 0 | 6 5 4 3 2 1 0 | 6 5 4 3 2 1 0 |
 * -----------------------------------------------------------------
 * |     Node 1    |     Node 2    |     Node 3    |     Node 4    |
 * -----------------------------------------------------------------
 *
 * @version 0.1
 * @date started at 2004-05-25
 * @author Guillem Sole
 */
class ItemHeapChunk
{
    public :
        /*
         * Creator of the class.
         *
         * @return the new object.
         */
        ItemHeapChunk()
        {
        }

        /*
         * Returns the first free node slot of the chunk.
         *
         * @return slot position.
         */
        UINT32
        getFirstFreeSlot()
        {
            return (content[0].tag.link_occ >> 4);
        }

        /*
         * Sets the first free tag slot of the chunk. Must be lower
         * or equal than TAGS_PER_CHUNK.
         *
         * @return void.
         */
        void
        setFirstFreeSlot(UINT32 slot)
        {
            Q_ASSERT(slot <= EVENTS_PER_CHUNK);
            content[0].tag.link_occ = (content[0].tag.link_occ & 0x0F) | (slot << 4);
        }

        /*
         * Allocates a slot incrementing the occupancy.
         *
         * @return void.
         */
        UINT32
        allocateSlot()
        {
            UINT32 temp;

            temp = content[0].tag.link_occ >> 4;
            temp++;
            Q_ASSERT(temp <= EVENTS_PER_CHUNK);
            content[0].tag.link_occ = (content[0].tag.link_occ & 0x0F) | (temp << 4);
            return (temp - 1);
        }

        /*
         * Returns the pointer to the next chunk.
         *
         * @return the vector position.
         */
        UINT32
        getNextChunk()
        {
            return (((content[0].tag.link_occ & 0x0F) << 21)
                    | (content[1].tag.link_occ << 14)
                    | (content[2].tag.link_occ << 7)
                    | content[3].tag.link_occ);
        }

        /*
         * Sets the first free tag slot of the chunk. Must be lower
         * or equal than EVENTS_PER_CHUNK.
         *
         * @return void.
         */
        void
        setNextChunk(UINT32 slot)
        {
            content[0].tag.link_occ = (content[0].tag.link_occ & 0x70) | ((slot >> 21) & 0x0F);
            content[1].tag.link_occ = (slot >> 14) & 0x7F;
            content[2].tag.link_occ = (slot >> 7) & 0x7F;
            content[3].tag.link_occ = slot & 0x7F;
        }

        ItemHeapNode content[EVENTS_PER_CHUNK]; // Four tags per chunk.
} ;

/* @typdef ItemHeapEventVector
 * @brief
 * Dynamic array that holds all the events of the items.
 * Up to 268 * 10^6 chunks.
 */
typedef AEVector<ItemHeapChunk, ITEM_CHUNK_SIZE, ITEM_NUM_CHUNKS> ItemHeapEventVector;

/* @typdef ItemHeapVector
 * @brief
 * Dynamic array that holds all the items of the heap. Up to
 * 268*10^6 items. Needs 28 bits to index within the vector.
 */
class ItemHeapItemVector : public AEVector<UINT32, EVENT_CHUNK_SIZE, EVENT_NUM_CHUNKS>
{
    protected:
        /**
         * Allocates the segment number segment.
         *
         * @return void.
         */
        inline void
        allocateSegment(UINT32 segment)
        {
            segvector[segment] = new AEVectorNode<UINT32>(m2SegmentSize, segment);
            Q_ASSERT(segvector[segment] != NULL);
            ++numSegments;
            // Marks all the entries of the new reserved chunk as unitialized.
            for(UINT32 i = 0; i < m2SegmentSize; i++)
            {
                segvector[segment]->array[i] = ITEM_UNUSED;
            }
        }
} ;

#endif

/*****************************************************************************
 *
 * @author Oscar Rosell
 *
 * Copyright (C) 2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 *****************************************************************************/
#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H


enum EVENT_TYPE
{
    ET_CYCLE = 0,
    ET_NEW_ITEM,
    ET_MOVE_ITEM,
    ET_MOVE_ITEM_WITH_POS,
    ET_DELETE_ITEM,
    ET_ERROR,
    ET_COMMENT,
    ET_NON_CRITICAL_ERROR,
    ET_VERSION,
    ET_END_SIMULATION,
    ET_NEW_NODE,
    ET_NEW_EDGE,
    ET_SET_NODE_LAYOUT,
    ET_ENTER_NODE,
    ET_EXIT_NODE,
    ET_SET_CYCLE_TAG,
    ET_SET_CYCLE_TAG_STR,
    ET_SET_CYCLE_TAG_SET,
    ET_SET_NODE_TAG,
    ET_SET_NODE_TAG_STR,
    ET_SET_NODE_TAG_SET,
    ET_SET_ITEM_TAG,
    ET_SET_ITEM_TAG_STR,
    ET_SET_ITEM_TAG_SET,
    ET_NUM_DRAL_EVENTS
};

static const char* const EVENT_TYPE_STR[ET_NUM_DRAL_EVENTS] =
{
    "ET_CYCLE",
    "ET_NEW_ITEM",
    "ET_MOVE_ITEM",
    "ET_MOVE_ITEM_WITH_POS",
    "ET_DELETE_ITEM",
    "ET_ERROR",
    "ET_COMMENT",
    "ET_NON_CRITICAL_ERROR",
    "ET_VERSION",
    "ET_END_SIMULATION",
    "ET_NEW_NODE",
    "ET_NEW_EDGE",
    "ET_SET_NODE_LAYOUT",
    "ET_ENTER_NODE",
    "ET_EXIT_NODE",
    "ET_SET_CYCLE_TAG",
    "ET_SET_CYCLE_TAG_STR",
    "ET_SET_CYCLE_TAG_SET",
    "ET_SET_NODE_TAG",
    "ET_SET_NODE_TAG_STR",
    "ET_SET_NODE_TAG_SET",
    "ET_SET_ITEM_TAG",
    "ET_SET_ITEM_TAG_STR",
    "ET_SET_ITEM_TAG_SET",
};

#endif

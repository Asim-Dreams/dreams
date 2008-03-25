/*
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
 */

/**
  * @file LayoutResourceMultiRow.h
  */

#ifndef _LAYOUTRESOURCEMULTIROW_H
#define _LAYOUTRESOURCEMULTIROW_H

#include "DreamsDefs.h"
#include "layout/resource/LayoutResourceRow.h"

/*
 * @brief
 * This class represents a group of resource row that have the same
 * rules and have consecutive and related tracks identifiers.
 * @description
 *
 * @author Santi Galan
 * @date started at 2004-07-23
 * @version 0.1
 */
class LayoutResourceMultiRow : public LayoutResourceRow
{
    public:
        LayoutResourceMultiRow(DralDB * draldb, UINT32 row, DRAL_ID dralId, TRACK_ID trackId, TAG_ID tagIdColor, UINT32 rows, RowType type, bool isDrawOnMove);
        ~LayoutResourceMultiRow();

        inline UINT32 getNumRows() const;

    private:
        UINT32 lastTrackId; ///< Track identifier associated to the last row.
} ;

/*
 * Returns the number of rows of this node.
 *
 * @return the number of rows.
 */
UINT32
LayoutResourceMultiRow::getNumRows() const
{
    return lastTrackId - trackId + 1;
}

#endif // _LAYOUTRESOURCEMULTIROW_H.

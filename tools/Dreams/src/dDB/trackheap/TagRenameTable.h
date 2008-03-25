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
  * @file TagRenameTable.h
  */

#ifndef _DRALDB_TAGRENAMETABLE_H_
#define _DRALDB_TAGRENAMETABLE_H_

// General includes.
#include <set>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/GVector.h"

/*
 * @brief
 * This class holds the renaming of the rename table.
 *
 * @description
 * The class extends a growing vector that dynamically allocates
 * more space for the tags as needed. When a new chunk is allocated
 * all the values are set to 255, mark of not renamed, so coherency
 * is maintained.
 *
 * @version 0.1
 * @date started at 2004-10-14
 * @author Guillem Sole
 */
class GrowingRenameTable : public GVector<UINT8>
{
    protected:

        /*
         * This function is called to set the default value of the new
         * slots created.
         *
         * @return the default value.
         */
        inline UINT8
        defaultValue() const
        {
            return 255;
        }

    public:

        /*
         * Creator of this class. Forwards the function and resets the first
         * chunk.
         *
         * @return the new object.
         */
        GrowingRenameTable(UINT32 init_size)
            : GVector<UINT8>(init_size)
        {
            resetChunk(0, init_size);
        }
} ;

/*
 * @brief
 * This class implements a simple renaming table.
 *
 * @description
 * This class is used to collapse the different tag ids that
 * are set for a given track. As the tag ids are assigned in
 * arbitrary order, one track id usually have values for a few
 * different tag ids, but they have values that are quite different
 * (1, 54, ...). The problem of this sparse tags is that a lot
 * of tags must be allocated and only some of them used. Using this
 * rename table let us collapse all this values and only store
 * exactly the number of tags allocated.
 *
 * @version 0.1
 * @date started at 2004-10-13
 * @author Guillem Sole
 */
class TagRenameTable : public AMemObj
{
    public :
        TagRenameTable(INT32 numTags);
        virtual ~TagRenameTable();

        INT64 getObjSize() const;
        QString getUsageDescription() const;

        TAG_ID rename(TAG_ID index);
        bool isRenamed(TAG_ID index) const;

        inline UINT16 getNumRenamedTags() const;
        set<TAG_ID> getRenamedTags() const;

    private:
        TAG_ID * renaming; ///< Table of renaming.
        TAG_ID limit;      ///< Storage limit.
        TAG_ID nextIndex;  ///< Index that will be set to the next rename.
} ;

/*
 * This function returns the number of renamed tags in the renaming
 * table.
 *
 * @return number of renamed tags.
 */
UINT16
TagRenameTable::getNumRenamedTags() const
{
    return (UINT16) nextIndex;
}

#endif // _DRALDB_TAGRENAMETABLE_H_.

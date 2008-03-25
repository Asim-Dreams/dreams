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
  * @file GVector.h
  */

#ifndef _DRALDB_GVECTOR_H_
#define _DRALDB_GVECTOR_H_

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/aux/AMemObj.h"

/*
 * @brief
 * This class implements a growing vector.
 *
 * @description
 * This class is used to implement a growing vector of
 * pointers to a given class. When an out of bounds access
 * is done, the vector automatically grows until index * 1.25
 * position and sets all the new pointers to NULL.
 *
 * @version 0.1
 * @date started at 2004-10-13
 * @author Guillem Sole
 */
template<class C>
class GVector : public AMemObj
{
    protected:

        /*
         * This function is called to set the default value of the new
         * slots created.
         *
         * @return the default value.
         */
        virtual C defaultValue() const = 0;

    public :

        /*
         * Creator of this class.
         *
         * @return the new object.
         */
        GVector(UINT32 init_size)
        {
            Q_ASSERT(init_size > 0);
            C default_value;

            limit = init_size;
            vector = new C[init_size];
        }

        /*
         * Destructor of this class. Deallocates the vector.
         *
         * @return destroys the object.
         */
        virtual ~GVector()
        {
            delete [] vector;
        }

        /*
         * Accesses to the vector. If the index is out of bounds, more
         * space is allocated.
         *
         * @return a reference to the object in the index position.
         */
        inline C &
        operator[](UINT32 index)
        {
            // Checks if we have to allocate more space.
            if(index >= limit)
            {
                allocateSpace(index);
            }
            return vector[index];
        }

        /*
         * Accesses to the vector. If the index is out of bounds, more
         * space is allocated.
         *
         * @return the object in the index.
         */
        inline C
        constAccess(UINT32 index) const
        {
            // Checks if we have to allocate more space.
            if(index >= limit)
            {
                return defaultValue();
            }
            return vector[index];
        }

        /*
         * Returns the limit of the vector. 
         *
         * @return the limit.
         */
        inline UINT32
        getLimit() const
        {
            return limit;
        }

        /*
         * Gets the object size.
         *
         * @return the object size.
         */
        INT64
        getObjSize() const
        {
            return (sizeof(GVector) + (limit * sizeof(C)));
        }

        /*
         * Returns the usage description of this object.
         *
         * @return the usage description.
         */
        QString
        getUsageDescription() const
        {
            return "";
        }

    protected:

        /*
         * Resets a chunk of the vector.
         *
         * @return void.
         */
        void
        resetChunk(UINT32 init, UINT32 end)
        {
            C default_value;
            default_value = defaultValue();

            // Sets to NULL the new allocated entries.
            for(UINT32 i = init; i < end; i++)
            {
                vector[i] = default_value;
            }
        }

        /*
         * Allocates more space to the vector.
         *
         * @return void.
         */
        void
        allocateSpace(UINT32 index)
        {
            // Allocates space until index + 25 %.
            UINT32 new_limit; ///< Stores the new limit.
            C * temp;         ///< Temporal buffer to move the pointers.

            new_limit = index + (index >> 2);
            new_limit = QMAX(4, new_limit);
            temp = new C[new_limit];
            memcpy(temp, vector, limit * sizeof(C));

            // Delete the old vector.
            delete [] vector;

            // Sets the new state and resets the vector.
            vector = temp;
            resetChunk(limit, new_limit);

            limit = new_limit;
        }

        C * vector;   ///< This is the growing vector.
        UINT32 limit; ///< Number of entries allocated.
} ;

#endif // _DRALDB_GVECTOR_H_.

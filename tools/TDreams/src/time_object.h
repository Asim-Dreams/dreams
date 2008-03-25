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

#ifndef TIME_OBJECT_H
#define TIME_OBJECT_H

#include <utility>

struct TIME_OBJECT : private std::pair<INT32, double>
{
    typedef INT32 CYCLE_TYPE;
    typedef double NS_TYPE;
    TIME_OBJECT(CYCLE_TYPE cycle, double NS_TYPE);

    CYCLE_TYPE getCycle() const;
    NS_TYPE getNs() const;
};

inline
TIME_OBJECT::TIME_OBJECT(CYCLE_TYPE cycle, NS_TYPE ns) :
    std::pair<INT32, double>(cycle, ns)
{ }

inline
TIME_OBJECT::CYCLE_TYPE
TIME_OBJECT::getCycle() const
{
    return first;
}

inline
TIME_OBJECT::NS_TYPE
TIME_OBJECT::getNs() const
{
    return second;
}

static TIME_OBJECT operator+(const TIME_OBJECT& left, const TIME_OBJECT& right)
{
    TIME_OBJECT result(left.getCycle() + right.getCycle(), left.getNs() + right.getNs());
    return result;
}

#endif

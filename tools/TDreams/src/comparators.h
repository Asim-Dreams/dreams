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

#ifndef COMPARATORS_H
#define COMPARATORS_H

#include "lifetime.h"
#include "extractors.h"

// Comparators
template<class EXTRACTOR, class COMPARISON>
struct general_comparator
{
    typedef typename COMPARISON::first_argument_type FIRST_TYPE;
    typedef typename COMPARISON::second_argument_type SECOND_TYPE;
    general_comparator(const EXTRACTOR& extractor, const COMPARISON& comparison)
      : extractor_(extractor),
        comparison_(comparison)  
    {
    }
    bool operator()(LIFETIME_CLASS* one, LIFETIME_CLASS* other)
    {
        return comparison_(static_cast<FIRST_TYPE>(extractor_.getValue(*one)), static_cast<SECOND_TYPE>(extractor_.getValue(*other)));
    }
    EXTRACTOR extractor_;
    COMPARISON comparison_;
};

template<class COMPARISON>
struct total_time_comparator : public general_comparator<get_total_time, COMPARISON>
{
    typedef general_comparator<get_total_time, COMPARISON> PARENT_TYPE;
    total_time_comparator():
        PARENT_TYPE(get_total_time(), COMPARISON())
    {
    }
};

typedef total_time_comparator< std::greater<UINT32> > longer_request;
typedef total_time_comparator< std::less<UINT32> > shorter_request;

template<class COMPARISON>
struct stage_time_comparator : public general_comparator<get_stage_time, COMPARISON>
{
    typedef general_comparator<get_stage_time, COMPARISON> PARENT_TYPE;
    stage_time_comparator(UINT32 stage_id):
        PARENT_TYPE(get_stage_time(stage_id), COMPARISON())
    {
    }
};

typedef stage_time_comparator< std::greater<float> > longer_stage;
typedef stage_time_comparator< std::less<float> > shorter_stage;

template<class COMPARISON>
struct stage_set_time_comparator : public general_comparator<get_stage_set_time, COMPARISON>
{
    typedef general_comparator<get_stage_set_time, COMPARISON> PARENT_TYPE;
    stage_set_time_comparator(std::string substring):
        PARENT_TYPE(get_stage_set_time(substring), COMPARISON())
    {
    }
};
typedef stage_set_time_comparator< std::greater<float> > longer_stage_set;
typedef stage_set_time_comparator< std::less<float> > shorter_stage_set;

#endif

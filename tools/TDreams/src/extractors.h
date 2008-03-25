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

#ifndef EXTRACTORS_H
#define EXTRACTORS_H

#include "endpoint.h"
#include "lifetime.h"
#include <string>

// Extractors
struct get_total_time : public LIFETIME_EXTRACTOR
{
    get_total_time(){};

    UINT32 getValue(const LIFETIME_CLASS& lifetime) const
    {
        return lifetime.totalLife();
    }
};

struct get_stage_time : public LIFETIME_EXTRACTOR
{
    get_stage_time(UINT32 stage):
        stage_(stage){}

    float getValue(const LIFETIME_CLASS& lifetime) const
    {
        return lifetime.duration(stage_);
    }

    UINT32 stage_;
};

struct get_stage_set_time : public LIFETIME_EXTRACTOR
{
    get_stage_set_time(std::string name):
        name_(name){}

    float getValue(const LIFETIME_CLASS& lifetime) const
    {
        return lifetime.duration(name_);
    }

    bool readConfig(std::string config)
    {
        // Structure of config should be NODE_SET{NODE_NAME}
        std::string::size_type pos_1 = config.find('{');
        assert(pos_1 < config.size());
        std::string::size_type pos_2 = config.find('}', pos_1 + 1);
        name_ = config.substr(pos_1 + 1, pos_2 - 1 - pos_1);
        return true;
    }

    std::string name_;
};

struct get_interval_time : public LIFETIME_EXTRACTOR
{
    get_interval_time(){};

    double getValue(const LIFETIME_CLASS& lifetime) const
    {
        for (LIFETIME_EXTRACTOR::Stages::const_iterator it = begin(lifetime); it != end(lifetime); ++it)
        {
            start_->check(*it);
            end_->check(*it);
        }
        const double start_val = start_->time();
        const double end_val = end_->time();
        start_->clear();
        end_->clear();
        if (start_val == -1 || end_val == -1)
        {
            return 0;
        }
        else
        {
            return end_val - start_val;
        }
    }

    bool readConfig(std::string config)
    {
        // Structure of config should be NODE_INTERVAL{ENDPOINT, ENDPOINT}
        std::string::size_type pos_1 = config.find('{');
        assert(pos_1 < config.size());
        std::string::size_type pos_2 = config.find(',', pos_1 + 1);
        std::string first_endpoint = config.substr(pos_1 + 1, pos_2 - 1 - pos_1);
        std::string::size_type pos_3 = config.find('}', pos_2 + 1);
        std::string last_endpoint = config.substr(pos_2 + 1, pos_3 - 1 - pos_2);
        std::cout << "FE: " << first_endpoint << " LE: " << last_endpoint << std::endl;
        start_ = CreateEndpoint(first_endpoint);
        end_ = CreateEndpoint(last_endpoint);
        return true;
    }

    ENDPOINT* start_;
    ENDPOINT* end_;
};

#endif

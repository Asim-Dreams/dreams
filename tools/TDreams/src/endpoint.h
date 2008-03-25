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

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "lifetime.h"
#include <Singleton.h>
#include <Factory.h>

struct ENDPOINT
{
    ENDPOINT():
        time_(-1)
    {
    }
    virtual ~ENDPOINT(){};
    virtual bool check(const LIFETIME_CLASS::STAGE_CLASS& stage) = 0;
    virtual bool readConfig(std::string config) = 0;
    void clear()
    {
        time_ = -1;
    };
    double time() const {return time_;}
    double time_;
};

struct START_TRANSACTION : public ENDPOINT
{
    START_TRANSACTION()
    {
    }
    ~START_TRANSACTION()
    {
    }
    bool check(const LIFETIME_CLASS::STAGE_CLASS& stage) 
    {
        if (time_ == (-1))
        {
            time_ = stage.startTime().getNs();
            return true;
        }
        return false;
    }
    bool readConfig(std::string config)
    {
        return true;
    }
};

struct END_TRANSACTION : public ENDPOINT
{
    END_TRANSACTION()
    {
    }
    ~END_TRANSACTION()
    {
    }
    bool check(const LIFETIME_CLASS::STAGE_CLASS& stage) 
    {
        time_ = stage.startTime().getNs();
        return true;
    }

    bool readConfig(std::string config)
    {
        return true;
    }
};

struct FIRST_ITEM : public ENDPOINT
{
    FIRST_ITEM()
    {
    }
    ~FIRST_ITEM()
    {
    }
    bool check(const LIFETIME_CLASS::STAGE_CLASS& stage) 
    {
        if (time_ != -1)
        {
            return false;
        }
        const ITEM_TAGS* tags = 0;
        tags = THE_ITEM_TAGS_DB::Instance().getTags(stage.itemId());
        std::string tag_val;

        if (tags && tags->getTagValue(tag_, &tag_val) && tag_val == tag_value_)
        {
            time_ = stage.startTime().getNs();
            return true;
        }
        return false;
    }

    bool readConfig(std::string config)
    {
        return true;
    }

    std::string tag_;
    std::string tag_value_;
};

struct FIRST_STAGE : public ENDPOINT
{
    FIRST_STAGE()
    {
    }
    ~FIRST_STAGE()
    {
    }
    bool check(const LIFETIME_CLASS::STAGE_CLASS& stage) 
    {
        if (time_ != -1)
        {
            return false;
        }

        if (stage.nodeName().find(substring_) != std::string::npos)
        {
            time_ = stage.startTime().getNs();
            return true;
        }
        return false;
    }

    bool readConfig(std::string config)
    {
        // Structure of config should be FIRST_TIME_STAGE:STAGE_NAME
        std::string::size_type pos = config.find(':');
        assert(pos < config.size());
        pos++;
        std::string stage_name = config.substr(pos, config.size() - pos);
        substring_ = stage_name;
        return true;
    }

    std::string substring_;
};

struct LAST_ITEM : public ENDPOINT
{
    LAST_ITEM()
    {
    }
    ~LAST_ITEM()
    {
    }
    bool check(const LIFETIME_CLASS::STAGE_CLASS& stage) 
    {
        const ITEM_TAGS* tags;
        tags = THE_ITEM_TAGS_DB::Instance().getTags(stage.itemId());
        std::string tag_val;
        if (tags && tags->getTagValue(tag_, &tag_val) && tag_val == tag_value_)
        {
            time_ = stage.startTime().getNs();
            return true;
        }
        return false;
    }

    bool readConfig(std::string config)
    {
        return true;
    }

    std::string tag_;
    std::string tag_value_;
};

struct LAST_STAGE : public ENDPOINT
{
    LAST_STAGE()
    {
    }
    ~LAST_STAGE()
    {
    }
    bool check(const LIFETIME_CLASS::STAGE_CLASS& stage) 
    {
        if (stage.nodeName().find(substring_) != std::string::npos)
        {
            time_ = stage.startTime().getNs();
            return true;
        }
        return false;
    }

    bool readConfig(std::string config)
    {
        // Structure of config should be LAST_TIME_STAGE:STAGE_NAME
        std::string::size_type pos = config.find(':');
        assert(pos < config.size());
        pos++;
        std::string stage_name = config.substr(pos, config.size() - pos);
        substring_ = stage_name;
        return true;
    }

    std::string substring_;
};


typedef Loki::Factory<ENDPOINT, std::string> ENDPOINT_FACTORY;
typedef Loki::SingletonHolder<ENDPOINT_FACTORY> THE_ENDPOINT_FACTORY;

namespace
{
    template <class T>
    ENDPOINT*
    createEndpoint()
    {
        return new T;
    }

    bool RegisterEndpoints()
    {
        THE_ENDPOINT_FACTORY::Instance().Register("FIRST_TIME_STAGE", createEndpoint<FIRST_STAGE>);
        THE_ENDPOINT_FACTORY::Instance().Register("LAST_TIME_STAGE", createEndpoint<LAST_STAGE>);
        return true;
    }

    bool registered = RegisterEndpoints();

    ENDPOINT*
    CreateEndpoint(std::string config)
    {
        std::string::size_type pos_1 = config.find(':');
        assert(pos_1 < config.size());
        std::string endpoint_str = config.substr(0, pos_1);
        ENDPOINT* endpoint = THE_ENDPOINT_FACTORY::Instance().CreateObject(endpoint_str);
        assert(endpoint);
        endpoint->readConfig(config);
        return endpoint;
    }
}

#endif

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
#ifndef LIFETIME_H
#define LIFETIME_H

#include "node_db.h"
#include "item_tags.h"
#include "time_object.h"
#include <asim/dralListener.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <functional>
#include <assert.h>
#include <map>
#include <string>

#define NDEBUG

#ifdef NDEBUG
#define DEBUG(x) 
#else
#define DEBUG(x) x 
#endif

static const int MAX_NUM_STAGES=20;


class LIFETIME_CLASS
{
  public:
    typedef UINT32 NodeId;
    typedef UINT32 ItemId;
    typedef UINT64 TransactionId;

    class STAGE_CLASS
    {
      public:
        static const INT32 NULL_DURATION = -1;
        
        STAGE_CLASS(NodeId node_id, UINT32 cycle, const char* edge_name=NULL,const char* value=NULL):
            node_id_(node_id),
            cycle_(cycle),
            duration_(NULL_DURATION),
            edge_name_(edge_name),
            generic_event_value(value)
        {
            out_port = false;
        }

        bool out_port; // this event represents a message leaving a port (rather than entering)

        void dumpState(const std::string prefix = "", std::ostream& o_stream = std::cout) const;
        void dumpStateColumns(const std::string prefix = "", std::ostream& o_stream = std::cout, UINT32 start_ns=0) const;

        std::string nodeName() const;
        UINT16 nodeInstance() const;
        NodeId nodeId() const;
        void endStage(UINT32 cycle);
        TIME_OBJECT startTime() const;
        TIME_OBJECT duration() const;
        ItemId itemId() const;
        void setItemId(UINT32 item_id);
        UINT32 getCycle() const { return cycle_; }
        const char* getEdgeName() const { return edge_name_; }
        const char* getEventValue() const { return generic_event_value; }

      private: 
        NodeId node_id_;
        UINT32 cycle_;
        INT32 duration_;
        ItemId item_id_;
        const char* edge_name_;          // Should be NULL if this isn't an edge stage or a generic event
        const char* generic_event_value; // Should be NULL if this isn't a generic event
    };

    class start_time_comparator {
        public:
        bool operator()(STAGE_CLASS s1, STAGE_CLASS s2) const {
            if(s1.getCycle() < s2.getCycle()) { return false; } else { return true; }
        }
    };


  public:
  #if 0
    // Comparators
    template<class COMPARISON>
    struct total_time_comparator
    {
        bool operator()(LIFETIME_CLASS* one, LIFETIME_CLASS* other)
        {
            return COMPARISON()(one->totalLife(),other->totalLife());
        }
    };
    typedef total_time_comparator< std::greater<UINT32> > longer_request;
    typedef total_time_comparator< std::less<UINT32> > shorter_request;

    template<class COMPARISON>
    struct stage_time_comparator
    {
        stage_time_comparator(UINT32 stage_id)
        {
            stage_id_ = stage_id;
        }
        bool operator()(LIFETIME_CLASS* one, LIFETIME_CLASS* other)
        {
            return COMPARISON()(one->duration(stage_id_), other->duration(stage_id_));
        }
        UINT32 stage_id_;
    };
    typedef stage_time_comparator< std::greater<float> > longer_stage;
    typedef stage_time_comparator< std::less<float> > shorter_stage;

    template<class COMPARISON>
    struct stage_set_time_comparator
    {
        stage_set_time_comparator(std::string substring)
        {
            substring_ = substring;
        }
        bool operator()(LIFETIME_CLASS* one, LIFETIME_CLASS* other)
        {
            return COMPARISON()(one->duration(substring_),other->duration(substring_));
        }
        std::string substring_;
    };
    typedef stage_set_time_comparator< std::greater<float> > longer_stage_set;
    typedef stage_set_time_comparator< std::less<float> > shorter_stage_set;
#endif

    explicit LIFETIME_CLASS(TransactionId id);
    ~LIFETIME_CLASS();

    void addStage(NodeId node_id, ItemId item_id, UINT32 cycle, const char* edge_name=NULL, const char* generic_event=NULL);
    void addFutureStage(NodeId node_id, ItemId item_id, UINT32 cycle, const char* edge_name, const char* generic_event=NULL);
    void endLastStage(UINT32 cycle);
    void endStage(NodeId node_id, ItemId item_id, UINT32 cycle);
    TransactionId id() const;
    UINT32 totalLife() const;
    TIME_OBJECT::NS_TYPE duration(UINT32 stage) const;
    TIME_OBJECT::NS_TYPE duration(std::string substring) const;
    std::set<ItemId> getItems() const;

    bool hasEntered(std::string node_name) const;
    bool hasEntered(NodeId node_id) const;
    bool hasEnteredPartialMatch(std::string node_name_substring) const;

    template <class DUMPER>
    void dump(const DUMPER& dumper) const
    {
        dumper.dump(*this);
    }

    void dumpState(const std::string prefix, const std::string substring, std::ostream& o_stream = std::cout) const;

    UINT32 numStages() const;
    UINT32 startCycle() const;
    std::string stageName(UINT32 stage_num) const;
    UINT32 stageNodeInstance(UINT32 stage_num) const;
    UINT32 stageNode(UINT32 stage_num) const;

    static bool enable_color;

  protected:
    friend class LIFETIME_DUMPER;
    friend class LIFETIME_EXTRACTOR;
    typedef std::vector<STAGE_CLASS> Stages;
    Stages stages_;

    typedef std::priority_queue<STAGE_CLASS,std::vector<STAGE_CLASS>,start_time_comparator> FutureStages;
    FutureStages future_stages_;

    std::map<std::string, UINT64> numericTags_;
    std::map<std::string, std::string> stringTags_;

    const TransactionId id_;
};

class LIFETIME_EXTRACTOR
{
  public:
    LIFETIME_EXTRACTOR(){};

  protected:
    typedef LIFETIME_CLASS::Stages Stages;
    Stages::const_iterator begin(const LIFETIME_CLASS& lifetime) const;
    Stages::const_iterator end(const LIFETIME_CLASS& lifetime) const;
};

inline LIFETIME_EXTRACTOR::Stages::const_iterator
LIFETIME_EXTRACTOR::begin(const LIFETIME_CLASS& lifetime) const
{
    return lifetime.stages_.begin();
}

inline LIFETIME_EXTRACTOR::Stages::const_iterator
LIFETIME_EXTRACTOR::end(const LIFETIME_CLASS& lifetime) const
{
    return lifetime.stages_.end();
}

class LIFETIME_DUMPER
{
  public:
    LIFETIME_DUMPER(std::ostream& o_stream)
      : o_stream_(o_stream)
    {
    };
    virtual ~LIFETIME_DUMPER(){}
    virtual void dump(const LIFETIME_CLASS& lifetime) const = 0;
    virtual void setPrefix(std::string prefix) {}

  protected:
    typedef LIFETIME_CLASS::Stages Stages;
    Stages::const_iterator begin(const LIFETIME_CLASS& lifetime) const;
    Stages::const_iterator end(const LIFETIME_CLASS& lifetime) const;
    std::ostream& o_stream_;

};

inline LIFETIME_DUMPER::Stages::const_iterator
LIFETIME_DUMPER::begin(const LIFETIME_CLASS& lifetime) const
{
    return lifetime.stages_.begin();
}

inline LIFETIME_DUMPER::Stages::const_iterator
LIFETIME_DUMPER::end(const LIFETIME_CLASS& lifetime) const
{
    return lifetime.stages_.end();
}

inline std::string 
LIFETIME_CLASS::STAGE_CLASS::nodeName() const
{
    return NODE_DB_CLASS::getNodeName(node_id_);
}

inline UINT16
LIFETIME_CLASS::STAGE_CLASS::nodeInstance() const
{
    return NODE_DB_CLASS::getNodeInstance(node_id_);
}

inline LIFETIME_CLASS::NodeId 
LIFETIME_CLASS::STAGE_CLASS::nodeId() const
{
    return node_id_;
}

inline void
LIFETIME_CLASS::STAGE_CLASS::endStage(UINT32 cycle)
{
    assert(static_cast<INT32>(cycle - cycle_) >= 0);
    if (duration_ == NULL_DURATION)
    {
        duration_ = cycle - cycle_;
    }
}

inline TIME_OBJECT
LIFETIME_CLASS::STAGE_CLASS::startTime() const
{
    return TIME_OBJECT(cycle_, NODE_DB_CLASS::cyclesToNanoseconds(node_id_, cycle_));
}

inline TIME_OBJECT
LIFETIME_CLASS::STAGE_CLASS::duration() const
{
    return duration_ != NULL_DURATION ? TIME_OBJECT(duration_, NODE_DB_CLASS::cyclesToNanoseconds(node_id_, duration_)) : TIME_OBJECT(0, 0.0);
}

inline LIFETIME_CLASS::ItemId
LIFETIME_CLASS::STAGE_CLASS::itemId() const
{
    return item_id_;
}


inline void
LIFETIME_CLASS::STAGE_CLASS::setItemId(UINT32 item_id)
{
    item_id_ = item_id;
}

inline LIFETIME_CLASS::TransactionId
LIFETIME_CLASS::id() const
{
    return id_;
}

#endif

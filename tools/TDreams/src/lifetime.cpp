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
#include "lifetime.h"
#include "item_tags.h"
#include "svg_drawer.h"
#include "dumpers.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>

using namespace std;

bool LIFETIME_CLASS::enable_color  = false;

#define ESC_NORMAL    "\033[0m"

#define ESC_BLACK_D   "\033[0;30m"
#define ESC_RED_D     "\033[0;31m"
#define ESC_GREEN_D   "\033[0;32m"
#define ESC_YELLOW_D  "\033[0;33m"
#define ESC_BLUE_D    "\033[0;34m"
#define ESC_PURPLE_D  "\033[0;35m"
#define ESC_TEAL_D    "\033[0;36m"
#define ESC_WHITE_D   "\033[0;37m"

#define ESC_BLACK_L   "\033[1;30m"
#define ESC_RED_L     "\033[1;31m"
#define ESC_GREEN_L   "\033[1;32m"
#define ESC_YELLOW_L  "\033[1;33m"
#define ESC_BLUE_L    "\033[1;34m"
#define ESC_PURPLE_L  "\033[1;35m"
#define ESC_TEAL_L    "\033[1;36m"
#define ESC_WHITE_L   "\033[1;37m"

void
LIFETIME_CLASS::STAGE_CLASS::dumpState(const std::string prefix, std::ostream& o_stream) const
{
    const TIME_OBJECT start = startTime();
    const TIME_OBJECT dur = duration();

    o_stream << prefix << "ITEM_" << item_id_ << ":" 
        << THE_ITEM_TAGS_DB::Instance().getStringTag(item_id_, "CMDTYPE") << ":"
        << NODE_DB_CLASS::getNodeName(node_id_) << ":" 
        << NODE_DB_CLASS::getNodeInstance(node_id_) << ":" 
        << start.getCycle() << "(" << start.getNs() << " ns)" << ":" 
        << dur.getCycle() << "(" << dur.getNs() << " ns)" 
        << std::endl;
}
void
LIFETIME_CLASS::STAGE_CLASS::dumpStateColumns(const std::string prefix, std::ostream& o_stream, UINT32 start_ns) const
{
    const TIME_OBJECT start = startTime();
    const TIME_OBJECT dur = duration();
    double delta = start.getNs()-start_ns;
    static double last_delta = 0;


    ios_base::fmtflags original = o_stream.flags();

    // o_stream << ESC_YELLOW_L;

    o_stream.precision(2);
    o_stream <<  setfill(' ') << right << setprecision (2) << fixed << setfill('0')
             << setw(8)  << item_id_ << setfill(' ');
    if(THE_ITEM_TAGS_DB::Instance().getStringTag(item_id_, "CMDTYPE") != "") 
    {
        o_stream << setw(23) << THE_ITEM_TAGS_DB::Instance().getStringTag(item_id_, "CMDTYPE");
    } 
    else if(THE_ITEM_TAGS_DB::Instance().getStringTag(item_id_, "ZCMD") != "") 
    {
        o_stream << setw(13) << THE_ITEM_TAGS_DB::Instance().getStringTag(item_id_, "ZCMD");
        o_stream << setw(10) << THE_ITEM_TAGS_DB::Instance().getStringTag(item_id_, "ZSubCMD");
    }
    else if(THE_ITEM_TAGS_DB::Instance().getStringTag(item_id_, "BBOX_QUEUE") != "") 
    {
        o_stream << setw(23) << THE_ITEM_TAGS_DB::Instance().getStringTag(item_id_, "BBOX_QUEUE");
    } 
    else 
    {
        o_stream << setw(23) << THE_ITEM_TAGS_DB::Instance().getStringTag(item_id_, "CURRENT_STATE");
    } 

    if(getEventValue() == NULL) {
        o_stream << setw(40) << NODE_DB_CLASS::getNodeName(node_id_)
                 << setw(3)  << NODE_DB_CLASS::getNodeInstance(node_id_);
    } else {
        o_stream << setw(43) << "--EVENT--";
    }
    o_stream << setw(9)  << start.getCycle() << " (" 
             << setw(10) << start.getNs() << " ns)" 
             << setw(5)  << dur.getCycle()   << " (" 
             << setw(6)  << dur.getNs() << " ns) ";

    if(delta != 0 && ((delta-last_delta) > 10) && enable_color) {
        o_stream << ESC_RED_L;
    }
    o_stream << setw(6)  << delta << "ns ";
    if(enable_color) {
        o_stream << ESC_NORMAL;
    }

    o_stream << left;
    if(getEventValue() != NULL) {
        o_stream << getEdgeName() << "=" << getEventValue();
    } else if(getEdgeName() != NULL) {
        if(out_port) {
            o_stream << "o ";
        } else {
            o_stream << "i ";
        }
        o_stream << setw(18) << getEdgeName();
    } else {
        o_stream << setw(20) << "--STAGE--";
    }
    o_stream << std::endl;

    o_stream.flags(original);

    last_delta = delta;
}

LIFETIME_CLASS::LIFETIME_CLASS(TransactionId id):
    id_(id)
{
    stages_.reserve(MAX_NUM_STAGES);
}

LIFETIME_CLASS::~LIFETIME_CLASS()
{
}

void
LIFETIME_CLASS::addStage(NodeId node_id, ItemId item_id, UINT32 cycle, const char* edge_name, const char* generic_event_value)
{
    while(!future_stages_.empty() && (future_stages_.top().getCycle() <= cycle))
    {
        STAGE_CLASS popped_stage = future_stages_.top();
        future_stages_.pop();
        popped_stage.setItemId(popped_stage.itemId());
        popped_stage.out_port = true;
        stages_.push_back(popped_stage);
    }


    if (NODE_DB_CLASS::isActive(node_id))
    {
        STAGE_CLASS stage(node_id, cycle, edge_name, generic_event_value);
        stage.setItemId(item_id);
        stages_.push_back(stage);
    }
};

void
LIFETIME_CLASS::addFutureStage(NodeId node_id, ItemId item_id, UINT32 cycle, const char* edge_name, const char* generic_event_value)
{
    if (NODE_DB_CLASS::isActive(node_id))
    {
        STAGE_CLASS stage(node_id, cycle, edge_name, generic_event_value);
        stage.setItemId(item_id);
        future_stages_.push(stage);
    }
};

void 
LIFETIME_CLASS::endLastStage(UINT32 cycle)
{
    const Stages::reverse_iterator it_last = stages_.rbegin();
    if (stages_.empty())
    {
        return;
    }
    it_last->endStage(cycle);
};

void
LIFETIME_CLASS::endStage(NodeId node_id, ItemId item_id, UINT32 cycle)
{
    if (stages_.empty())
    {
        // Exit Without Stage -> First Stage
        addStage(node_id, item_id, cycle);
    }
    const Stages::reverse_iterator it_end = stages_.rend();
    // Lookup backwards for the stage to end
    for (Stages::reverse_iterator it = stages_.rbegin(); it!=it_end; ++it)
    {
        if (it->nodeId() == node_id && it->itemId() == item_id && it->getEdgeName() == NULL)  
        {
            // Found -> set the termination cycle
            it->endStage(cycle);
            return;
        }
    }
    // Not found: could happen if trace start cycle != 0
}

UINT32
LIFETIME_CLASS::totalLife() const
{
    if (stages_.empty())
    {
        return 0;
    }
    if (stages_.size() == 1)
    {
        return static_cast<UINT32>(stages_.front().duration().getNs());
    }

    double begin = stages_.front().startTime().getNs();
    double end = stages_.back().startTime().getNs();
    if(begin==end || ((end-begin)<1)) {return 0;} // Shouldn't be necessary... but floating point is wonky
    // Round to integer
    return static_cast<UINT32>(floor(end - begin));
};

TIME_OBJECT::NS_TYPE
LIFETIME_CLASS::duration(UINT32 stage) const
{
    return stages_[stage].duration().getNs();
}

TIME_OBJECT::NS_TYPE
LIFETIME_CLASS::duration(string substring) const
{
    const Stages::const_iterator it_end = stages_.end();
    TIME_OBJECT acum_duration(0,0.0);
    for (Stages::const_iterator it = stages_.begin(); it != it_end; ++it)
    {
        UINT32 idx = it->nodeName().find(substring);
        if (idx != string::npos)
        // Found substring
        {
            acum_duration = acum_duration + it->duration();
        }
    }
    return acum_duration.getNs();
}

set<UINT32>
LIFETIME_CLASS::getItems() const
{
    set<UINT32> items;
    // Copy item_ids from stages_ to items
    transform(stages_.begin(), stages_.end(), insert_iterator< set<UINT32> >(items, items.begin()), mem_fun_ref(&LIFETIME_CLASS::STAGE_CLASS::itemId));

    return items;
}

bool
LIFETIME_CLASS::hasEntered(string node_name) const
{
    Stages::const_iterator it_end = stages_.end();
    for (Stages::const_iterator it = stages_.begin(); it != it_end; ++it)
    {
        if (it->nodeName() == node_name)
        {
            return true;
        }
    }
    return false;
}

bool
LIFETIME_CLASS::hasEntered(NodeId node_id) const
{
    Stages::const_iterator it_end = stages_.end();
    for (Stages::const_iterator it = stages_.begin(); it != it_end; ++it)
    {
        if (it->nodeId() == node_id)
        {
            return true;
        }
    }
    return false;
}

bool
LIFETIME_CLASS::hasEnteredPartialMatch(string node_name_substring) const
{
    Stages::const_iterator it_end = stages_.end();
    for (Stages::const_iterator it = stages_.begin(); it != it_end; ++it)
    {
        UINT32 idx = it->nodeName().find(node_name_substring);
        if (idx != string::npos)
        {
            // Found substring
            return true;
        }
    }
    return false;
}

void
LIFETIME_CLASS::dumpState(const string prefix, const string substring, ostream& o_stream) const
{
    // Print total duration
    o_stream << prefix << "ID: " << id_ << endl;
    o_stream << prefix << "Total: " << totalLife() << endl;

    const Stages::const_iterator it_end = stages_.end();
    const string new_prefix = prefix + "\t";
    TIME_OBJECT acum_duration(0, 0.0);
    UINT32 matching_stages = 0;
    for (Stages::const_iterator it = stages_.begin(); it != it_end; ++it)
    {
        ostringstream str;

        it->dumpState(new_prefix, str);

        if (str.str().find(substring) != string::npos)
        {
            o_stream << str.str();
            acum_duration = acum_duration + it->duration();
            ++matching_stages;
        }
    }
    if (matching_stages)
    {
        o_stream << prefix << "Time on selected stages: " << acum_duration.getNs() << "ns" << endl;
    }
    o_stream << endl;
}

UINT32 
LIFETIME_CLASS::numStages() const
{
    return stages_.size();
}

UINT32 
LIFETIME_CLASS::startCycle() const
{
    return (!stages_.empty()) ? stages_[0].startTime().getCycle() : 0;
}

string 
LIFETIME_CLASS::stageName(UINT32 stage_num) const
{
    assert(stage_num < stages_.size());
    return stages_[stage_num].nodeName();
}

UINT32 
LIFETIME_CLASS::stageNodeInstance(UINT32 stage_num) const
{
    assert(stage_num < stages_.size());
    return stages_[stage_num].nodeInstance();
}

UINT32
LIFETIME_CLASS::stageNode(UINT32 stage_num) const
{
    assert(stage_num < stages_.size());
    return stages_[stage_num].nodeId();
}



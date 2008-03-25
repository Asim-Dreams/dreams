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

#include "configuration.h"
#include "listener.h"
#include "dumpers.h"
#include "extractors.h"
#include "comparators.h"
#include "dral_event_types.h"

bool LISTENER_CLASS::parse_moveitems  = false;
bool LISTENER_CLASS::parse_tag_events  = false;
bool LISTENER_CLASS::filter_zero_length_events  = true;

namespace 
{
    INT32 CYCLE_STEP = -1;
}

UINT32 getCycleStep()
{
    if (CYCLE_STEP == -1)
    {
        const string cycle_step = THE_CONFIGURATION_DB::Instance().getParameter("cycle-step");
        char* end;
        UINT32 cycle_step_n = strtol(cycle_step.c_str(), &end, 0);

        if (*end != '\0')
        {
            /* NaN */
            cycle_step_n = 5000;
        }
        CYCLE_STEP = cycle_step_n;
    }
    return CYCLE_STEP;
}

using namespace std;

LISTENER_CLASS::LISTENER_CLASS(const std::string lookup_tag):
        lookup_tag_(lookup_tag),
        o_stream_ptr(0),
        silent_(false)
{
    // Add default dummy node
    NODE_DB_CLASS::addNode(NODE_DB_CLASS::NULL_NODE_ID,"XXX",0);

    memset(&numDralEvents[0], 0, sizeof(UINT32) * ET_NUM_DRAL_EVENTS);
}

LISTENER_CLASS::~LISTENER_CLASS()
{
    const map<UINT64, LIFETIME_CLASS*>::const_iterator it_end = did2LifetimeTable_.end();
    for(map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.begin(); it!=it_end; ++it)
    {
        // Remove allocated memory
        delete it->second;
    }
}

void
LISTENER_CLASS::NewNode(UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance)
{
    ++numDralEvents[ET_NEW_NODE];
    NODE_DB_CLASS::addNode(node_id, node_name, instance);
    NODE_DB_CLASS::setParent(node_id, parent_id);

    if (THE_CONFIGURATION_DB::Instance().isIgnored(node_name))
    {
       NODE_DB_CLASS::setActivationStatus(node_id, false);
       DEBUG(cout << "Node " << node_id << "[" << node_name << "]" << "is deactivated." << endl);
    }
}

void
LISTENER_CLASS::Cycle(UINT64 cycle)
{
    ++numDralEvents[ET_CYCLE];


    if ((cycle % getCycleStep()) == 0)
    {
        DEBUG(cout << "Cycle: " << cycle << endl);
    }
    cycle_ = cycle;
}

void
LISTENER_CLASS::Cycle(UINT16 clock_id, UINT64 cycle, UINT16 phase)
{
    ++numDralEvents[ET_CYCLE];

    THE_CLOCK_DB::Instance().setCycle(clock_id, cycle);
    if ((cycle % getCycleStep()) == 0)
    {
        DEBUG(cout << THE_CLOCK_DB::Instance().getName(clock_id) << ". Cycle: " << cycle << endl);
    }

    cycle_ = cycle;
}


void
LISTENER_CLASS::EnterNode(UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position [])
{
    ++numDralEvents[ET_ENTER_NODE];

    Uid2DidTable::iterator it = uid2didTable_.find(item_id);

    if (it == uid2didTable_.end())
    {
        // The did number has not been set for this item.
        // This means the item is not part of a transaction.
        // Just ignore it and save some memory
        return;
    }

    map<UINT64, LIFETIME_CLASS*>::iterator it2 = did2LifetimeTable_.find(it->second);

    if (it2 != did2LifetimeTable_.end())
    {
        (it2->second)->addStage(node_id, item_id, NODE_DB_CLASS::getNodeCycle(node_id));
    }
    else
    {
        DEBUG(cout << "Not FOUND !!!!" << endl);
    }
};

void
LISTENER_CLASS::MoveItems (UINT16 edge_id, UINT32 numOfItems, UINT32 * items)
{
    ++numDralEvents[ET_ENTER_NODE];

    if(!parse_moveitems) return;

    for(UINT32 i=0; i<numOfItems; i++) 
    {
        Uid2DidTable::iterator it = uid2didTable_.find(items[i]);

        if (it == uid2didTable_.end())
        {
            // The did number has not been set for this item.
            // This means the item is not part of a transaction.
            // Just ignore it and save some memory
            continue;
        }

        map<UINT64, LIFETIME_CLASS*>::iterator it2 = did2LifetimeTable_.find(it->second);

        if (it2 != did2LifetimeTable_.end())
        {
            (it2->second)->addStage(Edge2Src[edge_id], 
                                    items[i], 
                                    (NODE_DB_CLASS::getNodeCycle(Edge2Src[edge_id])),
                                    EdgeName[edge_id]);
            (it2->second)->addFutureStage(Edge2Dst[edge_id], 
                                          items[i], 
                                          (NODE_DB_CLASS::getNodeCycle(Edge2Dst[edge_id])+EdgeLatency[edge_id]),
                                          EdgeName[edge_id]);
        }
        else
        {
            DEBUG(cout << "Not FOUND !!!!" << endl);
        }
    }
}

void
LISTENER_CLASS::ExitNode(UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position [])
{
    ++numDralEvents[ET_EXIT_NODE];
    Uid2DidTable::iterator it = uid2didTable_.find(item_id);

    if (it == uid2didTable_.end())
    {
        // The did number has not been set for this item
        // Ignore it
        return;
    }

    map<UINT64, LIFETIME_CLASS*>::iterator it2 = did2LifetimeTable_.find(it->second);

    if (it2 != did2LifetimeTable_.end())
    {
        (it2->second)->endStage(node_id, item_id, NODE_DB_CLASS::getNodeCycle(node_id));
    }
    else
    {
        DEBUG(cout << "Not FOUND !!!!" << endl);
    }
};

void
LISTENER_CLASS::SetItemTag(UINT32 item_id, char tag_name [], UINT64 value)
{
    ++numDralEvents[ET_SET_ITEM_TAG];

    string name(tag_name);
    if (name == lookup_tag_)
    {
        if (value == NULL_ID)
        {
            // This value is not valid -> it's not a valid transaction
            return;
        }

        uid2didTable_[item_id] = value;
        if (did2LifetimeTable_[value]==NULL)
        {
            // Adding lifetime
            did2LifetimeTable_[value] = new LIFETIME_CLASS(value);
        }
    }
}

void
LISTENER_CLASS::SetItemTagString(
        UINT32 item_id, char tag_name [], char str [])
{
    ++numDralEvents[ET_SET_ITEM_TAG_STR];

    THE_ITEM_TAGS_DB::Instance().addStringTag(item_id, tag_name, 0, str);

    if(parse_tag_events) {
        Uid2DidTable::iterator it = uid2didTable_.find(item_id);
        if (it == uid2didTable_.end()) { return; }
        map<UINT64, LIFETIME_CLASS*>::iterator it2 = did2LifetimeTable_.find(it->second);

        (it2->second)->addStage(15, item_id, cycle_, tag_name, str);
        //cout << "SetItemTagString: " << tag_name << " - " << str << " in cycle " << cycle_ << "\n";
    }
};

void 
LISTENER_CLASS::SetNodeClock(UINT16 node_id, UINT16 clock_id)
{
    NODE_DB_CLASS::setNodeClock(node_id, clock_id);
}

void
LISTENER_CLASS::NewClock(UINT16 node_id, UINT64 freq, UINT16 skew, UINT16 divisions, const char name [])
{
    THE_CLOCK_DB::Instance().addClock(node_id, freq, skew, name);
}

void 
LISTENER_CLASS::StartActivity(UINT64 start_activity_cycle)
{
    NODE_DB_CLASS::expandClock();
}


void 
LISTENER_CLASS::dumpLifetime(const LIFETIME_CLASS& lifetime, const string substring, ostream& o_stream) const
{
    if (substring == "")
    {
        if (mode_ == OM_SIMPLE)
        {
            SIMPLE_DUMPER dumper(o_stream);
            dumper.setPrefix("\t");
            lifetime.dump(dumper);
        }
        else if (mode_ == OM_COLUMNS)
        {
            COLUMN_DUMPER dumper(o_stream);
            lifetime.dump(dumper);
        }
        else // OM_NORMAL
        {
            NORMAL_DUMPER dumper(o_stream);
            lifetime.dump(dumper);
        }
    }
    else
    {
        lifetime.dumpState("", substring, o_stream);
    }
}

void
LISTENER_CLASS::filterEmptyTransactions() 
{
    std::map<UINT64, LIFETIME_CLASS*>::iterator it_end = did2LifetimeTable_.end();
    for(std::map<UINT64, LIFETIME_CLASS*>::iterator it = did2LifetimeTable_.begin(); it!=it_end; ++it)
    {
        if (it->second->totalLife() == 0)
        {
            did2LifetimeTable_.erase(it);
        }
    }

}

void 
LISTENER_CLASS::dumpTransactions(SELECTOR& selector, const string substring) const
{
    IN_PROGRESS_CLASS in_progress;

    const map<UINT64, LIFETIME_CLASS*>::const_iterator it_end = did2LifetimeTable_.end();
    for(map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.begin(); it!=it_end; ++it)
    {
        assert(it->second!=NULL);
        if (selector.isValid(it->second))
        {
            dumpLifetime(*(it->second), substring, getOStream());
        }
        if (!silent_) in_progress.update();
    }
}

void
LISTENER_CLASS::dumpWorse(UINT32 number, const UINT32 stage, SELECTOR& selector) const
{
    longer_stage comp(stage);
    LIFETIME_DUMPER* dumper = getNewDumper(getOStream());
    dumper->setPrefix("\t");
    dumpFirst(number, selector, comp, *dumper);
    delete(dumper);
}

void
LISTENER_CLASS::dumpWorse(UINT32 number, const string stage_substring, SELECTOR& selector) const
{
    longer_stage_set comp(stage_substring);
    LIFETIME_DUMPER* dumper = getNewDumper(getOStream());
    dumper->setPrefix("\t");
    dumpFirst(number, selector, comp, *dumper);
    delete(dumper);
}

void
LISTENER_CLASS::dumpWorse(UINT32 number, SELECTOR& selector) const
{
    longer_request comp;
    LIFETIME_DUMPER* dumper = getNewDumper(getOStream());
    dumper->setPrefix("\t");
    dumpFirst(number, selector, comp, *dumper);
    delete(dumper);
}

void
LISTENER_CLASS::dumpBest(UINT32 number, const UINT32 stage, SELECTOR& selector) const
{
    shorter_stage comp(stage);
    LIFETIME_DUMPER* dumper = getNewDumper(getOStream());
    dumper->setPrefix("\t");
    dumpFirst(number, selector, comp, *dumper);
    delete(dumper);
}

void
LISTENER_CLASS::dumpBest(UINT32 number, const string stage_substring, SELECTOR& selector) const
{
    shorter_stage_set comp(stage_substring);
    LIFETIME_DUMPER* dumper = getNewDumper(getOStream());
    dumper->setPrefix("\t");
    dumpFirst(number, selector, comp, *dumper);
    delete(dumper);
}

void
LISTENER_CLASS::dumpBest(UINT32 number, SELECTOR& selector) const
{
    shorter_request comp;
    LIFETIME_DUMPER* dumper = getNewDumper(getOStream());
    dumper->setPrefix("\t");
    dumpFirst(number, selector, comp, *dumper);
    delete(dumper);
}

void
LISTENER_CLASS::dumpItem(UINT32 item_id, const string substring) const
{
    Uid2DidTable::const_iterator it_item = uid2didTable_.find(item_id);

    if (it_item != uid2didTable_.end())
    {
        Did2LifetimeTable::const_iterator it_tran = did2LifetimeTable_.find(it_item->second);
        assert(it_tran != did2LifetimeTable_.end());
        dumpLifetime(*(it_tran->second), substring, getOStream());
    }
    else
    {
        cout << "Item not found in database." << endl;
    }
}

void
LISTENER_CLASS::dumpSVG(const vector<TransactionId>& transaction_ids, const string label_tag) const
{
    FLOW_DIAGRAM_DRAW draw;

    // Open /dev/null
    ofstream null_s("/dev/null");
    if (!null_s)
    {
        cerr << "Error opening /dev/null" << endl;
        return;
    }

    for (vector<TransactionId>::const_iterator id_it = transaction_ids.begin(); id_it != transaction_ids.end(); ++id_it)
    {
        map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.find(*id_it);

        cout << "ID: " << *id_it << endl;

        if (it!=did2LifetimeTable_.end())
        {
            SVG_DUMPER dumper(null_s);
            dumper.setArrowTag(label_tag);
            it->second->dump(dumper);
            draw.Add(dumper.draw_);
        }
        else
        {
            cout << "Item not found in database." << endl;
        }
    }
    draw.Dump(getOStream());
}

void
LISTENER_CLASS::dumpTransaction(LIFETIME_CLASS::TransactionId transaction_id, const string substring) const
{
    map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.find(transaction_id);

    if (it!=did2LifetimeTable_.end())
    {
        dumpLifetime(*(it->second), substring, getOStream());
    }
    else
    {
        cout << "Item not found in database." << endl;
    }
}

void
LISTENER_CLASS::dumpHistogram(const SELECTOR& selector) const
{
    get_total_time extractor;

    dumpHistogram(selector, extractor);
}

void
LISTENER_CLASS::dumpTotalTimes(const SELECTOR& selector) const
{
    map<UINT64, LIFETIME_CLASS*>::const_iterator it_end = did2LifetimeTable_.end();

    getOStream() << lookup_tag_ << "\tS_TIME\tDURATION" << endl;

    for(map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.begin(); it!=it_end; ++it)
    {
        if (it->second)
        {
            if (selector.isValid(it->second))
            {
                getOStream() << it->first << "\t" <<  it->second->startCycle() << "\t" << it->second->totalLife() << endl;
            }
        }
    }
}

ostream&
LISTENER_CLASS::getOStream() const
{
    if (o_stream_ptr)
    {
        return *o_stream_ptr;
    }
    else
    {
        return cout;
    }
}

void 
LISTENER_CLASS::setOutputMode(OUTPUT_MODE mode)
{
    DEBUG(cerr << "Setting output mode to " << mode << endl);
    mode_ = mode;
}

 
LIFETIME_DUMPER*
LISTENER_CLASS::getNewDumper(std::ostream& o_stream) const
{
    if (mode_ == OM_SIMPLE)
    {
        return(new SIMPLE_DUMPER(o_stream));
    }
    else if (mode_ == OM_COLUMNS)
    {
        return(new COLUMN_DUMPER(o_stream));
    }
    else // OM_NORMAL
    {
        return(new NORMAL_DUMPER(o_stream));
    }
}


bool 
LISTENER_CLASS::setOutputFile(const string o_file)
{
    delete o_stream_ptr;
    if (o_file != "")
    {
        o_stream_ptr = new ofstream(o_file.c_str());
        return (o_stream_ptr->good());
    }
    else
    {
        o_stream_ptr = 0;
        return cout.good();
    }
}

void
LISTENER_CLASS::clearEvents()
{
    uid2didTable_.clear();
    did2LifetimeTable_.clear();

    cycle_ = 0;
    memset(&numDralEvents[0], 0, sizeof(UINT32) * ET_NUM_DRAL_EVENTS);

    NODE_DB_CLASS::clear();
}

void
inProgress()
{
    const UINT32 animation_size = 4;
    static char array_anim[animation_size] = {'-',
                              '\\',
                              '|',
                              '/'};
    static UINT32 idx = 0;

    cout << "\rIn Progress " << array_anim[idx];
    flush(cout);
    idx = (idx + 1) % animation_size;
}

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
#ifndef LISTENER_H
#define LISTENER_H

#include "node_db.h"
#include "selector.h"
#include "lifetime.h"
#include "item_tags.h"
#include "dral_event_types.h"
#include "comparators.h"
#include "dumpers.h"
#include <asim/dralListenerConverter.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <assert.h>
#include <map>

static const UINT32 NULL_ID = UINT32(-1);

#ifdef NDEBUG
#define DEBUG(x) 
#else
#define DEBUG(x) x 
#endif


void inProgress();

UINT32 getCycleStep();

class IN_PROGRESS_CLASS
{
    enum {CTR_THRESHOLD = 50};
  public:
    IN_PROGRESS_CLASS():
        counter_(CTR_THRESHOLD)
    {
    }
    ~IN_PROGRESS_CLASS()
    {
        DEBUG(std::cout << "\nOperation End" << std::endl);
    }
    void update()
    {
        counter_ = --counter_;
        if (counter_ == 0)
        {
            inProgress();
            counter_ = CTR_THRESHOLD;
        }
    }
  private:
    UINT32 counter_;
};

class LISTENER_CLASS : public DRAL_LISTENER_OLD_CLASS
{
  public:
    typedef LIFETIME_CLASS::TransactionId TransactionId;

    enum OUTPUT_MODE
    {
        OM_NORMAL=0,
        OM_SIMPLE,
        OM_COLUMNS,
        OM_NUM_MODES
    };
    explicit LISTENER_CLASS(const std::string lookup_tag);
    virtual ~LISTENER_CLASS();

    virtual void Cycle (UINT64 cycle);
    virtual void NewItem (UINT32 item_id){
        ++numDralEvents[ET_NEW_ITEM];
        if (lookup_tag_ == "ITEMID")
        {
            SetItemTag(item_id, "ITEMID", item_id);
        }
    };
    virtual void MoveItems (
        UINT16 edge_id, UINT32 numOfItems, UINT32 * items);
//      {
//          ++numDralEvents[ET_MOVE_ITEM];
//      };
    virtual void MoveItemsWithPositions (
        UINT16 edge_id, UINT32 numOfItems,
        UINT32 * items, UINT32 * positions){
        ++numDralEvents[ET_MOVE_ITEM_WITH_POS];
        };
    virtual void DeleteItem (UINT32 item_id){
        ++numDralEvents[ET_DELETE_ITEM];
        };
    virtual void EndSimulation (void){
        ++numDralEvents[ET_END_SIMULATION];
        DEBUG(std::cout << "Simulation End Reached." << std::endl);
        for (UINT32 i = 0; i<ET_NUM_DRAL_EVENTS; ++i) {
            DEBUG(std::cout << EVENT_TYPE_STR[i] << " " << numDralEvents[i] << std::endl);
        }
            
    };
    virtual void Error (char * error){
        std::cout << "ERROR: " << error << std::endl;
        ++numDralEvents[ET_ERROR];
    };
    virtual void NonCriticalError (char * error){
        ++numDralEvents[ET_NON_CRITICAL_ERROR];
    };
    virtual void Version (UINT16 version){
        ++numDralEvents[ET_VERSION];
    };
    virtual void NewNode (
        UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance);
    virtual void NewEdge (
        UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,
        UINT32 bandwidth, UINT32 latency, char * name){
        ++numDralEvents[ET_NEW_EDGE];
        Edge2Src[edge_id] = sourceNode;
        Edge2Dst[edge_id] = destNode;
        EdgeLatency[edge_id] = latency;
        EdgeName[edge_id] = strdup(name);
    };
    virtual void SetNodeLayout (
        UINT16 node_id, UINT32 capacity, UINT16 dim, UINT32 capacities []){
        ++numDralEvents[ET_SET_NODE_LAYOUT];
    };
    virtual void EnterNode (
        UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position []);

    virtual void ExitNode (
        UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 position []);

    virtual void SetCycleTag(char tag_name [], UINT64 value){
        ++numDralEvents[ET_SET_CYCLE_TAG];
    };
    virtual void SetCycleTagString(char tag_name [], char str []){
        ++numDralEvents[ET_SET_CYCLE_TAG_STR];
    };
    virtual void SetCycleTagSet(char tag_name [], UINT32 nval, UINT64 set []){
        ++numDralEvents[ET_SET_CYCLE_TAG_SET];
    };
    virtual void SetItemTag(UINT32 item_id, char tag_name [], UINT64 value);
    virtual void SetItemTagString(UINT32 item_id, char tag_name [], char str []);

    virtual void SetItemTagSet( UINT32 item_id, char tag_name [], UINT32 nval, UINT64 set []){
        std::string name(tag_name);
        ++numDralEvents[ET_SET_ITEM_TAG_SET];
    };
    virtual void SetNodeTag(
        UINT16 node_id, char tag_name [], UINT64 value,
        UINT16 level, UINT32 list []){
        ++numDralEvents[ET_SET_NODE_TAG];
    };
    virtual void SetNodeTagString(
        UINT16 node_id, char tag_name [], char str [],
        UINT16 level, UINT32 list []){
        ++numDralEvents[ET_SET_NODE_TAG_STR];
    }
    virtual void SetNodeTagSet(
        UINT16 node_id, char tag_name [], UINT16 n, UINT64 set [],
        UINT16 level, UINT32 list []){
        ++numDralEvents[ET_SET_NODE_TAG_SET];
    }
    virtual void Comment (UINT32 magic_num, char comment []){
        ++numDralEvents[ET_COMMENT];
    };

    virtual void CommentBin (UINT16 magic_num, char * cont, UINT32 length){
        ++numDralEvents[ET_COMMENT];
    };
    virtual void SetNodeInputBandwidth(UINT16 node_id, UINT32 bandwidth){};
    virtual void SetNodeOutputBandwidth(UINT16 node_id, UINT32 bandwidth){};
    virtual void StartActivity (UINT64 start_activity_cycle);
    virtual void SetTagDescription (char tag_name [], char description []){};
    virtual void SetTagSingleValue (
        UINT32 item_id, char * tag_name,
        UINT64 value, UBYTE time_span_flags){
    };
    virtual void SetTagString (
        UINT32 item_id, char * tag_name,
        char * str, UBYTE time_span_flags){
        };
    virtual void SetTagSet (
        UINT32 item_id, char * tag_name, UINT32 set_size,
        UINT64 * set, UBYTE time_span_flags){
        };
    virtual void EnterNode (
        UINT16 node_id, UINT32 item_id, UINT32 slot){
        };
    virtual void ExitNode (UINT16 node_id, UINT32 slot){};
    virtual void SetCapacity (
        UINT16 node_id, UINT32 capacity,
        UINT32 capacities [], UINT16 dimensions){};
    virtual void SetHighWaterMark (UINT16 node_id, UINT32 mark){};
    virtual void Comment (char * comment){};
    virtual void AddNode (
        UINT16 node_id, char * node_name,UINT16 parent_id, UINT16 instance){};
    virtual void AddEdge (
        UINT16 sourceNode, UINT16 destNode, UINT16 edge_id,
        UINT32 bandwidth, UINT32 latency, char * name){};

    virtual void SetNodeClock(UINT16 node_id, UINT16 clock_id);
    virtual void NewClock(UINT16 clock_id, UINT64 freq, UINT16 skew, UINT16 divisions, const char name []);
    virtual void Cycle(UINT16 clock_id, UINT64 cycle, UINT16 phase);

    void dumpLifetime(const LIFETIME_CLASS& lifetime, const std::string substring = "", std::ostream& o_stream = std::cout) const;
    void dumpTransactions(SELECTOR& selector, const std::string substring = "") const;
    void dumpState(const std::string substring = "") const;

//  template <class COMPARATOR, class DUMPER>
//  void dumpFirst(UINT32 number, const SELECTOR& selector, COMPARATOR compare, const DUMPER& dumper) const;
    template <class COMPARATOR>
    void dumpFirst(UINT32 number, const SELECTOR& selector, COMPARATOR compare, LIFETIME_DUMPER& dumper) const;
    template <class COMPARATOR, class DUMPER>
    void dumpOrdered(UINT32 number, const SELECTOR& selector, const COMPARATOR& comparator, const DUMPER& dumper) const;
    template <class COMPARATOR>
    void dumpOrdered(UINT32 number, const SELECTOR& selector, const COMPARATOR& comparator) const;

    void filterEmptyTransactions();

    void dumpWorse(UINT32 number, const UINT32 stage, SELECTOR& selector) const;
    void dumpWorse(UINT32 number, const std::string stage_substring, SELECTOR& selector) const;
    void dumpWorse(UINT32 number, SELECTOR& selector) const;
    void dumpBest(UINT32 number, const UINT32 stage, SELECTOR& selector) const;
    void dumpBest(UINT32 number, const std::string stage_substring, SELECTOR& selector) const;
    void dumpBest(UINT32 number, SELECTOR& selector) const;

    template <class EXTRACTOR>
    void dumpMean(const SELECTOR& selector, const EXTRACTOR& extractor) const;
    void dumpItem(UINT32 item_id, const std::string substring = "") const;
    template <class DUMPER>
    void dumpTransaction(LIFETIME_CLASS::TransactionId transaction_id, const DUMPER& dumper) const;
    void dumpTransaction(LIFETIME_CLASS::TransactionId transaction_id, const std::string substring = "") const;
    void dumpSVG(const std::vector<TransactionId>& transaction_ids, std::string label_tag) const;

    template <class EXTRACTOR>
    void dumpHistogram(const SELECTOR& selector, const EXTRACTOR& extractor) const;
    void dumpHistogram(const SELECTOR& selector) const;

    template <class EXTRACTOR>
    void dumpTotalTimes(const SELECTOR& selector, const EXTRACTOR& extractor) const;
    void dumpTotalTimes(const SELECTOR& selector) const;

    std::ostream& getOStream() const;
    void setOutputMode(OUTPUT_MODE mode);
    bool setOutputFile(const std::string o_file);

    LIFETIME_DUMPER* LISTENER_CLASS::getNewDumper(std::ostream& o_stream = std::cout) const;

    void
    setSilentMode(bool on)
    {
        silent_ = on;
    }

    void
    clearEvents();

    static bool parse_moveitems;
    static bool parse_tag_events;
    static bool filter_zero_length_events;

  private:
    // Should be a hash table but STL doesn't have one
    typedef std::map<UINT64, UINT64> Uid2DidTable;
    Uid2DidTable uid2didTable_; 

    typedef std::map<UINT64, LIFETIME_CLASS*> Did2LifetimeTable;
    Did2LifetimeTable did2LifetimeTable_;

    UINT64 cycle_;
    std::string lookup_tag_;

    UINT32 numDralEvents[ET_NUM_DRAL_EVENTS];
    std::map<UINT16, UINT16>    Edge2Src;
    std::map<UINT16, UINT16>    Edge2Dst;
    std::map<UINT16, UINT32>    EdgeLatency;
    std::map<UINT16, char*>     EdgeName;

    std::ostream* o_stream_ptr;

    OUTPUT_MODE mode_;

    bool silent_;
};

//template <class COMPARATOR, class DUMPER>
template <class COMPARATOR>
void
LISTENER_CLASS::dumpFirst(UINT32 number, const SELECTOR& selector, COMPARATOR compare, LIFETIME_DUMPER& dumper) const
{
    IN_PROGRESS_CLASS in_progress;
    // Trivial case
    if (number == 0)
    {
        return;
    }

    std::priority_queue<LIFETIME_CLASS*, std::vector<LIFETIME_CLASS*>, COMPARATOR> first(compare);

    std::map<UINT64, LIFETIME_CLASS*>::const_iterator it_end = did2LifetimeTable_.end();
    for(std::map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.begin(); it!=it_end; ++it)
    {
        if (it->second)
        {
            if (selector.isValid(it->second))
            {
                first.push(it->second);
            }
        }
        if (first.size()>number)
        {
            first.pop();
        }
        if (!silent_) in_progress.update();
    }

    while(!first.empty())
    {
        dumper.dump(*(first.top()));
        first.pop();
    }
}

template <class COMPARATOR, class DUMPER>
void
LISTENER_CLASS::dumpOrdered(UINT32 number, const SELECTOR& selector, const COMPARATOR& comparator, const DUMPER& dumper) const
{
    IN_PROGRESS_CLASS in_progress;
    // Trivial case
    if (number == 0)
    {
        return;
    }

    std::priority_queue<LIFETIME_CLASS*, std::vector<LIFETIME_CLASS*>, COMPARATOR> first(comparator);

    std::map<UINT64, LIFETIME_CLASS*>::const_iterator it_end = did2LifetimeTable_.end();
    for(std::map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.begin(); it!=it_end; ++it)
    {
        if (it->second)
        {
            if (selector.isValid(it->second))
            {
                first.push(it->second);
            }
        }
        if (first.size()>number)
        {
            first.pop();
        }
        if (!silent_) in_progress.update();
    }

    while(!first.empty())
    {
        dumper.dump(*(first.top()));
        first.pop();
    }
}

template <class COMPARATOR>
void
LISTENER_CLASS::dumpOrdered(UINT32 number, const SELECTOR& selector, const COMPARATOR& comparator) const
{
    if (mode_ == OM_SIMPLE)
    {
        SIMPLE_DUMPER dumper(getOStream());
        dumpOrdered(number, selector, comparator, dumper);
    }
    else if (mode_ == OM_COLUMNS)
    {
        COLUMN_DUMPER dumper(getOStream());
        dumpOrdered(number, selector, comparator, dumper);
    }
    else // OM_NORMAL
    {
        NORMAL_DUMPER dumper(getOStream());
        dumpOrdered(number, selector, comparator, dumper);
    }

}

template <class EXTRACTOR>
void
LISTENER_CLASS::dumpMean(const SELECTOR& selector, const EXTRACTOR& extractor) const
{
    dumpHistogram(selector, extractor);
#if 0
    UINT32 counter = 0;
    UINT64 total = 0;

    std::map<UINT64, LIFETIME_CLASS*>::const_iterator it_end = did2LifetimeTable_.end();

    for(std::map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.begin(); it!=it_end; ++it)
    {
        if (it->second)
        {
            if (selector.isValid(it->second))
            {
                ++counter;
                total+=extractor.getValue(*(it->second));
                std::cout << "VALUE: " << extractor.getValue(*(it->second)) << std::endl;
            }
        }
    }
    cout << "Mean time: " << double(total)/double(counter) << " cycles." << endl;
#endif
}

template <class EXTRACTOR>
void
LISTENER_CLASS::dumpHistogram(const SELECTOR& selector, const EXTRACTOR& extractor) const
{
    std::map<UINT64, LIFETIME_CLASS*>::const_iterator it_end = did2LifetimeTable_.end();

    getOStream() << "CYCLE\t\tAVERAGE_LIFETIME\t\tTOTAL_LIFETIMES\t\tINSTANCES" << endl;

    std::map<UINT32, float> value_map;
    std::map<UINT32, UINT32> count_map;

    // This assumes requests are ordered temporally in the structure. Bigger id must imply newer request 
    for(std::map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.begin(); it!=it_end; ++it)
    {
        if (it->second)
        {
            if (selector.isValid(it->second))
            {
                UINT32 region = it->second->startCycle() / getCycleStep(); 
                value_map[region] += extractor.getValue(*(it->second));
                count_map[region] += 1;
            }
        }
    }
    for (std::map<UINT32, float>::const_iterator it = value_map.begin(); it != value_map.end(); ++it)
    {
        getOStream() << it->first * getCycleStep() << "-" << ((it->first + 1) * getCycleStep()) << "\t\t" << float(it->second)/float(count_map[it->first]) << "\t\t" << it->second << "\t\t" << count_map[it->first] << endl;
    }
}

template <class EXTRACTOR>
void 
LISTENER_CLASS::dumpTotalTimes(const SELECTOR& selector, const EXTRACTOR& extractor) const
{
    map<UINT64, LIFETIME_CLASS*>::const_iterator it_end = did2LifetimeTable_.end();

    getOStream() << lookup_tag_ << "\tS_TIME\tDURATION" << endl;

    for(map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.begin(); it!=it_end; ++it)
    {
        if (it->second)
        {
            if (selector.isValid(it->second))
            {
                getOStream() << it->first << "\t" <<  it->second->startCycle() << "\t" << extractor.getValue(*(it->second)) << endl;
            }
        }
    }
}

template <class DUMPER>
void
LISTENER_CLASS::dumpTransaction(LIFETIME_CLASS::TransactionId transaction_id, const DUMPER& dumper) const
{
    map<UINT64, LIFETIME_CLASS*>::const_iterator it = did2LifetimeTable_.find(transaction_id);

    if (it!=did2LifetimeTable_.end())
    {
        dumper.Dump(*(it->second));
    }
    else
    {
        cout << "Item not found in database." << endl;
    }
}

#endif

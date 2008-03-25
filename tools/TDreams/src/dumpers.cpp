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

#include "dumpers.h"
#include "item_tags.h"
#include "svg_drawer.h"
#include <iostream>

using namespace std;

NORMAL_DUMPER::NORMAL_DUMPER(ostream& o_stream)
  : LIFETIME_DUMPER(o_stream),
    prefix_("")
{
}

NORMAL_DUMPER::~NORMAL_DUMPER()
{
}

void
NORMAL_DUMPER::dump(const LIFETIME_CLASS& lifetime) const
{
    // Print total duration
    o_stream_ << prefix_ << "ID: " << lifetime.id() << endl;
    o_stream_ << prefix_ << "Total: " << lifetime.totalLife() << endl;

    const Stages::const_iterator it_end = end(lifetime);
    const string new_prefix = prefix_ + "\t";
    for (Stages::const_iterator it = begin(lifetime); it != it_end; ++it)
    {
        it->dumpState(new_prefix, o_stream_);
    }
    o_stream_ << endl;
}

void
NORMAL_DUMPER::setPrefix(string prefix)
{
    prefix_ = prefix;
}

SIMPLE_DUMPER::SIMPLE_DUMPER(ostream& o_stream)
  : LIFETIME_DUMPER(o_stream),
    prefix_("")
{
}

SIMPLE_DUMPER::~SIMPLE_DUMPER()
{
}

void
SIMPLE_DUMPER::dump(const LIFETIME_CLASS& lifetime) const
{
    (o_stream_) << prefix_ << lifetime.id() << "\t" << lifetime.startCycle();
    for (Stages::const_iterator it = begin(lifetime); it != end(lifetime); ++it)
    {
        (o_stream_) << "\t" << it->startTime().getCycle() << "\t" << it->duration().getCycle();
    }
    (o_stream_) << endl;
}

void
SIMPLE_DUMPER::setPrefix(string prefix)
{
    prefix_ = prefix;
}

COLUMN_DUMPER::COLUMN_DUMPER(ostream& o_stream)
  : LIFETIME_DUMPER(o_stream),
    prefix_("")
{
}

COLUMN_DUMPER::~COLUMN_DUMPER()
{
}

void
COLUMN_DUMPER::dump(const LIFETIME_CLASS& lifetime) const
{
    // Print total duration
    o_stream_ << prefix_ << "ID:    " << lifetime.id() << endl;
    o_stream_ << prefix_ << "Total: " << lifetime.totalLife() << " ns" << endl;

    const Stages::const_iterator it_end = end(lifetime);
    const string new_prefix = prefix_ + "\t";
    UINT32 start_ns = begin(lifetime)->startTime().getNs();
    for (Stages::const_iterator it = begin(lifetime); it != it_end; ++it)
    {
        it->dumpStateColumns(new_prefix, o_stream_, start_ns);
    }
    o_stream_ << endl;
}

void
COLUMN_DUMPER::setPrefix(string prefix)
{
    prefix_ = prefix;
}


SVG_DUMPER::SVG_DUMPER(ostream& o_stream)
  : LIFETIME_DUMPER(o_stream),
    arrow_tag_name_("")
{
}

SVG_DUMPER::~SVG_DUMPER()
{
}

void
SVG_DUMPER::setArrowTag(std::string tag_name)
{
    arrow_tag_name_ = tag_name;
}

void
SVG_DUMPER::dump(const LIFETIME_CLASS& lifetime) const
{
    map<UINT64 /*item_id*/, UINT32 /*last_appearance*/> last_apps;
    set<UINT32 /*node_ids*/> nodes;

    for (Stages::const_iterator it = begin(lifetime); it != end(lifetime); ++it)
    {
        const UINT64 item_id = it->itemId();
        if (last_apps.find(item_id) != last_apps.end())
        {
            // Node Start: node of last appearance
            const UINT32 node_start = (begin(lifetime) + last_apps[item_id])->nodeId();
            // Cycle Start: cycle of last appearance
            const UINT32 cycle_start = static_cast<UINT32>((begin(lifetime) + last_apps[item_id])->startTime().getNs()); 
            draw_.AddEdge(
                            node_start,
                            cycle_start,
                            it->nodeId() /*Node End*/,
                            static_cast<UINT32>(it->startTime().getNs()) /*Cyc End*/,
                            THE_ITEM_TAGS_DB::Instance().getStringTag(item_id, arrow_tag_name_) 
                         );
            nodes.insert((begin(lifetime) + last_apps[item_id])->nodeId());
            nodes.insert(it->nodeId());
        }
        last_apps[item_id] = it - begin(lifetime);
    }

    for (set<UINT32>::const_iterator it=nodes.begin(); it!=nodes.end(); ++it)
    {
        draw_.AddNode(*it,NODE_DB_CLASS::getNodeName(*it));
    }
    draw_.Dump(o_stream_);
}

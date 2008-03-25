/*
 * Copyright (C) 2005-2006 Intel Corporation
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
 */

//
// @ORIGINAL_AUTHOR: Federico Ardanaz
//
// This file does not contain any code
// it just contains additional information for
// inclusion with doxygen


// =======================================================================================  
/*! 

\page WATCH Watch View Tutorial 
 

\section REQUIREMENTS Architect Requirements


\section VISUALIZATION Visualiztion desires

\code 

//
// Array that represents a Store Buffer. The store buffer will be implemented
// It doesn't try to represent any special 
//
UINT64 cache[256][8];

bool lookup(UINT64 address)
{
    bool hit;
    UINT64 set;
    UINT64 way;

    get_cache_position(address, &set, &way);

    hit = (cache[set][way] == address);

    return hit;
}

int main(int argc, char argv *[])
{
    ...
    bool simulate = true;
    while(simulate)
    {
        ...
        hit = lookup(address);
        ...
    }
    ...
}

\endcode

\section DRAL Activity file generation

Let's start using the tools that Dreams and DRAL offers.  The first step to get
a graphical representation of the acitivity in the simulator is add DRAL
commands to the code. Those commands are used to notify the visualizer
(Dreams) all the actions that occur during the simulation.

\code 

//
// We include the DralServer specification
//
#include "asim/dralServer.h"

// 
// We define a global dralServer to be used in all the places we want to record some
// activity
//
DRAL_SERVER dral; 

UINT64 cache[256][8];

bool lookup(UINT64 address)
{
    bool hit;
    UINT64 set;
    UINT64 way;

    get_cache_position(address, &set, &way);

    hit = (cache[set][way] == address);

    return hit;
}

int main(int argc, char argv *[])
{
    //
    // We create the DralServer. In the constructor, we must specify
    // the name of the output file where the activity will be recorded.
    //
    dral = new DRAL_SERVER_CLASS("Events.drl.gz");

    ...

    bool simulate = true;
    while(simulate)
    {

        ...
        hit = lookup(address);
    }
}

\endcode

Once a DralServer is created, we can start adding activity commands to the code.
The DRAL language is based in two main abstractions: \em Nodes and \em Edges.
For DRAL, the model is represented as a graph. The nodes represents structures
that have storage. The edges represents the "wires" that transmit signals.

In our example we will define a node that will represent the cache we are
modelling. Each element defined in the graph, has an identificator. This id is
used to refer to the element, once it is created. In order to visualize correctly
the node, is required to specify the node capacity. In our example, the node will
have 2 dimensions. The first dimension will have 256 entries, the second 8.
\code 

#include "asim/dralServer.h"

DRAL_SERVER dral; 

UINT64 cache[256][8];

//
// Id used by dral to identify the cache in DRAL.
//
UINT16 cacheId;

bool lookup(UINT64 address)
{
    bool hit;
    UINT64 set;
    UINT64 way;

    get_cache_position(address, &set, &way);

    hit = (cache[set][way] == address);

    return hit;
}

int main(int argc, char argv *[])
{
    dral = new DRAL_SERVER_CLASS("Events.drl.gz");
    ...

    //
    // We define a new node to represent the cache in the DRAL graph.
    // The first parameter is the name of the node. We can ignore the
    // second param for now. The NewNode method 
    //
    cacheId = NewNode("Cache", -1);

    // 
    // We define the node capacity. Our cache will have 2 dimensions.
    // The first dimension has 256 entries, the second has 8.
    //
    UINT32 cacheSize[2] = {256, 8};
    SetNodeLayout(cacheId, 2, cacheSize);

    bool simulate = true;
    while(simulate)
    {
        ...
        hit = lookup(address);
        ...
    }
}

\endcode

Finally we have defined all the elements we want to visualize. Now we
must notify the activity. We are interested in visualize a red square
in the screen when a lookup results as a miss, and a green square when
an access hits in the cache.

The DRAL/Dreams design divides the activity from how that activity will
be displayed. In our simulator we must only report the activity, that
is generate an event when we hit/miss in the cache.

The best place to add the event generation code is the lookup function.
When when access the cache we will set a tag in the position we access.
All the elements that compose the simulated structure, can contains tags.
The tags are a sort of properties attached to the elements. The tags are
identified by name.

\code 

#include "asim/dralServer.h"

DRAL_SERVER dral; 

UINT64 cache[256][8];

UINT16 cacheId;

bool lookup(UINT64 address)
{
    bool hit;
    UINT64 set;
    UINT64 way;

    get_cache_position(address, &set, &way);

    hit = (cache[set][way] == address);

    //
    // We set the HIT tag to the position <set, wat> of the "Cache" node. The
    // position is specifyed as an array as long as the number of dimensions of
    // the structure.  We identify the tag by name.
    //
    UINT32 pos[2]; 
    pos[0] = set; 
    pos[1] = way;
    SetNodeTag(cacheId, "HIT", hit, 2, pos);

    return hit;
}

int main(int argc, char argv *[])
{
    dral = new DRAL_SERVER_CLASS("Events.drl.gz");
    ...

    cacheId = NewNode("Cache", -1);

    UINT32 cacheSize[2] = { 256, 8 };
    SetNodeLayout(cacheId, 2, cacheSize);

    bool simulate = true;
    while(simulate)
    {
        ...
        hit = lookup(address);
        ...
    }
}

\endcode

\section ADF Visualization description file

Once we have finished coding the activity description in our simulator, we need
to define how the activity is displayed in the screen. Dreams uses the ADF file
to know how to display the activity file (DRAL file).

The ADF file is an XML file where the Dreams user specifyes which and how are
the DRAL structures displayed. It is divided in sections. We will focus in those
sections that are relevat to Floorplan View.

The first section in the ADF is the tag specification section. All the tags that
will be used in the ADF must be defined in that section.

/code

<?xml version="1.0" encoding="ISO8859-1"?>
<!DOCTYPE dreams2-adf PUBLIC "-//ASIM//DTD DREAMS2/ADF 1.0//EN" "http://aforge.shr.intel.com/home/asim/xml/dtd/dreams2-adf-2.0.dtd">
<dreams-adf version="2.7">
    <!-- ....................................................... -->
    <!-- .. Descriptors for most common DRAL tags             .. -->    
    <!-- ....................................................... -->
    <tagdescriptor tagname="HIT" type="integer"/>

/endcode

Let's define the section where the Floorplan view is defined. That section is
defined as a HTML table. The elements are defined as \em rectangles

/code

<?xml version="1.0" encoding="ISO8859-1"?>
<!DOCTYPE dreams2-adf PUBLIC "-//ASIM//DTD DREAMS2/ADF 1.0//EN" "http://aforge.shr.intel.com/home/asim/xml/dtd/dreams2-adf-2.0.dtd">
<dreams-adf version="2.7">
    <!-- ....................................................... -->
    <!-- .. Descriptors for most common DRAL tags             .. -->    
    <!-- ....................................................... -->
    <tagdescriptor tagname="HIT" type="integer"/>

    <watchwindow node="DBUFFER">
        <rule>
            if(item_inside()) { color = "lightblue"; }
        </rule>
        <show itag = "ITEMID"/>
        <pointer ntag = "WRPTR" name="HEAD"/>
        <pointer ntag = "RDPTR" name="TAIL"/>
    </watchwindow>

</dreams-adf>

/endcode

*/


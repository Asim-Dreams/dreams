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

\page FLOORPLAN Floorplan View Tutorial 
 
In the "Floorplan View Tutorial" a simple example will be used to explain the 
motivation of the Floorplan display and the steps needed to obtain a representation.
We will start from the requirements that an architect will need and walk throught 
the tools that DRAL language and Dreams tools offers.

\section REQUIREMENTS Architect Requirements

Let's imagine an arquitect that is coding a cache simulator. To verify the
correctnes, the arquitect will be interested in seing the distribution of the
access to the cache.  He also should want to see the range of address that populates the cache, in order to detect how some data structures are scattered
across the structure. 

One tipical display that the architect would desire is to have the cache
displayed as two dimension array. In one dimension, the sets that compose the
cache, in the other dimension, the ways that compose the sets. 

\image html CacheDesire.png "An example of cache display"

\section VISUALIZATION Visualiztion desires

Let's start specifying how we will visualize the cache activity. Our cache
arquitect has decided that he is interested in first seeing the hit/miss
activity in his cache. In order to visualize the cache lookup, he will fill any
square that represents a cache element in green when the lookup results as hit,
and he will fill the element in red when the lookup results a miss.

For the rest of the tutorial, we will take the following code as the code that
simulates the cache behaviour. Next you can find only the cache lookup routine.

\code 

//
// Array that represents the cache. This cache has 256 sets and 8 ways. Assuming a 
// line of 16 bytes, we have a cache of 32Kb. The array will contain the address 
// of the element that resides in the entry
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
to define how the activity is displayed in the screen. Dreams uses an XML file
(called ADF, Architect Definition File) to describe how to display the activity
file (DRAL file).

The ADF file is an XML file where the Dreams user specifyes which and how are
the DRAL structures displayed. It is divided into sections. We will focus in
those sections that are relevat to Floorplan View.

The first section in the ADF is the tag specification section. All the tags that
will be used in the ADF must be defined in that section.

\verbatim
<?xml version="1.0" encoding="ISO8859-1"?>
<!DOCTYPE dreams2-adf PUBLIC "-//ASIM//DTD DREAMS2/ADF 1.0//EN" "http://aforge.shr.intel.com/home/asim/xml/dtd/dreams2-adf-2.0.dtd">
<dreams-adf version="2.7">
    <!-- ....................................................... -->
    <!-- .. Descriptors for most common DRAL tags             .. -->    
    <!-- ....................................................... -->
    <tagdescriptor tagname="HIT" type="integer"/>
</dreams-adf>

\endverbatim

Let's define the section where the Floorplan view is defined. To define where
to place the elements, ADF uses a HTML table. The elements are defined as \em
rectangles. We can only place one \em rectangle per table cell.

We will visualize the Cache we have previously defined. We define a \em
rectangle with name "Cache". We specify the name setting the attribute \em
node. The name must match the name given in the simulator. We also wan't to see
the name over the node, for that reason we define the attribute \em title. The
\em title attribute can be set with four different values: north, south, east
and west. Each of those values specify where we want the name displayed.
Finally we can specify if we want the title displayed inside the node display
(titleontop="y") or outside (titleontop="n").

A node definition must have also how to display all the dimensions it has. In
our case, the cache has two dimension. The first dimension has 256 elements,
and our architect has decided to display it as a square of 16 elements hight
per 16 elements wide. In each element, we will display, all
second dimension elements using a horizontal distribution.

\verbatim
<?xml version="1.0" encoding="ISO8859-1"?>
<!DOCTYPE dreams2-adf PUBLIC "-//ASIM//DTD DREAMS2/ADF 1.0//EN" "http://aforge.shr.intel.com/home/asim/xml/dtd/dreams2-adf-2.0.dtd">
<dreams-adf version="2.7">
    <!-- ....................................................... -->
    <!-- .. Descriptors for most common DRAL tags             .. -->    
    <!-- ....................................................... -->
    <tagdescriptor tagname="HIT" type="integer"/>
    <dreams3>
        <table>
            <tr>
                <td>
                    <rectangle node="Cache" title="north" titleontop="n">
                        <layout>
                            <dimension value="16x16"/>
                            <dimension value="h"/>
                        </layout>
                    </rectangle>
                </td>
            </tr>
        </table>
    </dreams3>
</dreams-adf>

\endverbatim

Until now, we have defined the distribution of all cache lines in our canvas. We
must now define how to display activity. In the simulator on each cache lookup,
we set what we called a slot tag. The tag we chose was the one called "HIT".
Well the idea is use the value stored that tag to color all the slots that
compose the cache. 

To specify a rule, we must define a \em rule environment. There we write the
rules using a C-like syntax. In Floorplan ADF, when you specify a rule, it will
be used for all the slots that compose the structure you are displaying. That
is, when we write:

\verbatim
    if(stag("HIT") == 1) { fcolor = "green"; }
\endverbatim

We mean: "if a slot has a tag named HIT and it has a value equal to 1, then fill the
shape that represent the slot with green". And this rule is applied to all the slots
that compose the cache.

The final ADF should look like:

\verbatim
<?xml version="1.0" encoding="ISO8859-1"?>
<!DOCTYPE dreams2-adf PUBLIC "-//ASIM//DTD DREAMS2/ADF 1.0//EN" "http://aforge.shr.intel.com/home/asim/xml/dtd/dreams2-adf-2.0.dtd">
<dreams-adf version="2.7">
    <!-- ....................................................... -->
    <!-- .. Descriptors for most common DRAL tags             .. -->    
    <!-- ....................................................... -->
    <tagdescriptor tagname="HIT" type="integer"/>
    <dreams3>
        <table>
            <tr>
                <td>
                    <rectangle node="Cache" title="north" titleontop="n">
                        <rule>
                            if(stag("HIT") == 1) { fcolor = "green"; }
                            if(stag("HIT") == 0) { fcolor = "red"; }
                        </rule>
                        <layout>
                            <dimension value="16x16"/>
                            <dimension value="h"/>
                        </layout>
                    </rectangle>
                </td>
            </tr>
        </table>
    </dreams3>
</dreams-adf>

\endverbatim

And that's it! You have generated a full Floorplan view.

*/


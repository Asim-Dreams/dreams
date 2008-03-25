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


// ========================================================================================  
/*! 

\page RESOURCE Resource View Tutorial 
 
\section INTRO Introduction

In the "Resource View Tutroial" a simple example will be used to explain the 
motivation of the Resource display and the steps needed to obtain a meaningfull representation.
In this tutorial we will use the learn by example approach with an incremental complexity
and clearely exposing the Architect's responsability in making the tool usefull and extraxting
all it's potential.

\section REQUIREMENTS Architect Requirements

Let's assume the arquitect is modeling a simple DLX-like CPU pipeline with five stages.
These stages

\image html 2dreams-screenshot "An example of complex resource display"

\section VISUALIZATION Visualiztion desires

\code 

int main(int argc, char argv *[])
{
    ...
    bool simulate = true;
    while(simulate)
    {
        ...
        
        do_fetch();
        do_decode();
        do_issue();
        do_exec();
        do_writeback();
        
        clock++;
        ...
    }
    ...
}

\endcode

\section Activity file generation (DRAL)

Let's start using the tools that Dreams and DRAL offers.  The first step to get
a graphical representation of the acitivity in the simulator is add DRAL
commands to the code. Those commands are used to notify the visualizer
(Dreams) all the actions that occur during the simulation.

To register all the activity during the simulation, we need a DralServer.  The
DralServer is the responsible of dumping all the DRAL commands to disk (DRAL file).
This file is read after by the Dreams visualization tool.

\code 

//
// We include the DralServer specification
//
#include "asim/dralServer.h"

DRAL_SERVER dral; 
 
int main(int argc, char argv *[])
{
    dral = new DRAL_SERVER_CLASS("Events.drl.gz");
    ...
    bool simulate = true;
    while(simulate)
    {
        ...
        do_fetch();
        do_decode();
        do_issue();
        do_exec();
        do_writeback();
        
        clock++;
        ...
    }
    ...
}
\endcode

Once a DralServer is created, we can start adding the micro architecure description of 
the machine we are modeling.
The DRAL language is based in two main abstractions: \em Nodes and \em Edges.
For DRAL, the model is represented as a graph. The nodes represents structures
that have storage. The edges represents the "wires" that transmit signals.

In our example we will define a node that will represent the cache we are
modelling. Each element defined in the graph, has an identificator. This id is
used to refer to the element, once it is created.

\code 

#include "asim/dralServer.h"

DRAL_SERVER dral; 

int main(int argc, char argv *[])
{
   dral = new DRAL_SERVER_CLASS("Events.drl.gz");

   // Create nodes for each major cluster
   fetchId     = dral->NewNode("Fetch");
   decodeId    = dral->NewNode("Decode");
   issueId     = dral->NewNode("Issue");
   execId      = dral->NewNode("Exec");
   wbId        = dral->NewNode("Writeback");
   rfId        = dral->NewNode("RF");

   // Create edges to represent the major wires and buses 
   fetch2decodeId  = dral->NewEdge(fetchId,decodeId,fetchBandwidth,fetch2decodeLatency,"fetch");
   decode2issueId  = dral->NewEdge(decodeId,fetchId,decodeBandwidth,decode2issueLatency,"decode");
   issue2execId    = dral->NewEdge(issueId,execId,issueBandwidth,issue2execatency,"issue");
   exec2wbId       = dral->NewEdge(execId,wbId,execBandwidth,exec2ewbLatency,"exec");
   wb2rfId         = dral->NewEdge(wbId,rfId,wbBandwidth,wbLatency,"wb");

   ...
   bool simulate = true;
   while(simulate)
   {
       ...
       do_fetch();
       do_decode();
       do_issue();
       do_exec();
       do_writeback();
       
       clock++;
       ...
   }
   ...
}
\endcode

Finally, we can start adding the activity description. In order to do so, we add yet another abstraction: the item.
An item is an object that has a finite lifespan, it is created, it is modified and moved across the machine and it is
eventually destroyed. You can use items to represent transactions of any kind in your machine. Items can represent 
instructions, cache lines, memory transactions, etc. For the sake of simplicity, in this example we will use items
to represent instructions and instruction blocks (non decoded bytes) moving in our five stages pipeline.

Let's start with the fetch stage of our machine, we want to represent the fact that a block of bytes is 
being fetched from the memory at a starting address, we gonna use the edge that carries the bytes to 
the decoder stage to accomplish that. A simple and intuitive way to do so is to abstract the block of 
read bytes with an item and move such item across the declared edge.

\code

do_fetch()
{
    ...
    if (not_stalled_fetch)
    {
       readBytes(PC);
       
       blockId = dral->NewItem();
       dral->SetItemTag(blockId,"FetchBlockAddress",PC);
       dral->MoveItem(fetch2decodeId,blockId);

       ...
    }
}
\endcode

Since the architect can accoringly specify the bandwith of a wire, more than
one item can actually be injected in an edge in a given cycle, following the previous
example let's assume we can decode up to four instructions from the fetched 
the code could be like the following:

\code

do_decode()
{
    ...
    if (not_stalled_decode)
    {
       decodeFourInstructions(fetched_bytes);
       
       instId1 = dral->NewItem();
       instId2 = dral->NewItem();
       instId3 = dral->NewItem();
       instId4 = dral->NewItem();

       dral->SetItemTag(instId1,"Address",PC);
       dral->SetItemTag(instId1,"Dissasembly",disasm_string[0]);
       
       dral->SetItemTag(instId2,"Address",PC+16);
       dral->SetItemTag(instId2,"Dissasembly",disasm_string[1]);

       dral->SetItemTag(instId3,"Address",PC+32);
       dral->SetItemTag(instId3,"Dissasembly",disasm_string[2]);
       
       dral->SetItemTag(instId4,"Address",PC+48);
       dral->SetItemTag(instId4,"Dissasembly",disasm_string[3]);

       dral->MoveItems(fetch2decodeId,instId1,instId2,instId3,instId4);
       
       ...
    }
}
\endcode

The resource view is also capable of showing state information. The microarchitecure
is often populated by state machines, the architect can use the node to abstract these
state machines and assign tags to such a node.

Let's assume that we have a floating-point division functional unit that is not fully 
pipelined on our machine and we stall issue until this operation completes.
The architect wants to know how frequent this event is for some particular program since
this condition hurts performance. We may represent information this assiging this stall 
condition to a node like in the following code: 

\code
do_issue()
{
    ...
    if (ongoing_division)
    {
       dral->SetNodeTag(issueId,"DivisionStall",1);
       ...
    }
    else
    {
       dral->SetNodeTag(issueId,"DivisionStall",0);


       issueInstructions();
       ...
    }
}
\endcode

Another common need for the architect is to represent the utilization of a given
resource, let's assume that our example machine can issue up to four instructions
per cycle, we want to see which issue ports have been used and which one have not.
One code that would allow to express that in the dral file would be:

\code
issueInstructions()
{
    ...
   int issued_instructions =0;     
   for (int i=0;i<4;i++)     
   {
       sprintf(tgName,"IssuePort_%d",i);
       dral->SetNodeTag(issueId,tgName,canIssue[i]);
       if (canIssue[i] && haveInstructionToIssue[i])
       {
           // issue the instruction
           issued_instructions++;
       }
   }
   dral->SetNodeTag(issuedId,"IssuedInstructions",issued_instructions);
   ...
}
\endcode
 */


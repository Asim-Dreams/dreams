/*
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
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "asim/dralClient.h"
#include "asim/dralServer.h"
#include "asim/dralListenerOld.h"
#include "asim/dralListenerConverter.h"

using namespace std;


bool finish=false;
bool only_graph=false;
bool list_clocks=false;
UINT32 num_events=0;
DRAL_SERVER server;
UINT64 current_cycle=0;

bool count_cycles = false;
INT64 print_cycle = 0;

UINT64 first_cycle=0;
UINT64 last_cycle=UINT64_MAX;
UINT16 cid=0; //Reference clock id

set<UINT32> liveItems;

class DRALLISTENER_CLASS : public DRAL_LISTENER_OLD_CLASS
{
  public:
    void Cycle (UINT64);
    void NewItem (UINT32);
    void SetTagSingleValue (
        UINT32, char *, UINT64, unsigned char);
    void SetTagString (
        UINT32, char *, char *, unsigned char);
    void SetTagSet (
        UINT32, char *, UINT32, UINT64 *, unsigned char);
    void MoveItems (UINT16, UINT32, UINT32 *);
    void MoveItemsWithPositions (UINT16, UINT32, UINT32 *, UINT32 *);
    void EnterNode (UINT16, UINT32, UINT32);
    void ExitNode (UINT16, UINT32);
    void DeleteItem (UINT32);
    void EndSimulation (void);
    void AddNode (UINT16, char *, UINT16, UINT16);
    void AddEdge (UINT16, UINT16, UINT16, UINT32, UINT32, char *);
    void SetCapacity (UINT16,UINT32, UINT32 *, UINT16);
    void SetHighWaterMark (UINT16, UINT32);
    void Comment (UINT32 magic_num, char * comment);
    void Comment (char * );
    void Error (char * e);
    void NonCriticalError (char * e);

    void Version (UINT16);
    void SetNodeLayout (UINT16,UINT32, UINT16, UINT32 *);
    void SetItemTag (UINT32, char *, UINT64);
    void SetItemTagString (UINT32, char *, char *);
    void SetItemTagSet (UINT32, char *, UINT32, UINT64 *);
    void EnterNode (UINT16, UINT32, UINT16, UINT32 *);
    void ExitNode (UINT16, UINT32, UINT16, UINT32 *);
    void SetNodeTag (UINT16, char *, UINT64, UINT16, UINT32 *);
    void SetNodeTagString (UINT16, char *, char *, UINT16, UINT32 *);
    void SetNodeTagSet (UINT16, char *, UINT16, UINT64 *, UINT16, UINT32 *);

    void SetCycleTag (char *, UINT64);
    void SetCycleTagString (char *, char *);
    void SetCycleTagSet (char *, UINT32, UINT64 *);
    void NewNode (UINT16, char *, UINT16, UINT16);
    void NewEdge (UINT16, UINT16, UINT16, UINT32, UINT32, char *);
    void SetNodeInputBandwidth (UINT16, UINT32);
    void SetNodeOutputBandwidth (UINT16, UINT32);
    void StartActivity (UINT64);
    void SetTagDescription (char *, char *);

    void CommentBin (UINT16, char *, UINT32);

    void SetNodeClock (UINT16, UINT16);
    void NewClock (UINT16, UINT64, UINT16, const char []);
    void NewClock (UINT16, UINT64, UINT16, UINT16, const char []);
    void Cycle (UINT16, UINT64, UINT16);
};


void
DRALLISTENER_CLASS::SetNodeClock (UINT16 nodeId, UINT16 clockId)
{
   server->SetNodeClock(nodeId,clockId);
   num_events++;
}

void
DRALLISTENER_CLASS::NewClock (
    UINT16 clockId, UINT64 freq, UINT16 skew, const char name [])
{
    if (list_clocks)
    {
        cout << "Clock id: " << clockId << ", name: " << name << ", freq: " << freq
             << ", skew: " << skew << endl;
    }
    server->NewClock(clockId,freq,skew,name);
    num_events++;
}

void
DRALLISTENER_CLASS::NewClock (
    UINT16 clockId, UINT64 freq, UINT16 skew, UINT16 divisions, const char name [])
{
    if (list_clocks)
    {
        cout << "Clock id: " << clockId << ", name: " << name << ", freq: " << freq
             << ", skew: " << skew << ", divisions: " << divisions << endl;
    }
    server->NewClock(clockId,freq,skew,divisions, name);
    num_events++;
}

void
DRALLISTENER_CLASS::Cycle (UINT16 clockId, UINT64 cycle, UINT16 phase)
{
    if (clockId == cid)
    {
        current_cycle=cycle;
        if (count_cycles && print_cycle && (current_cycle%print_cycle == 0))
        {
            cerr << "Cycle: " << current_cycle << endl;
        }
    }
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        if (current_cycle == first_cycle)
        {
            liveItemsList::iterator it = liveItems.begin();
            while (it != liveItems.end())
            {
                server->NewItem(*it);
                ++it;
            }
            liveItems.clear();
        }
        server->Cycle(clockId, cycle, phase);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::Cycle(UINT64 n)
{
    current_cycle=n;
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->Cycle (current_cycle);
    }
    num_events++;
    if (count_cycles && print_cycle && (current_cycle%print_cycle == 0))
    {
        cerr << "Cycle: " << current_cycle << endl;
    }
}

void
DRALLISTENER_CLASS::NewItem(UINT32 item_id)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->NewItem (item_id);
    }
    else
    {
        liveItems.insert(item_id);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::MoveItems(UINT16 edge_id, UINT32 n, UINT32 item_id[])
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->MoveItems (edge_id, n, item_id);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::MoveItemsWithPositions (
    UINT16 edge_id, UINT32 n, UINT32 * items, UINT32 * positions)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->MoveItems(edge_id,n,items,positions);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::DeleteItem(UINT32 item_id)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->DeleteItem (item_id);
    }
    else
    {
        liveItemsList::iterator it = liveItems.find(item_id);
        if (it != liveItems.end())
        {
            liveItems.erase(it);
        }
    }
    num_events++;
}

void
DRALLISTENER_CLASS::EndSimulation(void)
{
    finish=true;
    cerr << "Simulation end" << endl;
}

void
DRALLISTENER_CLASS::Error(char * error)
{
    cerr << error << endl;
}

void
DRALLISTENER_CLASS::NonCriticalError(char * error)
{
    cerr << error << endl;
}

void
DRALLISTENER_CLASS::Version (UINT16 version)
{
    if (version < 2)
    {
        cerr << "The file you are trying to translate is version: " << version
        << endl;
        cerr << "The output file will be version 2, so the DRAL 1.0 commands "
        " no longer existing in DRAL 2.0 will be lost" << endl;
    }
    num_events++;
}






void
DRALLISTENER_CLASS::NewNode(
    UINT16 node_id, char * name,UINT16 parent_id, UINT16 instance)
{
    server->NewNode (node_id,name,parent_id,instance);
    num_events++;
}

void
DRALLISTENER_CLASS::NewEdge(
    UINT16 source, UINT16 destination, UINT16 edge_id, 
    UINT32 bandwidth, UINT32 latency, char * name)
{
    server->NewEdge (edge_id,source,destination,bandwidth,latency,name);
    num_events++;
}

void
DRALLISTENER_CLASS::SetNodeLayout (
    UINT16 node_id,UINT32 , UINT16 dim, UINT32 * cap)
{
    server->SetNodeLayout(node_id,dim,cap);
    num_events++;
}

void
DRALLISTENER_CLASS::SetNodeInputBandwidth (UINT16 node_id, UINT32 bandwidth)
{
    server->SetNodeInputBandwidth(node_id, bandwidth);
    num_events++;
}

void
DRALLISTENER_CLASS::SetNodeOutputBandwidth (UINT16 node_id, UINT32 bandwidth)
{
    server->SetNodeOutputBandwidth(node_id, bandwidth);
    num_events++;
}

void
DRALLISTENER_CLASS::StartActivity (UINT64)
{
    if (only_graph || list_clocks)
    {
        finish=true;
        return;
    }
    server->StartActivity();
    num_events++;
}

void DRALLISTENER_CLASS::SetTagDescription (char * tag, char * desc)
{
    server->SetTagDescription(tag,desc);
    num_events++;
}

void
DRALLISTENER_CLASS::SetItemTag (
    UINT32 item_id, char * tag_name, UINT64 value)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetItemTag(item_id,tag_name,value);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::SetItemTagString (UINT32 item_id, char * tag_name, char * str)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetItemTag(item_id,(const char *)tag_name,str);
    }
    num_events++;
}


void
DRALLISTENER_CLASS::SetItemTagSet (
    UINT32 item_id, char * tag_name, UINT32 n, UINT64 * nval)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetItemTag(item_id,(const char *)tag_name,n,nval);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::Comment (UINT32 magic_num, char * comment)
{
    if (!only_graph)
    {
        server->Comment(magic_num, comment);
        num_events++;
    }
}

void
DRALLISTENER_CLASS::CommentBin (
    UINT16 magic_num, char * contents, UINT32 length)
{
    if (!only_graph)
    {
        server->CommentBin(magic_num, contents, length);
        num_events++;
    }
}

void DRALLISTENER_CLASS::SetCycleTag (char * tag_name, UINT64 value)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetCycleTag(tag_name,value);
    }
    num_events++;
}


void DRALLISTENER_CLASS::SetCycleTagString (char * tag_name, char *str)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetCycleTag(tag_name,str);
    }
    num_events++;
}


void DRALLISTENER_CLASS::SetCycleTagSet (
    char * tag_name, UINT32 n, UINT64 * val)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetCycleTag(tag_name,n,val);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::EnterNode (
    UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 * pos)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->EnterNode(node_id,item_id,dim,pos);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::ExitNode (
    UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 * pos)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->ExitNode(node_id,item_id,dim,pos);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::SetNodeTag (
    UINT16 node_id, char * tag_name, UINT64 val, UINT16 lev, UINT32 * pos)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetNodeTag(node_id,(const char *)tag_name,val,lev,pos);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::SetNodeTagString (
    UINT16 node_id, char * tag_name, char * str, UINT16 lev, UINT32 * pos)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetNodeTag(
        node_id,(const char *)tag_name,(const char *)str,lev,pos);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::SetNodeTagSet (
    UINT16 node_id, char * tag_name, UINT16 n,
    UINT64 * nval, UINT16 lev, UINT32 * pos)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetNodeTag(
        node_id,(const char *)tag_name,n,nval,lev,pos);
    }
    num_events++;
}


void
DRALLISTENER_CLASS::SetHighWaterMark (UINT16 node_id, UINT32 mark)
{
    server->SetNodeTag(node_id,"HIGHWATERMARK",mark);
    num_events++;
}


void
DRALLISTENER_CLASS::SetTagSingleValue(
    UINT32 item_id, char * tag_name, UINT64 value, unsigned char)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetItemTag(item_id,tag_name,value);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::SetTagString(
    UINT32 item_id, char * tag_name, char * str, unsigned char)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetItemTag(item_id,tag_name,str);
    }
    num_events++;
}

void
DRALLISTENER_CLASS::SetTagSet(
    UINT32 item_id,  char * tag_name,UINT32 n,UINT64 * set, unsigned char)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        server->SetItemTag(item_id,tag_name,n,set);
    }
    num_events++;
}


void
DRALLISTENER_CLASS::EnterNode(UINT16, UINT32, UINT32)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        cerr << "Warning: Found a DRAL 1.0 EnterNode. Ignored." << endl;
    }
    num_events++;
}

void
DRALLISTENER_CLASS::ExitNode(UINT16, UINT32)
{
    if (current_cycle >= first_cycle && current_cycle <= last_cycle)
    {
        cerr << "Warning: Found a DRAL 1.0 ExitNode. Ignored." << endl;
    }
    num_events++;
}

void
DRALLISTENER_CLASS::AddNode(
    UINT16 node_id, char * name,UINT16 parent_id, UINT16 instance)
{
    server->NewNode (node_id,name,parent_id,instance);
    num_events++;
}

void
DRALLISTENER_CLASS::AddEdge(
    UINT16 source, UINT16 destination, UINT16 edge_id, 
    UINT32 bandwidth, UINT32 latency, char * name)
{
    server->NewEdge (edge_id,source,destination,bandwidth,latency,name);
    num_events++;
}


void
DRALLISTENER_CLASS::SetCapacity (
    UINT16 node_id, UINT32, UINT32 * capacities, UINT16 n)
{
    server->SetNodeLayout(node_id,n,capacities);
    num_events++;
}


void
DRALLISTENER_CLASS::Comment (char * comment)
{
    server->Comment(0, comment);
    num_events++;
}


int main(int argn, char * argv []) {

    DRALLISTENER_CLASS listener;
    bool ascii_output = true;
    bool count_events = false;
    bool compression = false;
    
    INT64 print_events = 0;

    for (INT32 i=1; i<argn; i++)
    {
        if (!strcmp(argv[i],"-b"))
        {
            ascii_output=false;
        }
        else if (!strcmp(argv[i],"-e") && (argn > i))
        {
            print_events = strtol(argv[++i], NULL, 0);
            if (print_events)
            {
                count_events=true;
            }
        }
        else if (!strcmp(argv[i],"-c") && (argn > i))
        {
            print_cycle = strtol(argv[++i], NULL, 0);
            if (print_cycle)
            {
                count_cycles=true;
            }
        }
        else if (!strcmp(argv[i],"-fc") && (argn > i))
        {
            first_cycle = strtol(argv[++i], NULL, 0);
            cerr << "WARNING: The use of the -fc paramter may produce a "
            << "no coherent dralFileOut" << endl;
        }
        else if (!strcmp(argv[i],"-lc") && (argn > i))
        {
            last_cycle = strtol(argv[++i], NULL, 0);
            cerr << "WARNING: The use of the -lc paramter may produce a "
            << "no coherent dralFileOut" << endl;
        }
        else if (!strcmp(argv[i],"-z"))
        {
            compression = true;
        }
        else if (!strcmp(argv[i],"-og"))
        {
            only_graph = true;
        }
        else if (!strcmp(argv[i],"-listclocks"))
        {
            list_clocks = true;
        }
        else if (!strcmp(argv[i],"-cid") && (argn > i))
        {
            cid = strtol(argv[++i], NULL, 0);
        }
        else
        {
            cerr << "Usage: dralTranslator [-b] [-z] [-og] [-e <n>|-c <n>] [-fc <n>] "
            << "[-lc <n>] <dralFileIn >dralFileOut" << endl
            << "\t-b:\t Makes the output file in binary format." << endl
            << "\t\t The default is ascii format" << endl
            << "\t-z:\t Compress the output using zlib" << endl
            << "\t-e <n>:  Reports the number of events processed every <n> "
            << "events" << endl
            << "\t-c <n>:  Reports the number of cycles processed every <n> "
            << "cycles" << endl
            << "\t-og:\t Only writes the graph definition" << endl
            << "\t-listclocks:\t Shows all the different clock ids of the "
            << "events file" << endl
            << "\t-cid <clockId>:\t Specifies the reference clock id" << endl
            << "\t-fc <n>: All the events found before the <n>th cycle in "
            << "dralFileIn will be ignored" << endl
            << "\t-lc <n>: All the events found after the <n>th cycle in "
            << "dralFileIn will be ignored" << endl
            << "\tWARNING: The use of the -fc or -lc paramters may produce a "
            << "no coherent dralFileOut" << endl;
            exit(-1);
        }
    }

    if (ascii_output)
    {
        server = new DRAL_SERVER_CLASS ("dummy_file",32768,false,false,false);
        server->SwitchToDebug(1,compression);
    }
    else
    {
        server = new DRAL_SERVER_CLASS (1,32768,false,compression,false);
    }

    DRAL_LISTENER_CONVERTER_CLASS converter(&listener);

    DRAL_CLIENT_CLASS client(0, &converter, 64 * 1024);

    if (!list_clocks)
    {
        server->TurnOn();
    }

    while (!finish)
    {
        UINT32 n = client.ProcessNextEvent(true,(count_events | only_graph ?1:10000));
        if (count_events && (num_events%print_events == 0))
        {
            cerr << "Events: " << num_events << endl;
        }
        if (!n || current_cycle > last_cycle)
        {
            finish=true;
        }
    }
    delete server;
    exit(1);
}

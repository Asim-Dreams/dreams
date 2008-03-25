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
#include <map>
#include <list>
#include "asim/dralClient.h"
#include "asim/dralServer.h"
#include "asim/dralListenerOld.h"
#include "asim/dralListenerConverter.h"
#include "PT_Colors.h"
#include "PT_Data.h"

using namespace std;

static bool detailed_output = false;

bool finish=false;
UINT32 num_events=0;
UINT64 current_cycle=0;

bool count_cycles = false;
INT64 print_cycle = 0;

UINT64 first_cycle=0;
UINT64 last_cycle=UINT64_MAX;


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
    void SetTagDescription ( char *, char * );
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
    void CommentBin (UINT16, char *, UINT32) {}
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

    void SetNodeClock(UINT16, UINT16) {}
    void NewClock(UINT16, UINT64, UINT16, UINT16, const char []) {}
    void Cycle(UINT16, UINT64, UINT16);


    map<UINT32,list<UINT32> > DID2ItemIDs; // maps a DID to a vector of item_id's

    map<UINT16,char*>         NodeList;
    map<UINT16,char*>         EdgeList;
    map<string,string>        EdgeName;
    map<UINT16,UINT16>        EdgeFromList;
    map<UINT16,UINT16>        EdgeToList;
    map<UINT16,UINT16>        EdgeLatency;
    map<UINT32,UINT64>        ItemBirth;
    map<UINT32,UINT64>        ItemDeath;
    map<UINT32,char*>         ItemDis;
    map<UINT32,UINT64>        ItemIP;
    map<UINT32,UINT64>        ItemUID;
    map<UINT32,UINT64>        ItemDID;
    map<UINT32,UINT16>        CPU;
    map<UINT32,list<UINT16> > MoveListEdge;
    map<UINT32,list<UINT64> > MoveListCycle;
    map<UINT32,list<UINT64> > ReplayList;
    map<UINT32,list<UINT64> > BranchMPList;
    map<UINT32,list<UINT64> > DcacheMiss;
    map<UINT32,list<UINT64> > DcacheHit;
    map<UINT32,list<UINT64> > McacheMiss;
    map<UINT32,list<UINT64> > McacheHit;
    map<UINT32,list<UINT64> > RcacheMiss;
    map<UINT32,list<UINT64> > RcacheHit;
    map<UINT32,char>          InstType;

    PT_ASCIIWriteStream *ptv_out;
    PT_ID ptv_id;

    void PrintHeader(void);
    void PrintPTV(void);

    void DefineDetailedEvent(char* event_name, char character);

  private:
    PT_Config_Record *instruction_record;
    PT_Config_Record *offpath_record;
    PT_Config_Record *bundle_record;
    PT_Config_Data  *IP_DataDef;
    PT_Config_Data  *UID_DataDef;
    PT_Config_Data  *Thread_DataDef;
    PT_Config_Data  *null_DataDef;
    PT_Config_Data  *Edge_DataDef;
    PT_Config_Data  *Type_DataDef;
    PT_Config_Data  *Dis_DataDef;
    PT_Config_Event *Birth_EventDef;
    PT_Config_Event *Retire_EventDef;
    PT_Config_Event *Replay_EventDef;
    PT_Config_Event *BranchMP_EventDef;
    PT_Config_Event *Issued_EventDef;
    PT_Config_Event *Issued_EventDef2;
    PT_Config_Event *Execute_EventDef;
    PT_Config_Event *DCMiss_EventDef;
    PT_Config_Event *DCHit_EventDef;
    PT_Config_Event *MCMiss_EventDef;
    PT_Config_Event *MCHit_EventDef;
    PT_Config_Event *RCMiss_EventDef;
    PT_Config_Event *RCHit_EventDef;
    map<char*, PT_Config_Event*> Detailed_EventDef;
    map<char*, PT_Config_Event*> Other_EventDef;
  public:
    map<string, char> DetailedEvents;

};

char *strip_bad_chars(char *in) {
    char *out = strdup(in);

    // PTV doesn't like .'s in it's strings.  :(
    for(unsigned int x=0; x<strlen(in); x++) {
        if(in[x] == '.') {
            //printf("Converting %c to _\n", out[x]);
            out[x] = '_';
        }
        // or -'s
        if(in[x] == '-') {
            out[x] = '_';
        }
    }
    return(out);
}

void DRALLISTENER_CLASS::Cycle(UINT64 n) {
    //current_cycle=n;
}

void DRALLISTENER_CLASS::Cycle(UINT16 x, UINT64 y, UINT16 z) {
    //cout << "Cycle3  " << x << "  " << y << "  " << z << endl;
    if(x==0) {
        // Arbitrarily choosing to use clock domain 0 as our cycle count
        current_cycle=y;
    }
}

void DRALLISTENER_CLASS::NewItem(UINT32 item_id) {
    //cout << "New item id=" << item_id << " cycle " << current_cycle << endl;
    ItemBirth[item_id] = current_cycle;
}

void DRALLISTENER_CLASS::MoveItems(UINT16 edge_id, UINT32 n, UINT32 item_id[]) {
    for(UINT32 i=0;i<n; i++) {
        MoveListEdge[ item_id[i]].push_back(edge_id);
        MoveListCycle[item_id[i]].push_back(current_cycle);
    }
    num_events++;
}

void DRALLISTENER_CLASS::MoveItemsWithPositions (UINT16 edge_id, UINT32 n, UINT32 * items, UINT32 * positions) {
    //cout << "MoveItemsWithPositions: " << edge_id << " " <<  n << endl;
}

void DRALLISTENER_CLASS::DeleteItem(UINT32 item_id) {
    ItemDeath[item_id] = current_cycle;
}

void DRALLISTENER_CLASS::EndSimulation(void) {
    finish=true;
    cout << "Simulation end." << endl;
}

void DRALLISTENER_CLASS::Error(char * error) {
    cerr << error << endl;
}

void DRALLISTENER_CLASS::NonCriticalError(char * error) {
    cerr << error << endl;
}

void DRALLISTENER_CLASS::Version (UINT16 version) {
    if (version < 2)
    {
        cerr << "The file you are trying to translate is version: " << version
        << endl;
        cerr << "The output file will be version 2, so the DRAL 1.0 commands "
        " no longer existing in DRAL 2.0 will be lost" << endl;
    }
    num_events++;
}

void DRALLISTENER_CLASS::NewNode(UINT16 node_id, char * name,UINT16 parent_id, UINT16 instance) {
    NodeList[node_id] = strdup(name);
}

void DRALLISTENER_CLASS::NewEdge( UINT16 source, UINT16 destination, UINT16 edge_id, UINT32 bandwidth, UINT32 latency, char * name) {
    //cout << "New edge  from " << NodeList[source] << " to " << NodeList[destination] << " " << name << endl;
    char uname[60];
    sprintf(uname, "%s_to_%s__%s\0", strip_bad_chars(NodeList[source]), strip_bad_chars(NodeList[destination]),strip_bad_chars(name));

    if(EdgeName.find((string)uname)==EdgeName.end()) {
        EdgeList[edge_id]     = strdup(uname);
        EdgeName[(string)EdgeList[edge_id]]  = (string)strdup(name);
        //cout << "Adding " << edge_id << " " << uname << " to name list" << endl;
        EdgeFromList[edge_id] = source;
        EdgeToList[edge_id]   = destination;
        EdgeLatency[edge_id]  = latency;
    } else {
        //cout << "Already exists:  " << edge_id << " " << uname << " in name list" << endl;
    }

    num_events++;

}

void DRALLISTENER_CLASS::SetNodeLayout (UINT16 node_id,UINT32 , UINT16 dim, UINT32 * cap) {
    //cout << "Node layout"  << endl;
}

void DRALLISTENER_CLASS::SetNodeInputBandwidth (UINT16 node_id, UINT32 bandwidth) {
    //cout << "Set node input bandwidth"  << endl;
}

void DRALLISTENER_CLASS::SetNodeOutputBandwidth (UINT16 node_id, UINT32 bandwidth) {
    //cout << "Set node output bandwidth"  << endl;
}

void DRALLISTENER_CLASS::StartActivity (UINT64 cycle) {
    //cout << "Start activity "  << endl;
}

void DRALLISTENER_CLASS::SetItemTag ( UINT32 item_id, char * tag_name, UINT64 value) {
    if(!(strcmp(tag_name, "VPC")) || !(strcmp(tag_name, "PA"))) {
        ItemIP[item_id] = value;
    } else if(!(strcmp(tag_name, "CPU"))) {
        CPU[item_id] = value;
    } else if(!(strcmp(tag_name, "UID"))) {
        ItemUID[item_id] = value;
    } else if(!(strcmp(tag_name, "DID"))) {
        ItemDID[item_id] = value;
//      if( DID2ItemIDs[value].find(item_id) == DID2ItemIDs[value].end()) {
            DID2ItemIDs[value].push_back(item_id);
//      }
    }    
}

void DRALLISTENER_CLASS::SetItemTagString (UINT32 item_id, char * tag_name, char * str) {
    if(!(strcmp(tag_name, "DIS")) || !(strcmp(tag_name, "CMDTYPE"))) {
        ItemDis[item_id] = strdup(str);
    } else if(!(strcmp(tag_name, "EXCPTYPE"))) {
        if(!(strcmp(str, "REPLAY"))) {
            ReplayList[item_id].push_back(current_cycle);
        } else if(!(strcmp(str, "CBRANCHMP"))) {
            BranchMPList[item_id].push_back(current_cycle);
        }
    } else if(!(strcmp(tag_name, "DCACHE"))) {
        if(!(strcmp(str, "miss"))) {
            DcacheMiss[item_id].push_back(current_cycle);
        } else if(!(strcmp(str, "hit"))) {
            DcacheHit[item_id].push_back(current_cycle);
        }
    } else if(!(strcmp(tag_name, "RCACHE"))) {
        if(!(strcmp(str, "miss"))) {
            RcacheMiss[item_id].push_back(current_cycle);
        } else if(!(strcmp(str, "hit"))) {
            RcacheHit[item_id].push_back(current_cycle);
        }
    } else if(!(strcmp(tag_name, "TYPE"))) {
        InstType[item_id] = str[0];
    } else {
        //cout << "Set item tag string " << tag_name << "  " << str << endl;
    }
}


void DRALLISTENER_CLASS::SetItemTagSet ( UINT32 item_id, char * tag_name, UINT32 n, UINT64 * nval) {
    //cout << "Set item tag set: " << tag_name << endl;
}

void DRALLISTENER_CLASS::Comment (UINT32 magic_num, char * comment) {
    //cout << "Comment: " << comment << endl;
}

void DRALLISTENER_CLASS::SetCycleTag (char * tag_name, UINT64 value) {
    //cout << "Set cycle tag: " << tag_name << endl;
}


void DRALLISTENER_CLASS::SetCycleTagString (char * tag_name, char *str) {
    //cout << "Set cycle tag string: " << tag_name << endl;
}


void DRALLISTENER_CLASS::SetCycleTagSet (
    char * tag_name, UINT32 n, UINT64 * val)
{
    //cout << "Set cycle tag set: " << tag_name << endl;
}

void DRALLISTENER_CLASS::EnterNode ( UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 * pos) {
    //cout << "Enter node: " << node_id << endl;
}

void DRALLISTENER_CLASS::ExitNode ( UINT16 node_id, UINT32 item_id, UINT16 dim, UINT32 * pos) {
    //cout << "Exit node: " << node_id << endl;
}

void DRALLISTENER_CLASS::SetNodeTag ( UINT16 node_id, char * tag_name, UINT64 val, UINT16 lev, UINT32 * pos) {
    //cout << "Set node tag: " << tag_name << endl;
}

void DRALLISTENER_CLASS::SetNodeTagString ( UINT16 node_id, char * tag_name, char * str, UINT16 lev, UINT32 * pos) {
    //cout << "Set node tag string: " << tag_name << endl;
}

void DRALLISTENER_CLASS::SetNodeTagSet ( UINT16 node_id, char * tag_name, UINT16 n, UINT64 * nval, UINT16 lev, UINT32 * pos) {
    //cout << "Set node tag set: " << tag_name << endl;
}


void DRALLISTENER_CLASS::SetHighWaterMark (UINT16 node_id, UINT32 mark) {
    //cout << "Set HighWaterMark: " << node_id << "  " << mark << endl;
}


void DRALLISTENER_CLASS::SetTagSingleValue( UINT32 item_id, char * tag_name, UINT64 value, unsigned char) {
}

void DRALLISTENER_CLASS::SetTagString( UINT32 item_id, char * tag_name, char * str, unsigned char) {
}

void DRALLISTENER_CLASS::SetTagSet( UINT32 item_id,  char * tag_name,UINT32 n,UINT64 * set, unsigned char) {
}

void DRALLISTENER_CLASS::SetTagDescription ( char *, char * ) {
}

void DRALLISTENER_CLASS::EnterNode(UINT16, UINT32, UINT32) {
        cerr << "Warning: Found a DRAL 1.0 EnterNode. Ignored." << endl;
}

void DRALLISTENER_CLASS::ExitNode(UINT16, UINT32) {
        cerr << "Warning: Found a DRAL 1.0 ExitNode. Ignored." << endl;
}

void DRALLISTENER_CLASS::AddNode( UINT16 node_id, char * name,UINT16 parent_id, UINT16 instance) {
}

void DRALLISTENER_CLASS::AddEdge( UINT16 source, UINT16 destination, UINT16 edge_id, UINT32 bandwidth, UINT32 latency, char * name) {
}


void DRALLISTENER_CLASS::SetCapacity ( UINT16 node_id, UINT32, UINT32 * capacities, UINT16 n) {
}


void DRALLISTENER_CLASS::Comment (char * comment) {
    comment;
}

void DRALLISTENER_CLASS::DefineDetailedEvent(char* event_name, char character) {
    //else if(detailed_output && !strncmp("CBOX_to_EMPTY_STOP_1__RING_STOP_0_RING_STOP_11_", i->second,47)) 
    Detailed_EventDef[event_name] = new PT_Config_Event(event_name, character, Pipe_Blue, "Detail");
    ptv_out->WriteStatement(*Detailed_EventDef[event_name],&ptv_id);
}

void DRALLISTENER_CLASS::PrintHeader() {
    printf("Printing header\n");
    instruction_record = new PT_Config_Record("instruction", "Instruction record");
    ptv_out->WriteStatement(*instruction_record,&ptv_id);

    offpath_record = new PT_Config_Record("offpath", "Off-path Instruction record");
    ptv_out->WriteStatement(*offpath_record,&ptv_id);

    bundle_record = new PT_Config_Record("bundle", "Bundle Instruction record");
    ptv_out->WriteStatement(*bundle_record,&ptv_id);

    IP_DataDef = new PT_Config_Data("IP", Pipe_String, "Instruction IP");
    ptv_out->WriteStatement(*IP_DataDef,&ptv_id);

    UID_DataDef = new PT_Config_Data("UID", Pipe_String, "Unique Instruction ID");
    ptv_out->WriteStatement(*UID_DataDef,&ptv_id);

    Thread_DataDef = new PT_Config_Data("Thread", Pipe_Integer, "Core Number");
    ptv_out->WriteStatement(*Thread_DataDef,&ptv_id);

    null_DataDef = new PT_Config_Data("null", Pipe_String, "null");
    ptv_out->WriteStatement(*null_DataDef,&ptv_id);

    Edge_DataDef = new PT_Config_Data("Edge", Pipe_String, "edge");
    ptv_out->WriteStatement(*Edge_DataDef,&ptv_id);

    Type_DataDef = new PT_Config_Data("Type", Pipe_String, "type");
    ptv_out->WriteStatement(*Type_DataDef,&ptv_id);

    Dis_DataDef = new PT_Config_Data("Disassembly", Pipe_String, "disassembly");
    ptv_out->WriteStatement(*Dis_DataDef,&ptv_id);

    Birth_EventDef = new PT_Config_Event("Birth", '(', Pipe_Dark_Green, "Instruction birth");
    ptv_out->WriteStatement(*Birth_EventDef,&ptv_id);

    Retire_EventDef = new PT_Config_Event("Retire", ')', Pipe_Yellow, "Instruction retire");
    ptv_out->WriteStatement(*Retire_EventDef,&ptv_id);

    Replay_EventDef = new PT_Config_Event("Replay", 'R', Pipe_Red, "Replay");
    ptv_out->WriteStatement(*Replay_EventDef,&ptv_id);

    BranchMP_EventDef = new PT_Config_Event("BranchMP", 'B', Pipe_Red, "Replay");
    ptv_out->WriteStatement(*BranchMP_EventDef,&ptv_id);

    DCMiss_EventDef = new PT_Config_Event("DCMiss", 'd', Pipe_Red, "Data Cache Miss");
    ptv_out->WriteStatement(*DCMiss_EventDef,&ptv_id);

    DCHit_EventDef = new PT_Config_Event("DCHit", 'd', Pipe_Blue, "Data Cache Hit");
    ptv_out->WriteStatement(*DCHit_EventDef,&ptv_id);

    RCMiss_EventDef = new PT_Config_Event("RCMiss", 'r', Pipe_Red, "Ring Cache Miss");
    ptv_out->WriteStatement(*RCMiss_EventDef,&ptv_id);

    RCHit_EventDef = new PT_Config_Event("RCHit", 'r', Pipe_Blue, "Ring Cache Hit");
    ptv_out->WriteStatement(*RCHit_EventDef,&ptv_id);

    map<UINT16, char*>::iterator i=EdgeList.begin(); 

    for(; i != EdgeList.end(); i++) { 
        if(detailed_output && !strcmp("MRQ_2_LSB_MC_HIT", (EdgeName[(string)i->second]).c_str())) {
            MCHit_EventDef = new PT_Config_Event(i->second, 'm', Pipe_Blue, "Middle Cache Hit");
            ptv_out->WriteStatement(*MCHit_EventDef,&ptv_id);
        } else if(detailed_output && !strcmp("MRQ_2_LSB_MC_MISS", (EdgeName[(string)i->second]).c_str())) {
            MCMiss_EventDef = new PT_Config_Event(i->second, 'm', Pipe_Red, "Middle Cache Miss");
            ptv_out->WriteStatement(*MCMiss_EventDef,&ptv_id);
        } else if(detailed_output && !strcmp("ISSUED_INST", (EdgeName[(string)i->second]).c_str())) {
            Issued_EventDef = new PT_Config_Event(i->second, 'I', Pipe_Blue, "Instruction Issued");
            ptv_out->WriteStatement(*Issued_EventDef,&ptv_id);
        } else if(detailed_output && !strcmp("DRB_2_LSB_ISSUED_INST", (EdgeName[(string)i->second]).c_str())) {
            Issued_EventDef2 = new PT_Config_Event(i->second, 'I', Pipe_Blue, "Instruction Issued");
            ptv_out->WriteStatement(*Issued_EventDef2,&ptv_id);
        } else if(detailed_output && !strcmp("EXECUTED_INST", (EdgeName[(string)i->second]).c_str())) {
            Execute_EventDef = new PT_Config_Event(i->second, 'E', Pipe_Blue, "Execute");
            ptv_out->WriteStatement(*Execute_EventDef,&ptv_id);
        } 
        
        else if(DetailedEvents.find((string)i->second) != DetailedEvents.end()) {
            //cout << "Found " << i->second << " in detailed events!!!" << endl;
            DefineDetailedEvent(i->second,DetailedEvents[(string)i->second]);
        } 

        else  {
            cout << i->second << endl;
        }
        
        /*
        
        else {
            printf("OtherEventDef:_%s_\n", i->second);
            //Other_EventDef = new PT_Config_Event(i->second, '.', Pipe_Black, (EdgeName[(string)i->second]).c_str());
            Other_EventDef[EdgeList[i->first]] = new PT_Config_Event(i->second, '.', Pipe_Black, (EdgeName[(string)i->second]).c_str());
            ptv_out->WriteStatement(*Other_EventDef[EdgeList[i->first]],&ptv_id);
        }
        */
    }

}


void DRALLISTENER_CLASS::PrintPTV() {

    ptv_id = 1;
    PT_Statement *current;
    PT_Event_Statement *current_event;
    PT_EventData_Statement *current_event_data;
    PT_Data_Statement *current_data;
    char null_str[6] = "null";
    char temp_string[256];

    printf("Printing data\n");

//    for(map<UINT32, char*>::iterator q=ItemDis.begin(); 
//            q != ItemDis.end(); 
//            q++) 
//
      for(map<UINT32,list<UINT32> >::iterator q=DID2ItemIDs.begin(); 
              q != DID2ItemIDs.end(); 
              q++) {

          if(ItemDis[q->second.front()]) {
              current = new PT_Open_Statement(instruction_record);
              ptv_out->WriteStatement(*current,&ptv_id);
              delete(current);

              current_event = new PT_Event_Statement( ptv_id, Birth_EventDef, ItemBirth[q->second.front()], 1, 1);
              ptv_out->WriteStatement(*current_event,&ptv_id);
              delete(current_event);
              current_event_data = new PT_EventData_Statement( ptv_id, null_DataDef, null_str);
              ptv_out->WriteStatement(*current_event_data,&ptv_id);
              delete(current_event_data);

              current_data = new PT_Data_Statement( ptv_id, Dis_DataDef, ItemDis[q->second.front()]);
              ptv_out->WriteStatement(*current_data,&ptv_id);
              delete(current_data);

              sprintf(temp_string, "0x%llx", ItemIP[q->second.front()]);
              temp_string[0] = InstType[q->second.front()];
              temp_string[1] = '\0';
              current_data = new PT_Data_Statement( ptv_id, Type_DataDef, temp_string);
              ptv_out->WriteStatement(*current_data,&ptv_id);
              delete(current_data);

              if(ItemIP[q->second.front()]) {
                  sprintf(temp_string, "0x%llx", ItemIP[q->second.front()]);
                  current_data = new PT_Data_Statement( ptv_id, IP_DataDef, temp_string);
                  ptv_out->WriteStatement(*current_data,&ptv_id);
                  delete(current_data);
              }

          } else {
              continue;
          }

          //cout << endl << "DID " << q->first << ":";

          for(list<UINT32>::iterator i=q->second.begin(); 
                  i != q->second.end(); 
                  i++) { 
              //cout << " " << *i;

              if(ItemUID[*i]) {
                  sprintf(temp_string, "%llu", ItemUID[*i]);
                  current_data = new PT_Data_Statement( ptv_id, UID_DataDef, temp_string);
                  ptv_out->WriteStatement(*current_data,&ptv_id);
                  delete(current_data);
              }

              // PTV recognizes the "Thread" field for coloring.
              //current_data = new PT_Data_Statement( ptv_id, Thread_DataDef, &CPU[*i]);
              //ptv_out->WriteStatement(*current_data,&ptv_id);

              list<UINT16>::iterator j=MoveListEdge[*i].begin(); 
              list<UINT64>::iterator k=MoveListCycle[*i].begin(); 
              for(; j != MoveListEdge[*i].end(); j++,k++) { 
                  if(Detailed_EventDef.find(EdgeList[*j]) != Detailed_EventDef.end()) {
                      current_event = new PT_Event_Statement( ptv_id, Detailed_EventDef[EdgeList[*j]], *k, 1, 1);
                      ptv_out->WriteStatement(*current_event,&ptv_id);
                      delete(current_event);

                      sprintf(temp_string, "%s_to_%s", strip_bad_chars(NodeList[EdgeFromList[*j]]), strip_bad_chars(NodeList[EdgeToList[*j]]));
                      current_event_data = new PT_EventData_Statement( ptv_id, Edge_DataDef, temp_string);
                      ptv_out->WriteStatement(*current_event_data,&ptv_id);
                      delete(current_event_data);

                  } else if(Other_EventDef.find(EdgeList[*j]) != Other_EventDef.end()) {
                      //current_event = new PT_Event_Statement( ptv_id, Other_EventDef[*j], *k, EdgeLatency[*j], 1);
                      current_event = new PT_Event_Statement( ptv_id, Other_EventDef[EdgeList[*j]], *k, 1, 1);
                      ptv_out->WriteStatement(*current_event,&ptv_id);
                      delete(current_event);
                      // We lose the EdgeList[*j] string with this implementation :(
                      sprintf(temp_string, "%s_to_%s", strip_bad_chars(NodeList[EdgeFromList[*j]]), strip_bad_chars(NodeList[EdgeToList[*j]]));
                      current_event_data = new PT_EventData_Statement( ptv_id, Edge_DataDef, temp_string);
                      ptv_out->WriteStatement(*current_event_data,&ptv_id);
                      delete(current_event_data);
                      //fprintf(ptv_out, "%u %s 1 %d %llu \n", Inst_ID, EdgeList[*j], EdgeLatency[*j] ,*k);
                      //fprintf(ptv_out, "%u Edge %s-to-%s\n", Inst_ID, NodeList[EdgeFromList[*j]], NodeList[EdgeToList[*j]]);
                  } else {
                      //cout << "Can't find " << EdgeList[*j] << endl;
                  }
              }
              for(list<UINT64>::iterator m=ReplayList[*i].begin(); m != ReplayList[*i].end(); m++) { 
                  current_event = new PT_Event_Statement( ptv_id, Replay_EventDef, *m, 1, 1);
                  ptv_out->WriteStatement(*current_event,&ptv_id);
                  delete(current_event);
                  current_event_data = new PT_EventData_Statement( ptv_id, null_DataDef, null_str);
                  ptv_out->WriteStatement(*current_event_data,&ptv_id);
                  delete(current_event_data);
              }
              for(list<UINT64>::iterator m=BranchMPList[*i].begin(); m != BranchMPList[*i].end(); m++) { 
                  current_event = new PT_Event_Statement( ptv_id, BranchMP_EventDef, *m, 1, 1);
                  ptv_out->WriteStatement(*current_event,&ptv_id);
                  delete(current_event);
                  current_event_data = new PT_EventData_Statement( ptv_id, null_DataDef, null_str);
                  ptv_out->WriteStatement(*current_event_data,&ptv_id);
                  delete(current_event_data);
              }
              for(list<UINT64>::iterator m=DcacheMiss[*i].begin(); m != DcacheMiss[*i].end(); m++) { 
                  current_event = new PT_Event_Statement( ptv_id, DCMiss_EventDef, *m, 1, 1);
                  ptv_out->WriteStatement(*current_event,&ptv_id);
                  delete(current_event);
                  current_event_data = new PT_EventData_Statement( ptv_id, null_DataDef, null_str);
                  ptv_out->WriteStatement(*current_event_data,&ptv_id);
                  delete(current_event_data);
              }
              for(list<UINT64>::iterator m=DcacheHit[*i].begin(); m != DcacheHit[*i].end(); m++) { 
                  current_event = new PT_Event_Statement( ptv_id, DCHit_EventDef, *m, 1, 1);
                  ptv_out->WriteStatement(*current_event,&ptv_id);
                  delete(current_event);
                  current_event_data = new PT_EventData_Statement( ptv_id, null_DataDef, null_str);
                  ptv_out->WriteStatement(*current_event_data,&ptv_id);
                  delete(current_event_data);
              }
              for(list<UINT64>::iterator m=RcacheMiss[*i].begin(); m != RcacheMiss[*i].end(); m++) { 
                  current_event = new PT_Event_Statement( ptv_id, RCMiss_EventDef, *m, 1, 1);
                  ptv_out->WriteStatement(*current_event,&ptv_id);
                  delete(current_event);
                  current_event_data = new PT_EventData_Statement( ptv_id, null_DataDef, null_str);
                  ptv_out->WriteStatement(*current_event_data,&ptv_id);
                  delete(current_event_data);
              }
              for(list<UINT64>::iterator m=RcacheHit[*i].begin(); m != RcacheHit[*i].end(); m++) { 
                  current_event = new PT_Event_Statement( ptv_id, RCHit_EventDef, *m, 1, 1);
                  ptv_out->WriteStatement(*current_event,&ptv_id);
                  delete(current_event);
                  current_event_data = new PT_EventData_Statement( ptv_id, null_DataDef, null_str);
                  ptv_out->WriteStatement(*current_event_data,&ptv_id);
                  delete(current_event_data);
              }
//              if(ItemDeath[*i]) {
//                  current_event = new PT_Event_Statement( ptv_id, Retire_EventDef, ItemDeath[*i], 1, 1);
//                  ptv_out->WriteStatement(*current_event,&ptv_id);
//                  current_event_data = new PT_EventData_Statement( ptv_id, null_DataDef, null_str);
//                  ptv_out->WriteStatement(*current_event_data,&ptv_id);
//              }
          }

          if(ItemDeath[(q->second.back())]) {
              current_event = new PT_Event_Statement( ptv_id, Retire_EventDef, ItemDeath[(q->second.back())], 1, 1);
              ptv_out->WriteStatement(*current_event,&ptv_id);
              delete(current_event);
              current_event_data = new PT_EventData_Statement( ptv_id, null_DataDef, null_str);
              ptv_out->WriteStatement(*current_event_data,&ptv_id);
              delete(current_event_data);
          }

          current = new PT_Close_Statement(ptv_id);
          ptv_out->WriteStatement(*current,&ptv_id);
          delete(current);
      }


}


int main(int argn, char * argv []) {
    DRALLISTENER_CLASS listener;
    bool count_events = false;
    char* out_filename = NULL;
    
    for (INT32 i=1; i<argn; i++)
    {
        if (!strcmp(argv[i],"-d"))
        {
            detailed_output=true;
        }
        else if (!strcmp(argv[i],"-o") && (argn > i))
        {
            out_filename = strdup(argv[++i]);
        }
        else
        {
            cerr << "Usage: dral2PTV "
            << "[-o ptvFileOut] < dralFileIn " << endl
            << "\t-d:  extra detail added to ptv output " << endl
            << "\t-o <filename>: Set the pipetrace output filename to <filename>."
            << "\tThe default filename is Events.pt" << endl;
            exit(-1);
        }
    }

    const char *home = getenv("HOME");
    if (!home) {
        cerr <<  "Couldn't getenv HOME" << endl; 
        exit(-1);
    }
    char rc_filename[200];
    sprintf(rc_filename, "%s/.asim/dral2PTVrc", home);
    FILE* rc_fp;
    if((rc_fp=fopen(rc_filename, "r")) )
    {
        cout << "Opened dral2PTVrc" << endl;
        char str[50];
        char letter;
        while(fscanf(rc_fp, "%s %c", str, &letter) != EOF) 
        {
            //cout << str << " : " << letter << endl;
            listener.DetailedEvents[(string)str] = letter;

        }
        fclose(rc_fp);
    }

    DRAL_LISTENER_CONVERTER_CLASS converter(&listener);
    DRAL_CLIENT_CLASS client(0,&converter,8*1024*1024);

    PT_Error *error=NULL;
    if(out_filename == NULL) {
        listener.ptv_out = new PT_ASCIIWriteStream("Events.pt", error);
    } else {
        listener.ptv_out = new PT_ASCIIWriteStream(out_filename, error);
    }

    while (!finish)
    {
        UINT32 n = client.ProcessNextEvent(true,(count_events?1:10000));
        if (!n || current_cycle > last_cycle)
        {
            finish=true;
        }
    }

    //listener.NewEdge( 1, 1, 999999, 1, 1, "_"); 

    listener.PrintHeader();
    listener.PrintPTV();

    delete(listener.ptv_out);
}



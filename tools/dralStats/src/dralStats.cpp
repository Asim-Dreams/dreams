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

//#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include "asim/dralClient.h"
#include "asim/dralServer.h"
#include "asim/dralListenerOld.h"
#include "asim/dralListenerConverter.h"

using namespace std;

#define MAX_HIST_SIZE 200

static unsigned int HIST_BIN_SIZE = 10;

static bool ring_output = false;
static bool histogram_output = false;
static bool stats_output = false;

bool finish=false;
UINT32 num_events=0;
UINT64 current_cycle=0;


class DRALLISTENER_CLASS : public DRAL_LISTENER_OLD_CLASS
{
  public:
    void Cycle (UINT64);
    void NewItem (UINT32);
    void SetTagSingleValue ( UINT32, char *, UINT64, unsigned char);
    void SetTagString ( UINT32, char *, char *, unsigned char);
    void SetTagSet ( UINT32, char *, UINT32, UINT64 *, unsigned char);
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

    map<UINT16,UINT32>        EdgeTraversalCount;
    map<UINT16,UINT64>        EdgeLastTraversal;
    map<UINT16,UINT64>        EdgeInterArrivalTotal;

    map<string,unsigned int *> EdgeHist;

    map<UINT16,char*>         NodeList;
    map<UINT16,string>        EdgeList;
    map<string,UINT16>        ListEdge;
    map<string,string>        EdgeName;
    map<UINT16,UINT16>        EdgeFromList;
    map<UINT16,UINT16>        EdgeToList;
    map<UINT16,UINT16>        EdgeLatency;
    map<UINT32,UINT64>        ItemBirth;
    map<UINT32,UINT64>        ItemDeath;
    map<UINT32,char*>         ItemDis;
    map<UINT32,UINT64>        ItemIP;
    map<UINT32,UINT64>        ItemUID;
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

    void dump_stats();
    void dump_hist();
    void dump_ring();
    void populate_array_cw(unsigned int *x, string ring);
    void populate_array_ccw(unsigned int *x, string ring);
    void dump_array(unsigned int *x);

  private:

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

void DRALLISTENER_CLASS::dump_stats() {
    map<UINT16, string>::iterator i=EdgeList.begin(); 
    printf("%80s%10s%s\n", " ", "Bandwidth", " InterArrival Rate");
    for(; i != EdgeList.end(); i++) { 
        if(EdgeTraversalCount[i->first] > 0) {
            printf("%80s", i->second.c_str() );
            printf("%10d", EdgeTraversalCount[i->first]);
            if(EdgeInterArrivalTotal[i->first] > 0) {
                double r = (double)EdgeInterArrivalTotal[i->first] / ((double)EdgeTraversalCount[i->first] - 1.0);
                printf("\t%6.1f",r) ;
            }
            cout << endl;
        }
    }

}

void DRALLISTENER_CLASS::dump_hist() {
    // Printing the header to stderr so you still see the header if you grep for something
    fprintf(stderr, "%70s ", "  " );
    for(int j=0; j < MAX_HIST_SIZE/HIST_BIN_SIZE; j++) { fprintf(stderr, "%6d", (j+1)*HIST_BIN_SIZE); }
    fprintf(stderr, "\n");

    fprintf(stdout, "%70s ", "  " );
    for(int j=0; j < MAX_HIST_SIZE/HIST_BIN_SIZE; j++) { fprintf(stdout, "%6d", (j+1)*HIST_BIN_SIZE); }
    fprintf(stdout, "\n");


    map<string, UINT16>::iterator i=ListEdge.begin(); 
    for(; i != ListEdge.end(); i++) { 
        if(EdgeTraversalCount[i->second] > 0) {
            printf("%70s ", i->first.c_str() );

            for(int j=0; j < MAX_HIST_SIZE/HIST_BIN_SIZE; j++) { 
                printf("%6d", (EdgeHist[i->first])[j]);
            }
            cout << endl;
        }
    }

}


void DRALLISTENER_CLASS::populate_array_cw(unsigned int *x, string ring) {
    x[0]  = EdgeTraversalCount[ListEdge["CBOX_to_CBOX::RING_STOP_0_RING_STOP_1" + ring + "_CW"]];
    x[1]  = EdgeTraversalCount[ListEdge["CBOX_to_SBOX_0::RING_STOP_1_RING_STOP_2" + ring + "_CW"]];
    x[2]  = EdgeTraversalCount[ListEdge["SBOX_0_to_CBOX::RING_STOP_2_RING_STOP_3" + ring + "_CW"]];
    x[3]  = EdgeTraversalCount[ListEdge["CBOX_to_CBOX::RING_STOP_3_RING_STOP_4" + ring + "_CW"]];
    x[4]  = EdgeTraversalCount[ListEdge["CBOX_to_EMPTY_STOP_0::RING_STOP_4_RING_STOP_5" + ring + "_CW"]];
    x[5]  = EdgeTraversalCount[ListEdge["EMPTY_STOP_0_to_CBOX::RING_STOP_5_RING_STOP_6" + ring + "_CW"]];
    x[6]  = EdgeTraversalCount[ListEdge["CBOX_to_CBOX::RING_STOP_6_RING_STOP_7" + ring + "_CW"]];
    x[7]  = EdgeTraversalCount[ListEdge["CBOX_to_SBOX_1::RING_STOP_7_RING_STOP_8" + ring + "_CW"]];
    x[8]  = EdgeTraversalCount[ListEdge["SBOX_1_to_CBOX::RING_STOP_8_RING_STOP_9" + ring + "_CW"]];
    x[9]  = EdgeTraversalCount[ListEdge["CBOX_to_CBOX::RING_STOP_9_RING_STOP_10" + ring + "_CW"]];
    x[10] = EdgeTraversalCount[ListEdge["CBOX_to_EMPTY_STOP_1::RING_STOP_10_RING_STOP_11" + ring + "_CW"]];
    x[11] = EdgeTraversalCount[ListEdge["EMPTY_STOP_1_to_CBOX::RING_STOP_11_RING_STOP_0" + ring + "_CW"]];
}

void DRALLISTENER_CLASS::populate_array_ccw(unsigned int *x, string ring) {
    x[0]  = EdgeTraversalCount[ListEdge["CBOX_to_CBOX::RING_STOP_1_RING_STOP_0" + ring + "_CCW"]];
    x[1]  = EdgeTraversalCount[ListEdge["SBOX_0_to_CBOX::RING_STOP_2_RING_STOP_1" + ring + "_CCW"]];
    x[2]  = EdgeTraversalCount[ListEdge["CBOX_to_SBOX_0::RING_STOP_3_RING_STOP_2" + ring + "_CCW"]];
    x[3]  = EdgeTraversalCount[ListEdge["CBOX_to_CBOX::RING_STOP_4_RING_STOP_3" + ring + "_CCW"]];
    x[4]  = EdgeTraversalCount[ListEdge["EMPTY_STOP_0_to_CBOX::RING_STOP_5_RING_STOP_4" + ring + "_CCW"]];
    x[5]  = EdgeTraversalCount[ListEdge["CBOX_to_EMPTY_STOP_0::RING_STOP_6_RING_STOP_5" + ring + "_CCW"]];
    x[6]  = EdgeTraversalCount[ListEdge["CBOX_to_CBOX::RING_STOP_7_RING_STOP_6" + ring + "_CCW"]];
    x[7]  = EdgeTraversalCount[ListEdge["SBOX_1_to_CBOX::RING_STOP_8_RING_STOP_7" + ring + "_CCW"]];
    x[8]  = EdgeTraversalCount[ListEdge["CBOX_to_SBOX_1::RING_STOP_9_RING_STOP_8" + ring + "_CCW"]];
    x[9]  = EdgeTraversalCount[ListEdge["CBOX_to_CBOX::RING_STOP_10_RING_STOP_9" + ring + "_CCW"]];
    x[10] = EdgeTraversalCount[ListEdge["EMPTY_STOP_1_to_CBOX::RING_STOP_11_RING_STOP_10" + ring + "_CCW"]];
    x[11] = EdgeTraversalCount[ListEdge["CBOX_to_EMPTY_STOP_1::RING_STOP_0_RING_STOP_11" + ring + "_CCW"]];
}

void DRALLISTENER_CLASS::dump_array(unsigned int *x) {
    printf("C0--%2d%%--C1--%2d%%--S0--%2d%%--C2--%2d%%--C3\n",x[0],x[1],x[2],x[3] );  // RING_STOP_0_RING_STOP_1_Addr_CW
    printf("|                                    |\n" );
    printf("%2d%%                                 %2d%%\n",x[11],x[4]);
    printf("|                                    |\n" );
    printf("E1                                  E0\n" );
    printf("|                                    |\n" );
    printf("%2d%%                                 %2d%%\n",x[10],x[5]);
    printf("|                                    |\n" );
    printf("C7--%2d%%--C6--%2d%%--S1--%2d%%--C5--%2d%%--C4\n",x[9],x[8],x[7],x[6] );
}

void DRALLISTENER_CLASS::dump_ring() {
    cout << endl << "Address Ring CW" << endl;
    unsigned int x[12];
    unsigned int x_total = 0;
    string ring = "_Addr";
    populate_array_cw(x, ring);
    for(unsigned int i=0; i<12; i++) { x_total += x[i]; }
    for(unsigned int i=0; i<12; i++) { x[i] =  (x[i]*100 / x_total); }
    dump_array(x);

    cout << endl << "Address Ring CCW" << endl;
    for(unsigned int i=0; i<12; i++) { x[i] =  0; }
    x_total = 0;
    ring = "_Addr";
    populate_array_ccw(x, ring);
    for(unsigned int i=0; i<12; i++) { x_total += x[i]; }
    for(unsigned int i=0; i<12; i++) { x[i] =  (x[i]*100 / x_total); }
    dump_array(x);

    cout << endl << "Block Ring CW" << endl;
    for(unsigned int i=0; i<12; i++) { x[i] =  0; }
    x_total = 0;
    ring = "_Block";
    populate_array_cw(x, ring);
    for(unsigned int i=0; i<12; i++) { x_total += x[i]; }
    for(unsigned int i=0; i<12; i++) { x[i] =  (x[i]*100 / x_total); }
    dump_array(x);

    cout << endl << "Block Ring CCW" << endl;
    for(unsigned int i=0; i<12; i++) { x[i] =  0; }
    x_total = 0;
    ring = "_Block";
    populate_array_ccw(x, ring);
    for(unsigned int i=0; i<12; i++) { x_total += x[i]; }
    for(unsigned int i=0; i<12; i++) { x[i] =  (x[i]*100 / x_total); }
    dump_array(x);

    cout << endl << "Ack Ring CW" << endl;
    for(unsigned int i=0; i<12; i++) { x[i] =  0; }
    x_total = 0;
    ring = "_Ack";
    populate_array_cw(x, ring);
    for(unsigned int i=0; i<12; i++) { x_total += x[i]; }
    for(unsigned int i=0; i<12; i++) { x[i] =  (x[i]*100 / x_total); }
    dump_array(x);

    cout << endl << "Ack Ring CCW" << endl;
    for(unsigned int i=0; i<12; i++) { x[i] =  0; }
    x_total = 0;
    ring = "_Ack";
    populate_array_ccw(x, ring);
    for(unsigned int i=0; i<12; i++) { x_total += x[i]; }
    for(unsigned int i=0; i<12; i++) { x[i] =  (x[i]*100 / x_total); }
    dump_array(x);

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

    EdgeTraversalCount[edge_id] += n;

    //cout << "Last: " << EdgeLastTraversal[edge_id] << " current " << current_cycle << " for " << edge_id << endl;
    if(EdgeLastTraversal[edge_id] != 0) { 
        EdgeInterArrivalTotal[edge_id] += (current_cycle - EdgeLastTraversal[edge_id]);  // Only for the first of n transactions

        if((current_cycle - EdgeLastTraversal[edge_id])> MAX_HIST_SIZE) {
            (EdgeHist[EdgeList[edge_id]])[MAX_HIST_SIZE/HIST_BIN_SIZE]++;
        } else {
            (EdgeHist[EdgeList[edge_id]])[(current_cycle - EdgeLastTraversal[edge_id])/HIST_BIN_SIZE]++;
        }
        if(n>1) {
            (EdgeHist[EdgeList[edge_id]])[0] += (n-1);
        }

    }

    EdgeLastTraversal[edge_id] = current_cycle;

    num_events++;
}

void DRALLISTENER_CLASS::MoveItemsWithPositions (UINT16 edge_id, UINT32 n, UINT32 * items, UINT32 * positions) {
    cout << "MoveItemsWithPositions: " << edge_id << " " <<  n << endl;
    exit(-1);
}

void DRALLISTENER_CLASS::DeleteItem(UINT32 item_id) {
    ItemDeath[item_id] = current_cycle;
}

void DRALLISTENER_CLASS::EndSimulation(void) {
    finish=true;
    //cout << "Simulation end." << endl;
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
    char uname[100];
    sprintf(uname, "%s_to_%s::%s\0", strip_bad_chars(NodeList[source]), strip_bad_chars(NodeList[destination]),strip_bad_chars(name));

//  if(EdgeName.find((string)uname)==EdgeName.end()) {
        EdgeList[edge_id]     = strdup(uname);
        EdgeHist[EdgeList[edge_id]]     = new unsigned int [MAX_HIST_SIZE+1];
        for(int j=0; j < MAX_HIST_SIZE/HIST_BIN_SIZE; j++) { (EdgeHist[EdgeList[edge_id]])[j] = 0; }
        //cout << "Allocated array for " << edge_id << endl;
        if(ListEdge.find((string)uname)==ListEdge.end()) {
            ListEdge[(string)uname]       = edge_id;
        }
        EdgeName[EdgeList[edge_id]]  = (string)strdup(name);
        //cout << "Adding " << edge_id << " " << uname << " to name list" << endl;
        EdgeFromList[edge_id] = source;
        EdgeToList[edge_id]   = destination;
        EdgeLatency[edge_id]  = latency;

        EdgeTraversalCount[edge_id] = 0;
        EdgeLastTraversal[edge_id] = 0;
        EdgeInterArrivalTotal[edge_id] = 0;  
//  } else {
//      cout << "Already exists:  " << edge_id << " " << uname << " in name list" << endl;
//  }

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
    }    
}

void DRALLISTENER_CLASS::SetItemTagString (UINT32 item_id, char * tag_name, char * str) {
    /*
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
    */
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
    // Not called?
    //cout << "New edge " << name << " id " << edge_id << " from " << source << " to " << destination << " latency " << latency << endl;
}


void DRALLISTENER_CLASS::SetCapacity ( UINT16 node_id, UINT32, UINT32 * capacities, UINT16 n) {
}


void DRALLISTENER_CLASS::Comment (char * comment) {
    //comment;
}

void print_help() {
    cerr << "Usage: dralStats "
        << "[-r -h <bin_size> -s] < dralFileIn " << endl
        << "\t-r:  extra detail for BMP ring model " << endl
        << "\t-h <bin_size>: print a histogram, using bin size <bin_size> " << endl
        << "\t-r:  print port BW and avg interarrival rates " << endl;
    exit(-1);
}


int main(int argn, char * argv []) {
    DRALLISTENER_CLASS listener;
    bool count_events = false;
    
    for (INT32 i=1; i<argn; i++)
    {
        if (!strcmp(argv[i],"-r"))
        {
            ring_output=true;
        }
        else if (!strcmp(argv[i],"-s"))
        {
            stats_output=true;
        }
        else if (!strcmp(argv[i],"-h"))
        {
            if((argn>i+1) && (argv[i+1][0]>='0') && (argv[i+1][0]<='9')) {
                histogram_output=true;
                sscanf(argv[++i], "%d", &HIST_BIN_SIZE);
            } else {
                cout << "You didn't specify a histogram bin size, so I'm assuming you want help." << endl;
                print_help();
            }
        }
        else
        {
            print_help();
        }
    }

    DRAL_LISTENER_CONVERTER_CLASS converter(&listener);
    DRAL_CLIENT_CLASS client(0,&converter,8*1024*1024);

    while (!finish)
    {
        UINT32 n = client.ProcessNextEvent(true,(count_events?1:10000));
        if (!n || current_cycle == UINT64_MAX)
        {
            finish=true;
        }
    }

    if(stats_output || (!histogram_output && !ring_output)) {
        listener.dump_stats();
    }

    if(histogram_output) {
        listener.dump_hist();
    }

    if(ring_output) {
        listener.dump_ring();
    }

}



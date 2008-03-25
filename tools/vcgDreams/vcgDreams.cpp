// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

/**
    @file   vcgDreams.cpp
    @author Federico Ardanaz
 */

#include "vcgDreams.h" 

int main(const int argc, char *argv[], const char *envp[])
{
    // in/out streams
    string istream;       // input stream 
    string cstream;       // ouput stream for connection graph
    string tstream;       // ouput stream for hierarchy tree  
    int  nedge=3;          // default nearedge threshold  
    int  minLat=0;         // default min latency  
    bool selfEdge=true;   // default self edges  

    // process options
    for( int i = 1; i < argc; i++ )
    {
        string argument(argv[i]);

        // info options
        if( !argument.compare("-h") )               { usage(argv[0]); }
        // I/O options (text)
        else if( !argument.compare("-i") )          { i++; istream = string(argv[i]); }
        else if( !argument.compare("-t") )          { i++; tstream = string(argv[i]); }
        else if( !argument.compare("-c") )          { i++; cstream = string(argv[i]); }
        else if( !argument.compare("-near") )       { i++; nedge   = atoi(argv[i]);   }
        else if( !argument.compare("-self") )       { i++; selfEdge= atoi(argv[i]);   }
        else if( !argument.compare("-mlat") )       { i++; minLat  = atoi(argv[i]);   }
        // unknown option
        else usage(argv[0]);
    }

    // read the begining of the drl file...
    bool dralOk     = true;
    bool readyGraph = false;
    bool eof        = false;
    LISTENER_CLASS* listener = new LISTENER_CLASS(istream,nedge,selfEdge,minLat);
    int fd_trace = open(istream.c_str(), O_RDONLY);
    if (fd_trace == -1)
    {
        cerr << "Error opening events file " << istream << endl;
        usage(argv[0]);
    }
    DRAL_CLIENT_CLASS client(fd_trace, listener, 1024*64);
  
    while (dralOk && !readyGraph && !eof)
    {
        // dral library call
        eof = (client.ProcessNextEvent(true,1)<=0);

        // update flags
        dralOk     = listener->DralOk();
        readyGraph = listener->GraphReady();
    }
    
    if (dralOk)
    {
        vcgGraph* connect = listener->GetConnectionGraph();
        vcgGraph* tree    = listener->GetHierarchyGraph();

        FILE *fc = fopen(cstream.c_str(),"w");
        FILE *ft = fopen(tstream.c_str(),"w");

        if (!fc || !ft)
        {
            cerr << "IO error while creating output files " << cstream << " and " << tstream << endl;
            exit(-1);
        }
        connect->toVCG(fc);
        tree->toVCG(ft);
    }
    close(fd_trace);
}

void usage(string argv0)
{
    cerr << "Usage: " << argv0 << " -i <file> -t <file> -c <file> [-near value] [-self value] [-mlat value] [-h]" << endl;
    cerr << "\t -h:\t This help info" << endl;
    cerr << "\t -i:\t Input drl file" << endl;
    cerr << "\t -t:\t Hierarchy Tree file (output)" << endl;
    cerr << "\t -c:\t Connection Graph file (output)" << endl;
    cerr << "\t -near:\t Specify the latency threshold to declare 'near' edges" << endl;
    cerr << "\t -self:\t 0/1 show self-edges (default 1)" << endl;
    cerr << "\t -mlat:\t minimum edge latency, edges below that are not shown (default 0)" << endl;
    exit(-1);
}



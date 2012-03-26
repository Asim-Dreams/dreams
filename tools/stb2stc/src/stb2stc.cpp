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

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "asim/agt_syntax.h"
#include "asim/AGTTypes.h"
#include <string.h>

using namespace std;

#define MAX_LINE_SIZE 128
#define THREADS 5
#define MAX_OPENED_FILES 1

INT32 persample;

typedef struct
{
    // Stripchart data array for every thread.
    char description[100];  // Holds description of full stripchart
    INT32 elems;            // Elements available in values array.
    INT32 threads;            // Active Threads
    INT32 visible[THREADS];   // For every Threads the visibility status
    INT32 activated[THREADS];   // For every Threads the visibility status
    INT32 smooth[THREADS];    // For every Threads the smooth status
    INT32 smoothView[THREADS];    // Activates the smooth view. 
    INT32 breakSmooth[THREADS];    // For every Threads the smooth value when break view is used
    INT32 breakView[THREADS];    // For every Threads these variables activates the break view 
    INT32 relativeView[THREADS];// For every Threads these variables activates the relative view 
    INT32 maxView[THREADS];    // For every Threads these variables activates the relative to max view 
    INT32 hSkew[THREADS];    // Horizontal skew
    INT32 vSkew[THREADS];    // Vertical skew
    INT32 lineWidth[THREADS];    // Line Width to view the strip 

    float *values[THREADS];  // These THREADS array holds the real data
                // obtained from performance model.
    float maxValue[THREADS]; // Max values to maxView;
    float maxInStripValue[THREADS]; // Max values to relativeView;
} 
StripData;

typedef struct
{
    INT32 cycle;
    char description[128];
    INT32 position;
} 
MarkerData;

typedef struct 
{
    INT32 elems;             // Contains the number of Strips available
    StripData *fullData;        // All the Strips on this file
    INT32 marker_elems;        // Number of markers in this file 
    MarkerData *markers;        // Marker data    
    INT32 lines;            // Lines in file
    INT32 frequency;            // Frequency of data in cycles
} 
StripEntry;

typedef struct 
{    
    INT32 active[MAX_OPENED_FILES];           // Mask with opened files needed
                                            // to allow to close the files when
                                            // they are opened.
    StripEntry files[MAX_OPENED_FILES];     // All the files opened
}
allFilesData;

struct LineData 
{
    INT32 position;
    char description[100];
    INT32 threads;
    INT32 frequency;
    INT32 base_frequency;
    INT32 max_elems;
};


allFilesData *theVectorDB;


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This method opens a binary file that contains stripchart data previous     //
// translated using stc2stb program provided with StripChartViewer            //
// distribution.                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
void do_openStripFileVersion1_0(INT32 input, INT32 lines, INT32 strips)
{ 
    float value;
    INT32 Counter = 0;
    INT32 blocks,markers;
    INT32 i;
    INT32 *stripdata;
    INT32 *actual;
    INT32 file;
    float max;
    LineData *readed;
    StripData *tmp;
    INT32 frequency;
    INT32 r;

    printf("Converting 1.0\n");

    // Reading header. It contains lines,strips,blocks and markers integer. Remember
    // the size of integer element in C++ using cxx in TRU64 and Alpha platform
    read(input,(char *)&blocks,sizeof(int));
    read(input,(char *)&markers,sizeof(int));

    // We have an static structure to maintain opened all the files the 
    // user has requested before during this execution. At this point we 
    // must search the first unused file structure and alloc memory to
    // store it and its data.
    file = 0;

    theVectorDB->files[file].fullData = (StripData *)malloc(sizeof(StripData)*blocks);
    theVectorDB->files[file].elems = blocks; 
    theVectorDB->files[file].lines = lines; 
    theVectorDB->files[file].marker_elems = markers; 

    // Now we have all the blocks allocated.
    // We use the LineData structure to read the data from disk.
    readed = new struct LineData[blocks];

    r=read(input,(char *)readed,blocks*sizeof(struct LineData));

    // We have the data and we must create all the required stripcharts headers
    for(i = 0;i<blocks;i++)
    {
        // Number of threads active at this block
        theVectorDB->files[file].fullData[i].threads = readed[i].threads;
        // Number of threads active at this block
        theVectorDB->files[file].fullData[i].elems = lines;

        // Copying description 
        for(INT32 j = 0;j<100;j++) 
        {
            theVectorDB->files[file].fullData[i].description[j] = readed[i].description[j];
        }
    
        // Alloc memory for data from stripcharts
        for(INT32 j = 0;j<readed[i].threads;j++) 
        {
            theVectorDB->files[file].fullData[i].values[j] = (float *)malloc(sizeof(float)*lines);
            theVectorDB->files[file].fullData[i].visible[j] = 0;
            theVectorDB->files[file].fullData[i].activated[j] = 0;
            theVectorDB->files[file].fullData[i].smooth[j] = 0;
            theVectorDB->files[file].fullData[i].smoothView[j] = 0;
            theVectorDB->files[file].fullData[i].breakSmooth[j] = 0;
            theVectorDB->files[file].fullData[i].maxValue[j] = readed[i].max_elems;
        }
        // Calculate Accumulated strip
        if(readed[i].threads>1) 
        {
            INT32 j = readed[i].threads;    
            theVectorDB->files[file].fullData[i].values[j] = (float *)malloc(sizeof(float)*lines);
            theVectorDB->files[file].fullData[i].visible[j] = 0;
            theVectorDB->files[file].fullData[i].activated[j] = 0;
            theVectorDB->files[file].fullData[i].smooth[j] = 0;
            theVectorDB->files[file].fullData[i].smoothView[j] = 0;
            theVectorDB->files[file].fullData[i].breakSmooth[j] = 0;
        }

        // Updating file frequency
	if(persample==1) {
            theVectorDB->files[file].frequency = 1;
	}
	else {
            theVectorDB->files[file].frequency = readed[i].base_frequency;
	}

    }

    delete readed;

    theVectorDB->files[file].markers = (MarkerData *)malloc(sizeof(MarkerData)*markers);

    for(i = 0;i<markers;i++) 
    {
        read(input,(char *)(&(theVectorDB->files[file].markers[i].position)),sizeof(int));
        read(input,(char *)(&(theVectorDB->files[file].markers[i].cycle)),sizeof(int));
        theVectorDB->files[file].markers[i].cycle = theVectorDB->files[file].markers[i].cycle/theVectorDB->files[file].frequency;
        read(input,(char *)(&(theVectorDB->files[file].markers[i].description[0])),sizeof(char)*128);
    }

    // Allocating space to read all the stripchart data in only one call system

    stripdata = (INT32 *)malloc(sizeof(int)*strips*lines);

    if(stripdata==NULL) 
    {
        printf("Not enougth memory\n");
        exit(-1);
    }

//
// I read 100 blocks and a remainder if we need it.
//

    int lines_iters, lines_rem;

    lines_iters=lines/100;
    lines_rem=lines%100;

    for(i = 0;i<lines;i+=lines_iters) 
    {
        //progress->setProgress(i);
        read(input,&(((char *)stripdata)[strips*sizeof(int)*i]),strips*sizeof(int)*lines_iters);
    }

    i-=lines_iters;

    if(lines_rem!=0){
        read(input,&(((char *)stripdata)[strips*sizeof(int)*i]),strips*sizeof(int)*lines_rem);
    }
    
     
    // Skipping the first number because it is the line number
    actual = &(stripdata[0]);

    max = 0.0;

    for(i = 0;i<lines;i++) 
    {
        for(INT32 j = 0;j<blocks;j++) 
        {
            for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
            {
                if(actual[0]<0) 
                { 
                    actual[0] = 0; 
                    theVectorDB->files[file].fullData[j].values[k][i] = 0.0;
                } 
                else 
                { 
                    theVectorDB->files[file].fullData[j].values[k][i] = ((float)actual[0])/(float)theVectorDB->files[file].frequency;
                    max = (((float)actual[0])/(float)theVectorDB->files[file].frequency)>max?(((float)actual[0])/(float)theVectorDB->files[file].frequency):max;
                }
                actual++;
            }    
        }
        actual++;
    }

    delete stripdata;

    float Accumulated;

    // We remove negative values on every strip.
    for(i = 0;i<lines;i++) 
    {
        for(INT32 j = 0;j<blocks;j++) 
        {
            Accumulated = 0.0;
            for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
            {
                if(theVectorDB->files[file].fullData[j].values[k][i]<0.0) 
                {
                    theVectorDB->files[file].fullData[j].values[k][i] = 0.0;
                }
                Accumulated+=theVectorDB->files[file].fullData[j].values[k][i];
            }
            if(theVectorDB->files[file].fullData[j].threads>1) 
            {
                theVectorDB->files[file].fullData[j].values[theVectorDB->files[file].fullData[j].threads][i] = Accumulated;
            }
        }
    }

    for(INT32 j = 0;j<blocks;j++) 
    {
        if(theVectorDB->files[file].fullData[j].threads>1)
        {
            theVectorDB->files[file].fullData[j].threads++;
        }    
        for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
        {
            theVectorDB->files[file].fullData[j].maxInStripValue[k] = 0.0;
            theVectorDB->files[file].fullData[j].lineWidth[k] = 1;
            theVectorDB->files[file].fullData[j].hSkew[k] = 0;
            theVectorDB->files[file].fullData[j].vSkew[k] = 0;
            theVectorDB->files[file].fullData[j].maxView[k] = 0;
            theVectorDB->files[file].fullData[j].relativeView[k] = 0;
            theVectorDB->files[file].fullData[j].breakView[k] = 0;
            for(i = 0;i<lines;i++) 
            {
                if(theVectorDB->files[file].fullData[j].maxInStripValue[k] < theVectorDB->files[file].fullData[j].values[k][i]) 
                {
                    theVectorDB->files[file].fullData[j].maxInStripValue[k] = theVectorDB->files[file].fullData[j].values[k][i];
                }
            }
        }
    }

    for(i = 0;i<lines;i++) 
    {
        for(INT32 j = 0;j<blocks;j++) 
        {
            for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
            {
                theVectorDB->files[file].fullData[j].maxInStripValue[k] = (theVectorDB->files[file].fullData[j].maxInStripValue[k]>theVectorDB->files[file].fullData[j].values[k][i])?theVectorDB->files[file].fullData[j].maxInStripValue[k]:theVectorDB->files[file].fullData[j].values[k][i];
            }
        }
    }
}

void do_openStripFileVersion1_1(INT32 input)
{
    UINT64 lines,strips,blocks,markers,bytes_per_line;
    LineData *readed;
    INT32 file;
    INT32 marker_pointer;
    UINT64 value_readed;
    char *data;
    UINT64 *data_int;
    UINT32 total_bytes;
    UINT32 block_size;
    UINT32 block_remaining;
    UINT64 *actualLine;
    UINT64 tag;
    UINT64 cycle;
    float max;
    INT32 data_line;

    printf("Converting 1.1\n");

    //
    // Read the header values... with these values we know the size of the file.
    //
    read(input,(char *)&lines,sizeof(UINT64));
    read(input,(char *)&strips,sizeof(UINT64));
    read(input,(char *)&blocks,sizeof(UINT64));
    read(input,(char *)&markers,sizeof(UINT64));
    read(input,(char *)&bytes_per_line,sizeof(UINT64));

    // We have an static structure to maintain opened all the files the 
    // user has requested before during this execution. At this point we 
    // must search the first unused file structure and alloc memory to
    // store it and its data.
    file = 0;
    theVectorDB->files[file].fullData = (StripData *)malloc(sizeof(StripData)*blocks);
    theVectorDB->files[file].elems = blocks; 
    theVectorDB->files[file].lines = lines-markers; 
    theVectorDB->files[file].marker_elems = markers; 

    // Now we have all the blocks allocated.
    // We use an UINT64 to read the data from disk because internally we use 32
    // bits values in some fields and all the data in disk is stored using 64 bits.
    readed = new struct LineData[blocks];

    for(UINT32 i=0; i<blocks;i++) {
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].position=value_readed;

        read(input,(char *)readed[i].description,sizeof(char)*100);
	// Cleaning the data after blanks.
	for(UINT32 counter=0;counter<100;counter++) if(readed[i].description[counter]==' ')readed[i].description[counter]='\0';
	
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].threads=value_readed;
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].frequency=value_readed;
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].base_frequency=value_readed;
        read(input,(char *)&value_readed,sizeof(value_readed));
	readed[i].max_elems=value_readed;
	
        // Number of threads active at this block
        theVectorDB->files[file].fullData[i].threads = readed[i].threads;
        // Number of threads active at this block
        theVectorDB->files[file].fullData[i].elems = lines-markers;

        // Copying description 
        for(INT32 j = 0;j<100;j++) 
        {
            theVectorDB->files[file].fullData[i].description[j] = readed[i].description[j];
        }
    
        // Alloc memory for data from stripcharts
        for(INT32 j = 0;j<readed[i].threads;j++) 
        {
            theVectorDB->files[file].fullData[i].values[j] = (float *)malloc(sizeof(float)*(lines-markers));
            theVectorDB->files[file].fullData[i].visible[j] = 0;
            theVectorDB->files[file].fullData[i].activated[j] = 0;
            theVectorDB->files[file].fullData[i].smooth[j] = 0;
            theVectorDB->files[file].fullData[i].smoothView[j] = 0;
            theVectorDB->files[file].fullData[i].breakSmooth[j] = 0;
            theVectorDB->files[file].fullData[i].maxValue[j] = readed[i].max_elems;
        }
        // Calculate Accumulated strip
        if(readed[i].threads>1) 
        {
            INT32 j = readed[i].threads;    
            theVectorDB->files[file].fullData[i].values[j] = (float *)malloc(sizeof(float)*(lines-markers));
            theVectorDB->files[file].fullData[i].visible[j] = 0;
            theVectorDB->files[file].fullData[i].activated[j] = 0;
            theVectorDB->files[file].fullData[i].smooth[j] = 0;
            theVectorDB->files[file].fullData[i].smoothView[j] = 0;
            theVectorDB->files[file].fullData[i].breakSmooth[j] = 0;
        }

        // Updating file frequency
	if(persample==1) {
            theVectorDB->files[file].frequency = 1;
	}
	else {
            theVectorDB->files[file].frequency = readed[i].base_frequency;
	}

    }

    delete readed;

    // At this point we create the data to store markers.
    theVectorDB->files[file].markers = (MarkerData *)malloc(sizeof(MarkerData)*markers);

    // Now the markers data is stored in the same area as the data. We need a pointer
    // know where is the next marker to be readed an filled. When we scan the data we
    // can found markers and we must save the data in the different marker structs.
    marker_pointer=0;

    //
    // We need 16 extra bytes to store the tag and the cycle number.
    //
    total_bytes=sizeof(char)*(bytes_per_line+16)*lines;
    block_size=total_bytes/100;
    block_remaining=total_bytes%100;

    data=(char *)malloc(total_bytes);
  
    INT32 dt=0;
  
    for(UINT32 i=0; i<100;i++) {
        dt+=read(input,(char *)&(data[block_size*i]),block_size);
        //progress->setProgress(i);
    }
    if(block_remaining!=0) {
        dt+=read(input,(char *)&(data[block_size*100]),block_remaining);
        //progress->setProgress(100);
    }

    data_int=(UINT64 *)data;

    data_line=0;

    for(UINT32 i=0; i<lines;i++) {
        actualLine=&(data_int[i*(strips+2)]);
	
        tag=actualLine[0];

        if(tag==0) {
            // Tag 0 is the standard data line
            cycle=actualLine[1];
	    UINT32 position_in_line=2;
             
            for(UINT32 j=0;j<blocks;j++) {
	        if(theVectorDB->files[file].fullData[j].threads==1) {
		    //theVectorDB->files[file].fullData[j].values[0][i]=(float)actualLine[position_in_line++]/(float)theVectorDB->files[file].frequency;
		    theVectorDB->files[file].fullData[j].values[0][data_line]=(float)actualLine[position_in_line++]/(float)theVectorDB->files[file].frequency;
		}
		else {
		    UINT64 Accum=0;
                    for(UINT32 thr=0; thr<(UINT32)(theVectorDB->files[file].fullData[j].threads);thr++) {
                        Accum+=actualLine[position_in_line];
			theVectorDB->files[file].fullData[j].values[thr][data_line]=(float)actualLine[position_in_line++]/(float)theVectorDB->files[file].frequency;
		    }
		    theVectorDB->files[file].fullData[j].values[theVectorDB->files[file].fullData[j].threads][data_line]=(float)Accum/(float)theVectorDB->files[file].frequency;
		}
	    }
	    data_line++;
	    
        }
	else if(tag==1) {
            // Tag 1 is a marker line

	}
	else {
            // With this file version a different Tag is not posible then we stop.
            cerr<<"TAG "<<tag<<" IS UNSUPPORTED"<<endl;
	    exit(-1);
	}
    }

    // We remove negative values on every strip.

    for(UINT32 i = 0;i<(lines-markers);i++) 
    {
        for(UINT32 j = 0;j<blocks;j++) 
        {
            float Accumulated = 0.0;
            for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
            {
                if(theVectorDB->files[file].fullData[j].values[k][i]<0.0) 
                {
                    theVectorDB->files[file].fullData[j].values[k][i] = 0.0;
                }
                Accumulated+=theVectorDB->files[file].fullData[j].values[k][i];
            }
            if(theVectorDB->files[file].fullData[j].threads>1) 
            {
                theVectorDB->files[file].fullData[j].values[theVectorDB->files[file].fullData[j].threads][i] = Accumulated;
            } 
        }
    }

    for(UINT32 j = 0;j<blocks;j++) 
    {
        if(theVectorDB->files[file].fullData[j].threads>1)
        {
            theVectorDB->files[file].fullData[j].threads++;
        }    
        for(INT32 k = 0;k<theVectorDB->files[file].fullData[j].threads;k++) 
        {
            theVectorDB->files[file].fullData[j].maxInStripValue[k] = 0.0;
            theVectorDB->files[file].fullData[j].lineWidth[k] = 1;
            theVectorDB->files[file].fullData[j].hSkew[k] = 0;
            theVectorDB->files[file].fullData[j].vSkew[k] = 0;
            theVectorDB->files[file].fullData[j].maxView[k] = 0;
            theVectorDB->files[file].fullData[j].relativeView[k] = 0;
            theVectorDB->files[file].fullData[j].breakView[k] = 0;
            for(UINT32 i = 0;i<(lines-markers);i++) 
            {
                if(theVectorDB->files[file].fullData[j].maxInStripValue[k] < theVectorDB->files[file].fullData[j].values[k][i]) 
                {
                    theVectorDB->files[file].fullData[j].maxInStripValue[k] = theVectorDB->files[file].fullData[j].values[k][i];
                }
            }
        }
    }
}

void do_openStripFile(char *str)
{
    INT32 input;

    INT32 versionH;
    INT32 versionL;
    INT64 version;
  
    theVectorDB = (allFilesData*)malloc(sizeof(allFilesData));
  
    // Open the file ( TODO: select the file )
    input = open(str,O_RDONLY);

    if(input==-1)return;

    read(input,(char *)&versionL,sizeof(int));
    read(input,(char *)&versionH,sizeof(int));
   
    version=versionH;
    version=version<<32;
    version=version+versionL;

    if(versionL==11) {
       do_openStripFileVersion1_1(input );
    }
    else {
       do_openStripFileVersion1_0(input, versionL, versionH);
    }   
}

void do_dumpStripData(char *str)
{
    UINT32 columns=1;
    INT32 i,j,k;
    INT32 output;
    char strTMP[128];
    
    output = open(str,O_CREAT | O_TRUNC | O_WRONLY, 0666);

    for(i=0;i<theVectorDB->files[0].elems;i++) {
        sprintf(strTMP,"%d %d ",columns, theVectorDB->files[0].fullData[i].threads );
        columns+=theVectorDB->files[0].fullData[i].threads;
        write(output,strTMP, strlen(strTMP));

        theVectorDB->files[0].fullData[i].description[strlen(theVectorDB->files[0].fullData[i].description)]='\n';
        write(output,theVectorDB->files[0].fullData[i].description,strlen(theVectorDB->files[0].fullData[i].description));
    }

    for(j=0;j<theVectorDB->files[0].fullData[0].elems;j++) {
        sprintf(strTMP,"%10d ",j*theVectorDB->files[0].frequency);
        write(output,strTMP, strlen(strTMP));
        for(i=0;i<theVectorDB->files[0].elems;i++) {
            for(k=0;k<theVectorDB->files[0].fullData[i].threads;k++) {
                sprintf(strTMP,"%8.1f ",theVectorDB->files[0].fullData[i].values[k][j]); 
                write(output,strTMP, strlen(strTMP));
	    }
	}
	sprintf(strTMP,"\n");
        write(output,strTMP, strlen(strTMP));
    }
}

int main(int argc,char **argv)
{
    if(argc!=4) {
        cerr<<"Incorrect parameters"<<endl<<"stb2stc input_file output_file [pc/ps]"<<endl;
        return(-1);
    }
    else {
	if(strcmp(argv[3],"pc")==0) {
	    persample=0;
	}
	else if(strcmp(argv[3],"ps")==0) {
            persample=1;
	}
	else {
            cerr<<"Incorrect parameters"<<endl<<"stb2stc input_file output_file [pc/ps]"<<endl;
            return(-1);
	}
        do_openStripFile(argv[1]);
	cout<<"File "<<argv[1]<<" readed"<<endl;
        do_dumpStripData(argv[2]);
	cout<<"File "<<argv[2]<<" writed"<<endl;
    }
}


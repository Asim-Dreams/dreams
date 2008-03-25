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
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "asim/agt_syntax.h"
#include "asim/AGTTypes.h"

using namespace std;

#define MAX_LINE_SIZE 128

int main(int argc,char **argv)
{
    INT32 output,i,j;
    INT32 threads,frequency,base_frequency,max_elems,position,line_elems,extra_data;
    char description[100];
    INT32 *data,lines=0,blocks=0,marks=0;
    INT32 data_beginning;
    char ch,string_marker[MAX_LINE_SIZE];
    INT32 pr_cycle;

    cout<<"Beginning\n";

    // Test parameters. We need an input and output file.
    // stc file is an standard stripchart ascii file.
    // stb file is an standard stripchart binary file.
    if(argc==1) 
    {
        cout<< "Incorrect parameters.\n\tstc2stb input.stc output.stb\n";
        return(-1);
    }

    // Open input and output.
    ifstream input(argv[1]);

    output=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0666);

    // Take the first value
    input>>position;

    // write header data to hold number of lines and number of stripcharts.
    write(output,(char *)&extra_data,sizeof(INT32));
    write(output,(char *)&extra_data,sizeof(INT32));
    write(output,(char *)&extra_data,sizeof(INT32));
    write(output,(char *)&extra_data,sizeof(INT32));

    cout<<"Coding headers\n";

    // While the data taked is different to zero we read and store the data in 
    // output file
    while(position!=0)
    {
        for(int i=0;i<100;i++)
        {
            description[i]=' ';
        }
        
        input>>description>>threads>>frequency>>base_frequency>>max_elems;
        write(output,(char *)&position,sizeof(INT32));
        write(output,description,100);
        write(output,(char *)&threads,sizeof(INT32));
        write(output,(char *)&frequency,sizeof(INT32));
        write(output,(char *)&base_frequency,sizeof(INT32));
        write(output,(char *)&max_elems,sizeof(INT32));
        blocks++;
        line_elems=position+threads;
        input>>position;
    }

    // Counting markers in input file. Every marker has -1 as 2 first characters.
    // First we save the actual position.

    cout<<"Coding markers\n";

    data_beginning=input.tellg();

    do
    {
        if(position==-1) 
        {
            // Read the marker number
            input>>position;
            write(output,(char *)&position,sizeof(INT32));
            // Marker line. Read the rest of the line. Count the marker as a new marker.
            i=0;
            do 
            {
                ch=input.get();
                string_marker[i++]=ch;
            } 
            while (ch!='\n');
            // Remove the last \n by an \0
            string_marker[i-1]='\0';

            for(j=i;j<MAX_LINE_SIZE;j++) 
            {
                string_marker[j]='\0';
            }

            input>>position;
            pr_cycle=position-base_frequency;
            // Write cycle and string
            write(output,(char *)&pr_cycle,sizeof(INT32));
            write(output,string_marker,sizeof(char)*MAX_LINE_SIZE);

            // In position we have the next cycle number.

            marks++;
        }
        else 
        {
            // Standard Line. Read strips elements.
            for(i=1;i<line_elems;i++) 
            { 
                input>>position;
            } 
        }   
    } 
    while(!input.eof());

    input.close();

    // We are at the initial data position.
    ifstream input2(argv[1],ios::in);
    input2.seekg(data_beginning);

    cout<<"Coding strips\n";

    lines=0;

    // Alloc line_elems integers to hold data.
    data=new int(line_elems);

    // Setting the initial value.
    data[0]=0;

    do
    {
        if(data[0]==-1) 
        {
            // Marker line. Read the rest of the line. Count the marker as a new marker.
            do 
            {
                ch=input2.get();
            }
            while (ch!='\n');

            input2>>data[0];
        }
        else 
        {
            // Standard Line. Read strips elements.
            for(i=1;i<line_elems;i++) 
            { 
                input2>>data[i]; 
            }
            
            write(output,(char *)&(data[1]),sizeof(INT32)*(line_elems-1));
            lines++;
            data[0]=data[i-1];
            if(lines%5000==0)
	    {
	        cout<<lines<<" lines converted\n";
	    }
        }

    } 
    while(!input2.eof());


    // Putting general variables on header...
    lseek(output,0,SEEK_SET);

    write(output,(char *)&lines,sizeof(INT32));
    cout<<"Lines: "<<lines<<"\n";

    write(output,(char *)&line_elems,sizeof(INT32));
    cout<<"Line elems: "<<line_elems-2<<"\n";

    write(output,(char *)&blocks,sizeof(INT32));
    cout<<"Blocks: "<<blocks<<"\n";

    write(output,(char *)&marks,sizeof(INT32));
    cout<<"Marks: "<<marks<<"\n";

    close(output);

    exit(0);
}


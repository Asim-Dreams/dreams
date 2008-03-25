/*
 * Copyright (C) 2003-2006 Intel Corporation
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

#ifndef _StripEntry_H
#define _StripEntry_H

#include <qcolor.h>
#include <qstring.h>
#include "asim/agt_syntax.h"
#include "asim/AGTTypes.h"
#define THREADS 5
#define MAX_OPENED_FILES 8

//
// This file contains the structures needed to maintain a multifile stripchart
// viewer. The system only has one allFilesData. Some values are defined to
// obtain static limits and to simplify the code. The structure basis is:
// ----------------
// | AllFilesData |
// ----------------
//        |
//        |        ---------------
//        |--------| StripEntry0 |
//        |        ---------------
//        |               |
//        |               |
//        |               |       --------------
//        |               |-------|  StripData |
//        |               |       --------------
//        |               |             |
//        |               |             |--- Points Thread 0
//        |               |             |
//        |               |             |--- Points Thread 1
//        |               |             |
//        |               |             |--- Points Thread 2
//        |               |             |
//        |               |             |--- ...
//        |               |             |
//        |               |             |--- Points Thread K0 (K0<THREADS)
//        |               |
//        |               |
//        |               |       --------------
//        |               |-------|  StripData |
//        |               |       --------------
//        |               |             |
//        |               |             |--- Points Thread 0
//        |               |             |
//        |               |             |--- Points Thread 1
//        |               |             |
//        |               |             |--- Points Thread 2
//        |               |             |
//        |               |             |--- ...
//        |               |             |
//        |               |             |--- Points Thread K1 (K1<THREADS)
//        |               |
//        |               |
//        |               |       --------------
//        |               |-------|  StripData |
//        |               |       --------------
//        |               |             |
//        |               |             |--- Points Thread 0
//        |               |             |
//        |               |             |--- Points Thread 1
//        |               |             |
//        |               |             |--- Points Thread 2
//        |               |             |
//        |               |             |--- ...
//        |               |             |
//        |               |             |--- Points Thread K2 (K2<THREADS)
//        |               |
//        |               |
//        |               |-------...
//        |               |
//        |               |
//        |               |       --------------
//        |               |-------|  StripData |
//        |                       --------------
//        |                             |
//        |                             |--- Points Thread 0
//        |                             |
//        |                             |--- Points Thread 1
//        |                             |
//        |                             |--- Points Thread 2
//        |                             |
//        |                             |--- ...
//        |                             |
//        |                             |--- Points Thread KN (KN<THREADS)
//        |
//        |
//        |        ---------------
//        |--------| StripEntry1 |
//        |        ---------------
//        |               |
//        |               |
//        |               |       --------------
//        |               |-------|  StripData |
//        |               |       --------------
//        |               |             |
//        |               |             |--- Points Thread 0
//        |               |             |
//        |               |             |--- Points Thread 1
//        |               |             |
//        |               |             |--- Points Thread 2
//        |               |             |
//        |               |             |--- ...
//        |               |             |
//        |               |             |--- Points Thread K0 (K0<THREADS)
//        |               |
//        |               |
//        |               |       --------------
//        |               |-------|  StripData |
//        |               |       --------------
//        |               |             |
//        |               |             |--- Points Thread 0
//        |               |             |
//        |               |             |--- Points Thread 1
//        |               |             |
//        |               |             |--- Points Thread 2
//        |               |             |
//        |               |             |--- ...
//        |               |             |
//        |               |             |--- Points Thread K1 (K1<THREADS)
//        |               |
//        |               |
//        |               |       --------------
//        |               |-------|  StripData |
//        |               |       --------------
//        |               |             |
//        |               |             |--- Points Thread 0
//        |               |             |
//        |               |             |--- Points Thread 1
//        |               |             |
//        |               |             |--- Points Thread 2
//        |               |             |
//        |               |             |--- ...
//        |               |             |
//        |               |             |--- Points Thread K2 (K2<THREADS)
//        |               |
//        |               |
//        |               |-------...
//        |               |
//        |               |
//        |               |       --------------
//        |               |-------|  StripData |
//        |                       --------------
//        |                             |
//        |                             |--- Points Thread 0
//        |                             |
//        |                             |--- Points Thread 1
//        |                             |
//        |                             |--- Points Thread 2
//        |                             |
//        |                             |--- ...
//        |                             |
//        |                             |--- Points Thread KN (KN<THREADS)
//        |
//        |
//        |--------***
//        |
//        |
//        |
//        |        ---------------
//        |--------| StripEntryM | (M<MAX_OPENED_FILES)
//                 ---------------
//                        |
//                        |
//                        |       --------------
//                        |-------|  StripData |
//                        |       --------------
//                        |             |
//                        |             |--- Points Thread 0
//                        |             |
//                        |             |--- Points Thread 1
//                        |             |
//                        |             |--- Points Thread 2
//                        |             |
//                        |             |--- ...
//                        |             |
//                        |             |--- Points Thread K0 (K0<THREADS)
//                        |
//                        |
//                        |       --------------
//                        |-------|  StripData |
//                        |       --------------
//                        |             |
//                        |             |--- Points Thread 0
//                        |             |
//                        |             |--- Points Thread 1
//                        |             |
//                        |             |--- Points Thread 2
//                        |             |
//                        |             |--- ...
//                        |             |
//                        |             |--- Points Thread K1 (K1<THREADS)
//                        |
//                        |
//                        |       --------------
//                        |-------|  StripData |
//                        |       --------------
//                        |             |
//                        |             |--- Points Thread 0
//                        |             |
//                        |             |--- Points Thread 1
//                        |             |
//                        |             |--- Points Thread 2
//                        |             |
//                        |             |--- ...
//                        |             |
//                        |             |--- Points Thread K2 (K2<THREADS)
//                        |
//                        |
//                        |-------...
//                        |
//                        |
//                        |       --------------
//                        |-------|  StripData |
//                                --------------
//                                      |
//                                      |--- Points Thread 0
//                                      |
//                                      |--- Points Thread 1
//                                      |
//                                      |--- Points Thread 2
//                                      |
//                                      |--- ...
//                                      |
//                                      |--- Points Thread KN (KN<THREADS)
//         
//         


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

    QColor color[THREADS];  // Colors for every line
    INT32 lineStyle[THREADS]; // The line style selected (Refer to PenStyle in QT Docs)
} 
StripData;

typedef struct
{
    INT32 cycle;
    char description[128];
    QColor color;
    INT32 position;
    bool visible;
} 
MarkerData;

typedef struct 
{
    QString *fileName;         // Holds the filename of these Stripcharts
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
    QColor HAxis;
    QColor VAxis;
    QColor Markers;
    QColor HGrid;
    QColor VGrid;
    QColor Background;
}
applicationProperties;

typedef struct 
{    
    INT32 active[MAX_OPENED_FILES];           // Mask with opened files needed
                                            // to allow to close the files when
                                            // they are opened.
    StripEntry files[MAX_OPENED_FILES];     // All the files opened

    applicationProperties generalProperties;// General values
}
allFilesData;

class LittleSelectorImpl;
class ColorsImpl;

#endif

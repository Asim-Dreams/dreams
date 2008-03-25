

//
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
 * @file
 * @author Jesus Corbal
 *
 * @brief First filter prototype
 *
 * 
 */


#ifndef _FILTER_H_
#define _FILTER_H_

#include <qcolor.h>

#include "dDB/DralDBSyntax.h"

typedef enum {
    FILTER_NONE = 0,
    FILTER_TRIANGLE,
    FILTER_QUADRATIC,
    FILTER_GAUSSIAN,
    NUM_FILTERS
} FILTER_T;

/*
 * Class FILTER
 *
 */

class QColorFilterClass
{
  private:

     // filter parameters
     unsigned long        taps;        // filter size
     float            *coef;        // filter coeficients
     float            sat;        // alpha factor correction

     // input-associated parameters
     unsigned int        stride;        // useful to unify x and y filters

  public:
     
     // constructor/destructor
     QColorFilterClass(unsigned long size=1, FILTER_T filt_type=FILTER_QUADRATIC, unsigned int stride=1, float saturation=1.0);
     ~QColorFilterClass();

     // setting methods
     void    SetNewSaturation(float newsat);
     void    SetNewStride(unsigned int stride);

     // accessors
     bool        NoAlphaCorrection();
     unsigned long    GetSize();    
     unsigned long    GetDelay();    // syntactic sugar : same as GetSize
     unsigned long    GetTaps();

     // main methods
     QColor    FilterColorStream(QColor *stream);
     QColor    FilterColorStream(QColor *stream, QColor bg);
     QColor    FilterColorStream(UINT32 head, QColor *stream);
     QColor    FilterColorStream(UINT32 head, QColor *stream, QColor bg);
};

typedef QColorFilterClass FILTER_CLASS; 
typedef FILTER_CLASS *FILTER;

#endif

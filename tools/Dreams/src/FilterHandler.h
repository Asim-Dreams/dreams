

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

#ifndef _FILEHANDLER_H
#define _FILEHANDLER_H


#include "Filter.h"

typedef enum {
    X_DIM,
    Y_DIM,
    T_DIM,
    NUM_DIM
} FILTER_DIMENSION_T;


/*
 * Class FILTERHANDLER
 *
 */


class FilterHandlerClass
{
  private:

     // filters
     FILTER            xFilter;    // x-axis filter
     FILTER            yFilter;    // y-axis filter
     FILTER            tFilter;    // time filter

     // filter parameters
     unsigned int        xWidth;             // x-axis width (taps)
     unsigned int        yWidth;             // y-axis width (taps)
     unsigned int        tWidth;             // time width (taps)

     // internal storage
     bool            preprocessed;
     UINT32            frameX, frameY;    // Frame dimensions
     QColor            *frameBuffer;    
     QColor            *frameBufferTmp;    

     // internal methods
     QColor            AccessFrameBuffer(unsigned int x, unsigned int y);
     void            ProcessFullRectangle(unsigned long rectangleId, unsigned long cyc, QColor bg);

  public:
     
     // constructor/destructor
     FilterHandlerClass();            
     ~FilterHandlerClass();
     
     // Accessors
     inline UINT32 GetTaps(void) const { Q_ASSERT(tFilter); return tFilter->GetTaps(); }
     inline UINT32 GetDelay(void) const { Q_ASSERT(tFilter); return tFilter->GetDelay(); }

     // filter activation methods
     void    ActivateFilter(FILTER_DIMENSION_T dim, unsigned long size=1, FILTER_T filt_type=FILTER_QUADRATIC, float saturation=1.0);
     void    ActivateXFilter(unsigned long size=0, FILTER_T filt_type=FILTER_QUADRATIC, float saturation=1.0);
     void    ActivateYFilter(unsigned long size=0, FILTER_T filt_type=FILTER_QUADRATIC, float saturation=1.0);
     void    ActivateTFilter(unsigned long size=0, FILTER_T filt_type=FILTER_QUADRATIC, float saturation=1.0);

     // temporal methods (for time-only domain filtering)
     QColor DoTemporalFiltering(QColor *stream, QColor bg);

     // full-domain filter methods
     QColor    DoFullFiltering(unsigned long rectangleId, unsigned int x, unsigned int y, unsigned long cyc, QColor bg);
};

typedef FilterHandlerClass FILTER_HANDLER_CLASS; 
typedef FILTER_HANDLER_CLASS *FILTER_HANDLER;

#endif

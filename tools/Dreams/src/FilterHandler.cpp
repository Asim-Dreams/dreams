

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

// General includes.
#include <stdio.h>
#include <assert.h>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "CSLCache.h"
#include "FilterHandler.h"

/*
 * Class FILTER_HANDLER
 *
 */


// ---------------------------
//   Basic class support
// ---------------------------

// Constructor

FILTER_HANDLER_CLASS::FilterHandlerClass ()
{
   // reset filters
   xFilter = NULL;
   yFilter = NULL;
   tFilter = NULL;

   // reset internal storage
   preprocessed = false;
   frameBuffer = NULL;
   frameBufferTmp = NULL;
}

// Destructor

FILTER_HANDLER_CLASS::~FilterHandlerClass()
{
   delete xFilter;
   delete yFilter;
   delete tFilter;
   delete frameBuffer;
   delete frameBufferTmp;
}


// ---------------------------
//   Filter Activation Methods
// ---------------------------

void
FILTER_HANDLER_CLASS::ActivateFilter(
    FILTER_DIMENSION_T dim,        ///< Dimension (x,y or time)
    unsigned long size,        ///< Size (lateral one, min. 0)
    FILTER_T filt_type,        ///< Filter shape
    float saturation)        ///< Alpha correction factor
{
   FILTER    *filter;

   // select appropiate filter and reset it
   switch(dim)
   {
      case X_DIM: filter = &xFilter; 
                delete xFilter;
                break;
      case Y_DIM: filter = &yFilter; 
                delete yFilter;
                  break;
      case T_DIM: filter = &tFilter; 
                delete tFilter;
                break;
      default:    assert(0);
   }

   // create filter (if taps!=1)
   unsigned int taps = (2*size)+1; 
   if(size) {
      *filter = new QColorFilterClass(size, filt_type, 1, saturation);    // Stride is set preemptively to 1
   } 
}

void
FILTER_HANDLER_CLASS::ActivateXFilter(
    unsigned long size,        ///< Size (lateral one, min. 0)
    FILTER_T filt_type,        ///< Filter shape
    float saturation)        ///< Alpha correction factor
{
   this->ActivateFilter(X_DIM, size, filt_type, saturation);
}


void
FILTER_HANDLER_CLASS::ActivateYFilter(
    unsigned long size,        ///< Size (lateral one, min. 0)
    FILTER_T filt_type,        ///< Filter shape
    float saturation)        ///< Alpha correction factor
{
   this->ActivateFilter(Y_DIM, size, filt_type, saturation);
}


void
FILTER_HANDLER_CLASS::ActivateTFilter(
    unsigned long size,        ///< Size (lateral one, min. 0)
    FILTER_T filt_type,        ///< Filter shape
    float saturation)        ///< Alpha correction factor
{
   this->ActivateFilter(T_DIM, size, filt_type, saturation);
}


// ---------------------------
//   Internal methods
// ---------------------------

#define IND(a,b)    ((a)*(frameX))+(b)    // syntax sugar for addressing linear vector as an array

QColor
FILTER_HANDLER_CLASS::AccessFrameBuffer(
    unsigned int x,        ///< x-axis location
    unsigned int y)        ///< y-axis location
{
   unsigned int offX = 0, offY = 0;

   // update offsets according to filter information
   if(xFilter) offX = xFilter->GetSize();
   if(yFilter) offY = yFilter->GetSize();
   assert( (x+offX) < frameX );
   assert( (y+offY) < frameY );

   // return result
   return (frameBuffer[IND(y+offY,x+offX)]);
}


void
FILTER_HANDLER_CLASS::ProcessFullRectangle(
    unsigned long rectangleId,    ///< rectange identifier for info
    unsigned long cyc,        ///< current cycle
    QColor bg)            ///< background color (needed for alpha correction)
{
   // local variables
   unsigned int offX=0, offY=0;

   // step1: create frame buffers
   delete frameBuffer;
   delete frameBufferTmp;
   //frameX = draldb->GetRectangleXsize(rectangleId);
   //frameY = draldb->GetRectangleYsize(rectangleId);
   if(xFilter) {
      offX = xFilter->GetSize();
      frameX += 2*offX;
   }
   if(yFilter) {
      offY = yFilter->GetSize();
      frameY += 2*offY;
      tFilter->SetNewStride(frameX);
   }

   frameBuffer    = new QColor[frameY*frameX];
   frameBufferTmp = new QColor[frameY*frameX];

   // step2: put background color in borders of the frames
   for(UINT32 x=0; x<frameX; x++) 
   {
      for(UINT32 y = 0; y < offY; y++) 
         frameBufferTmp[IND(y,x)] = frameBuffer[IND(y,x)] = bg; 

      for(UINT32 y = (frameY-offY); y<frameY; y++) 
         frameBufferTmp[IND(y,x)] = frameBuffer[IND(y,x)] = bg; 
   }

   for(UINT32 y = offY; y < (frameY-offY); y++) 
   {
      for(UINT32 x = 0; x < offX; x++) 
         frameBufferTmp[IND(y,x)] = frameBuffer[IND(y,x)] = bg; 
      for(UINT32 x = (frameX-offX); x < frameX; x++) 
         frameBufferTmp[IND(y,x)] = frameBuffer[IND(y,x)] = bg; 
   }

   // step3: handle temporal domain
   for(UINT32 y = offY; y < (frameY-offY); y++) 
   {
      for(UINT32 x = offX; x < (frameX-offX); x++) 
      {
         //frameBuffer[IND(y,x)] = DoTemporalFiltering(rectangleId, x, y, cyc, bg);
      }
   }

   // optimization: don't bother if x and y filters are disabled
   if(xFilter==NULL && yFilter==NULL) return;

   // step4: handle x-axis domain
   if(xFilter) 
   {
      for(UINT32 y = offY; y < (frameY-offY); y++) 
      {
         for(UINT32 x = offX; x < (frameX-offX); x++) 
         {
            if(xFilter->NoAlphaCorrection()) 
               frameBufferTmp[IND(y,x)] = xFilter->FilterColorStream(0, &frameBuffer[IND(y-offY,x-offX)]);
            else 
               frameBufferTmp[IND(y,x)] = xFilter->FilterColorStream(0, &frameBuffer[IND(y-offY,x-offX)], bg);
         }
      }
   }
   else    
   {
      for(UINT32 y=offY; y < (frameY-offY); y++) 
      {
         for(UINT32 x=offX; x < (frameX-offX); x++) 
         {
            frameBufferTmp[IND(y,x)] = frameBuffer[IND(y,x)];
         }
      }
   }

   // step5: handle y-axis domain
   if(yFilter) 
   {
      for(UINT32 y = offY; y < (frameY-offY); y++) 
      {
         for(UINT32 x = offX; x < (frameX-offX); x++) 
         {
            if(yFilter->NoAlphaCorrection()) 
               frameBuffer[IND(y,x)] = yFilter->FilterColorStream(0, &frameBufferTmp[IND(y-offY,x-offX)]);
            else 
               frameBuffer[IND(y,x)] = yFilter->FilterColorStream(0, &frameBufferTmp[IND(y-offY,x-offX)], bg);
         }
      }
   }
   else    {
      for(UINT32 y=offY; y < (frameY-offY); y++) 
      {
         for(UINT32 x=offX; x < (frameX-offX); x++) 
         {
            frameBuffer[IND(y,x)] = frameBufferTmp[IND(y,x)];
         }
      }
   }

}

// ---------------------------
//   Time domain wrapper
// ---------------------------

QColor
FILTER_HANDLER_CLASS::DoTemporalFiltering (
    QColor *stream,          ///< Temporal array of elements
    QColor  bg)                   ///<  layout  background color
{
   QColor finalColor;

   if(tFilter->NoAlphaCorrection()) finalColor = tFilter->FilterColorStream(0, stream);        
   else finalColor = tFilter->FilterColorStream(0, stream, bg);

   return finalColor;
}

// -----------------------------
//   Full-domain filter methods
// -----------------------------

QColor
FILTER_HANDLER_CLASS::DoFullFiltering (
    unsigned long rectangleId,    ///<  track layout identifier
    unsigned int x,             ///<  x-axis location
    unsigned int y,             ///<  y-axis location
    unsigned long cyc,             ///<  current cycle
    QColor  bg)                  ///<  layout  background color
{

   // Check if the Rectangle has already been pre-processed
   if(!preprocessed) ProcessFullRectangle(rectangleId, cyc, bg); 

   // Access the rectangle QColor frame buffer
   return AccessFrameBuffer(x,y);
}

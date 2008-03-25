

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

#include <stdio.h>
#include <math.h>
#include <assert.h>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "Filter.h"


/*
 * Class FILTER
 *
 */


// ---------------------------
//   Basic class support
// ---------------------------

// Constructor

FILTER_CLASS::QColorFilterClass (
    unsigned long size,    ///< Filter Size
    FILTER_T filt_type,    ///< Filter Type
    unsigned int stride,    ///< Input stream stride
    float saturation)    ///< Alpha compensation
{
   assert((size&1));        // we want the number of taps to be odd
   this->taps = size;
   this->stride = stride;
   this->sat = (saturation < 1.0)? 1.0 : saturation;    // No much sense to apply lower than 1 saturation
   coef = new float[taps];

   if(filt_type == FILTER_TRIANGLE) {
      for(UINT32 i=0; i<=taps/2; i++) { coef[i] = ((float)i/(float)taps)/2.0; }
      coef[(taps/2)] = 1.0;
      for(UINT32 i=0; i<taps/2; i++) { coef[i+(taps/2)+1] = coef[(taps/2)-1-i]; }
   }
   else if(filt_type == FILTER_QUADRATIC) {
      for(UINT32 i=0; i<=taps/2; i++) { coef[i] = 1.0 - (((float)i/((float)(taps/2)))*((float)i/((float)(taps/2)))); }
      coef[(taps/2)] = 1.0;
      for(UINT32 i=0; i<taps/2; i++) { coef[i+(taps/2)+1] = coef[(taps/2)-1-i]; }
   }
   else if(filt_type == FILTER_GAUSSIAN) {
      for(UINT32 i=0; i<=taps/2; i++) { 
        float sigma = (float) (taps+3/4); // crappy approximation
        float k = 1/(sqrt(2*M_PI*sigma));
    float exponent = -((float)(i*i))/2.0*sigma*sigma;
    coef[i] = k * exp(exponent); 
      }
      coef[(taps/2)] = 1.0;
      for(UINT32 i=0; i<taps/2; i++) { coef[i+(taps/2)+1] = coef[(taps/2)-1-i]; }
   }
   else assert(0); // invalid filter

   // We want the filter to be normalized
   float overall = 0.0;
   for(UINT32 i=0; i<taps; i++) { overall+= coef[i]; }
   for(UINT32 i=0; i<taps; i++) { coef[i]/= overall; }

   // for(int i=0; i<taps; i++) printf("[%f]",coef[i]);

}

// Destructor

FILTER_CLASS::~QColorFilterClass()
{
   delete coef;
}

// Setting methods

void
FILTER_CLASS::SetNewSaturation(float newsat)
{
   this->sat = (newsat < 1.0)? 1.0 : newsat;
}

void
FILTER_CLASS::SetNewStride(unsigned int stride)
{
  this->stride = stride;
}

// Accessors

bool
FILTER_CLASS::NoAlphaCorrection()
{
   return((this->sat==1.0));

}

unsigned long
FILTER_CLASS::GetSize()
{
   return((taps-1)/2);
}

unsigned long
FILTER_CLASS::GetDelay()
{
   return((taps-1)/2);
}

unsigned long
FILTER_CLASS::GetTaps()
{
   return(taps);
}

// ---------------------------
//   Basic filtering support
// ---------------------------

#define CIRC_IND(a,b)    ( (((a)+(b))>=(this->taps))? ((a)+(b)-(this->taps)) : ((a)+(b)) )

// Do the basic filtering (no control over alpha and saturation)
QColor
FILTER_CLASS::FilterColorStream (
    UINT32 head,        ///< QColor input stream head
    QColor* stream)        ///< QColor input stream
{
   // local variables
   float        ACCr=0.0, ACCg=0.0, ACCb=0.0;
   int r, g, b;
   QColor        res;

   // Do the FIR stuff
   for(UINT32 i=0; i<taps; i++) {
      stream[CIRC_IND(head, i*stride)].getRgb(&r,&g, &b);
      ACCr += ((float)r)*coef[i];
      ACCg += ((float)g)*coef[i];
      ACCb += ((float)b)*coef[i];
   }

   // Convert floating point values onto real colors
   r = (ACCr < 0.0)? 0 : (ACCr > 255.0)? 255 : (int)ACCr;
   g = (ACCg < 0.0)? 0 : (ACCg > 255.0)? 255 : (int)ACCg;
   b = (ACCb < 0.0)? 0 : (ACCb > 255.0)? 255 : (int)ACCb;

   // Create result QColor
   res.setRgb((int)ACCr, (int)ACCg, (int)ACCb);
   return(res);
}


// Establish background control and saturation, so that we
// can control alpha factor

QColor
FILTER_CLASS::FilterColorStream (
    UINT32 head,        ///< QColor input stream head
    QColor* stream,        ///< QColor input stream
    QColor  bg)         ///< background QColor 
{
   // local variables
   QColor  res;
   float   ACCr=0.0, ACCg=0.0, ACCb=0.0;
   float   scale=0.0, alpha;
   int     r, g, b;

   // Calculate alpha factor
   for(UINT32 i=0; i<taps; i++) {
      if(stream[CIRC_IND(head, i*stride)]==bg) { scale += coef[i]; }
   }

   // Calculate alpha and return if bg threshold
   if(scale > 1.0) scale = 1.0;
   alpha = scale/this->sat;
   if( (1.0-scale) < 0.0001 ) return(bg); 

   // Do the FIR stuff (apply alpha ponderation)
   for(UINT32 i=0; i<taps; i++) {
      stream[CIRC_IND(head, i*stride)].getRgb(&r,&g, &b);
      ACCr += (((float)r)*coef[i])/(1.0-scale);
      ACCg += (((float)g)*coef[i])/(1.0-scale);
      ACCb += (((float)b)*coef[i])/(1.0-scale);
   }

   // Apply alpha
   ACCr = (ACCr*(1-alpha)) + ((float)bg.red()*alpha);
   ACCg = (ACCg*(1-alpha)) + ((float)bg.green()*alpha);
   ACCb = (ACCb*(1-alpha)) + ((float)bg.blue()*alpha);

   // Convert floating point values onto real colors
   r = (ACCr < 0.0)? 0 : (ACCr > 255.0)? 255 : (int)ACCr;
   g = (ACCg < 0.0)? 0 : (ACCg > 255.0)? 255 : (int)ACCg;
   b = (ACCb < 0.0)? 0 : (ACCb > 255.0)? 255 : (int)ACCb;

   // Create result QColor
   res.setRgb((int)ACCr, (int)ACCg, (int)ACCb);
   return(res);
}



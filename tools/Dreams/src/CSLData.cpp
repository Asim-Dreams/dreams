/*
 * Copyright (C) 2004-2006 Intel Corporation
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

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "CSLData.h"

#define DELTA(c,f) (((double)(f) - (double)(c)) / (double)steps)
#define CLAMP(c) c = ((c) < 0) ? 0 : (((c) > 255) ? 255 : (c))

void 
CSLData::fadeParams(QColor fade_color, UINT32 steps)
{
 INT32  r, g, b;
 INT32 fr,fg,fb;

 //
 // Get the destination color (the color to which we have to fade to)
 //
 fade_color.getRgb(&fr,&fg,&fb);


 //
 // Consider the following problem:  you have a red color (255) and you
 // need to fade to a light grey (220)  in 30 steps. The obvious formula 
 //
 //  255 - 220 / 30  = 1.16
 //
 // yields a step value that is not integer. If you round down to 1, you
 // will never get to the true background color by accumulating the 30
 // steps (255 - 30 is only 225, not 220). If you round up to 2, you will
 // pass your destination color (220). The relatively simple solution used
 // here is to keep all info (original color --per channel-- and DELTA) 
 // in double format. Then, we only round when we do a "setRgb()". Maybe
 //

 //
 // Remember (in FP double to avoid the rounding off problem) the original
 // value of fill color and its delta
 //
 if ( getFillColorSet() )
 { 
     fill_color.getRgb(&r,&g,&b);
     fill_fade.r  = r;
     fill_fade.g  = g;
     fill_fade.b  = b;
     fill_fade.dr = DELTA(r,fr);
     fill_fade.dg = DELTA(g,fg);
     fill_fade.db = DELTA(b,fb);
     compute_mask = compute_mask & ~CSL_FILL_COLOR_MASK;
 }

 //
 // Remember (in FP double to avoid the rounding off problem) the original
 // value of letter color and its delta
 //
 if ( getBorderColorSet() )
 { 
     border_color.getRgb(&r,&g,&b);
     border_fade.r  = r;
     border_fade.g  = g;
     border_fade.b  = b;
     border_fade.dr = DELTA(r,fr);
     border_fade.dg = DELTA(g,fg);
     border_fade.db = DELTA(b,fb);
     compute_mask = compute_mask & ~CSL_BORDER_COLOR_MASK;
 }

 //
 // Remember (in FP double to avoid the rounding off problem) the original
 // value of letter color and its delta
 //
 if ( getLetterSet() || getLetterColorSet() )
 { 
     letter_color.getRgb(&r,&g,&b);
     letter_fade.r  = r;
     letter_fade.g  = g;
     letter_fade.b  = b;
     letter_fade.dr = DELTA(r,fr);
     letter_fade.dg = DELTA(g,fg);
     letter_fade.db = DELTA(b,fb);
     compute_mask = compute_mask & ~CSL_LETTER_COLOR_MASK;
 }
}

void 
CSLData::fadeStep(UINT32 step)
{
 INT32  r, g, b;


 if ( getFillColorSet() )
 {
     fill_fade.r += fill_fade.dr;
     fill_fade.g += fill_fade.dg;
     fill_fade.b += fill_fade.db;
     fill_color.setRgb((INT32)fill_fade.r,(INT32)fill_fade.g,(INT32)fill_fade.b);
 }

 if ( getBorderColorSet() )
 {
     border_fade.r += border_fade.dr;
     border_fade.g += border_fade.dg;
     border_fade.b += border_fade.db;
     border_color.setRgb((INT32)border_fade.r,(INT32)border_fade.g,(INT32)border_fade.b);
 }

 if ( getLetterColorSet() )
 {
     letter_fade.r += letter_fade.dr;
     letter_fade.g += letter_fade.dg;
     letter_fade.b += letter_fade.db;
     letter_color.setRgb((INT32)letter_fade.r,(INT32)letter_fade.g,(INT32)letter_fade.b);
 }

}

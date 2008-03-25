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

#ifndef _PT_COLORS_H_
#define _PT_COLORS_H_

// Pipe color enumeration - These are the colors that can be attached to events
// for display purposes. 
// msblacks - I kept this definition the same as PSLib since no need to change
// it, and will be familiar to developers				
///////////////////////////////////////////////////////////////////////////////

typedef enum {
        Pipe_No_Color                =  0,
        Pipe_Dark_Blue               =  1,
        Pipe_Blue                    =  2,
        Pipe_Cyan                    =  3,
        Pipe_Dark_Green              =  4,
        Pipe_Green                   =  5,
        Pipe_Yellow                  =  6,
        Pipe_Orange                  =  7,
        Pipe_Red                     =  8,
        Pipe_Pink                    =  9,
        Pipe_Magenta                 = 10,
        Pipe_Purple                  = 11,
        Pipe_Brown                   = 12,
        Pipe_Gray                    = 13,
        Pipe_Light_Gray              = 14,
        Pipe_White                   = 15,
        Pipe_Black                   = 16,
        Pipe_Max_Colors				
} Pipe_Color;

#endif
 





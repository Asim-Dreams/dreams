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


#ifndef __vcgColorAttr
#define __vcgColorAttr


/** named vcg colors */
typedef enum 
{
	vcgColor_AQUAMARINE,
	vcgColor_BLACK,
	vcgColor_BLUE,
	vcgColor_CYAN,
	vcgColor_DARKBLUE,
	vcgColor_DARKCYAN,
	vcgColor_DARKGREEN,
	vcgColor_DARKGREY,
	vcgColor_DARKMAGENTA,
	vcgColor_DARKRED,
	vcgColor_DARKYELLOW,
	vcgColor_GOLD,
	vcgColor_GREEN,
	vcgColor_KHAKI,
	vcgColor_LIGHTBLUE,
	vcgColor_LIGHTCYAN,
	vcgColor_LIGHTGREEN,
	vcgColor_LIGHTGREY,
	vcgColor_LIGHTMAGENTA,
	vcgColor_LIGHTRED,
	vcgColor_LIGHTYELLOW,
	vcgColor_LILAC,
	vcgColor_MAGENTA,
	vcgColor_ORANGE,
	vcgColor_ORCHID,
	vcgColor_PINK,
	vcgColor_PURPLE,
	vcgColor_RED,
	vcgColor_TURQUOISE,
	vcgColor_WHITE,
	vcgColor_YELLOW,
	vcgColor_YELLOWGREEN,
	vcgColor_NUM_VALUES
} vcgColor;


/**
  * Color attribute representation (helper static class).
  * vcgen framework component
  * @version 0.1
  * @date started at 2002-02-25 
  * @author Federico Ardanaz
  */
class vcgColorAttr
{
	public:
	
		/**
		 * Method to get the vcg-syntax representation of the attribute 
		 * @param value binary representation
		 * @return vcg-syntax representation of the attribute
		 */ 
		static char* toStr(vcgColor color);
		
};

#endif

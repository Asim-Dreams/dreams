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


#ifndef __vcgTextModeAttr
#define __vcgTextModeAttr


/** vcg TextMode values */
typedef enum 
{
	vcgTextMode_CENTER,
	vcgTextMode_LEFT_JUSTIFY,
	vcgTextMode_RIGHT_JUSTIFY,
	vcgTextMode_NUM_VALUES
} vcgTextMode;


/**
  * TextMode attribute representation (helper static class).
  * vcgen framework component
  * @version 0.1
  * @date started at 2002-02-25 
  * @author Federico Ardanaz
  */
class vcgTextModeAttr
{
	public:
	
		/**
		 * Method to get the vcg-syntax representation of the attribute 
		 * @param value binary representation
		 * @return vcg-syntax representation of the attribute
		 */ 
		static char* toStr(vcgTextMode value);
		
};

#endif

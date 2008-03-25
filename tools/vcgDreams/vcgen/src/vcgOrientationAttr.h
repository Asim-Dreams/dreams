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


#ifndef __vcgOrientationAttr
#define __vcgOrientationAttr


/** vcg orientation values */
typedef enum 
{
	vcgOrientation_TOP_TO_BOTTOM,
	vcgOrientation_BOTTOM_TO_TOP,
	vcgOrientation_LEFT_TO_RIGHT,
	vcgOrientation_RIGHT_TO_LEFT,
	vcgOrientation_NUM_VALUES
} vcgOrientation;


/**
  * Orientation attribute representation (helper static class).
  * vcgen framework component
  * @version 0.1
  * @date started at 2002-02-25 
  * @author Federico Ardanaz
  */
class vcgOrientationAttr
{
	public:
	
		/**
		 * Method to get the vcg-syntax representation of the attribute 
		 * @param value binary representation
		 * @return vcg-syntax representation of the attribute
		 */ 
		static char* toStr(vcgOrientation value);
		
};

#endif

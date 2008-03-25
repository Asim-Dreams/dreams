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


#ifndef __vcgColorEntry
#define __vcgColorEntry

#include <string.h>
#include "vcgComponent.h"

/**
  * This is a tiny helper class to encapsulate ad-hoc defined colors.
  * vcgen framework component
  * @version 0.1
  * @date started at 2002-02-28 
  * @author Federico Ardanaz
  */
class vcgColorEntry : public vcgComponent
{
	public:
		/**
		  * Constructor.
		  * @param index color number
		  * @param red red component [0.255] 
		  * @param green green component [0.255] 
		  * @param blue blue component [0.255] 
		  */
		vcgColorEntry(int index, int red, int green, int blue)
		{ idx = index; r = red; g = green; b = blue; }

		/** 
		  * vcgComponent interface.
		  */
		void toVCG(FILE *f, int tabLevel)
		{
			this->myTabLevel = tabLevel;
			putTabs(f);fprintf(f,"colorentry %d : %d %d %d\n",idx,r,g,b);
		}
		
	protected:
		int idx;
		int r,g,b;
}; 

#endif

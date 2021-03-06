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


#ifndef __vcgClassNameEntry
#define __vcgClassNameEntry

#include <string.h>
#include <stdlib.h>
#include "vcgComponent.h"

/**
  * This is a tiny helper class to encapsulate class name definitions.
  * vcgen framework component
  * @version 0.1
  * @date started at 2002-02-28 
  * @author Federico Ardanaz
  */
class vcgClassNameEntry : public vcgComponent
{
	public:
		/**
		  * Constructor.
		  * @param id class name index
		  * @param cn class name description 
		  */
		vcgClassNameEntry(int id, char *cn)
		{ idx = id; classname = strdup(cn); }

		/** Destructor. */	
		virtual ~vcgClassNameEntry(void)
		{ free(classname); }

		/**
		  * vcgComponent interface method.
		  */
		void toVCG(FILE *f, int tabLevel)
		{
			this->myTabLevel = tabLevel;
			putTabs(f);fprintf(f,"classname %d : \"%s\"\n",idx,classname);
		}
		
	protected:
		int idx;
		char* classname;
};


#endif

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

  
#ifndef __vcgComponentList
#define __vcgComponentList

#include "vcgComponent.h"
#include "List.h"

// If -MD compiler option (GEM cxx) is used assert is not defined
// eventhough we inserted the corresponding include. 
// This can happens when compiling this .h through another
// specific-purpose makefile...

#ifndef assert
#define assert(n)
#endif


/**
  * This is a wrapper to a simple List which introduce the concept of "Layers".
  * @version 0.1
  * @date started at 2002-02-28 
  * @author Federico Ardanaz
  */
class vcgComponentList
{
	public:

		/**
		 * Default constructor.
		 */
		vcgComponentList()
		{
			for (int i=0;i<NUM_LAYERS;i++)
			{
				compList[i] = new List<vcgComponent*>(true);
			}
		}

		/**
		 * Destructor.
		 */
		~vcgComponentList()
		{
			for (int i=0;i<NUM_LAYERS;i++)
			{
				delete compList[i];
			}
		}


		/**
		 * Add a component in the corresponding layer.
		 * @param c Component to be added
		 */
		inline void add(vcgComponent* c)
		{
			assert(c->getLayer() < NUM_LAYERS); 
			(compList[c->getLayer()])->add(c); 
		}
		 

		/**
		 * To recover a layer list.
		 * @return the layer list
		 */
		inline List<vcgComponent*>* getLayer(int layer)
		{
			assert(layer<NUM_LAYERS); 
			return compList[layer];
		}		 
		 
	private:	
		List<vcgComponent*>* compList[NUM_LAYERS];
};
		
#endif

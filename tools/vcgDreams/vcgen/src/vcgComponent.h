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


#ifndef __vcgComponent
#define __vcgComponent

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/** 
 to allow some order in the syntax description
 we define layers for each component class, the
 espected values are:

 0 -> graph attributes (color definitions, etc)
 1 -> nodes
 2 -> edges
 3 -> (sub)graphs

 new classes of vcgComponent should introduce
 new layer levels ...
*/

#define ATTRIB_LAYER	0
#define NODE_LAYER		1
#define EDGE_LAYER		2
#define GRAPH_LAYER		3

#define NUM_LAYERS		4

/**
  * Interface to graph components (nodes and edges).
  * vcgen framework component
  * @version 0.1
  * @date started at 2002-02-25 
  * @author Federico Ardanaz
  */
class vcgComponent
{
	public:
		/**
		  * Default Constructor.
		  */
		vcgComponent(void) 
		{
			myParent=NULL;
			myTabLevel=0;
			myLayer=0;
		}
		
		/** 
		  * Virtual function to dump vcg-syntax of each component
		  *
		  */
		virtual void toVCG(FILE *f, int tabLevel) = 0;
		
		/**
		  * Set the parent component.
		  * The "parent" of an node or edge is the graph it
		  * belongs to, the parent of a (sub)graph is the
		  * graph it belongs to (or NULL if it's the root graph).
		  * @param comp the parent component
		  */
		inline  void setParent(vcgComponent* comp)
		{myParent = comp;}
		
		/** 
		  * Give the indetation level.
		  * To improve indentation we define tab levels
		  * so that the generated code is more human-readable
		  * 
		  * @return indentation level (number of tabs)
		  */
		inline int getTabLevel(void)
		{return myTabLevel;};

		/**
		  * Recover the layer of the component.
		  * The layer of a component is used to decide
		  * a relative order when dumping out the
		  * vcg-syntax representation of a graph.
		  * @return Component Layer
		  */
		inline int getLayer(void)
		{ return myLayer; }

	protected:
		/**
		  * Helper method to put tabs on a file.
		  * The number of tabs characters is picked
		  * from myTabLevel object field.
		  * @param f FILE where tu put tabs on.
		  */
		inline void putTabs(FILE *f)
		{ putTabs (f,0);}
		
		/**
		  * Helper method to put tabs on a file.
		  * @param f FILE where tu put tabs on.
		  * @param plus extra tabulation indicator.
		  */
		inline void putTabs(FILE *f,int plus)
		{
			int tab = myTabLevel+plus;
			if (tab>=32) tab=31;
			for (int i=0;i<tab;i++) 
				fprintf(f,"%c",'\t');
		}

		
	protected:
		vcgComponent* myParent;
		int myTabLevel;
		int myLayer;
};


#endif

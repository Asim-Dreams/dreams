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


#ifndef __vcgBackEdge
#define __vcgBackEdge

#include "vcgComponent.h"
#include "vcgNode.h"
#include "vcgEdge.h"

/**
  * Back Edge component.
  * vcgen framework component
  * @version 0.1
  * @date started at 2002-02-27 
  * @author Federico Ardanaz
  */
class vcgBackEdge : public vcgEdge
{
	public:
		/**
		  * Constructor based on node objects.
		  * @param src Source Node
		  * @param dst Destination Node
		  */
		vcgBackEdge(vcgNode *src, vcgNode *dst, char* label=NULL):vcgEdge(src,dst,label)
		{}

		/**
		  * Constructor based on node names (strings).
		  * @param srcTitle Source Node
		  * @param dstTitle Destination Node
		  */
		vcgBackEdge(char *srcTitle, char *dstTitle, char* label=NULL):vcgEdge(srcTitle,dstTitle,label)
		{}

		/**
		  * Destructor
		  */
		~vcgBackEdge(void){}

		/**
		  * vcgComponent interface methods.
		  */
		void toVCG(FILE *f,int tabLevel);
};

#endif

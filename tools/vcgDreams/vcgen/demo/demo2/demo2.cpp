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

// more or less the same as demo1 but without string alloc
#include "demo2.h"

#define NUMNODES 4

int main()
{
	int i,j;
	vcgEdge *edge;	
	vcgNode *node;	
	char str[64];
	char str1[64];
	char str2[64];
	
	// build the main object
	vcgGraph *g = new vcgGraph();
	
	// set graph properties
	g->setTitle("demo1 graph");
	g->addColorEntry(100,255,0,0);
	
	// add some nodes
	for (i=0;i<NUMNODES;i++)
	{
		sprintf (str,"node%d",i);
		node = new vcgNode();
		node->setTitle(str);
		g->addComponent(node);
	}
	// put the last with ad-hoc color
	node->setColor(100);
	
	// add the edges to produce a K graph
	for (i=0;i<NUMNODES;i++)
	{
		for (j=0;j<NUMNODES;j++)
		{
			if (i!=j)
			{
				sprintf (str1,"node%d",i);
				sprintf (str2,"node%d",j);
				edge = new vcgEdge(str1,str2);
				g->addComponent(edge);
			}
		}
	}
	edge->setColor(vcgColor_CYAN);
	
	// build a sub-graph
	// build the main object
	vcgGraph *sg = new vcgGraph();
	sg->setTitle("demo1 sub-graph");
	// add some nodes
	for (i=0;i<NUMNODES;i++)
	{
		sprintf (str,"snode%d",i);
		node = new vcgNode();
		node->setTitle(str);
		node->setLevel(i);
		sg->addComponent(node);
	}
	
	// add the edges to produce a C graph
	for (i=0;i<(NUMNODES-1);i++)
	{
		sprintf (str1,"snode%d",i);
		sprintf (str2,"snode%d",(i+1));
		edge = new vcgEdge(str1,str2);
		sg->addComponent(edge);
	}
	sprintf (str1,"snode%d",(NUMNODES-1));
	edge = new vcgEdge(str1,"snode0");
	sg->addComponent(edge);
	
	// add the  sub-graph
	g->addComponent(sg);

	// add a cross-edge from g to sg
	sprintf (str1,"node%d",(NUMNODES-1));
	sprintf (str2,"snode%d",(NUMNODES-1));
	edge = new vcgEdge(str1,str2);
	g->addComponent(edge);
	
	// set some node defaults to the subgraph
	vcgNode* ndef = sg->getNodeDefaults();
	ndef->setShape(vcgShape_RHOMB);
	
	// dump out the whole stuff
	g->toVCG();
	delete g;
}

